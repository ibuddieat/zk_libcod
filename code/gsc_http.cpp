#include "config.hpp"

#if COMPILE_HTTP == 1

// Include the self-contained HTTP client (and its Mongoose/OpenSSL/STL deps)
// BEFORE gsc.hpp. gsc.hpp -> declarations.hpp #defines `level` as a macro,
// which otherwise mangles identifiers named `level` inside the system headers
// (e.g. OpenSSL's SSL_set_security_level, libstdc++ string_view literals).
#include "http_client.hpp"

#include "gsc_http.hpp"
#include "gsc_websocket.hpp"

static HttpClient *http_client = NULL;
static int http_pending_requests = 0;

/*
 * httpFetch(url, method, data, headers, timeout, ::onDone, ::onError)
 *
 * Performs an asynchronous HTTP(S) request. The connection is closed once the
 * response is received. Results are delivered to one of the script callbacks:
 *   onDone(status, body, headers)  - headers is a string-keyed array
 *   onError(error)                 - error is a descriptive string
 * Headers are separated by \r\n, e.g. "Content-Type: application/json".
 */
void gsc_http_fetch()
{
	char *url;
	char *method;
	char *data;
	char *headers;
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

	http_client->request(method, url, data, strlen(data), headers,
		[onDoneCallback](const HttpClient::Response &res)
		{
			http_pending_requests--;

			if ( onDoneCallback && Scr_IsSystemActive() )
			{
				stackPushArray();
				for ( const auto &header : res.headers )
				{
					stackPushString((char *)header.first.c_str());
					stackPushArrayLast();
					stackPushString((char *)header.second.c_str());
					stackPushArrayLast();
				}
				stackPushString((char *)res.body.c_str());
				stackPushInt(res.status);

				short threadId = Scr_ExecThread(onDoneCallback, 3);
				Scr_FreeThread(threadId);
			}
		},
		[onErrorCallback, url](const std::string &error)
		{
			http_pending_requests--;

			if ( onErrorCallback && Scr_IsSystemActive() )
			{
				stackPushString((char *)error.c_str());

				short threadId = Scr_ExecThread(onErrorCallback, 1);
				Scr_FreeThread(threadId);
			}
			else
				Com_Printf("httpFetch error while fetching %s: %s\n", url, error.c_str());
		},
		timeout);
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
