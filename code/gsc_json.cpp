#include "gsc_json.hpp"

#if COMPILE_JSON == 1

#include "lib/cJSON.h"

/*
 * Native JSON support for GSC, backed by cJSON (code/lib/cJSON.[ch]).
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
 */

// VAR_MASK / SL_MAX_STRING_INDEX / MAX_ARRAYINDEX come from declarations.hpp
// (engine variable-system constants).

#include <pthread.h>

#define JSON_MAX_DEPTH 64

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

static void json_to_gsc_push(cJSON *node, int depth)
{
	if ( depth > JSON_MAX_DEPTH || node == NULL || cJSON_IsNull(node) )
	{
		stackPushUndefined();
		return;
	}

	if ( cJSON_IsBool(node) )
	{
		stackPushInt(cJSON_IsTrue(node) ? 1 : 0);
		return;
	}

	if ( cJSON_IsNumber(node) )
	{
		double d = node->valuedouble;

		// Integral and within signed 32-bit range -> int, otherwise float.
		// The (int)d cast is only reached after the range check (short-circuit).
		if ( d >= -2147483648.0 && d <= 2147483647.0 && d == (double)(int)d )
			stackPushInt((int)d);
		else
			stackPushFloat((float)d);
		return;
	}

	if ( cJSON_IsString(node) )
	{
		stackPushString(node->valuestring ? node->valuestring : "");
		return;
	}

	if ( cJSON_IsArray(node) )
	{
		stackPushArray();
		for ( cJSON *c = node->child; c != NULL; c = c->next )
		{
			json_to_gsc_push(c, depth + 1);
			stackPushArrayLast();
		}
		return;
	}

	if ( cJSON_IsObject(node) )
	{
		stackPushArray();
		for ( cJSON *c = node->child; c != NULL; c = c->next )
		{
			json_to_gsc_push(c, depth + 1);
			if ( c->string )
				Scr_AddArrayStringIndexed(SL_GetString(c->string, 0));
			else
				stackPushArrayLast();
		}
		return;
	}

	stackPushUndefined();
}

// ===========================================================================
// GSC -> JSON : walk the engine variable tree and build a cJSON document.
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

static cJSON * gsc_object_to_json(unsigned int objectId, int depth);

// Convert a single variable entry (by id) to a cJSON node.
static cJSON * gsc_entry_to_json(unsigned int id, int depth)
{
	VariableValueInternal *entry = &scrVarGlob[id];
	int type = entry->w.type & VAR_MASK;

	switch ( type )
	{
	case VAR_INTEGER:
		return cJSON_CreateNumber((double)entry->u.u.intValue);

	case VAR_FLOAT:
		return cJSON_CreateNumber((double)entry->u.u.floatValue);

	case VAR_STRING:
	case VAR_ISTRING:
		return cJSON_CreateString(SL_ConvertToString(entry->u.u.stringValue));

	case VAR_VECTOR:
	{
		const float *v = entry->u.u.vectorValue;
		cJSON *arr = cJSON_CreateArray();
		if ( v )
		{
			cJSON_AddItemToArray(arr, cJSON_CreateNumber((double)v[0]));
			cJSON_AddItemToArray(arr, cJSON_CreateNumber((double)v[1]));
			cJSON_AddItemToArray(arr, cJSON_CreateNumber((double)v[2]));
		}
		return arr;
	}

	case VAR_OBJECT:
	case VAR_STRUCT:
	case VAR_ARRAY:
		return gsc_object_to_json(entry->u.u.pointerValue, depth + 1);

	default:
		// entities, threads, functions, undefined etc. have no JSON form.
		return cJSON_CreateNull();
	}
}

// Convert an array/struct object (by object id) to a cJSON array or object.
// Single sibling walk: collect (name,id) once, decide array-vs-object from the
// collected keys, then build. Pointer-range names (engine-internal entries
// that have no script-visible key) are skipped during collection.
static cJSON * gsc_object_to_json(unsigned int objectId, int depth)
{
	if ( depth > JSON_MAX_DEPTH || objectId == 0 )
		return cJSON_CreateNull();

	unsigned int size = GetArraySize(objectId);
	if ( size == 0 )
		return cJSON_CreateObject(); // empty -> {} (documented limitation)

	json_kv *items = (json_kv *)malloc(sizeof(json_kv) * size);
	if ( items == NULL )
		return cJSON_CreateNull(); // OOM (practically unreachable at GSC limits)

	unsigned int count = 0;
	bool hasStringKey = false;
	unsigned int it = objectId;

	for ( unsigned int i = 0; i < size; i++ )
	{
		it = FindNextSibling(it);
		if ( it == 0 )
			break; // engine returned fewer siblings than GetArraySize claimed

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
		return cJSON_CreateObject();
	}

	// All integer-indexed -> JSON array, sorted ascending by index. Sorting
	// the raw `name` works because name = index + MAX_ARRAYINDEX (monotonic).
	if ( !hasStringKey )
	{
		qsort(items, count, sizeof(json_kv), json_kv_cmp);

		cJSON *arr = cJSON_CreateArray();
		for ( unsigned int i = 0; i < count; i++ )
			cJSON_AddItemToArray(arr, gsc_entry_to_json(items[i].id, depth));

		free(items);
		return arr;
	}

	// Mixed/string keys -> JSON object. Integer keys are stringified.
	cJSON *obj = cJSON_CreateObject();
	for ( unsigned int i = 0; i < count; i++ )
	{
		unsigned int name = items[i].name;
		cJSON *child = gsc_entry_to_json(items[i].id, depth);

		if ( name < SL_MAX_STRING_INDEX )
		{
			cJSON_AddItemToObject(obj, SL_ConvertToString(name), child);
		}
		else
		{
			char keybuf[32];
			snprintf(keybuf, sizeof(keybuf), "%u", name - MAX_ARRAYINDEX);
			cJSON_AddItemToObject(obj, keybuf, child);
		}
	}
	free(items);
	return obj;
}

// Convert a top-level script function parameter to a cJSON node. Unlike nested
// entries (read via scrVarGlob), params are read through the public stack API.
// All accessor return values are checked so a mismatched/undefined param
// degrades to JSON null instead of using uninitialized memory.
static cJSON * gsc_param_to_json(int param)
{
	unsigned int objectId;
	if ( stackGetParamObject(param, &objectId) )
		return gsc_object_to_json(objectId, 0);

	switch ( stackGetParamType(param) )
	{
	case VAR_INTEGER:
	{
		int v;
		if ( !stackGetParamInt(param, &v) )
			return cJSON_CreateNull();
		return cJSON_CreateNumber((double)v);
	}

	case VAR_FLOAT:
	{
		float v;
		if ( !stackGetParamFloat(param, &v) )
			return cJSON_CreateNull();
		return cJSON_CreateNumber((double)v);
	}

	case VAR_STRING:
	{
		char *v = NULL;
		if ( !stackGetParamString(param, &v) || v == NULL )
			return cJSON_CreateNull();
		return cJSON_CreateString(v);
	}

	case VAR_ISTRING:
	{
		char *v = NULL;
		if ( !stackGetParamLocalizedString(param, &v) || v == NULL )
			return cJSON_CreateNull();
		return cJSON_CreateString(v);
	}

	case VAR_VECTOR:
	{
		vec3_t v;
		if ( !stackGetParamVector(param, v) )
			return cJSON_CreateNull();
		cJSON *arr = cJSON_CreateArray();
		cJSON_AddItemToArray(arr, cJSON_CreateNumber((double)v[0]));
		cJSON_AddItemToArray(arr, cJSON_CreateNumber((double)v[1]));
		cJSON_AddItemToArray(arr, cJSON_CreateNumber((double)v[2]));
		return arr;
	}

	default:
		return cJSON_CreateNull();
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
	cJSON *root = cJSON_Parse(str);
	if ( root == NULL )
	{
		stackError("gsc_json_parse() failed to parse JSON input");
		stackPushUndefined();
		return;
	}

	json_to_gsc_push(root, 0);
	cJSON_Delete(root);
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
	cJSON *root = gsc_param_to_json(0);
	if ( root == NULL )
	{
		stackPushUndefined();
		return;
	}

	char *out;
	if ( pretty )
		out = cJSON_Print(root);
	else
		out = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);

	if ( out == NULL )
	{
		stackPushUndefined();
		return;
	}

	stackPushString(out);
	free(out);
}

