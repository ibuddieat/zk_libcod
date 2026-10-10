#include "config.hpp"

#if COMPILE_HTTP == 1

// Before gsc.hpp: its `level` macro (declarations.hpp) breaks the STL headers
#include "http_websocket_client.hpp"

#include "gsc_http_ws.hpp"

static HttpWsClient &client()
{
	HttpWsClient &c = HttpWsClient::get();
	c.log = Com_Printf;
	return c;
}

// Runs a script callback with the args already pushed
static void run_callback(int handle, unsigned int args)
{
	short threadId = Scr_ExecThread(handle, args);
	Scr_FreeThread(threadId);
}

/*
 * httpFetch(url, method, data, headers, timeout, ::onDone, ::onError, [allowInsecure])
 *
 * timeout is in SECONDS (0 = no overall limit). Exactly one callback runs, on a
 * later server frame:
 *   onDone(status, body, headers)  - headers is a string-keyed array
 *   onError(error)                 - error is a descriptive string
 * Request headers are separated by \r\n, e.g., "Content-Type: application/json".
 * Redirects are not followed. Callbacks are dropped if the level changes first.
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
	qboolean allowInsecure = qfalse;

	if ( Scr_GetNumParam() < 7 )
	{
		stackError("httpFetch() takes at least 7 arguments: url, method, data, headers, timeout, onDone, onError, [allowInsecure]");
		stackPushUndefined();
		return;
	}

	if ( !stackGetParamString(0, &url) ||
	     !stackGetParamString(1, &method) ||
	     !stackGetParamString(2, &data) ||
	     !stackGetParamString(3, &headers) ||
	     !stackGetParamInt(4, &timeout) ||
	     !stackGetParamFunction(5, &onDoneCallback) ||
	     !stackGetParamFunction(6, &onErrorCallback) ||
	     ( Scr_GetNumParam() >= 8 && !stackGetParamInt(7, &allowInsecure) ) )
	{
		stackError("httpFetch() has an undefined or wrong-typed argument");
		stackPushUndefined();
		return;
	}

	// Clamp to a day so the conversion cannot overflow
	if ( timeout > 86400 )
		timeout = 86400;

	client().fetch(method, url, data, headers, allowInsecure, timeout > 0 ? timeout * 1000 : 0,
		[onDoneCallback](const HttpWsClient::Response &res)
		{
			// headers[name] = value (CoD2x returns a flat [name, value, ...] list instead)
			stackPushArray();
			for ( const auto &header : res.headers )
			{
				// Skip a header too big for a script string
				if ( header.first.size() > GSC_MAX_STRING_BYTES || header.second.size() > GSC_MAX_STRING_BYTES )
					continue;

				stackPushString(header.second.c_str());
				unsigned int key = SL_GetString(header.first.c_str(), 0);
				Scr_AddArrayStringIndexed(key);
				// The array holds its own reference to the key now
				SL_RemoveRefToString(key);
			}
			stackPushString(res.body.c_str());
			stackPushInt(res.status);
			run_callback(onDoneCallback, 3);
		},
		[onErrorCallback](const std::string &error)
		{
			stackPushString(error.c_str());
			run_callback(onErrorCallback, 1);
		});
}

/*
 * webSocketConnect(url, headers, ::onConnect, ::onMessage, ::onClose, ::onError, [reconnectDelayMs], [pingIntervalMs], [allowInsecure])
 *
 * Opens a ws:// or wss:// connection. Returns the connection id or -1 if all
 * slots are taken. Callbacks run on a later server frame:
 *   onConnect()                              - handshake completed
 *   onMessage(text)                          - a TEXT message was received
 *   onClose(closedByRemote, fullyDisconnected)
 *   onError(error)
 * Headers are separated by \r\n. reconnectDelayMs defaults to 2000 (0 disables
 * auto-reconnect). pingIntervalMs defaults to 15000 (0 disables keepalive pings).
 *
 * Connections belong to the level that opened them: they are closed on map
 * change and map_restart. Open them again from the new level's init.
 */
void gsc_websocket_connect()
{
	const char *url;
	const char *headers;
	int onConnectCallback;
	int onMessageCallback;
	int onCloseCallback;
	int onErrorCallback;
	int reconnectDelayMs = 2000;
	int pingIntervalMs = 15000;
	qboolean allowInsecure = qfalse;

	if ( Scr_GetNumParam() < 6 )
	{
		stackError("webSocketConnect() takes at least 6 arguments: url, headers, onConnect, onMessage, onClose, onError, [reconnectDelayMs], [pingIntervalMs], [allowInsecure]");
		stackPushInt(-1);
		return;
	}

	if ( !stackGetParamString(0, &url) ||
	     !stackGetParamString(1, &headers) ||
	     !stackGetParamFunction(2, &onConnectCallback) ||
	     !stackGetParamFunction(3, &onMessageCallback) ||
	     !stackGetParamFunction(4, &onCloseCallback) ||
	     !stackGetParamFunction(5, &onErrorCallback) ||
	     ( Scr_GetNumParam() >= 7 && !stackGetParamInt(6, &reconnectDelayMs) ) ||
	     ( Scr_GetNumParam() >= 8 && !stackGetParamInt(7, &pingIntervalMs) ) ||
	     ( Scr_GetNumParam() >= 9 && !stackGetParamInt(8, &allowInsecure) ) )
	{
		stackError("webSocketConnect() has an undefined or wrong-typed argument");
		stackPushInt(-1);
		return;
	}

	// Negative would wrap as unsigned; 0 disables
	if ( reconnectDelayMs < 0 )
		reconnectDelayMs = 0;
	if ( pingIntervalMs < 0 )
		pingIntervalMs = 0;

	HttpWsClient::WsCallbacks cb;
	cb.open = [onConnectCallback]()
	{
		run_callback(onConnectCallback, 0);
	};
	cb.message = [onMessageCallback](const std::string &message)
	{
		stackPushString(message.c_str());
		run_callback(onMessageCallback, 1);
	};
	cb.close = [onCloseCallback](bool isClosedByRemote, bool isFullyDisconnected)
	{
		stackPushBool(isFullyDisconnected);
		stackPushBool(isClosedByRemote);
		run_callback(onCloseCallback, 2);
	};
	cb.error = [onErrorCallback](const std::string &error)
	{
		stackPushString(error.c_str());
		run_callback(onErrorCallback, 1);
	};

	int id = client().ws_open(url, headers, reconnectDelayMs, pingIntervalMs, allowInsecure, std::move(cb));
	if ( id < 0 )
		stackError("webSocketConnect() has no free websocket client slots available");
	stackPushInt(id);
}

/*
 * webSocketSendText(connectionId, message)
 * Sends a TEXT frame. Returns true if queued, false if not connected.
 */
void gsc_websocket_sendtext()
{
	int idx;
	const char *message;

	if ( Scr_GetNumParam() != 2 || !stackGetParamInt(0, &idx) || !stackGetParamString(1, &message) )
	{
		stackError("webSocketSendText() takes 2 arguments: connectionId, message");
		stackPushBool(false);
		return;
	}

	stackPushBool(client().ws_send(idx, message));
}

/*
 * webSocketClose(connectionId)
 * Closes the connection (no auto-reconnect). Returns true if the id was valid.
 */
void gsc_websocket_close()
{
	int idx;

	if ( Scr_GetNumParam() != 1 || !stackGetParamInt(0, &idx) )
	{
		stackError("webSocketClose() takes 1 argument: connectionId");
		stackPushBool(false);
		return;
	}

	stackPushBool(client().ws_close(idx));
}

/*
 * The functions below drive the client shared by httpFetch and webSocket*:
 * once per server frame, on every level load and map_restart, and on quit.
 */
void gsc_http_poll()
{
	client().poll();
}

void gsc_http_level_changed()
{
	client().level_changed();
}

void gsc_http_shutdown()
{
	client().shutdown();
}

#endif
