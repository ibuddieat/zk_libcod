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

void gsc_json_load()
{
	char *path;

	if ( !stackGetParams("s", &path) )
	{
		stackError("gsc_json_load() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	JsonTimer _t("json_load", path);

	fileHandle_t f;
	int len = FS_FOpenFileByMode(path, &f, FS_READ);
	if ( len <= 0 )
	{
		// File missing or empty: quiet undefined (no log spam for try-load).
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
	// leave uninitialized bytes for cJSON_Parse to scan past the data.
	int bytesRead = FS_Read(buffer, len, f);
	FS_FCloseFile(f);
	if ( bytesRead < 0 )
		bytesRead = 0;
	if ( bytesRead > len )
		bytesRead = len;
	buffer[bytesRead] = '\0';

	cJSON *root = cJSON_Parse(buffer);
	free(buffer);

	if ( root == NULL )
	{
		stackError("gsc_json_load() failed to parse JSON from '%s'", path);
		stackPushUndefined();
		return;
	}

	json_to_gsc_push(root, 0);
	cJSON_Delete(root);
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
	cJSON *root = gsc_param_to_json(1);
	if ( root == NULL )
	{
		stackPushInt(0);
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

	int written = FS_Write(out, strlen(out), f);
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
//        - load: fopen + fread + cJSON_Parse  (all in the worker)
//        - save: cJSON_Print + fopen + fwrite (the cJSON tree is built on the
//          main thread first, since reading GSC values needs the main VM —
//          tree-walk is microseconds for typical data; printing and disk I/O
//          are the slow parts and they happen in the worker).
//   3. GSC periodically polls  json_async_done()  -> array of finished jobIds.
//   4. For each finished id, GSC calls  json_async_result(id)  to claim the
//      value (load) or 1/0 success flag (save). Claiming frees the job.
//
// Thread-safety: workers ONLY touch their own job struct (mutex-protected
// status field) and heap data they own. They never touch GSC state. The
// engine FS_* API is not thread-safe, so workers use plain libc fopen/fread/
// fwrite against an absolute path resolved on the main thread using
// fs_homepath/fs_game. Paths are sandboxed: must be relative, no ".." segments.
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
	char  *abspath;    // resolved absolute path (owned)

	// Load output (filled by worker):
	cJSON *load_root;  // ownership transferred to caller on json_async_result

	// Save input (built on main thread, consumed by worker):
	cJSON *save_root;
	int    save_pretty;
	int    save_ok;    // 1 on successful write, 0 otherwise  (mutex-guarded)

	struct json_async_job *next;
};

static pthread_mutex_t json_async_mutex   = PTHREAD_MUTEX_INITIALIZER;
static json_async_job *json_async_jobs    = NULL;
static int             json_async_next_id = 1;
static int             json_async_pending = 0;

// Build "<fs_homepath>/<fs_game-or-main>/<rel>" — main thread only.
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
	if ( job->abspath  != NULL ) free(job->abspath);
	if ( job->load_root != NULL ) cJSON_Delete(job->load_root);
	if ( job->save_root != NULL ) cJSON_Delete(job->save_root);
	free(job);
}

// Worker: read the file, parse it, store the cJSON tree.
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

	// Defensive: refuse > 256 MB even in async path (saner upper bound).
	if ( len <= 0 || len > 256L * 1024L * 1024L )
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

	cJSON *parsed = cJSON_Parse(buf);
	free(buf);

	pthread_mutex_lock(&json_async_mutex);
	if ( parsed == NULL )
	{
		job->status = JSON_ASYNC_STATUS_ERROR;
	}
	else
	{
		job->load_root = parsed;
		job->status   = JSON_ASYNC_STATUS_DONE;
	}
	pthread_mutex_unlock(&json_async_mutex);
	return NULL;
}

// Worker: print the cJSON tree and write to disk.
static void * json_async_save_worker(void *arg)
{
	json_async_job *job = (json_async_job *)arg;

	char *text = job->save_pretty ? cJSON_Print(job->save_root) : cJSON_PrintUnformatted(job->save_root);
	cJSON_Delete(job->save_root);
	job->save_root = NULL;

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

	size_t want    = strlen(text);
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
	job->kind    = JSON_ASYNC_KIND_LOAD;
	job->status  = JSON_ASYNC_STATUS_PENDING;
	job->abspath = abs;

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

	// Build the cJSON tree on the MAIN thread (reading GSC state is single-
	// threaded). Worker will print + write + free the tree.
	cJSON *root = gsc_param_to_json(1);
	if ( root == NULL )
	{
		free(abs);
		stackPushInt(0);
		return;
	}

	json_async_job *job = (json_async_job *)calloc(1, sizeof(json_async_job));
	if ( job == NULL ) { free(abs); cJSON_Delete(root); stackPushInt(0); return; }
	job->kind        = JSON_ASYNC_KIND_SAVE;
	job->status      = JSON_ASYNC_STATUS_PENDING;
	job->abspath     = abs;
	job->save_root   = root;
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
		stackPushUndefined();   // not finished yet — leave in list
		return;
	}

	// Unlink + take ownership.
	if ( prev != NULL ) prev->next = job->next;
	else                json_async_jobs = job->next;
	json_async_pending--;

	int   kind   = job->kind;
	int   status = job->status;
	cJSON *root  = job->load_root;
	int   sok    = job->save_ok;
	job->load_root = NULL; // taken

	pthread_mutex_unlock(&json_async_mutex);

	// Push the return value while OUTSIDE the mutex (json_to_gsc_push may
	// recurse and we don't want to hold the lock during that).
	if ( kind == JSON_ASYNC_KIND_LOAD )
	{
		if ( status == JSON_ASYNC_STATUS_DONE && root != NULL )
		{
			json_to_gsc_push(root, 0);
			cJSON_Delete(root);
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

#endif
