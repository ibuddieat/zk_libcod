#include "gsc_json.hpp"

#if COMPILE_JSON == 1

#include "lib/yyjson.h"

/*
 * Native JSON support for GSC, backed by yyjson (code/lib/yyjson.[ch]).
 *
 * Value mapping (see doc/added_script_functions.md):
 *   JSON object   <-> GSC struct (string-keyed array)
 *   JSON array    <-> GSC array  (integer-indexed)
 *   JSON string   <-> GSC string
 *   JSON number   <-> GSC int (when integral and 32-bit) else float
 *   JSON true/false -> GSC 1 / 0      (GSC has no bool type)
 *   JSON null     <-> GSC undefined
 *
 * GSC arrays and structs are the same underlying associative array, so on
 * serialize we decide array-vs-object from the key types: an array iff every
 * key is an integer index. Empty arrays serialize as {} (documented default).
 *
 * Engine internals are read exactly as the reverse-engineered server does
 * (Refrences/CoD2rev_Server/src/script/scr_variable.cpp): for a variable id,
 * scrVarGlob[id].w.type & VAR_MASK is the type and scrVarGlob[id].u.u is the
 * value union. Key encoding (scr_variable.cpp): name < SL_MAX_STRING_INDEX is a
 * string key; name >= MAX_ARRAYINDEX is an integer index (index = name -
 * MAX_ARRAYINDEX). The live gsc_utils_getarraykeys() uses the same iteration.
 *
 * Backend swap (cJSON -> yyjson, audit 2026-06): yyjson has no mutable parser
 * globals (cJSON's global_error / global_hooks raced with our async workers),
 * tracks string length natively (so embedded NULs are detected on push, not
 * silently truncated), and parses 5-10x faster on adversarial inputs.
 */

// VAR_MASK / SL_MAX_STRING_INDEX / MAX_ARRAYINDEX come from declarations.hpp
// (engine variable-system constants).

#include <climits>
#include <pthread.h>

#define JSON_MAX_DEPTH 64

// The engine's script string allocator (MT_AllocIndex / MEMORY_NODE_COUNT) caps a
// single string at 64 KB; handing it a larger one triggers Scr_TerminalError and
// drops the server. Guard every string we push to the VM (parsed values and the
// stringify result) under that ceiling. File writes (json_save) are unaffected.
#define JSON_MAX_STRING 65000

// Engine MAX_QPATH = 64. Path strings passed to FS_FOpen* longer than this are
// either truncated by the FS layer (silent data loss) or trip the BG sanitizer.
// Refuse with a clear error before we ever hit the FS.
#define JSON_MAX_PATH 64

// ===========================================================================
// Production hardening helpers
// ===========================================================================
// Tunable via dvars set from cfg or the console (no registerCvar needed; we
// just look them up each call and use the documented defaults if absent):
//   scr_json_max_load_bytes   default 8 MB   (json_load refuses larger files)
//   scr_json_slow_warn_ms     default 25 ms  (any sync json_* slower logs WARN)
//   scr_json_async_max_jobs   default 64     (async submission cap)

static long now_ms()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec * 1000L + ts.tv_nsec / 1000000L;
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

// RAII timer: logs a single WARN line on scope exit if the elapsed wall time
// exceeded scr_json_slow_warn_ms. Cheap (one clock_gettime at ctor/dtor).
class JsonTimer
{
	const char *func;
	const char *detail;
	long t0;
public:
	JsonTimer(const char *f, const char *d) : func(f), detail(d ? d : ""), t0(now_ms()) {}
	~JsonTimer()
	{
		int threshold = dvar_int_or("scr_json_slow_warn_ms", 25);
		long elapsed = now_ms() - t0;
		if ( threshold > 0 && elapsed > threshold )
			Com_Printf("[JSON] WARN: %s took %ld ms (%s)\n", func, elapsed, detail);
	}
};

// ===========================================================================
// JSON -> GSC : push the converted value onto the script (out-param) stack.
// Each call leaves exactly one value on the stack, so recursion composes the
// same way gsc_utils_getarraykeys() builds its result.
// ===========================================================================

static void json_to_gsc_push(yyjson_val *node, int depth)
{
	if ( depth >= JSON_MAX_DEPTH || node == NULL || yyjson_is_null(node) )
	{
		stackPushUndefined();
		return;
	}

	if ( yyjson_is_bool(node) )
	{
		stackPushInt(yyjson_is_true(node) ? 1 : 0);
		return;
	}

	if ( yyjson_is_num(node) )
	{
		// yyjson_is_int matches BOTH SINT and UINT subtypes; reading the wrong
		// half of the union via the other accessor reinterprets a 20-digit
		// unsigned as a negative i64. Branch on the actual subtype first.
		if ( yyjson_is_sint(node) )
		{
			int64_t v = yyjson_get_sint(node);
			if ( v >= INT_MIN && v <= INT_MAX )
			{
				stackPushInt((int)v);
				return;
			}
		}
		else if ( yyjson_is_uint(node) )
		{
			uint64_t u = yyjson_get_uint(node);
			if ( u <= (uint64_t)INT_MAX )
			{
				stackPushInt((int)u);
				return;
			}
		}
		// Real OR int out-of-32-bit-range -> float.
		stackPushFloat((float)yyjson_get_num(node));
		return;
	}

	if ( yyjson_is_str(node) )
	{
		const char *s = yyjson_get_str(node);
		size_t len = yyjson_get_len(node);
		if ( s == NULL )
		{
			stackPushUndefined();
			return;
		}
		if ( len >= JSON_MAX_STRING )
		{
			// Pushing a >=64 KB string would terminate the VM; drop to undefined.
			Com_Printf("[JSON] WARN: string value >= %d bytes replaced with undefined\n", JSON_MAX_STRING);
			stackPushUndefined();
			return;
		}
		// yyjson tracks length explicitly, so a JSON "\0" inside a string
		// stays as a NUL in `s`. The engine string allocator is C-string based
		// and would truncate at the NUL silently - reject so callers don't
		// silently lose data.
		if ( memchr(s, '\0', len) != NULL )
		{
			Com_Printf("[JSON] WARN: string value contains embedded NUL, replaced with undefined\n");
			stackPushUndefined();
			return;
		}
		stackPushString(s);
		return;
	}

	if ( yyjson_is_arr(node) )
	{
		stackPushArray();
		yyjson_val *elem;
		yyjson_arr_iter iter;
		yyjson_arr_iter_init(node, &iter);
		while ( (elem = yyjson_arr_iter_next(&iter)) != NULL )
		{
			json_to_gsc_push(elem, depth + 1);
			stackPushArrayLast();
		}
		return;
	}

	if ( yyjson_is_obj(node) )
	{
		stackPushArray();
		yyjson_val *key, *val;
		yyjson_obj_iter iter;
		yyjson_obj_iter_init(node, &iter);
		while ( (key = yyjson_obj_iter_next(&iter)) != NULL )
		{
			val = yyjson_obj_iter_get_val(key);
			const char *kstr = yyjson_get_str(key);
			size_t klen = yyjson_get_len(key);

			if ( kstr == NULL )
				continue;

			// Same engine string-table cap as values - a >= 64 KB key would
			// terminate the VM inside SL_GetString. Skip the entry so the
			// parse stays alive.
			if ( klen >= JSON_MAX_STRING )
			{
				Com_Printf("[JSON] WARN: object key >= %d bytes skipped\n", JSON_MAX_STRING);
				continue;
			}
			// Same embedded-NUL guard as values - the engine string table is
			// C-string indexed and would silently truncate.
			if ( memchr(kstr, '\0', klen) != NULL )
			{
				Com_Printf("[JSON] WARN: object key contains embedded NUL, skipped\n");
				continue;
			}

			json_to_gsc_push(val, depth + 1);
			// Scr_AddArrayStringIndexed adds the array's own ref to the key,
			// so release the temporary ref SL_GetString handed us - otherwise
			// every object key leaks one string-table slot.
			unsigned int k = SL_GetString(kstr, 0);
			Scr_AddArrayStringIndexed(k);
			SL_RemoveRefToString(k);
		}
		return;
	}

	stackPushUndefined();
}

// ===========================================================================
// GSC -> JSON : walk the engine variable tree and build a yyjson mutable tree.
// ===========================================================================

// One slot per child collected during the single sibling walk. The raw
// `name` is enough for both decisions: name < SL_MAX_STRING_INDEX is a
// string key, name >= MAX_ARRAYINDEX is an integer index, and because
// name = index + MAX_ARRAYINDEX for integer keys, sorting by raw `name`
// sorts by index.
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

// Convert a single variable entry (by id) to a yyjson mutable node.
static yyjson_mut_val * gsc_entry_to_json(yyjson_mut_doc *doc, unsigned int id, int depth)
{
	VariableValueInternal *entry = &scrVarGlob[id];
	int type = entry->w.type & VAR_MASK;

	switch ( type )
	{
	case VAR_INTEGER:
		return yyjson_mut_int(doc, entry->u.u.intValue);

	case VAR_FLOAT:
		return yyjson_mut_real(doc, (double)entry->u.u.floatValue);

	case VAR_STRING:
	case VAR_ISTRING:
		return yyjson_mut_strcpy(doc, SL_ConvertToString(entry->u.u.stringValue));

	case VAR_VECTOR:
	{
		const float *v = entry->u.u.vectorValue;
		yyjson_mut_val *arr = yyjson_mut_arr(doc);
		if ( v != NULL )
		{
			yyjson_mut_arr_append(arr, yyjson_mut_real(doc, (double)v[0]));
			yyjson_mut_arr_append(arr, yyjson_mut_real(doc, (double)v[1]));
			yyjson_mut_arr_append(arr, yyjson_mut_real(doc, (double)v[2]));
		}
		return arr;
	}

	case VAR_OBJECT:
	case VAR_STRUCT:
	case VAR_ARRAY:
		return gsc_object_to_json(doc, entry->u.u.pointerValue, depth + 1);

	default:
		// entities, threads, functions, undefined etc. have no JSON form.
		return yyjson_mut_null(doc);
	}
}

// Convert an array/struct object (by object id) to a yyjson mutable
// array or object. Single sibling walk: collect (name,id) once, decide
// array-vs-object from the collected keys, then build. Pointer-range
// names (engine-internal entries that have no script-visible key) are
// skipped during collection.
static yyjson_mut_val * gsc_object_to_json(yyjson_mut_doc *doc, unsigned int objectId, int depth)
{
	if ( depth >= JSON_MAX_DEPTH || objectId == 0 )
		return yyjson_mut_null(doc);

	// Count entries by walking the sibling ring - size-independent. GetArraySize
	// only maintains a count for VAR_ARRAY; a VAR_OBJECT struct (spawnstruct,
	// level, self) leaves u.o.u.size uninitialized, so we never trust it and walk
	// to the ring terminator instead. The SL_MAX_STRING_INDEX cap (max string-
	// table entries) guards a malformed ring. This makes struct serialization
	// correct too.
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
		return yyjson_mut_obj(doc); // empty -> {} (documented limitation)

	json_kv *items = (json_kv *)malloc(sizeof(json_kv) * total);
	if ( items == NULL )
		return yyjson_mut_null(doc); // OOM (practically unreachable at GSC limits)

	unsigned int count = 0;
	bool hasStringKey = false;
	it = objectId;

	for ( unsigned int i = 0; i < total; i++ )
	{
		it = FindNextSibling(it);
		if ( it == 0 )
			break;

		unsigned int name = GetVariableName(it);

		// Skip engine-internal pointer-range entries (no script-visible key).
		if ( name >= SL_MAX_STRING_INDEX && name < MAX_ARRAYINDEX )
			continue;

		items[count].name = name;
		items[count].id   = it;
		if ( name < SL_MAX_STRING_INDEX )
			hasStringKey = true;
		count++;
	}

	// Every entry was filtered out (only engine-internal pointer-range names).
	// Treat the same as the zero-size case so empty containers always emit `{}`.
	if ( count == 0 )
	{
		free(items);
		return yyjson_mut_obj(doc);
	}

	// All integer-indexed -> JSON array, sorted ascending by index. Sorting
	// the raw `name` works because name = index + MAX_ARRAYINDEX (monotonic).
	if ( !hasStringKey )
	{
		qsort(items, count, sizeof(json_kv), json_kv_cmp);

		yyjson_mut_val *arr = yyjson_mut_arr(doc);
		for ( unsigned int i = 0; i < count; i++ )
			yyjson_mut_arr_append(arr, gsc_entry_to_json(doc, items[i].id, depth));

		free(items);
		return arr;
	}

	// Mixed/string keys -> JSON object. Integer keys are stringified.
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	for ( unsigned int i = 0; i < count; i++ )
	{
		unsigned int name = items[i].name;
		yyjson_mut_val *child = gsc_entry_to_json(doc, items[i].id, depth);

		// yyjson_mut_obj_add_val BORROWS the key pointer (doesn't copy). The
		// pointer returned by SL_ConvertToString is only valid until the next
		// string-table mutation (other JSON ops, GSC assignments...), and the
		// SL pointer also lives outside the mut_doc allocator entirely, so
		// async save would dangle it across the worker handoff. Always copy
		// the key into the doc so its lifetime tracks yyjson_mut_doc_free.
		yyjson_mut_val *key;
		if ( name < SL_MAX_STRING_INDEX )
		{
			key = yyjson_mut_strcpy(doc, SL_ConvertToString(name));
		}
		else
		{
			char keybuf[32];
			snprintf(keybuf, sizeof(keybuf), "%u", name - MAX_ARRAYINDEX);
			key = yyjson_mut_strcpy(doc, keybuf);
		}
		yyjson_mut_obj_add(obj, key, child);
	}
	free(items);
	return obj;
}

// Convert a top-level script function parameter to a yyjson mutable node.
// Unlike nested entries (read via scrVarGlob), params are read through the
// public stack API. All accessor return values are checked so a mismatched/
// undefined param degrades to JSON null instead of using uninitialized memory.
static yyjson_mut_val * gsc_param_to_json(yyjson_mut_doc *doc, int param)
{
	unsigned int objectId;
	if ( stackGetParamObject(param, &objectId) )
	{
		// stackGetParamObject also accepts entities/threads (all stored as a
		// pointer on the stack). Only arrays and structs have a JSON form;
		// walking an entity reads a bogus size and emits garbage, so gate the
		// type here.
		int t = Scr_GetPointerType(param);
		if ( t == VAR_ARRAY || t == VAR_STRUCT )
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
		return yyjson_mut_real(doc, (double)v);
	}

	case VAR_STRING:
	{
		char *v = NULL;
		if ( !stackGetParamString(param, &v) || v == NULL )
			return yyjson_mut_null(doc);
		return yyjson_mut_strcpy(doc, v);
	}

	case VAR_ISTRING:
	{
		char *v = NULL;
		if ( !stackGetParamLocalizedString(param, &v) || v == NULL )
			return yyjson_mut_null(doc);
		return yyjson_mut_strcpy(doc, v);
	}

	case VAR_VECTOR:
	{
		vec3_t v;
		if ( !stackGetParamVector(param, v) )
			return yyjson_mut_null(doc);
		yyjson_mut_val *arr = yyjson_mut_arr(doc);
		yyjson_mut_arr_append(arr, yyjson_mut_real(doc, (double)v[0]));
		yyjson_mut_arr_append(arr, yyjson_mut_real(doc, (double)v[1]));
		yyjson_mut_arr_append(arr, yyjson_mut_real(doc, (double)v[2]));
		return arr;
	}

	default:
		return yyjson_mut_null(doc);
	}
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
	yyjson_doc *doc = yyjson_read(str, strlen(str), YYJSON_READ_ALLOW_BOM);
	if ( doc == NULL )
	{
		stackError("gsc_json_parse() failed to parse JSON input");
		stackPushUndefined();
		return;
	}

	yyjson_val *root = yyjson_doc_get_root(doc);
	json_to_gsc_push(root, 0);
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
	yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
	if ( doc == NULL )
	{
		stackPushUndefined();
		return;
	}

	yyjson_mut_val *root = gsc_param_to_json(doc, 0);
	if ( root == NULL )
	{
		yyjson_mut_doc_free(doc);
		stackPushUndefined();
		return;
	}
	yyjson_mut_doc_set_root(doc, root);

	// INF_AND_NAN_AS_NULL matches cJSON's silent NaN->null behavior so a
	// stray non-finite float does not fail the entire stringify.
	yyjson_write_flag flags = YYJSON_WRITE_INF_AND_NAN_AS_NULL;
	if ( pretty )
		flags |= YYJSON_WRITE_PRETTY;

	size_t out_len = 0;
	char *out = yyjson_mut_write(doc, flags, &out_len);
	yyjson_mut_doc_free(doc);

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
	int maxBytes = dvar_int_or("scr_json_max_load_bytes", 8 * 1024 * 1024);
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

	yyjson_doc *doc = yyjson_read(buffer, bytesRead, YYJSON_READ_ALLOW_BOM);
	free(buffer);

	if ( doc == NULL )
	{
		stackError("gsc_json_load() failed to parse JSON from '%s'", path);
		stackPushUndefined();
		return;
	}

	json_to_gsc_push(yyjson_doc_get_root(doc), 0);
	yyjson_doc_free(doc);
}

void gsc_json_save()
{
	char *path;

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

	JsonTimer _t("json_save", path);
	yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
	if ( doc == NULL )
	{
		stackPushInt(0);
		return;
	}

	yyjson_mut_val *root = gsc_param_to_json(doc, 1);
	if ( root == NULL )
	{
		yyjson_mut_doc_free(doc);
		stackPushInt(0);
		return;
	}
	yyjson_mut_doc_set_root(doc, root);

	// INF_AND_NAN_AS_NULL matches cJSON's silent NaN->null behavior so a
	// stray non-finite float does not fail the entire stringify.
	yyjson_write_flag flags = YYJSON_WRITE_INF_AND_NAN_AS_NULL;
	if ( pretty )
		flags |= YYJSON_WRITE_PRETTY;

	size_t out_len = 0;
	char *out = yyjson_mut_write(doc, flags, &out_len);
	yyjson_mut_doc_free(doc);

	if ( out == NULL )
	{
		stackPushInt(0);
		return;
	}

	fileHandle_t f = FS_FOpenFileWrite(path);
	if ( f == 0 )
	{
		free(out);
		stackError("gsc_json_save() could not open '%s' for writing", path);
		stackPushInt(0);
		return;
	}

	int written = FS_Write(out, (int)out_len, f);
	FS_FCloseFile(f);
	free(out);

	stackPushInt(written > 0 ? 1 : 0);
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
	int    max_bytes;  // snapshot of scr_json_max_load_bytes at submit (load only)
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

// Build "<fs_homepath>/<fs_game-or-main>/<rel>" -- main thread only.
// Returns malloc'd string, or NULL if `rel` is empty / absolute / contains "..".
static char * json_async_resolve_path(const char *rel)
{
	if ( rel == NULL || rel[0] == '\0' || rel[0] == '/' )
		return NULL;
	if ( strstr(rel, "..") != NULL )
		return NULL;

	dvar_t *fs_home = Dvar_FindVar("fs_homepath");
	dvar_t *fs_game = Dvar_FindVar("fs_game");
	if ( fs_home == NULL || fs_home->current.string == NULL || fs_home->current.string[0] == '\0' )
		return NULL;

	const char *home = fs_home->current.string;
	const char *game = (fs_game != NULL && fs_game->current.string != NULL && fs_game->current.string[0] != '\0')
		? fs_game->current.string : "main";

	size_t need = strlen(home) + 1 + strlen(game) + 1 + strlen(rel) + 1;
	char *abs = (char *)malloc(need);
	if ( abs == NULL )
		return NULL;
	snprintf(abs, need, "%s/%s/%s", home, game, rel);
	return abs;
}

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
	// 32 MB hard cap on i386 as the upper bound. Operator sets the dvar; we
	// don't read the live dvar here because that would require main-thread
	// serialization and the snapshot is cheap.
	long hard_cap = 32L * 1024L * 1024L;
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

	yyjson_doc *parsed = yyjson_read(buf, got, YYJSON_READ_ALLOW_BOM);
	free(buf);

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

// Worker: print the yyjson doc and write to disk.
static void * json_async_save_worker(void *arg)
{
	json_async_job *job = (json_async_job *)arg;

	yyjson_write_flag flags = YYJSON_WRITE_INF_AND_NAN_AS_NULL;
	if ( job->save_pretty )
		flags |= YYJSON_WRITE_PRETTY;

	size_t want = 0;
	char *text = yyjson_mut_write(job->save_doc, flags, &want);
	yyjson_mut_doc_free(job->save_doc);
	job->save_doc = NULL;

	if ( text == NULL )
	{
		pthread_mutex_lock(&json_async_mutex);
		job->save_ok = 0;
		job->status  = JSON_ASYNC_STATUS_DONE;
		pthread_mutex_unlock(&json_async_mutex);
		return NULL;
	}

	FILE *f = fopen(job->abspath, "wb");
	if ( f == NULL )
	{
		free(text);
		pthread_mutex_lock(&json_async_mutex);
		job->save_ok = 0;
		job->status  = JSON_ASYNC_STATUS_DONE;
		pthread_mutex_unlock(&json_async_mutex);
		return NULL;
	}

	size_t written = fwrite(text, 1, want, f);
	fclose(f);
	free(text);

	pthread_mutex_lock(&json_async_mutex);
	job->save_ok = (written == want) ? 1 : 0;
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

	int maxJobs = dvar_int_or("scr_json_async_max_jobs", 64);
	if ( json_async_pending >= maxJobs )
	{
		stackError("gsc_json_load_async() too many pending jobs (%d / %d)", json_async_pending, maxJobs);
		stackPushInt(0);
		return;
	}

	char *abs = json_async_resolve_path(path);
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
	job->max_bytes = dvar_int_or("scr_json_max_load_bytes", 8 * 1024 * 1024);

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
	char *path;
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

	int maxJobs = dvar_int_or("scr_json_async_max_jobs", 64);
	if ( json_async_pending >= maxJobs )
	{
		stackError("gsc_json_save_async() too many pending jobs (%d / %d)", json_async_pending, maxJobs);
		stackPushInt(0);
		return;
	}

	char *abs = json_async_resolve_path(path);
	if ( abs == NULL )
	{
		stackError("gsc_json_save_async() invalid path '%s' (must be relative, no '..')", path);
		stackPushInt(0);
		return;
	}

	// Build the mutable doc on the MAIN thread (reading GSC state is single-
	// threaded). Worker will print + write + free the doc.
	yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
	if ( doc == NULL )
	{
		free(abs);
		stackPushInt(0);
		return;
	}
	yyjson_mut_val *root = gsc_param_to_json(doc, 1);
	if ( root == NULL )
	{
		yyjson_mut_doc_free(doc);
		free(abs);
		stackPushInt(0);
		return;
	}
	yyjson_mut_doc_set_root(doc, root);

	json_async_job *job = (json_async_job *)calloc(1, sizeof(json_async_job));
	if ( job == NULL ) { free(abs); yyjson_mut_doc_free(doc); stackPushInt(0); return; }
	job->kind        = JSON_ASYNC_KIND_SAVE;
	job->status      = JSON_ASYNC_STATUS_PENDING;
	job->abspath     = abs;
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

	// Push the return value while OUTSIDE the mutex (json_to_gsc_push may
	// recurse and we don't want to hold the lock during that).
	if ( kind == JSON_ASYNC_KIND_LOAD )
	{
		if ( status == JSON_ASYNC_STATUS_DONE && doc != NULL )
		{
			json_to_gsc_push(yyjson_doc_get_root(doc), 0);
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
