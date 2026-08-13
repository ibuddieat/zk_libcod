#include "config.hpp"

#if COMPILE_HTTP == 1

// Include the HTTP client (and its libwebsockets + STL dependencies) before
// gsc.hpp: gsc.hpp -> declarations.hpp #defines `level` as a macro, which would
// otherwise corrupt identifiers named `level` inside the STL headers.
#include "http_client.hpp"

#include "gsc_http.hpp"

static HttpClient *http_client = NULL;
static int http_pending_requests = 0;

/*
 * httpFetch(url, method, data, headers, timeout, ::onDone, ::onError)
 *
 * timeout is in SECONDS (0 disables it) - the natural unit for GSC. It bounds
 * the whole request; on expiry onError fires with "Timeout".
 *
 * Performs an asynchronous HTTP(S) request. The connection is closed once the
 * response is received. Exactly one script callback is invoked:
 *   onDone(status, body, headers)  - headers is a string-keyed array
 *   onError(error)                 - error is a descriptive string
 * Request headers are separated by \r\n, e.g. "Content-Type: application/json".
 *
 * A response whose body exceeds GSC_MAX_STRING_BYTES is routed to onError
 * instead of onDone, so a remote peer cannot crash the server with an oversized
 * reply. Callbacks are also dropped if the level that issued the request has
 * been unloaded by the time the response arrives.
 */
void gsc_http_fetch()
{
	const char *url;
	const char *method;
	const char *data;
	const char *headers;
	int timeout;
	int onDoneCallback;
	int onErrorCallback;

	if ( Scr_GetNumParam() != 7 )
	{
		stackError("httpFetch() takes 7 arguments: url, method, data, headers, timeout, onDone, onError");
		stackPushUndefined();
		return;
	}

	if ( !stackGetParamString(0, &url) ||
	     !stackGetParamString(1, &method) ||
	     !stackGetParamString(2, &data) ||
	     !stackGetParamString(3, &headers) ||
	     !stackGetParamInt(4, &timeout) ||
	     !stackGetParamFunction(5, &onDoneCallback) ||
	     !stackGetParamFunction(6, &onErrorCallback) )
	{
		stackError("httpFetch() has an undefined or wrong-typed argument");
		stackPushUndefined();
		return;
	}

	if ( !http_client )
		http_client = new HttpClient();

	http_pending_requests++;

	// The callback handles belong to the issuing level's scripts; capture its id
	// so a response arriving after a map change is not delivered to a dead handle.
	unsigned int levelId = scrVarPub.levelId;

	// strlen is the true body length: script strings are NUL-terminated and
	// cannot carry an embedded NUL, so no binary body is possible here.
	http_client->request(method, url, data, strlen(data), headers,
		[onDoneCallback, onErrorCallback, levelId](const HttpClient::Response &res)
		{
			http_pending_requests--;

			if ( !Scr_IsSystemActive() || scrVarPub.levelId != levelId )
				return;

			// An oversized body cannot be interned as a script string without a
			// fatal Scr_TerminalError, so report it through onError.
			if ( res.body.size() > GSC_MAX_STRING_BYTES )
			{
				if ( onErrorCallback )
				{
					char msg[96];
					snprintf(msg, sizeof(msg), "response body too large: %u bytes", (unsigned int)res.body.size());
					stackPushString(msg);
					short threadId = Scr_ExecThread(onErrorCallback, 1);
					Scr_FreeThread(threadId);
				}
				return;
			}

			if ( !onDoneCallback )
				return;

			// Build a string-keyed array, headers[name] = value. CoD2x instead
			// returns a flat [name, value, name, value] list the script must walk
			// in pairs (its gsc_http.cpp:56-62); the keyed array is easier to use.
			stackPushArray();
			for ( const auto &header : res.headers )
			{
				// Skip a pathologically large header rather than crash on it
				if ( header.first.size() > GSC_MAX_STRING_BYTES || header.second.size() > GSC_MAX_STRING_BYTES )
					continue;

				stackPushString(header.second.c_str());
				unsigned int key = SL_GetString(header.first.c_str(), 0);
				Scr_AddArrayStringIndexed(key);
				// The array now holds its own reference to the key; release ours
				// so the string-table slot is not leaked.
				SL_RemoveRefToString(key);
			}
			stackPushString(res.body.c_str());
			stackPushInt(res.status);

			short threadId = Scr_ExecThread(onDoneCallback, 3);
			Scr_FreeThread(threadId);
		},
		// Capture the URL by value: the error may fire asynchronously, after the
		// char* from the script stack is gone.
		[onErrorCallback, levelId, urlCopy = std::string(url)](const std::string &error)
		{
			http_pending_requests--;

			if ( onErrorCallback && Scr_IsSystemActive() && scrVarPub.levelId == levelId )
			{
				stackPushString(error.c_str());

				short threadId = Scr_ExecThread(onErrorCallback, 1);
				Scr_FreeThread(threadId);
			}
			else
				Com_Printf("httpFetch error while fetching %s: %s\n", urlCopy.c_str(), error.c_str());
		},
		timeout > 0 ? timeout * 1000 : 0);
}

/*
 * Drives the HTTP event loop. Called once per server frame, but only does work
 * while a request is in flight, so an idle server pays nothing (no per-frame
 * mg_mgr_poll syscall once all requests have completed).
 */
void gsc_http_poll()
{
	if ( http_client && http_pending_requests > 0 )
		http_client->poll();
}

/*
 * Drains any in-flight requests and releases the client. Called on shutdown,
 * where the per-frame poll no longer runs, so we poll synchronously (max ~1s).
 */
void gsc_http_shutdown()
{
	if ( !http_client )
		return;

	for ( int i = 0; i < 10 && http_pending_requests > 0; i++ )
		http_client->poll(100);

	delete http_client;
	http_client = NULL;
	http_pending_requests = 0;
}

#endif
