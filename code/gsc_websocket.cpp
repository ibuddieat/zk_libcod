#include "config.hpp"

#if COMPILE_WEBSOCKET == 1

// Include the WebSocket client (and its libwebsockets + STL dependencies) before
// gsc.hpp: gsc.hpp -> declarations.hpp #defines `level` as a macro, which would
// otherwise corrupt identifiers named `level` inside the STL headers.
#include "websocket.hpp"

#include "gsc_maxstring.hpp"
#include "gsc_websocket.hpp"

// Maximum number of simultaneous websocket clients
#define MAX_WEBSOCKET_CLIENTS 16

// Active connections; a NULL slot is free
static WebSocketClient *websocket_clients[MAX_WEBSOCKET_CLIENTS] = { NULL };

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
 *
 * Connections are owned by the level that opened them: the callback handles
 * die with the level's scripts, so all connections are closed automatically on
 * map change or restart. Open them again from the new level's init.
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

	if ( Scr_GetNumParam() >= 7 && !stackGetParamInt(6, &reconnectDelayMs) )
	{
		stackError("webSocketConnect() reconnectDelayMs must be an integer");
		stackPushInt(-1);
		return;
	}

	if ( Scr_GetNumParam() >= 8 && !stackGetParamInt(7, &pingIntervalMs) )
	{
		stackError("webSocketConnect() pingIntervalMs must be an integer");
		stackPushInt(-1);
		return;
	}

	// The client stores these as unsigned; a negative value would wrap to a
	// near-infinite delay, leaving a failed connection unreapable. Clamp to 0
	// (0 = disable auto-reconnect / disable pings).
	if ( reconnectDelayMs < 0 )
		reconnectDelayMs = 0;
	if ( pingIntervalMs < 0 )
		pingIntervalMs = 0;

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

	WebSocketClient *client = new WebSocketClient(headers, reconnectDelayMs, pingIntervalMs);

	// Callback function handles belong to the level's loaded scripts; remember
	// which level opened the connection so events arriving after a map change
	// are not delivered to stale handles (the connections themselves are also
	// closed on level load, see gsc_websocket_shutdown)
	unsigned int levelId = scrVarPub.levelId;

	client->onOpen([onConnectCallback, levelId]()
	{
		if ( onConnectCallback && Scr_IsSystemActive() && scrVarPub.levelId == levelId )
		{
			short threadId = Scr_ExecThread(onConnectCallback, 0);
			Scr_FreeThread(threadId);
		}
	});
	client->onMessage([onMessageCallback, levelId](const std::string &message)
	{
		if ( onMessageCallback && Scr_IsSystemActive() && scrVarPub.levelId == levelId )
		{
			// A frame too large to intern as a script string would crash the
			// server, so drop it rather than deliver it (see GSC_MAX_STRING_BYTES).
			if ( message.size() > GSC_MAX_STRING_BYTES )
			{
				Com_Printf("webSocket: dropped oversized text frame (%u bytes)\n", (unsigned int)message.size());
				return;
			}

			stackPushString(message.c_str());
			short threadId = Scr_ExecThread(onMessageCallback, 1);
			Scr_FreeThread(threadId);
		}
	});
	client->onClose([onCloseCallback, levelId](bool isClosedByRemote, bool isFullyDisconnected)
	{
		if ( onCloseCallback && Scr_IsSystemActive() && scrVarPub.levelId == levelId )
		{
			stackPushBool(isFullyDisconnected);
			stackPushBool(isClosedByRemote);
			short threadId = Scr_ExecThread(onCloseCallback, 2);
			Scr_FreeThread(threadId);
		}
	});
	client->onError([onErrorCallback, levelId](const std::string &error)
	{
		if ( onErrorCallback && Scr_IsSystemActive() && scrVarPub.levelId == levelId )
		{
			stackPushString(error.c_str());
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
	const char *message;

	if ( Scr_GetNumParam() != 2 )
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

	stackPushBool(websocket_clients[idx]->sendText(message));
}

/*
 * webSocketClose(connectionId)
 * Requests connection close (disables auto-reconnect). Returns true if the id
 * was valid.
 */
void gsc_websocket_close()
{
	int idx;

	if ( Scr_GetNumParam() != 1 || !stackGetParamInt(0, &idx) )
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
 * Closes and frees all websocket connections. Called on server shutdown and on
 * level load (a new level's scripts cannot own the previous level's
 * connections). The per-frame poll no longer runs in either case, so poll
 * synchronously to flush the close frames (max ~1s).
 */
void gsc_websocket_shutdown()
{
	int i;

	for ( i = 0; i < MAX_WEBSOCKET_CLIENTS; i++ )
	{
		if ( websocket_clients[i] )
			websocket_clients[i]->close();
	}

	for ( int attempt = 0; attempt < 10; attempt++ )
	{
		bool anyActive = false;
		for ( i = 0; i < MAX_WEBSOCKET_CLIENTS; i++ )
		{
			if ( websocket_clients[i] && !websocket_clients[i]->isDisconnected() )
			{
				websocket_clients[i]->poll(10);
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
