#include "gsc_json.hpp"

#if COMPILE_JSON == 1

#include "lib/yyjson.h"

/* 	Native JSON for GSC on yyjson (doc/script_reference/libcod/json):
	list <-> array indexed 0..n-1, object <-> array with string keys ("5" loads as index 5),
	number <-> int if integral and 32-bit else float, true/false -> 1/0, null <-> undefined.
	Other arrays and structs (spawnStruct, level) serialize as objects, {} if empty.
	Entities, threads and functions serialize as null.
 */
#include <cfloat>
#include <climits>
#include <cmath>
#include <pthread.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <unordered_set>
#include <vector>

extern dvar_t *fs_debug;

#define JSON_MAX_DEPTH 64      // deeper values become undefined or null
#define JSON_MAX_NODES 100000  // save walk budget, a shared subtree counts once per reference
#define JSON_MAX_VALUES 32768  // async load refuses docs with more values
#define JSON_MAX_STRING 65000 // one script string holds < 65531 bytes: https://github.com/voron00/CoD2rev_Server/blob/11c40a5/src/script/scr_memorytree.cpp#L630
#define JSON_MAX_PATH 64 // MAX_QPATH: https://github.com/voron00/CoD2rev_Server/blob/11c40a5/src/universal/q_shared.h#L176

// One load may use at most the cap and must leave the reserve free
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
#define JSON_ASYNC_HELD_LOADS    4                    // async loads in flight or unclaimed: file bytes <= this x scr_json_max_load_bytes
#define JSON_ASYNC_STACK         (256 * 1024)         // worker stack, the 8 MB default costs address space
#define JSON_ASYNC_KEEP_SAVES    1024                 // finished saves kept for json_async_result, older ones are dropped
#define JSON_QUIT_WAIT_MS        5000                 // quit waits this long for running saves

// Array child names: string key, then object key, then index + MAX_ARRAYINDEX (24-bit, negatives included):
// https://github.com/voron00/CoD2rev_Server/blob/11c40a5/src/script/scr_variable.cpp#L354
#define JSON_MIN_INDEX        -0x7E0002  // 1.3 IsValidArrayIndex: index + 0x7E0002 <= 0xFE0001
#define JSON_MAX_INDEX        0x7FFFFF
#define JSON_FIRST_INDEX_NAME (JSON_MIN_INDEX + MAX_ARRAYINDEX)  // 0x1FFFE

// long long: on i386 a 32-bit long overflows tv_sec * 1000 after ~24.8 days
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

	// Read the union field of the real type, setCvar can make a string dvar
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

// Shortest double for a float, so 0.1f writes as 0.1, not 0.10000000149011612
static double json_double_from_float(float f)
{
	char buf[32];
	// 9 digits always read back to the same float; fewer often do
	for ( int digits = 6; digits <= 9; digits++ )
	{
		snprintf(buf, sizeof(buf), "%.*g", digits, (double)f);
		if ( strtof(buf, NULL) == f )
			break;
	}
	return strtod(buf, NULL);
}

// Called once from custom_Com_InitDvars
void gsc_json_register_dvars(void)
{
	Dvar_RegisterInt("scr_json_max_load_bytes", JSON_DEF_MAX_LOAD_BYTES, 1, JSON_HARD_MAX_LOAD_BYTES, DVAR_ARCHIVE);
	Dvar_RegisterInt("scr_json_slow_warn_ms", JSON_DEF_SLOW_WARN_MS, 1, 60000, DVAR_ARCHIVE);
	Dvar_RegisterInt("scr_json_async_max_jobs", JSON_DEF_ASYNC_MAX_JOBS, 1, 256, DVAR_ARCHIVE);
}

// Prints one line on scope exit if the call took longer than scr_json_slow_warn_ms
// Copies detail: the result push can free a script-built path string
class JsonTimer
{
	const char *func;
	char detail[JSON_MAX_PATH];
	long long t0;
public:
	JsonTimer(const char *f, const char *d) : func(f), t0(now_ms()) { snprintf(detail, sizeof(detail), "%s", d ? d : ""); }
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

// A key written like an array index ("7", "-1": no leading zeros or "+") loads as that index,
// so arrays saved as objects come back unchanged
static bool json_key_index(yyjson_val *key, int *index)
{
	const char *s = yyjson_get_str(key);
	long v = strtol(s, NULL, 10);
	char canonical[16];
	snprintf(canonical, sizeof(canonical), "%ld", v);
	if ( v < JSON_MIN_INDEX || v > JSON_MAX_INDEX || strcmp(canonical, s) != 0 )
		return false;
	*index = (int)v;
	return true;
}

// Values the push replaced with undefined and keys it skipped, reported once per call
static int json_lost_values;
static int json_lost_keys;

// Pushes one value onto the script stack; json_cost() must mirror every branch
static void json_to_gsc_push(yyjson_val *node, int depth)
{
	size_t i, n;
	yyjson_val *key, *val;

	if ( node == NULL || yyjson_is_null(node) )
		stackPushUndefined();
	else if ( depth >= JSON_MAX_DEPTH )
	{
		json_lost_values++;
		stackPushUndefined();
	}
	else if ( yyjson_is_bool(node) )
		stackPushInt(yyjson_is_true(node));
	else if ( yyjson_is_sint(node) && yyjson_get_sint(node) >= INT_MIN && yyjson_get_sint(node) <= INT_MAX )
		stackPushInt((int)yyjson_get_sint(node));
	else if ( yyjson_is_uint(node) && yyjson_get_uint(node) <= INT_MAX )
		stackPushInt((int)yyjson_get_uint(node));
	else if ( yyjson_is_num(node) && fabs(yyjson_get_num(node)) <= FLT_MAX )
		stackPushFloat((float)yyjson_get_num(node));  // real, or int beyond 32 bits
	else if ( yyjson_is_str(node) && json_str_ok(node) )
		stackPushString(yyjson_get_str(node));
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
		unsigned int arrayId = scrVmPub.top->u.pointerValue;
		yyjson_obj_foreach(node, i, n, key, val)
		{
			if ( !json_str_ok(key) )
			{
				json_lost_keys++;
				continue;
			}

			int index;
			unsigned int name;
			if ( json_key_index(key, &index) )
				name = (index + MAX_ARRAYINDEX) & 0xFFFFFF;  // VAR_NAME_LOW_MASK
			else
				name = SL_GetString(yyjson_get_str(key), 0);

			// A repeated key replaces the earlier one, like JSON.parse; the engine never checks
			if ( FindVariable(arrayId, name) )
				RemoveVariable(arrayId, name);

			json_to_gsc_push(val, depth + 1);
			Scr_AddArrayStringIndexed(name);
			if ( name < SL_MAX_STRING_INDEX )
				SL_RemoveRefToString(name);  // the array holds its own ref
		}
	}
	else
	{
		json_lost_values++;  // string over 64 KB or with a NUL, number beyond float range
		stackPushUndefined();
	}
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
		int index;
		if ( !json_str_ok(key) )
			continue;
		c.vars++;
		json_cost(val, depth + 1, c);
		if ( !json_key_index(key, &index) )
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

	json_lost_values = json_lost_keys = 0;
	json_to_gsc_push(root, 0);
	if ( json_lost_values || json_lost_keys )
		Com_Printf("%s() %s: %d values became undefined and %d keys were skipped (nested deeper than %d, string over %d bytes or with a NUL, number beyond float range)\n",
			func, what, json_lost_values, json_lost_keys, JSON_MAX_DEPTH, JSON_MAX_STRING);
}

// ===========================================================================
// GSC -> JSON
// ===========================================================================

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
// so both budgets are needed. A NULL return aborts the whole call
static const char *json_walk_func;
static long json_walk_nodes;
static long json_walk_bytes;
static int json_walk_max_bytes;
static unsigned int json_walk_ancestors[JSON_MAX_DEPTH];
static int json_walk_nulled;  // arrays or structs too deep or in a reference cycle, saved as null

// Struct field names are canonical ids, mapped to names only while scripts load:
// https://github.com/voron00/CoD2rev_Server/blob/11c40a5/src/script/scr_main.cpp#L54
static const char **json_field_names;  // canonical id -> name
static unsigned int json_field_count;

// Copies the names, the engine frees the map and unused strings right after
void gsc_json_keep_field_names(void)
{
	const uint16_t *canonical = scrCompilePub.canonicalStrings;
	if ( canonical == NULL )
		return;

	size_t bytes = 0;
	for ( unsigned int s = 1; s < SL_MAX_STRING_INDEX; s++ )
	{
		if ( canonical[s] )
			bytes += strlen(SL_ConvertToString(s)) + 1;
	}

	free(json_field_names);
	json_field_count = scrVarPub.canonicalStrCount;
	json_field_names = (const char **)calloc(1, (json_field_count + 1) * sizeof(char *) + bytes);
	if ( json_field_names == NULL )
	{
		json_field_count = 0;
		return;
	}

	char *pool = (char *)(json_field_names + json_field_count + 1);
	for ( unsigned int s = 1; s < SL_MAX_STRING_INDEX; s++ )
	{
		if ( canonical[s] )
		{
			json_field_names[canonical[s]] = pool;
			pool = stpcpy(pool, SL_ConvertToString(s)) + 1;
		}
	}
}

// Copies s into the doc; NULL once the copied string bytes pass the cap
// Counts raw bytes, escapes can make the output up to 6x larger; the caller checks the real size
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
		stackError("%s() value has more than %d entries (a shared array counts at every reference)", json_walk_func, JSON_MAX_NODES);
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
		// Arrays and structs serialize, entities do not
		unsigned int objId = entry->u.u.pointerValue;
		int objType = objId != 0 ? scrVarGlob[objId].w.type & VAR_MASK : VAR_UNDEFINED;
		if ( objType == VAR_ARRAY || objType == VAR_STRUCT )
			return gsc_object_to_json(doc, objId, depth + 1);
		return yyjson_mut_null(doc);
	}

	default:
		return yyjson_mut_null(doc);  // entities, threads, functions, undefined
	}
}

// Indices 0..n-1 -> JSON array, other indices -> object with sorted "%d" keys,
// any string key or a struct -> object in insertion order
static yyjson_mut_val * gsc_object_to_json(yyjson_mut_doc *doc, unsigned int objectId, int depth)
{
	if ( depth >= JSON_MAX_DEPTH || objectId == 0 )
	{
		json_walk_nulled++;
		return yyjson_mut_null(doc);
	}

	for ( int i = 0; i < depth; i++ )
	{
		if ( json_walk_ancestors[i] == objectId )
		{
			json_walk_nulled++;
			return yyjson_mut_null(doc);
		}
	}
	json_walk_ancestors[depth] = objectId;
	bool isStruct = (scrVarGlob[objectId].w.type & VAR_MASK) == VAR_STRUCT;

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
		return NULL;

	unsigned int count = 0;
	bool hasStringKey = false;
	it = objectId;

	for ( unsigned int i = 0; i < total; i++ )
	{
		it = FindNextSibling(it);
		if ( it == 0 )
			break;

		unsigned int name = GetVariableName(it);
		if ( isStruct ? (name == 0 || name > json_field_count) : (name >= SL_MAX_STRING_INDEX && name < JSON_FIRST_INDEX_NAME) )
			continue;  // struct notify list, array object keys

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

	// Sorting raw names sorts indices, negatives first
	bool isArray = !hasStringKey;
	if ( !hasStringKey )
		qsort(items, count, sizeof(json_kv), json_kv_cmp);
	for ( unsigned int i = 0; i < count && isArray; i++ )
		isArray = items[i].name == MAX_ARRAYINDEX + i;

	if ( isArray )
	{
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

	// New children are linked at the head of the list, so string keys walk backwards
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	for ( unsigned int i = 0; i < count; i++ )
	{
		json_kv *kv = hasStringKey ? &items[count - 1 - i] : &items[i];
		yyjson_mut_val *child = gsc_entry_to_json(doc, kv->id, depth);
		if ( child == NULL )
		{
			free(items);
			return NULL;
		}

		// Keys are copied, the async worker writes the doc after scripts may free the string
		char keybuf[16];
		const char *keystr = keybuf;
		if ( isStruct )
			keystr = json_field_names[kv->name];
		else if ( kv->name < SL_MAX_STRING_INDEX )
			keystr = SL_ConvertToString(kv->name);
		else
			snprintf(keybuf, sizeof(keybuf), "%d", (int)kv->name - MAX_ARRAYINDEX);

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
		// Also true for entities, which serialize as null
		int type = Scr_GetPointerType(param);
		if ( type == VAR_ARRAY || type == VAR_STRUCT )
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

// NULL if the walk aborted, with one error printed
static yyjson_mut_doc * gsc_param_to_doc(int param, int maxBytes, const char *func)
{
	json_walk_func = func;
	json_walk_nodes = JSON_MAX_NODES;
	json_walk_bytes = json_walk_max_bytes = maxBytes;
	json_walk_nulled = 0;

	yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
	yyjson_mut_val *root = doc ? gsc_param_to_json(doc, param) : NULL;
	if ( root == NULL )
	{
		if ( json_walk_nodes >= 0 && json_walk_bytes >= 0 )
			stackError("%s() out of memory", func);  // a budget prints its own
		yyjson_mut_doc_free(doc);
		return NULL;
	}
	yyjson_mut_doc_set_root(doc, root);

	if ( json_walk_nulled )
		Com_Printf("%s() %d arrays or structs were saved as null (nested deeper than %d or inside themselves)\n", func, json_walk_nulled, JSON_MAX_DEPTH);
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

static yyjson_doc * json_read(char *buf, size_t len, const char *func, const char *what)
{
	yyjson_read_err err;
	yyjson_doc *doc = yyjson_read_opts(buf, len, JSON_READ_FLAGS, NULL, &err);
	if ( doc == NULL )
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

static char * json_os_path(const char *rel, const char *func);

void gsc_json_load()
{
	char *path;

	if ( !stackGetParams("s", &path) )
	{
		stackError("gsc_json_load() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	// Same folder as the saves and json_load_async, not the iwd search path
	char *abs = json_os_path(path, "gsc_json_load");
	if ( abs == NULL )
	{
		stackPushUndefined();
		return;
	}

	JsonTimer _t("json_load", path);

	// Missing or empty file: quiet undefined, so scripts can try-load
	struct stat st;
	FILE *f = stat(abs, &st) == 0 && S_ISREG(st.st_mode) && st.st_size > 0 ? fopen(abs, "rb") : NULL;
	free(abs);
	if ( f == NULL )
	{
		stackPushUndefined();
		return;
	}

	// Same size cap as json_load_async and the saves
	int maxBytes = dvar_int_or("scr_json_max_load_bytes", JSON_DEF_MAX_LOAD_BYTES);
	if ( st.st_size > maxBytes )
	{
		fclose(f);
		stackError("gsc_json_load() refusing '%s' (%lld bytes > scr_json_max_load_bytes %d)", path, (long long)st.st_size, maxBytes);
		stackPushUndefined();
		return;
	}

	size_t len = (size_t)st.st_size;
	char *buffer = (char *)malloc(len + 1);
	if ( buffer == NULL )
	{
		fclose(f);
		stackPushUndefined();
		return;
	}

	// A short read parses only what was read
	size_t bytesRead = fread(buffer, 1, len, f);
	fclose(f);
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

// "<fs_homepath>/<fs_gamedir>/<rel>", the path FS_FOpenFileWrite uses. Main thread only.
// Returns malloc'd, or NULL after printing why
static char * json_os_path(const char *rel, const char *func)
{
	if ( strlen(rel) >= JSON_MAX_PATH )
	{
		// Clipped, or a long path pushes the reason past the error buffer
		const char *more = strlen(rel) > JSON_MAX_PATH ? "..." : "";
		stackError("%s() path '%.*s%s' exceeds %d bytes (engine MAX_QPATH)", func, JSON_MAX_PATH, rel, more, JSON_MAX_PATH);
		return NULL;
	}

	dvar_t *fs_home = Dvar_FindVar("fs_homepath");
	if ( fs_home == NULL || fs_home->current.string == NULL || fs_home->current.string[0] == '\0' )
	{
		stackError("%s() fs_homepath is not set", func);
		return NULL;
	}

	// FS_BuildOSPath Sys_Errors past MAX_OSPATH; the margin covers "/" + fs_gamedir + "/" + NUL
	if ( strlen(fs_home->current.string) + strlen(rel) + MAX_QPATH + 3 >= MAX_OSPATH )
	{
		stackError("%s() fs_homepath '%s' is too long", func, fs_home->current.string);
		return NULL;
	}

	// An empty game makes FS_BuildOSPath use fs_gamedir
	char osPath[MAX_OSPATH];
	FS_BuildOSPath(fs_home->current.string, "", rel, osPath);
	if ( fs_debug->current.integer )
		Com_Printf("json (fs_homepath) : %s\n", osPath);

	// Same refusal as the engine's FS_CreatePath:
	// https://github.com/voron00/CoD2rev_Server/blob/11c40a5/src/universal/com_files.cpp#L557
	if ( rel[0] == '\0' || rel[0] == '/' || strstr(osPath, "..") != NULL || strstr(osPath, "::") != NULL )
	{
		stackError("%s() invalid path '%s' (must be relative, no '..')", func, rel);
		return NULL;
	}

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

static int json_save_rename(const char *tmp, const char *path, int id);

// Writes path.tmp<pid>_<id> and renames it over path, so a failed write keeps the old file. Returns 0 or an errno
// No fsync: a server crash keeps the data, only power loss can lose the last save
static int json_write_file(const char *path, int tmpId, const char *text, size_t len)
{
	char tmp[MAX_OSPATH + 32];
	snprintf(tmp, sizeof(tmp), "%s.tmp%d_%d", path, (int)getpid(), tmpId);

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

	if ( err == 0 && json_save_rename(tmp, path, tmpId) != 0 )
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

	if ( Scr_GetNumParam() < 2 )
	{
		stackError("gsc_json_save() requires a value to save");
		stackPushInt(0);
		return;
	}

	int pretty = 0;
	if ( Scr_GetNumParam() > 2 )
		pretty = Scr_GetInt(2);

	char *osPath = json_os_path(path, "gsc_json_save");
	if ( osPath == NULL )
	{
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
// Detached workers read and parse, or write, off the main thread. GSC polls
// json_async_done() and claims with json_async_result(), which frees the job.
// Workers never touch script state (a save doc is built on the main thread) and
// use libc file calls, the engine FS is not thread-safe.

#define JSON_ASYNC_KIND_LOAD    0
#define JSON_ASYNC_KIND_SAVE    1
#define JSON_ASYNC_STATUS_PENDING 0
#define JSON_ASYNC_STATUS_DONE    1
#define JSON_ASYNC_STATUS_ERROR   2

struct json_async_job
{
	int    id;
	int    kind;
	int    status;     // mutex-guarded
	int    max_bytes;  // snapshot of scr_json_max_load_bytes at submit
	char  *abspath;

	yyjson_doc     *load_doc;   // load result, taken by json_async_result
	yyjson_mut_doc *save_doc;
	int             save_pretty;
	int             save_ok;    // mutex-guarded
	bool            superseded; // a newer save of the path is on disk, mutex-guarded

	long long bytes;     // load: file size at submit
	int       err;       // errno of a failed read or write
	char      why[96];   // failure printed on claim, empty = quiet like json_load

	struct json_async_job *next;
};

static pthread_mutex_t json_async_mutex   = PTHREAD_MUTEX_INITIALIZER;
static json_async_job *json_async_jobs    = NULL;
static int             json_async_next_id = 1;

// The job must be unlinked already
static void json_async_free_job(json_async_job *job)
{
	if ( job == NULL ) return;
	if ( job->abspath != NULL ) free(job->abspath);
	if ( job->load_doc != NULL ) yyjson_doc_free(job->load_doc);
	if ( job->save_doc != NULL ) yyjson_mut_doc_free(job->save_doc);
	free(job);
}

// Moves a save into place. Older pending saves of the same path skip their rename and still
// return 1, newer data is already on disk. id 0 = json_save, newer than every job.
// Paths compare as text, "a//b.json" and "a/b.json" are different
static int json_save_rename(const char *tmp, const char *path, int id)
{
	int rc = 0;
	pthread_mutex_lock(&json_async_mutex);
	json_async_job *self = json_async_jobs;
	while ( self != NULL && self->id != id )
		self = self->next;

	if ( self != NULL && self->superseded )
		unlink(tmp);
	else if ( (rc = rename(tmp, path)) == 0 )
	{
		for ( json_async_job *j = json_async_jobs; j != NULL; j = j->next )
		{
			if ( j->kind == JSON_ASYNC_KIND_SAVE && j->status == JSON_ASYNC_STATUS_PENDING && (id == 0 || j->id < id) && strcmp(j->abspath, path) == 0 )
				j->superseded = true;
		}
	}
	pthread_mutex_unlock(&json_async_mutex);
	return rc;
}

// Caps running jobs plus finished loads not claimed yet (they hold their doc), and the file
// bytes those loads hold. Finished saves hold nothing. loadBytes -1 for a save
static bool json_async_full(const char *func, long long loadBytes)
{
	int maxJobs = dvar_int_or("scr_json_async_max_jobs", JSON_DEF_ASYNC_MAX_JOBS);
	long long maxHeld = (long long)JSON_ASYNC_HELD_LOADS * dvar_int_or("scr_json_max_load_bytes", JSON_DEF_MAX_LOAD_BYTES);
	int jobs = 0;
	long long held = 0;

	pthread_mutex_lock(&json_async_mutex);
	for ( json_async_job *j = json_async_jobs; j != NULL; j = j->next )
	{
		if ( j->status == JSON_ASYNC_STATUS_PENDING || j->load_doc != NULL )
		{
			jobs++;
			held += j->bytes;
		}
	}
	pthread_mutex_unlock(&json_async_mutex);

	if ( jobs >= maxJobs )
		stackError("%s() too many jobs running or unclaimed (%d / %d)", func, jobs, maxJobs);
	else if ( loadBytes >= 0 && held + loadBytes > maxHeld )
		stackError("%s() loads running or unclaimed hold %lld bytes, cap %lld (%d x scr_json_max_load_bytes)", func, held, maxHeld, JSON_ASYNC_HELD_LOADS);
	else
		return false;
	return true;
}

// Worker: read the file, parse it, keep the doc for json_async_result
static void * json_async_load_worker(void *arg)
{
	json_async_job *job = (json_async_job *)arg;

	FILE *f = fopen(job->abspath, "rb");
	if ( f == NULL )
	{
		if ( errno != ENOENT )
		{
			job->err = errno;
			snprintf(job->why, sizeof(job->why), "could not read: ");
		}
		pthread_mutex_lock(&json_async_mutex);
		job->status = JSON_ASYNC_STATUS_ERROR;
		pthread_mutex_unlock(&json_async_mutex);
		return NULL;
	}

	// fopen also opens a directory, so not a regular file reads as missing, like json_load
	struct stat st;
	long len = 0;
	if ( fstat(fileno(f), &st) == 0 && S_ISREG(st.st_mode) )
		len = (long)st.st_size;

	// The dvar as read at submit, the hard cap guards a bad value
	long hard_cap = (long)JSON_HARD_MAX_LOAD_BYTES;
	if ( job->max_bytes > 0 && (long)job->max_bytes < hard_cap )
		hard_cap = (long)job->max_bytes;
	if ( len <= 0 || len > hard_cap )
	{
		if ( len > hard_cap )
			snprintf(job->why, sizeof(job->why), "has %ld bytes, more than scr_json_max_load_bytes %ld", len, hard_cap);
		fclose(f);
		pthread_mutex_lock(&json_async_mutex);
		job->status = JSON_ASYNC_STATUS_ERROR;
		pthread_mutex_unlock(&json_async_mutex);
		return NULL;
	}

	char *buf = (char *)malloc((size_t)len + 1);
	if ( buf == NULL )
	{
		snprintf(job->why, sizeof(job->why), "out of memory");
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

	yyjson_read_err err;
	yyjson_doc *parsed = yyjson_read_opts(buf, got, JSON_READ_FLAGS, NULL, &err);
	free(buf);
	if ( parsed == NULL )
		snprintf(job->why, sizeof(job->why), "invalid JSON at byte %u: %s", (unsigned)err.pos, err.msg);

	// Early reject of docs no push could take
	if ( parsed != NULL && yyjson_doc_get_val_count(parsed) > JSON_MAX_VALUES )
	{
		snprintf(job->why, sizeof(job->why), "has %u values, more than %d", (unsigned)yyjson_doc_get_val_count(parsed), JSON_MAX_VALUES);
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
	if ( text == NULL )
		snprintf(job->why, sizeof(job->why), "could not write JSON");
	else if ( want > (size_t)job->max_bytes )
		snprintf(job->why, sizeof(job->why), "output of %u bytes exceeds scr_json_max_load_bytes %d", (unsigned)want, job->max_bytes);
	else if ( (job->err = json_write_file(job->abspath, job->id, text, want)) != 0 )
		snprintf(job->why, sizeof(job->why), "could not write: ");
	int ok = job->why[0] == '\0';
	free(text);

	pthread_mutex_lock(&json_async_mutex);
	job->save_ok = ok;
	job->status  = JSON_ASYNC_STATUS_DONE;
	pthread_mutex_unlock(&json_async_mutex);
	return NULL;
}

// Returns 1 if the worker thread started
static int json_async_spawn(json_async_job *job, void *(*worker)(void *))
{
	pthread_t thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr, JSON_ASYNC_STACK);

	int rc = pthread_create(&thread, &attr, worker, job);
	pthread_attr_destroy(&attr);

	return rc == 0 ? 1 : 0;
}

// Undoes a submit whose worker did not start, the job is still the list head
static void json_async_unlink_head_and_free(json_async_job *job)
{
	pthread_mutex_lock(&json_async_mutex);
	if ( json_async_jobs == job )
		json_async_jobs = job->next;
	pthread_mutex_unlock(&json_async_mutex);
	json_async_free_job(job);
}

// Every level load (fast_restart too) frees finished jobs, scripts lost their ids.
// Running jobs belong to their worker and are freed at the next load
void gsc_json_cleanup_on_level_load(void)
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
		Com_Printf("json: reaped %d unclaimed async jobs on level load\n", reaped);
}

/*
 * Keeps the newest JSON_ASYNC_KEEP_SAVES finished saves. Saves are often never claimed,
 * so without this a long level grows the job list and json_async_done without bound.
 * A dropped save that failed still prints why, once.
 */
static void json_async_trim_saves(void)
{
	int kept = 0;

	pthread_mutex_lock(&json_async_mutex);
	json_async_job **link = &json_async_jobs;
	while ( *link != NULL )
	{
		json_async_job *job = *link;
		bool finishedSave = job->kind == JSON_ASYNC_KIND_SAVE && job->status != JSON_ASYNC_STATUS_PENDING;
		if ( finishedSave && ++kept > JSON_ASYNC_KEEP_SAVES )
		{
			if ( job->why[0] != '\0' )
				Com_Printf("json: unclaimed save '%s' failed: %s%s\n", job->abspath, job->why, job->err ? strerror(job->err) : "");
			*link = job->next;
			json_async_free_job(job);
		}
		else
		{
			link = &job->next;
		}
	}
	pthread_mutex_unlock(&json_async_mutex);
}

// Server quit: running saves finish writing first, exit() would kill their threads mid-write
void gsc_json_shutdown(void)
{
	long long end = now_ms() + JSON_QUIT_WAIT_MS;
	int running;

	do
	{
		running = 0;
		pthread_mutex_lock(&json_async_mutex);
		for ( json_async_job *j = json_async_jobs; j != NULL; j = j->next )
		{
			if ( j->kind == JSON_ASYNC_KIND_SAVE && j->status == JSON_ASYNC_STATUS_PENDING )
				running++;
		}
		pthread_mutex_unlock(&json_async_mutex);
		if ( running > 0 )
			usleep(10000);
	} while ( running > 0 && now_ms() < end );

	if ( running > 0 )
		Com_Printf("json: quit with %d saves still writing, the old files stay\n", running);
}

// json_load_async(path) -> job id, 0 if refused
void gsc_json_load_async()
{
	char *path;
	if ( !stackGetParams("s", &path) )
	{
		stackError("gsc_json_load_async() requires a file path string");
		stackPushInt(0);
		return;
	}

	char *abs = json_os_path(path, "gsc_json_load_async");
	if ( abs == NULL )
	{
		stackPushInt(0);
		return;
	}

	// Refuse now what the worker would refuse; a missing file fails quietly there
	int maxBytes = dvar_int_or("scr_json_max_load_bytes", JSON_DEF_MAX_LOAD_BYTES);
	struct stat st;
	long long bytes = stat(abs, &st) == 0 ? (long long)st.st_size : 0;
	if ( bytes > maxBytes )
	{
		stackError("gsc_json_load_async() refusing '%s' (%lld bytes > scr_json_max_load_bytes %d)", path, bytes, maxBytes);
		free(abs);
		stackPushInt(0);
		return;
	}

	if ( json_async_full("gsc_json_load_async", bytes) )
	{
		free(abs);
		stackPushInt(0);
		return;
	}

	json_async_job *job = (json_async_job *)calloc(1, sizeof(json_async_job));
	if ( job == NULL ) { free(abs); stackPushInt(0); return; }
	job->kind      = JSON_ASYNC_KIND_LOAD;
	job->status    = JSON_ASYNC_STATUS_PENDING;
	job->abspath   = abs;
	job->max_bytes = maxBytes;
	job->bytes     = bytes;

	pthread_mutex_lock(&json_async_mutex);
	job->id   = json_async_next_id++;
	job->next = json_async_jobs;
	json_async_jobs = job;
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

// json_save_async(path, value, [pretty]) -> job id, 0 if refused
void gsc_json_save_async()
{
	const char *path;
	if ( !stackGetParamString(0, &path) )
	{
		stackError("gsc_json_save_async() first argument must be a path string");
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

	json_async_trim_saves();
	if ( json_async_full("gsc_json_save_async", -1) )
	{
		stackPushInt(0);
		return;
	}

	char *abs = json_os_path(path, "gsc_json_save_async");
	if ( abs == NULL )
	{
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

// json_async_done() -> list of finished job ids, may be empty
void gsc_json_async_done()
{
	json_async_trim_saves();
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

// json_async_result(id) -> load data, 1/0 for a save, undefined if unknown or still running.
// Claiming a finished job frees it
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
		stackPushUndefined();   // still running, stays listed
		return;
	}

	// Unlink and take the job
	if ( prev != NULL ) prev->next = job->next;
	else                json_async_jobs = job->next;

	int   kind     = job->kind;
	int   status   = job->status;
	yyjson_doc *doc = job->load_doc;
	int   sok      = job->save_ok;
	job->load_doc  = NULL;

	pthread_mutex_unlock(&json_async_mutex);

	if ( job->why[0] != '\0' )
		stackError("gsc_json_async_result() '%s' %s%s", job->abspath, job->why, job->err ? strerror(job->err) : "");

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
	else
	{
		stackPushInt( (status == JSON_ASYNC_STATUS_DONE && sok) ? 1 : 0 );
	}

	json_async_free_job(job);
}

#endif // COMPILE_JSON
