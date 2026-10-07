#include "gsc_json.hpp"

#if COMPILE_JSON == 1

#include "lib/yyjson.h"

/* 	Native JSON for GSC on yyjson. Mapping (doc/added_script_functions.md):
	object <-> string-keyed array, array <-> int-indexed array, number <-> int if
	integral and 32-bit else float, true/false -> 1/0, null <-> undefined.
	Serialize: an array with only int keys is a JSON array, else an object ({} if empty).
	Structs, entities, threads and functions serialize as null.
 */
#include <climits>
#include <pthread.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <unordered_set>
#include <vector>

extern dvar_t *fs_debug;

#define JSON_MAX_DEPTH 64      // deeper values become undefined / null
#define JSON_MAX_NODES 100000  // serialize walk budget; shared subtrees re-walk per reference
#define JSON_MAX_VALUES 32768  // async worker early reject on the yyjson value count
#define JSON_MAX_STRING 65000 // One script string holds < 65531 bytes: https://github.com/voron00/CoD2rev_Server/blob/11c40a5/src/script/scr_memorytree.cpp#L630
#define JSON_MAX_PATH 64 // MAX_QPATH: https://github.com/voron00/CoD2rev_Server/blob/11c40a5/src/universal/q_shared.h#L176

// One push (json_push_doc) may use at most the cap and must leave the reserve free.
// 65533 variables: https://github.com/voron00/CoD2rev_Server/blob/11c40a5/src/script/scr_variable.cpp#L3834
// 16384 strings: https://github.com/voron00/CoD2rev_Server/blob/11c40a5/src/script/scr_stringlist.cpp#L871
// 65536 8-byte string memory nodes: https://github.com/voron00/CoD2rev_Server/blob/11c40a5/src/script/scr_memorytree.cpp#L160
#define JSON_MAX_VARS 32768
#define JSON_MAX_STRINGS 4096
#define JSON_MAX_MT_NODES 16384
#define JSON_RESERVE_VARS 4096
#define JSON_RESERVE_STRINGS 1024
#define JSON_RESERVE_MT_NODES 4096
#define MT_SIZES 17                 // block sizes 2^0..2^16 nodes

// Script strings are bytes in the client's codepage, not UTF-8, so pass them through as is
#define JSON_READ_FLAGS (YYJSON_READ_ALLOW_BOM | YYJSON_READ_ALLOW_INVALID_UNICODE)
#define JSON_WRITE_FLAGS (YYJSON_WRITE_ALLOW_INVALID_UNICODE | YYJSON_WRITE_INF_AND_NAN_AS_NULL)

// Dvar defaults, read per call via dvar_int_or()
#define JSON_DEF_MAX_LOAD_BYTES  (8 * 1024 * 1024)    // scr_json_max_load_bytes
#define JSON_DEF_SLOW_WARN_MS    25                   // scr_json_slow_warn_ms
#define JSON_DEF_ASYNC_MAX_JOBS  64                   // scr_json_async_max_jobs
#define JSON_HARD_MAX_LOAD_BYTES (32 * 1024 * 1024)   // i386 ceiling

// long long: on i386 a 32-bit long overflows tv_sec * 1000 after ~24.8 days.
static long long now_ms()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (long long)ts.tv_sec * 1000LL + ts.tv_nsec / 1000000L;
}

static int dvar_int_or(const char *name, int fallback)
{
	dvar_t *d = Dvar_FindVar(name);
	if ( d == NULL )
		return fallback;

	// DvarValue is a union: reading the wrong field returns garbage. Dispatch
	// on the actual type so setCvar-created STRING dvars and natively-typed
	// INT dvars both work.
	int v = 0;
	switch ( d->type )
	{
	case DVAR_TYPE_INT:
		v = d->current.integer;
		break;
	case DVAR_TYPE_BOOL:
		v = d->current.boolean ? 1 : 0;
		break;
	case DVAR_TYPE_FLOAT:
		v = (int)d->current.decimal;
		break;
	case DVAR_TYPE_STRING:
		if ( d->current.string != NULL && d->current.string[0] != '\0' )
			v = atoi(d->current.string);
		break;
	default:
		break;
	}

	if ( v > 0 )
		return v;
	return fallback;
}

// GSC floats are 32-bit; printing the promoted double turns 3.1415f into
// 3.1414999961853027. %.9g (FLT_DECIMAL_DIG digits) round-trips to the
// identical float32.
static double json_double_from_float(float f)
{
	char buf[32];
	snprintf(buf, sizeof(buf), "%.9g", (double)f);
	return strtod(buf, NULL);
}

// Register the JSON tuning dvars at engine dvar-init so they appear in dvarlist
// with their defaults and min/max. Called once from custom_Com_InitDvars.
void gsc_json_register_dvars(void)
{
	Dvar_RegisterInt("scr_json_max_load_bytes", JSON_DEF_MAX_LOAD_BYTES, 1, JSON_HARD_MAX_LOAD_BYTES, DVAR_ARCHIVE);
	Dvar_RegisterInt("scr_json_slow_warn_ms", JSON_DEF_SLOW_WARN_MS, 1, 60000, DVAR_ARCHIVE);
	Dvar_RegisterInt("scr_json_async_max_jobs", JSON_DEF_ASYNC_MAX_JOBS, 1, 1024, DVAR_ARCHIVE);
}

// Prints one line on scope exit if the call took longer than scr_json_slow_warn_ms
class JsonTimer
{
	const char *func;
	const char *detail;
	long long t0;
public:
	JsonTimer(const char *f, const char *d) : func(f), detail(d ? d : ""), t0(now_ms()) {}
	~JsonTimer()
	{
		int threshold = dvar_int_or("scr_json_slow_warn_ms", JSON_DEF_SLOW_WARN_MS);
		long long elapsed = now_ms() - t0;
		if ( threshold > 0 && elapsed > threshold )
			Com_Printf("%s took %lld ms %s\n", func, elapsed, detail);
	}
};

// ===========================================================================
// JSON -> GSC
// ===========================================================================

// Script strings are C strings under 64 KB: a NUL would truncate, a longer one drops the server
static bool json_str_ok(yyjson_val *v)
{
	const char *s = yyjson_get_str(v);
	size_t len = yyjson_get_len(v);
	return s != NULL && len < JSON_MAX_STRING && memchr(s, '\0', len) == NULL;
}

// Pushes one value onto the script stack; json_cost() must mirror every branch
static void json_to_gsc_push(yyjson_val *node, int depth)
{
	size_t i, n;
	yyjson_val *key, *val;

	if ( depth >= JSON_MAX_DEPTH || node == NULL )
		stackPushUndefined();
	else if ( yyjson_is_bool(node) )
		stackPushInt(yyjson_is_true(node));
	else if ( yyjson_is_sint(node) && yyjson_get_sint(node) >= INT_MIN && yyjson_get_sint(node) <= INT_MAX )
		stackPushInt((int)yyjson_get_sint(node));
	else if ( yyjson_is_uint(node) && yyjson_get_uint(node) <= INT_MAX )
		stackPushInt((int)yyjson_get_uint(node));
	else if ( yyjson_is_num(node) )
		stackPushFloat((float)yyjson_get_num(node));  // real, or int beyond 32 bits
	else if ( yyjson_is_str(node) && json_str_ok(node) )
		stackPushString(yyjson_get_str(node));
	else if ( yyjson_is_str(node) )
	{
		Com_Printf("json: string of %u bytes or with a NUL replaced with undefined\n", (unsigned)yyjson_get_len(node));
		stackPushUndefined();
	}
	else if ( yyjson_is_arr(node) )
	{
		stackPushArray();
		yyjson_arr_foreach(node, i, n, val)
		{
			json_to_gsc_push(val, depth + 1);
			stackPushArrayLast();
		}
	}
	else if ( yyjson_is_obj(node) )
	{
		stackPushArray();
		yyjson_obj_foreach(node, i, n, key, val)
		{
			if ( !json_str_ok(key) )
			{
				Com_Printf("json: key of %u bytes or with a NUL skipped\n", (unsigned)yyjson_get_len(key));
				continue;
			}
			json_to_gsc_push(val, depth + 1);
			unsigned int k = SL_GetString(yyjson_get_str(key), 0);
			Scr_AddArrayStringIndexed(k);
			SL_RemoveRefToString(k);  // the array holds its own ref
		}
	}
	else
		stackPushUndefined();  // null
}

// VM cost of one push, counted in push order without touching the VM
struct JsonCost
{
	int vars = 0;                          // Scr_MakeArray + one per element or member
	int mtNodes = 0;                       // string memory of the distinct strings
	std::unordered_set<std::string> seen;  // distinct strings
	std::vector<int> newSizes;             // block size of each string not interned yet
};

// A new string takes one 2^size-node block of MT_AllocIndex(len + 1 + 4):
// https://github.com/voron00/CoD2rev_Server/blob/11c40a5/src/script/scr_stringlist.cpp#L874
// https://github.com/voron00/CoD2rev_Server/blob/11c40a5/src/script/scr_memorytree.cpp#L622
static int json_mt_size(size_t len)
{
	int nodes = (int)((len + 1 + 4 + 7) / 8), size = 0;
	while ( (1 << size) < nodes )
		size++;
	return size;
}

static void json_cost_str(yyjson_val *v, JsonCost &c)
{
	const char *s = yyjson_get_str(v);
	size_t len = yyjson_get_len(v);
	if ( !c.seen.emplace(s, len).second )
		return;
	c.mtNodes += 1 << json_mt_size(len);
	if ( !SL_FindStringOfLen(s, len + 1) )
		c.newSizes.push_back(json_mt_size(len));
}

static void json_cost(yyjson_val *node, int depth, JsonCost &c)
{
	size_t i, n;
	yyjson_val *key, *val;

	if ( depth >= JSON_MAX_DEPTH || node == NULL )
		return;
	if ( yyjson_is_str(node) && json_str_ok(node) )
		json_cost_str(node, c);
	if ( !yyjson_is_ctn(node) )
		return;
	c.vars++;
	if ( yyjson_is_arr(node) )
	{
		yyjson_arr_foreach(node, i, n, val)
		{
			c.vars++;
			json_cost(val, depth + 1, c);
		}
		return;
	}
	yyjson_obj_foreach(node, i, n, key, val)
	{
		if ( !json_str_ok(key) )
			continue;
		c.vars++;
		json_cost(val, depth + 1, c);
		json_cost_str(key, c);
	}
}

// Free list walks, stopped at limit. Free variables chain from entry 0 (AllocVariable):
// https://github.com/voron00/CoD2rev_Server/blob/11c40a5/src/script/scr_variable.cpp#L3824
// Free string slots chain from hashTable[0] (SL_Init, SL_GetStringOfLen):
// https://github.com/voron00/CoD2rev_Server/blob/11c40a5/src/script/scr_stringlist.cpp#L405
// https://github.com/voron00/CoD2rev_Server/blob/11c40a5/src/script/scr_stringlist.cpp#L865
static int json_free_vars(int limit)
{
	int n = 0;
	for ( unsigned int i = scrVarGlob[0].u.next; i && n < limit; i = scrVarGlob[scrVarGlob[i].hash.id].u.next )
		n++;
	return n;
}

static int json_free_strings(int limit)
{
	int n = 0;
	for ( unsigned int i = scrStringGlob.hashTable[0].status_next & HASH_NEXT_MASK; i && n < limit; i = scrStringGlob.hashTable[i].status_next & HASH_NEXT_MASK )
		n++;
	return n;
}

// Counts free blocks per size, then replays the MT_AllocIndex best fit on them:
// https://github.com/voron00/CoD2rev_Server/blob/11c40a5/src/script/scr_memorytree.cpp#L160
static bool json_mt_fits(const std::vector<int> &sizes, int reserve)
{
	int blocks[MT_SIZES], nodes = 0;
	for ( int s = 0; s < MT_SIZES; s++ )
		blocks[s] = MT_GetSubTreeSize(scrMemTreeGlob.head[s]);
	for ( int size : sizes )
	{
		int t = size;
		while ( t < MT_SIZES && blocks[t] == 0 )
			t++;
		if ( t == MT_SIZES )
			return false;
		blocks[t]--;
		while ( --t >= size )
			blocks[t]++;  // split halves stay free
	}
	for ( int s = 0; s < MT_SIZES; s++ )
		nodes += blocks[s] << s;
	return nodes >= reserve;
}

// Pushes the doc root if it fits the caps and the live headroom, else undefined and one error
static void json_push_doc(yyjson_doc *doc, const char *func, const char *what)
{
	yyjson_val *root = yyjson_doc_get_root(doc);
	char why[96] = "";

	{  // containers die here: a script error in the push longjmps past destructors
		JsonCost c;
		json_cost(root, 0, c);
		int strings = (int)c.seen.size(), fresh = (int)c.newSizes.size();

		if ( c.vars > JSON_MAX_VARS )
			snprintf(why, sizeof(why), "%d script variables, cap %d", c.vars, JSON_MAX_VARS);
		else if ( strings > JSON_MAX_STRINGS )
			snprintf(why, sizeof(why), "%d strings, cap %d", strings, JSON_MAX_STRINGS);
		else if ( c.mtNodes > JSON_MAX_MT_NODES )
			snprintf(why, sizeof(why), "%d KB of string memory, cap %d KB", c.mtNodes / 128, JSON_MAX_MT_NODES / 128);
		else if ( json_free_vars(c.vars + JSON_RESERVE_VARS) < c.vars + JSON_RESERVE_VARS )
			snprintf(why, sizeof(why), "%d script variables, too few free", c.vars);
		else if ( json_free_strings(fresh + JSON_RESERVE_STRINGS) < fresh + JSON_RESERVE_STRINGS )
			snprintf(why, sizeof(why), "%d new strings, too few free", fresh);
		else if ( !json_mt_fits(c.newSizes, JSON_RESERVE_MT_NODES) )
			snprintf(why, sizeof(why), "string memory for %d new strings, too little free", fresh);
	}

	if ( why[0] )
	{
		stackError("%s() %s needs %s", func, what, why);
		stackPushUndefined();
		return;
	}
	json_to_gsc_push(root, 0);
}

// ===========================================================================
// GSC -> JSON : walk the engine variable tree and build a yyjson mutable tree.
// ===========================================================================

// Child names: < SL_MAX_STRING_INDEX string key, < MAX_ARRAYINDEX object key, else index + MAX_ARRAYINDEX:
// https://github.com/voron00/CoD2rev_Server/blob/11c40a5/src/script/scr_variable.cpp#L354
struct json_kv
{
	unsigned int name;
	unsigned int id;
};

static int json_kv_cmp(const void *a, const void *b)
{
	unsigned int na = ((const json_kv *)a)->name;
	unsigned int nb = ((const json_kv *)b)->name;

	if ( na < nb )
		return -1;
	if ( na > nb )
		return 1;
	return 0;
}

static yyjson_mut_val * gsc_object_to_json(yyjson_mut_doc *doc, unsigned int objectId, int depth);

// Walk state, main thread only. Shared subtrees and strings are copied per reference,
// so both budgets are needed. A NULL return aborts the whole call.
static const char *json_walk_func;
static long json_walk_nodes;
static long json_walk_bytes;
static int json_walk_max_bytes;
static unsigned int json_walk_ancestors[JSON_MAX_DEPTH];  // reference cycles become null

// Copies s into the doc; NULL once the copied string bytes pass the cap
// ponytail: raw bytes, escapes can grow the written text up to 6x before the caller's exact output check
static yyjson_mut_val * json_walk_str(yyjson_mut_doc *doc, const char *s)
{
	json_walk_bytes -= strlen(s);
	if ( json_walk_bytes < 0 )
	{
		stackError("%s() value has more than %d bytes of strings", json_walk_func, json_walk_max_bytes);
		return NULL;
	}
	return yyjson_mut_strcpy(doc, s);
}

static yyjson_mut_val * gsc_entry_to_json(yyjson_mut_doc *doc, unsigned int id, int depth)
{
	if ( --json_walk_nodes < 0 )
	{
		stackError("%s() value has more than %d entries (too large or cyclic)", json_walk_func, JSON_MAX_NODES);
		return NULL;
	}

	VariableValueInternal *entry = &scrVarGlob[id];
	int type = entry->w.type & VAR_MASK;

	switch ( type )
	{
	case VAR_INTEGER:
		return yyjson_mut_int(doc, entry->u.u.intValue);

	case VAR_FLOAT:
		return yyjson_mut_real(doc, json_double_from_float(entry->u.u.floatValue));

	case VAR_STRING:
	case VAR_ISTRING:
		return json_walk_str(doc, SL_ConvertToString(entry->u.u.stringValue));

	case VAR_VECTOR:
	{
		const float *v = entry->u.u.vectorValue;
		yyjson_mut_val *arr = yyjson_mut_arr(doc);
		if ( v != NULL )
		{
			yyjson_mut_arr_append(arr, yyjson_mut_real(doc, json_double_from_float(v[0])));
			yyjson_mut_arr_append(arr, yyjson_mut_real(doc, json_double_from_float(v[1])));
			yyjson_mut_arr_append(arr, yyjson_mut_real(doc, json_double_from_float(v[2])));
		}
		return arr;
	}

	case VAR_OBJECT:
	case VAR_STRUCT:
	case VAR_ARRAY:
	{
		// Only arrays serialize: struct field names are canonical ids, not script strings
		// (https://github.com/voron00/CoD2rev_Server/blob/11c40a5/src/script/scr_main.cpp#L108)
		unsigned int objId = entry->u.u.pointerValue;
		if ( objId != 0 && (scrVarGlob[objId].w.type & VAR_MASK) == VAR_ARRAY )
			return gsc_object_to_json(doc, objId, depth + 1);
		return yyjson_mut_null(doc);
	}

	default:
		return yyjson_mut_null(doc);  // entities, threads, functions, undefined
	}
}

// All index keys -> sorted JSON array, any string key -> JSON object
static yyjson_mut_val * gsc_object_to_json(yyjson_mut_doc *doc, unsigned int objectId, int depth)
{
	if ( depth >= JSON_MAX_DEPTH || objectId == 0 )
		return yyjson_mut_null(doc);

	for ( int i = 0; i < depth; i++ )
	{
		if ( json_walk_ancestors[i] == objectId )
			return yyjson_mut_null(doc);
	}
	json_walk_ancestors[depth] = objectId;

	// Count by walking the sibling ring; the cap guards a malformed ring
	unsigned int total = 0;
	unsigned int it = objectId;
	while ( total < SL_MAX_STRING_INDEX )
	{
		it = FindNextSibling(it);
		if ( it == 0 )
			break;
		total++;
	}
	if ( total == 0 )
		return yyjson_mut_obj(doc);  // empty -> {} (documented)

	json_kv *items = (json_kv *)malloc(sizeof(json_kv) * total);
	if ( items == NULL )
		return yyjson_mut_null(doc);

	unsigned int count = 0;
	bool hasStringKey = false;
	it = objectId;

	for ( unsigned int i = 0; i < total; i++ )
	{
		it = FindNextSibling(it);
		if ( it == 0 )
			break;

		unsigned int name = GetVariableName(it);
		if ( name >= SL_MAX_STRING_INDEX && name < MAX_ARRAYINDEX )
			continue;  // object keys, and negative indices which encode into this range

		items[count].name = name;
		items[count].id   = it;
		if ( name < SL_MAX_STRING_INDEX )
			hasStringKey = true;
		count++;
	}

	if ( count == 0 )
	{
		free(items);
		return yyjson_mut_obj(doc);
	}

	// Sorting raw names sorts indices, name = index + MAX_ARRAYINDEX
	if ( !hasStringKey )
	{
		qsort(items, count, sizeof(json_kv), json_kv_cmp);

		yyjson_mut_val *arr = yyjson_mut_arr(doc);
		for ( unsigned int i = 0; i < count; i++ )
		{
			yyjson_mut_val *elem = gsc_entry_to_json(doc, items[i].id, depth);
			if ( elem == NULL )
			{
				free(items);
				return NULL;
			}
			yyjson_mut_arr_append(arr, elem);
		}

		free(items);
		return arr;
	}

	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	for ( unsigned int i = 0; i < count; i++ )
	{
		yyjson_mut_val *child = gsc_entry_to_json(doc, items[i].id, depth);
		if ( child == NULL )
		{
			free(items);
			return NULL;
		}

		// Keys are copied, the async worker writes the doc after scripts may free the string
		unsigned int name = items[i].name;
		char keybuf[16];
		const char *keystr = keybuf;
		if ( name < SL_MAX_STRING_INDEX )
			keystr = SL_ConvertToString(name);
		else
			snprintf(keybuf, sizeof(keybuf), "%u", name - MAX_ARRAYINDEX);

		yyjson_mut_val *key = json_walk_str(doc, keystr);
		if ( key == NULL )
		{
			free(items);
			return NULL;
		}
		yyjson_mut_obj_add(obj, key, child);
	}
	free(items);
	return obj;
}

// Top-level param via the stack API; a failed accessor degrades to null
static yyjson_mut_val * gsc_param_to_json(yyjson_mut_doc *doc, int param)
{
	unsigned int objectId;
	if ( stackGetParamObject(param, &objectId) )
	{
		// Also true for structs and entities, only arrays serialize
		if ( Scr_GetPointerType(param) == VAR_ARRAY )
			return gsc_object_to_json(doc, objectId, 0);
		return yyjson_mut_null(doc);
	}

	switch ( stackGetParamType(param) )
	{
	case VAR_INTEGER:
	{
		int v;
		if ( !stackGetParamInt(param, &v) )
			return yyjson_mut_null(doc);
		return yyjson_mut_int(doc, v);
	}

	case VAR_FLOAT:
	{
		float v;
		if ( !stackGetParamFloat(param, &v) )
			return yyjson_mut_null(doc);
		return yyjson_mut_real(doc, json_double_from_float(v));
	}

	case VAR_STRING:
	{
		const char *v = NULL;
		if ( !stackGetParamString(param, &v) || v == NULL )
			return yyjson_mut_null(doc);
		return json_walk_str(doc, v);
	}

	case VAR_ISTRING:
	{
		const char *v = NULL;
		if ( !stackGetParamLocalizedString(param, &v) || v == NULL )
			return yyjson_mut_null(doc);
		return json_walk_str(doc, v);
	}

	case VAR_VECTOR:
	{
		vec3_t v;
		if ( !stackGetParamVector(param, v) )
			return yyjson_mut_null(doc);
		yyjson_mut_val *arr = yyjson_mut_arr(doc);
		yyjson_mut_arr_append(arr, yyjson_mut_real(doc, json_double_from_float(v[0])));
		yyjson_mut_arr_append(arr, yyjson_mut_real(doc, json_double_from_float(v[1])));
		yyjson_mut_arr_append(arr, yyjson_mut_real(doc, json_double_from_float(v[2])));
		return arr;
	}

	default:
		return yyjson_mut_null(doc);
	}
}

// NULL if the walk aborted
static yyjson_mut_doc * gsc_param_to_doc(int param, int maxBytes, const char *func)
{
	yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
	if ( doc == NULL )
		return NULL;

	json_walk_func = func;
	json_walk_nodes = JSON_MAX_NODES;
	json_walk_bytes = json_walk_max_bytes = maxBytes;
	yyjson_mut_val *root = gsc_param_to_json(doc, param);
	if ( root == NULL )
	{
		yyjson_mut_doc_free(doc);
		return NULL;
	}
	yyjson_mut_doc_set_root(doc, root);
	return doc;
}

// Writes and frees the doc. NaN and inf become null, as cJSON did. func NULL = worker thread, no print
static char * json_write_doc(yyjson_mut_doc *doc, int pretty, size_t *len, const char *func)
{
	yyjson_write_flag flags = JSON_WRITE_FLAGS;
	if ( pretty )
		flags |= YYJSON_WRITE_PRETTY;

	yyjson_write_err err;
	char *out = yyjson_mut_write_opts(doc, flags, NULL, len, &err);
	yyjson_mut_doc_free(doc);
	if ( out == NULL && func != NULL )
		stackError("%s() could not write JSON: %s", func, err.msg);
	return out;
}

// func NULL = worker thread, no print
static yyjson_doc * json_read(char *buf, size_t len, const char *func, const char *what)
{
	yyjson_read_err err;
	yyjson_doc *doc = yyjson_read_opts(buf, len, JSON_READ_FLAGS, NULL, &err);
	if ( doc == NULL && func != NULL )
		stackError("%s() invalid JSON in %s at byte %u: %s", func, what, (unsigned)err.pos, err.msg);
	return doc;
}

// ===========================================================================
// Script-facing functions
// ===========================================================================

void gsc_json_parse()
{
	char *str;

	if ( !stackGetParams("s", &str) )
	{
		stackError("gsc_json_parse() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	JsonTimer _t("json_parse", "");
	yyjson_doc *doc = json_read(str, strlen(str), "gsc_json_parse", "input");
	if ( doc == NULL )
	{
		stackPushUndefined();
		return;
	}

	json_push_doc(doc, "gsc_json_parse", "input");
	yyjson_doc_free(doc);
}

void gsc_json_stringify()
{
	if ( Scr_GetNumParam() < 1 )
	{
		stackError("gsc_json_stringify() requires a value argument");
		stackPushUndefined();
		return;
	}

	int pretty = 0;
	if ( Scr_GetNumParam() > 1 )
		pretty = Scr_GetInt(1);

	JsonTimer _t("json_stringify", "");
	yyjson_mut_doc *doc = gsc_param_to_doc(0, JSON_MAX_STRING, "gsc_json_stringify");
	size_t out_len = 0;
	char *out = doc ? json_write_doc(doc, pretty, &out_len, "gsc_json_stringify") : NULL;
	if ( out == NULL )
	{
		stackPushUndefined();
		return;
	}

	if ( out_len >= JSON_MAX_STRING )
	{
		stackError("gsc_json_stringify() result (%u bytes) exceeds the engine %d-byte string limit - use json_save", (unsigned)out_len, JSON_MAX_STRING);
		free(out);
		stackPushUndefined();
		return;
	}

	stackPushString(out);
	free(out);
}

void gsc_json_load()
{
	char *path;

	if ( !stackGetParams("s", &path) )
	{
		stackError("gsc_json_load() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( strlen(path) >= JSON_MAX_PATH )
	{
		stackError("gsc_json_load() path '%s' exceeds %d bytes (engine MAX_QPATH)", path, JSON_MAX_PATH);
		stackPushUndefined();
		return;
	}

	JsonTimer _t("json_load", path);

	fileHandle_t f;
	int len = FS_FOpenFileByMode(path, &f, FS_READ);
	if ( len <= 0 )
	{
		// File missing or empty: quiet undefined (no log spam for try-load).
		// A missing file leaves f == 0, but an existing 0-byte file still has an
		// open handle (only ~50 exist engine-wide) - close it so we don't leak.
		if ( f != 0 )
			FS_FCloseFile(f);
		stackPushUndefined();
		return;
	}

	// Size guard: refuse pathologically large files outright (would block the
	// main thread parsing). Use json_load_async for legitimately large data.
	int maxBytes = dvar_int_or("scr_json_max_load_bytes", JSON_DEF_MAX_LOAD_BYTES);
	if ( maxBytes > 0 && len > maxBytes )
	{
		FS_FCloseFile(f);
		stackError("gsc_json_load() refusing '%s' (%d bytes > scr_json_max_load_bytes %d) - use json_load_async", path, len, maxBytes);
		stackPushUndefined();
		return;
	}

	char *buffer = (char *)malloc(len + 1);
	if ( buffer == NULL )
	{
		FS_FCloseFile(f);
		stackPushUndefined();
		return;
	}

	// Null-terminate at the ACTUAL read length, not the open-time length, so a
	// short read (disk error / file truncated between open and read) doesn't
	// leave uninitialized bytes for the parser to scan past the data.
	int bytesRead = FS_Read(buffer, len, f);
	FS_FCloseFile(f);
	if ( bytesRead < 0 )
		bytesRead = 0;
	if ( bytesRead > len )
		bytesRead = len;
	buffer[bytesRead] = '\0';

	yyjson_doc *doc = json_read(buffer, bytesRead, "gsc_json_load", path);
	free(buffer);
	if ( doc == NULL )
	{
		stackPushUndefined();
		return;
	}

	json_push_doc(doc, "gsc_json_load", path);
	yyjson_doc_free(doc);
}

// "<fs_homepath>/<fs_gamedir>/<rel>", the path FS_FOpenFileWrite uses. Main thread only. Returns malloc'd, NULL on reject
static char * json_os_path(const char *rel)
{
	if ( rel == NULL || rel[0] == '\0' || rel[0] == '/' )
		return NULL;

	dvar_t *fs_home = Dvar_FindVar("fs_homepath");
	if ( fs_home == NULL || fs_home->current.string == NULL || fs_home->current.string[0] == '\0' )
		return NULL;

	// FS_BuildOSPath Sys_Errors past MAX_OSPATH; the margin covers "/" + fs_gamedir + "/" + NUL
	if ( strlen(fs_home->current.string) + strlen(rel) + MAX_QPATH + 3 >= MAX_OSPATH )
		return NULL;

	// An empty game makes FS_BuildOSPath use fs_gamedir
	char osPath[MAX_OSPATH];
	FS_BuildOSPath(fs_home->current.string, "", rel, osPath);
	if ( fs_debug->current.integer )
		Com_Printf("json (fs_homepath) : %s\n", osPath);

	// Same refusal as the engine's FS_CreatePath:
	// https://github.com/voron00/CoD2rev_Server/blob/11c40a5/src/universal/com_files.cpp#L557
	if ( strstr(osPath, "..") != NULL || strstr(osPath, "::") != NULL )
		return NULL;

	return strdup(osPath);
}

// mkdir -p of the parent dirs, like FS_CreatePath. Errors show up at fopen
static void json_mkdir_parents(const char *path)
{
	char buf[MAX_OSPATH + 16];
	snprintf(buf, sizeof(buf), "%s", path);

	for ( char *p = buf + 1; *p != '\0'; p++ )
	{
		if ( *p != '/' )
			continue;
		*p = '\0';
		mkdir(buf, 0755);
		*p = '/';
	}
}

// Writes path.tmpN and renames it over path, so a failed write keeps the old file. Returns 0 or an errno
// ponytail: no fsync, a server crash keeps the page cache; only power loss can lose the last save
static int json_write_file(const char *path, int tmpId, const char *text, size_t len)
{
	char tmp[MAX_OSPATH + 16];
	snprintf(tmp, sizeof(tmp), "%s.tmp%d", path, tmpId);

	json_mkdir_parents(path);
	FILE *f = fopen(tmp, "wb");
	if ( f == NULL )
		return errno;

	int err = 0;
	errno = 0;
	if ( fwrite(text, 1, len, f) != len )
		err = errno ? errno : EIO;

	// A full disk often shows only here, when the buffer is flushed
	if ( fclose(f) != 0 && err == 0 )
		err = errno;

	if ( err == 0 && rename(tmp, path) != 0 )
		err = errno;

	if ( err != 0 )
		unlink(tmp);

	return err;
}

void gsc_json_save()
{
	const char *path;

	if ( !stackGetParamString(0, &path) )
	{
		stackError("gsc_json_save() first argument must be a file path string");
		stackPushInt(0);
		return;
	}

	if ( strlen(path) >= JSON_MAX_PATH )
	{
		stackError("gsc_json_save() path '%s' exceeds %d bytes (engine MAX_QPATH)", path, JSON_MAX_PATH);
		stackPushInt(0);
		return;
	}

	if ( Scr_GetNumParam() < 2 )
	{
		stackError("gsc_json_save() requires a value to save");
		stackPushInt(0);
		return;
	}

	int pretty = 0;
	if ( Scr_GetNumParam() > 2 )
		pretty = Scr_GetInt(2);

	char *osPath = json_os_path(path);
	if ( osPath == NULL )
	{
		stackError("gsc_json_save() invalid path '%s' (must be relative, no '..')", path);
		stackPushInt(0);
		return;
	}

	// Never save a file that json_load would refuse
	JsonTimer _t("json_save", path);
	int maxBytes = dvar_int_or("scr_json_max_load_bytes", JSON_DEF_MAX_LOAD_BYTES);
	yyjson_mut_doc *doc = gsc_param_to_doc(1, maxBytes, "gsc_json_save");
	size_t out_len = 0;
	char *out = doc ? json_write_doc(doc, pretty, &out_len, "gsc_json_save") : NULL;
	if ( out == NULL )
	{
		free(osPath);
		stackPushInt(0);
		return;
	}

	if ( out_len > (size_t)maxBytes )
	{
		stackError("gsc_json_save() output of %u bytes exceeds scr_json_max_load_bytes %d", (unsigned)out_len, maxBytes);
		free(out);
		free(osPath);
		stackPushInt(0);
		return;
	}

	// Async job ids start at 1, so tmp id 0 never collides with a worker
	int err = json_write_file(osPath, 0, out, out_len);
	free(out);
	free(osPath);
	if ( err != 0 )
		stackError("gsc_json_save() could not write '%s': %s", path, strerror(err));

	stackPushInt(err == 0 ? 1 : 0);
}

// ===========================================================================
// Asynchronous API
// ===========================================================================
// Off-loads the slow parts of json_load / json_save to a detached worker
// thread, so big files don't hitch the main script VM. Mirrors the
// poll-and-drain convention used by libcod's mysql_async family.
//
// Lifecycle:
//   1. GSC calls  json_load_async(path)  or  json_save_async(path,value,[pretty])
//      Returns an integer jobId (>= 1) immediately, or 0 if submission failed
//      (bad path, too many in-flight jobs, thread create failure).
//   2. C spawns a DETACHED pthread that does the heavy work:
//        - load: fopen + fread + yyjson_read  (all in the worker)
//        - save: yyjson_mut_write + fopen + fwrite (the mutable doc is built on
//          the main thread first, since reading GSC values needs the main VM --
//          tree-walk is microseconds for typical data; printing and disk I/O
//          are the slow parts and they happen in the worker).
//   3. GSC periodically polls  json_async_done()  -> array of finished jobIds.
//   4. For each finished id, GSC calls  json_async_result(id)  to claim the
//      value (load) or 1/0 success flag (save). Claiming frees the job.
//
// Thread-safety: workers ONLY touch their own job struct (mutex-protected
// status field) and heap data they own. They never touch GSC state. yyjson has
// zero global state on the read path, so concurrent yyjson_read calls across
// worker threads + the main thread are safe. The engine FS_* API is not
// thread-safe, so workers use plain libc fopen/fread/ fwrite against an
// absolute path resolved on the main thread. Paths are sandboxed: must be
// relative, no ".." segments.
//
// Detached threads: workers are PTHREAD_CREATE_DETACHED so the OS reaps
// them; we never pthread_join. State sync happens via the status field
// under json_async_mutex.

#define JSON_ASYNC_KIND_LOAD    0
#define JSON_ASYNC_KIND_SAVE    1
#define JSON_ASYNC_STATUS_PENDING 0
#define JSON_ASYNC_STATUS_DONE    1
#define JSON_ASYNC_STATUS_ERROR   2

struct json_async_job
{
	int    id;
	int    kind;       // KIND_LOAD or KIND_SAVE
	int    status;     // STATUS_PENDING/DONE/ERROR  (mutex-guarded)
	int    max_bytes;  // snapshot of scr_json_max_load_bytes at submit
	char  *abspath;    // resolved absolute path (owned)

	// Load output (filled by worker):
	yyjson_doc *load_doc;  // ownership transferred to caller on json_async_result

	// Save input (built on main thread, consumed by worker):
	yyjson_mut_doc *save_doc;
	int             save_pretty;
	int             save_ok;    // 1 on successful write, 0 otherwise  (mutex-guarded)

	struct json_async_job *next;
};

static pthread_mutex_t json_async_mutex   = PTHREAD_MUTEX_INITIALIZER;
static json_async_job *json_async_jobs    = NULL;
static int             json_async_next_id = 1;
static int             json_async_pending = 0;

// Free everything a job owns. Job must already be unlinked from the list.
static void json_async_free_job(json_async_job *job)
{
	if ( job == NULL ) return;
	if ( job->abspath != NULL ) free(job->abspath);
	if ( job->load_doc != NULL ) yyjson_doc_free(job->load_doc);
	if ( job->save_doc != NULL ) yyjson_mut_doc_free(job->save_doc);
	free(job);
}

// Worker: read the file, parse it, store the yyjson doc.
static void * json_async_load_worker(void *arg)
{
	json_async_job *job = (json_async_job *)arg;

	FILE *f = fopen(job->abspath, "rb");
	if ( f == NULL )
	{
		pthread_mutex_lock(&json_async_mutex);
		job->status = JSON_ASYNC_STATUS_ERROR;
		pthread_mutex_unlock(&json_async_mutex);
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	long len = ftell(f);
	fseek(f, 0, SEEK_SET);

	// Honor scr_json_max_load_bytes snapshotted at submit time; fall back to
	// the i386 hard cap as the upper bound. Operator sets the dvar; we don't
	// read the live dvar here because that would require main-thread
	// serialization and the snapshot is cheap.
	long hard_cap = (long)JSON_HARD_MAX_LOAD_BYTES;
	if ( job->max_bytes > 0 && (long)job->max_bytes < hard_cap )
		hard_cap = (long)job->max_bytes;
	if ( len <= 0 || len > hard_cap )
	{
		fclose(f);
		pthread_mutex_lock(&json_async_mutex);
		job->status = JSON_ASYNC_STATUS_ERROR;
		pthread_mutex_unlock(&json_async_mutex);
		return NULL;
	}

	char *buf = (char *)malloc((size_t)len + 1);
	if ( buf == NULL )
	{
		fclose(f);
		pthread_mutex_lock(&json_async_mutex);
		job->status = JSON_ASYNC_STATUS_ERROR;
		pthread_mutex_unlock(&json_async_mutex);
		return NULL;
	}

	size_t got = fread(buf, 1, (size_t)len, f);
	fclose(f);
	if ( got > (size_t)len ) got = (size_t)len;
	buf[got] = '\0';

	yyjson_doc *parsed = json_read(buf, got, NULL, NULL);
	free(buf);

	// Early reject of docs no push could take
	if ( parsed != NULL && yyjson_doc_get_val_count(parsed) > JSON_MAX_VALUES )
	{
		Com_Printf("json_load_async: %s has %u values, cap %d, job failed\n", job->abspath, (unsigned)yyjson_doc_get_val_count(parsed), JSON_MAX_VALUES);
		yyjson_doc_free(parsed);
		parsed = NULL;
	}

	pthread_mutex_lock(&json_async_mutex);
	if ( parsed == NULL )
	{
		job->status = JSON_ASYNC_STATUS_ERROR;
	}
	else
	{
		job->load_doc = parsed;
		job->status   = JSON_ASYNC_STATUS_DONE;
	}
	pthread_mutex_unlock(&json_async_mutex);
	return NULL;
}

// Worker: write the doc to disk. The job-unique tmp file keeps same-path saves apart
static void * json_async_save_worker(void *arg)
{
	json_async_job *job = (json_async_job *)arg;

	size_t want = 0;
	char *text = json_write_doc(job->save_doc, job->save_pretty, &want, NULL);
	job->save_doc = NULL;

	// Same cap as json_save
	int ok = text != NULL && want <= (size_t)job->max_bytes && json_write_file(job->abspath, job->id, text, want) == 0;
	free(text);

	pthread_mutex_lock(&json_async_mutex);
	job->save_ok = ok;
	job->status  = JSON_ASYNC_STATUS_DONE;
	pthread_mutex_unlock(&json_async_mutex);
	return NULL;
}

// Spawn the worker as a detached thread. Returns 1 on success, 0 on failure.
// On failure the caller is expected to unlink + free the job immediately so
// the submission can fail upfront (returning jobId 0 to GSC) rather than
// surfacing through the poll/drain path.
static int json_async_spawn(json_async_job *job, void *(*worker)(void *))
{
	pthread_t thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	int rc = pthread_create(&thread, &attr, worker, job);
	pthread_attr_destroy(&attr);

	return rc == 0 ? 1 : 0;
}

// Helper used by the *_async submit functions to unwind a job that was linked
// into the global list but whose worker thread could not be created. Removes
// the job from the head of the list (always head, since we prepended it the
// instruction before, and GSC is single-threaded), decrements the pending
// counter, and frees everything the job owned. Safe to call only when no
// worker thread is running for this job.
static void json_async_unlink_head_and_free(json_async_job *job)
{
	pthread_mutex_lock(&json_async_mutex);
	if ( json_async_jobs == job )
		json_async_jobs = job->next;
	if ( json_async_pending > 0 )
		json_async_pending--;
	pthread_mutex_unlock(&json_async_mutex);
	json_async_free_job(job);
}

// Reap finished jobs that were never claimed. A job queued right before a map
// change survives the VM reset, but the script-side jobId does not, so the job
// can never be claimed and would sit in the list forever (counting against
// scr_json_async_max_jobs). Called from custom_SV_SpawnServer on every map
// load, main thread. PENDING jobs still belong to their worker and are left
// alone; once finished they are reaped on the next map load.
void gsc_json_cleanup_on_spawn_server(void)
{
	int reaped = 0;

	pthread_mutex_lock(&json_async_mutex);
	json_async_job **link = &json_async_jobs;
	while ( *link != NULL )
	{
		json_async_job *job = *link;
		if ( job->status != JSON_ASYNC_STATUS_PENDING )
		{
			*link = job->next;
			if ( json_async_pending > 0 )
				json_async_pending--;
			json_async_free_job(job);
			reaped++;
		}
		else
		{
			link = &job->next;
		}
	}
	pthread_mutex_unlock(&json_async_mutex);

	if ( reaped > 0 )
		Com_Printf("json: reaped %d unclaimed async jobs on map change\n", reaped);
}

// json_load_async(path) -> jobId   (0 on submission failure)
void gsc_json_load_async()
{
	char *path;
	if ( !stackGetParams("s", &path) )
	{
		stackError("gsc_json_load_async() requires a file path string");
		stackPushInt(0);
		return;
	}

	if ( strlen(path) >= JSON_MAX_PATH )
	{
		stackError("gsc_json_load_async() path '%s' exceeds %d bytes (engine MAX_QPATH)", path, JSON_MAX_PATH);
		stackPushInt(0);
		return;
	}

	int maxJobs = dvar_int_or("scr_json_async_max_jobs", JSON_DEF_ASYNC_MAX_JOBS);
	if ( json_async_pending >= maxJobs )
	{
		stackError("gsc_json_load_async() too many pending jobs (%d / %d)", json_async_pending, maxJobs);
		stackPushInt(0);
		return;
	}

	char *abs = json_os_path(path);
	if ( abs == NULL )
	{
		stackError("gsc_json_load_async() invalid path '%s' (must be relative, no '..')", path);
		stackPushInt(0);
		return;
	}

	json_async_job *job = (json_async_job *)calloc(1, sizeof(json_async_job));
	if ( job == NULL ) { free(abs); stackPushInt(0); return; }
	job->kind      = JSON_ASYNC_KIND_LOAD;
	job->status    = JSON_ASYNC_STATUS_PENDING;
	job->abspath   = abs;
	job->max_bytes = dvar_int_or("scr_json_max_load_bytes", JSON_DEF_MAX_LOAD_BYTES);

	pthread_mutex_lock(&json_async_mutex);
	job->id   = json_async_next_id++;
	job->next = json_async_jobs;
	json_async_jobs = job;
	json_async_pending++;
	pthread_mutex_unlock(&json_async_mutex);

	if ( !json_async_spawn(job, json_async_load_worker) )
	{
		stackError("gsc_json_load_async() failed to create worker thread");
		json_async_unlink_head_and_free(job);
		stackPushInt(0);
		return;
	}
	stackPushInt(job->id);
}

// json_save_async(path, value, [pretty]) -> jobId   (0 on submission failure)
void gsc_json_save_async()
{
	const char *path;
	if ( !stackGetParamString(0, &path) )
	{
		stackError("gsc_json_save_async() first argument must be a path string");
		stackPushInt(0);
		return;
	}

	if ( strlen(path) >= JSON_MAX_PATH )
	{
		stackError("gsc_json_save_async() path '%s' exceeds %d bytes (engine MAX_QPATH)", path, JSON_MAX_PATH);
		stackPushInt(0);
		return;
	}

	if ( Scr_GetNumParam() < 2 )
	{
		stackError("gsc_json_save_async() requires a value to save");
		stackPushInt(0);
		return;
	}
	int pretty = 0;
	if ( Scr_GetNumParam() > 2 )
		pretty = Scr_GetInt(2);

	int maxJobs = dvar_int_or("scr_json_async_max_jobs", JSON_DEF_ASYNC_MAX_JOBS);
	if ( json_async_pending >= maxJobs )
	{
		stackError("gsc_json_save_async() too many pending jobs (%d / %d)", json_async_pending, maxJobs);
		stackPushInt(0);
		return;
	}

	char *abs = json_os_path(path);
	if ( abs == NULL )
	{
		stackError("gsc_json_save_async() invalid path '%s' (must be relative, no '..')", path);
		stackPushInt(0);
		return;
	}

	// The walk reads script state, so it runs here; the worker writes and frees the doc
	int maxBytes = dvar_int_or("scr_json_max_load_bytes", JSON_DEF_MAX_LOAD_BYTES);
	yyjson_mut_doc *doc = gsc_param_to_doc(1, maxBytes, "gsc_json_save_async");
	if ( doc == NULL )
	{
		free(abs);
		stackPushInt(0);
		return;
	}

	json_async_job *job = (json_async_job *)calloc(1, sizeof(json_async_job));
	if ( job == NULL ) { free(abs); yyjson_mut_doc_free(doc); stackPushInt(0); return; }
	job->kind        = JSON_ASYNC_KIND_SAVE;
	job->status      = JSON_ASYNC_STATUS_PENDING;
	job->abspath     = abs;
	job->max_bytes   = maxBytes;
	job->save_doc    = doc;
	job->save_pretty = pretty;

	pthread_mutex_lock(&json_async_mutex);
	job->id   = json_async_next_id++;
	job->next = json_async_jobs;
	json_async_jobs = job;
	json_async_pending++;
	pthread_mutex_unlock(&json_async_mutex);

	if ( !json_async_spawn(job, json_async_save_worker) )
	{
		stackError("gsc_json_save_async() failed to create worker thread");
		json_async_unlink_head_and_free(job);
		stackPushInt(0);
		return;
	}
	stackPushInt(job->id);
}

// json_async_done() -> int-indexed array of finished jobIds (may be empty)
void gsc_json_async_done()
{
	stackPushArray();

	pthread_mutex_lock(&json_async_mutex);
	for ( json_async_job *j = json_async_jobs; j != NULL; j = j->next )
	{
		if ( j->status != JSON_ASYNC_STATUS_PENDING )
		{
			pthread_mutex_unlock(&json_async_mutex);
			stackPushInt(j->id);
			stackPushArrayLast();
			pthread_mutex_lock(&json_async_mutex);
		}
	}
	pthread_mutex_unlock(&json_async_mutex);
}

// json_async_result(jobId) -> value (load) | 1/0 (save) | undefined (bad id / still pending)
// Successfully claiming a job (status != PENDING) UNLINKS and FREES it.
void gsc_json_async_result()
{
	int jobId;
	if ( !stackGetParams("i", &jobId) )
	{
		stackError("gsc_json_async_result() requires a job id (int)");
		stackPushUndefined();
		return;
	}

	pthread_mutex_lock(&json_async_mutex);

	json_async_job *prev = NULL;
	json_async_job *job  = json_async_jobs;
	while ( job != NULL && job->id != jobId )
	{
		prev = job;
		job  = job->next;
	}

	if ( job == NULL )
	{
		pthread_mutex_unlock(&json_async_mutex);
		stackPushUndefined();   // unknown id
		return;
	}

	if ( job->status == JSON_ASYNC_STATUS_PENDING )
	{
		pthread_mutex_unlock(&json_async_mutex);
		stackPushUndefined();   // not finished yet -- leave in list
		return;
	}

	// Unlink + take ownership.
	if ( prev != NULL ) prev->next = job->next;
	else                json_async_jobs = job->next;
	json_async_pending--;

	int   kind     = job->kind;
	int   status   = job->status;
	yyjson_doc *doc = job->load_doc;
	int   sok      = job->save_ok;
	job->load_doc  = NULL; // taken

	pthread_mutex_unlock(&json_async_mutex);

	// Push outside the mutex
	if ( kind == JSON_ASYNC_KIND_LOAD )
	{
		if ( status == JSON_ASYNC_STATUS_DONE && doc != NULL )
		{
			json_push_doc(doc, "gsc_json_async_result", job->abspath);
			yyjson_doc_free(doc);
		}
		else
		{
			stackPushUndefined();
		}
	}
	else /* SAVE */
	{
		stackPushInt( (status == JSON_ASYNC_STATUS_DONE && sok) ? 1 : 0 );
	}

	json_async_free_job(job);
}

#endif // COMPILE_JSON
