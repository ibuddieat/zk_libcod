#include "config.hpp"

#if COMPILE_HTTP == 1

// Include the self-contained WebSocket client (and its Mongoose/OpenSSL/STL
// deps) BEFORE gsc.hpp. gsc.hpp -> declarations.hpp #defines `level` as a
// macro, which otherwise mangles identifiers named `level` inside the system
// headers (e.g. OpenSSL's SSL_set_security_level, libstdc++ string_view).
#include "websocket.hpp"

#include "gsc_websocket.hpp"

// Maximum number of simultaneous websocket clients
#define MAX_WEBSOCKET_CLIENTS 16

// Active connections; a NULL slot is free
static WebSocketClient *websocket_clients[MAX_WEBSOCKET_CLIENTS] = {NULL};

/*
 * webSocketConnect(url, headers, ::onConnect, ::onMessage, ::onClose, ::onError, [reconnectDelayMs], [pingIntervalMs])
 *
 * Opens a ws:// or wss:// connection. Returns the connection id (slot index)
 * or -1 on error. Callbacks:
 *   onConnect()                              - handshake completed
 *   onMessage(text)                          - a TEXT frame was received
 *   onClose(closedByRemote, fullyDisconnected)
 *   onError(error)
 * Headers are separated by \r\n. reconnectDelayMs defaults to 2000 (0 disables
 * auto-reconnect: connect once, no retry on close/error). pingIntervalMs
 * defaults to 15000 (0 disables keepalive pings).
 */
void gsc_websocket_connect()
{
	char *url;
	char *headers;
	int onConnectCallback;
	int onMessageCallback;
	int onCloseCallback;
	int onErrorCallback;
	int idx;
	int i;

	if ( Scr_GetNumParam() < 6 )
	{
		stackError("webSocketConnect() takes at least 6 arguments: url, headers, onConnect, onMessage, onClose, onError");
		stackPushInt(-1);
		return;
	}

	if ( !stackGetParamString(0, &url) ||
	     !stackGetParamString(1, &headers) ||
	     !stackGetParamFunction(2, &onConnectCallback) ||
	     !stackGetParamFunction(3, &onMessageCallback) ||
	     !stackGetParamFunction(4, &onCloseCallback) ||
	     !stackGetParamFunction(5, &onErrorCallback) )
	{
		stackError("webSocketConnect() has an undefined or wrong-typed argument");
		stackPushInt(-1);
		return;
	}

	idx = -1;
	for ( i = 0; i < MAX_WEBSOCKET_CLIENTS; i++ )
	{
		if ( websocket_clients[i] == NULL )
		{
			idx = i;
			break;
		}
	}
	if ( idx == -1 )
	{
		stackError("webSocketConnect() has no free websocket client slots available");
		stackPushInt(-1);
		return;
	}

	WebSocketClient *client;

	if ( Scr_GetNumParam() >= 8 )
	{
		int reconnectDelayMs;
		int pingIntervalMs;

		if ( !stackGetParamInt(6, &reconnectDelayMs) || !stackGetParamInt(7, &pingIntervalMs) )
		{
			stackError("webSocketConnect() reconnectDelayMs and pingIntervalMs must be integers");
			stackPushInt(-1);
			return;
		}

		client = new WebSocketClient(headers, reconnectDelayMs, pingIntervalMs);
	}
	else
		client = new WebSocketClient(headers);

	client->onOpen([onConnectCallback]()
	{
		if ( onConnectCallback && Scr_IsSystemActive() )
		{
			short threadId = Scr_ExecThread(onConnectCallback, 0);
			Scr_FreeThread(threadId);
		}
	});
	client->onMessage([onMessageCallback](const std::string &message)
	{
		if ( onMessageCallback && Scr_IsSystemActive() )
		{
			stackPushString((char *)message.c_str());
			short threadId = Scr_ExecThread(onMessageCallback, 1);
			Scr_FreeThread(threadId);
		}
	});
	client->onClose([onCloseCallback](bool isClosedByRemote, bool isFullyDisconnected)
	{
		if ( onCloseCallback && Scr_IsSystemActive() )
		{
			stackPushBool(isFullyDisconnected);
			stackPushBool(isClosedByRemote);
			short threadId = Scr_ExecThread(onCloseCallback, 2);
			Scr_FreeThread(threadId);
		}
	});
	client->onError([onErrorCallback](const std::string &error)
	{
		if ( onErrorCallback && Scr_IsSystemActive() )
		{
			stackPushString((char *)error.c_str());
			short threadId = Scr_ExecThread(onErrorCallback, 1);
			Scr_FreeThread(threadId);
		}
	});

	websocket_clients[idx] = client;
	client->connect(url);

	stackPushInt(idx);
}

/*
 * webSocketSendText(connectionId, message)
 * Sends a TEXT frame. Returns true on success, false if not connected.
 */
void gsc_websocket_sendtext()
{
	int idx;
	char *message;

	if ( Scr_GetNumParam() < 2 )
	{
		stackError("webSocketSendText() takes 2 arguments: connectionId, message");
		stackPushBool(false);
		return;
	}

	if ( !stackGetParamInt(0, &idx) || !stackGetParamString(1, &message) )
	{
		stackError("webSocketSendText() has an undefined or wrong-typed argument");
		stackPushBool(false);
		return;
	}

	if ( idx < 0 || idx >= MAX_WEBSOCKET_CLIENTS || websocket_clients[idx] == NULL )
	{
		stackPushBool(false);
		return;
	}

	bool result = websocket_clients[idx]->sendText(message);
	stackPushBool(result);
}

/*
 * webSocketClose(connectionId)
 * Requests connection close (disables auto-reconnect). Returns true if the id
 * was valid.
 */
void gsc_websocket_close()
{
	int idx;

	if ( Scr_GetNumParam() < 1 )
	{
		stackPushBool(false);
		return;
	}

	if ( !stackGetParamInt(0, &idx) )
	{
		stackPushBool(false);
		return;
	}

	if ( idx < 0 || idx >= MAX_WEBSOCKET_CLIENTS || websocket_clients[idx] == NULL )
	{
		stackPushBool(false);
		return;
	}

	websocket_clients[idx]->close();
	stackPushBool(true);
}

/* Drives all websocket connections and reaps fully-disconnected slots. */
void gsc_websocket_poll()
{
	for ( int i = 0; i < MAX_WEBSOCKET_CLIENTS; i++ )
	{
		if ( websocket_clients[i] )
		{
			websocket_clients[i]->poll();
			if ( websocket_clients[i]->isDisconnected() )
			{
				delete websocket_clients[i];
				websocket_clients[i] = NULL;
			}
		}
	}
}

/*
 * Closes and frees all websocket connections. Called on shutdown, where the
 * per-frame poll no longer runs, so we poll synchronously to flush close frames.
 */
void gsc_websocket_shutdown()
{
	int i;

	for ( i = 0; i < MAX_WEBSOCKET_CLIENTS; i++ )
	{
		if ( websocket_clients[i] )
		{
			websocket_clients[i]->close();
			websocket_clients[i]->poll(10);
		}
	}

	for ( int attempt = 0; attempt < 10; attempt++ )
	{
		bool anyActive = false;
		for ( i = 0; i < MAX_WEBSOCKET_CLIENTS; i++ )
		{
			if ( websocket_clients[i] && !websocket_clients[i]->isDisconnected() )
			{
				websocket_clients[i]->poll(100);
				anyActive = true;
			}
		}
		if ( !anyActive )
			break;
	}

	for ( i = 0; i < MAX_WEBSOCKET_CLIENTS; i++ )
	{
		if ( websocket_clients[i] )
		{
			delete websocket_clients[i];
			websocket_clients[i] = NULL;
		}
	}
}

#endif
