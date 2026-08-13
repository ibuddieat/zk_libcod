#ifndef WEBSOCKET_HPP
#define WEBSOCKET_HPP

#include <libwebsockets.h>

#include <functional>
#include <string>
#include <vector>
#include <cstring>
#include <cstdint>

// Frame size ceiling before the VM string intern would fatally error (see http_client.hpp).
#define GSC_MAX_STRING_BYTES 65000

// Driven in embedded (external-poll) mode, exactly like http_client.hpp: lws hands
// us its sockets via ADD/DEL/CHANGE_MODE_POLL_FD and we run a non-blocking poll()
// each server frame, feeding ready sockets back with lws_service_fd(). lws never
// sleeps on its own event loop (which would freeze the game thread), and all
// callbacks fire on this thread at the frame boundary.
#undef poll
#include <poll.h>
#undef poll

/*
 * WebSocket client on libwebsockets (MIT). Behaviour matches the round-1 Mongoose
 * client so the script API and semantics are unchanged:
 *   - ws:// and wss:// (wss is encrypt-only via mbedTLS: the connection is
 *     encrypted but the certificate is not verified, so the .so needs no system
 *     CA file - same as round-1 Mongoose)
 *   - auto-reconnect on error/remote close (reconnect_delay_ms 0 disables it)
 *   - periodic PING keepalive with a PONG watchdog (ping_interval_ms 0 disables)
 *   - poll-driven: call poll() every server frame; callbacks fire from within it
 *
 * lws requires all writes to happen from a WRITEABLE callback, so outgoing TEXT
 * frames, PINGs and the CLOSE are queued and flushed there. Incoming PING/CLOSE
 * are answered by lws itself; we only track the resulting state.
 */
class WebSocketClient
{
  public:
	using OnOpen = std::function<void()>;
	using OnMessage = std::function<void(const std::string &)>;
	using OnClose = std::function<void(bool isClosedByRemote, bool isFullyDisconnected)>;
	using OnError = std::function<void(const std::string &)>;

	/*
	 * reconnect_delay_ms: delay before reconnecting after a drop; 0 = connect once,
	 * no retry. ping_interval_ms: keepalive PING interval; 0 = disabled.
	 * pong_timeout_ms: max wait for a PONG after our PING; 0 = auto (half the ping
	 * interval; disabled when pings are disabled).
	 */
	WebSocketClient(std::string headers = "", unsigned reconnect_delay_ms = 2000,
	                unsigned ping_interval_ms = 15000, unsigned pong_timeout_ms = 0)
	{
		m_headers = std::move(headers);
		m_reconnect_ms = reconnect_delay_ms;
		m_ping_interval_ms = ping_interval_ms;
		m_pong_timeout_ms = pong_timeout_ms ? pong_timeout_ms : ( ping_interval_ms ? ping_interval_ms / 2 : 0 );

		lws_set_log_level(0, NULL);

		memset(m_protocols, 0, sizeof(m_protocols));
		m_protocols[0].name = "ws";
		m_protocols[0].callback = &WebSocketClient::lws_cb;
		m_protocols[0].rx_buffer_size = 4096;

		struct lws_context_creation_info info;
		memset(&info, 0, sizeof(info));
		info.port = CONTEXT_PORT_NO_LISTEN;
		info.protocols = m_protocols;
		// Required for client TLS setup. No CA trust store: TLS is encrypt-only (the
		// connection flags in try_connect_now skip certificate verification), so the
		// .so is self-contained with no system cert file, matching round-1 Mongoose.
		info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
		info.user = this;

		m_ctx = lws_create_context(&info);
	}

	~WebSocketClient()
	{
		if ( m_ctx )
			lws_context_destroy(m_ctx);
	}

	// Start (or replace) a connection to a ws:// or wss:// URL
	bool connect(const std::string &url)
	{
		m_url = url;
		m_disconnect = false;
		m_localClose = false;
		return try_connect_now();
	}

	// Drive networking; call once per server frame
	void poll(int ms = 0)
	{
		uint64_t now = now_ms();

		// Deliver an error from a connect that failed synchronously inside connect().
		// Deferred to here so the script callback never runs re-entrantly inside the
		// webSocketConnect builtin (which would corrupt the VM stack).
		if ( m_pendingError )
		{
			m_pendingError = false;
			if ( m_onError )
				m_onError(m_pendingErrorMsg);
		}

		// Reconnect timer (only when auto-reconnect is enabled)
		if ( m_reconnect_ms > 0 && !m_wsi && !m_disconnect && !m_url.empty() && now >= m_nextReconnect )
			try_connect_now();

		// Handshake watchdog (works even when auto-reconnect is disabled)
		if ( m_wsi && !m_connected && !m_disconnect && now >= m_connectDeadline )
			fail_connection("Connect timeout");

		// Keepalive PING
		if ( m_wsi && m_connected && !m_closing && m_ping_interval_ms > 0 && now >= m_nextPing )
		{
			m_wantPing = true;
			m_nextPing = now + m_ping_interval_ms;
			lws_callback_on_writable(m_wsi);
		}

		// PONG watchdog: no reply in time means the link is dead
		if ( m_wsi && m_connected && m_waitingPong && now >= m_pongDeadline )
		{
			m_waitingPong = false;
			fail_connection("Ping timeout (no pong received within time limit)");
		}

		service(ms);
	}

	// Queue a TEXT frame; false if not currently connected. lws flushes it from the
	// WRITEABLE callback armed here.
	bool sendText(const std::string &text)
	{
		if ( !m_wsi || !m_connected || m_closing )
			return false;
		m_sendQueue.push_back(text);
		lws_callback_on_writable(m_wsi);
		return true;
	}

	/*
	 * Requests a graceful close: queues a CLOSE frame (flushed after any pending
	 * TEXT) and disables auto-reconnect until connect() is called again.
	 */
	void close()
	{
		m_disconnect = true;
		m_localClose = true;
		if ( m_wsi && !m_closing )
		{
			m_closing = true;
			m_wantClose = true;
			lws_callback_on_writable(m_wsi);
		}
	}

	void onOpen(OnOpen cb) { m_onOpen = std::move(cb); }
	void onMessage(OnMessage cb) { m_onMessage = std::move(cb); }
	void onClose(OnClose cb) { m_onClose = std::move(cb); }
	void onError(OnError cb) { m_onError = std::move(cb); }

	bool isConnected() const { return m_connected; }
	// Fully done: no connection and no reconnect pending; safe to delete
	bool isDisconnected() const { return !m_wsi && !m_connected && !m_closing && m_disconnect; }

  private:
	static uint64_t now_ms() { return (uint64_t)(lws_now_usecs() / 1000); }

	// Report an error at the frame boundary and force the connection closed. The
	// kill's follow-up CLIENT_CONNECTION_ERROR must not report the error a second
	// time, so it is suppressed once.
	void fail_connection(const std::string &msg)
	{
		m_closing = true;
		if ( m_onError )
			m_onError(msg);
		m_suppressError = true;
		if ( m_wsi )
			lws_set_timeout(m_wsi, (enum pending_timeout)1, LWS_TO_KILL_ASYNC);
	}

	bool try_connect_now()
	{
		uint64_t now = now_ms();
		m_nextReconnect = now + m_reconnect_ms;
		m_connectDeadline = now + m_connectTimeoutMs;
		m_rxBuf.clear();
		m_rxOversized = false;
		m_sendQueue.clear();
		m_wantPing = false;
		m_wantClose = false;
		m_waitingPong = false;
		m_suppressError = false;

		// lws_parse_uri mutates its buffer, so parse a private copy.
		m_urlbuf = m_url;
		const char *prot = NULL, *address = NULL, *path = NULL;
		int port = 0;
		if ( lws_parse_uri(&m_urlbuf[0], &prot, &address, &port, &path) )
		{
			if ( m_reconnect_ms == 0 )
				m_disconnect = true;
			m_pendingError = true;
			m_pendingErrorMsg = "Failed to parse URL";
			return false;
		}
		bool ssl = ( prot && ( strcmp(prot, "wss") == 0 || strcmp(prot, "https") == 0 ) );
		if ( port == 0 )                        // lws_parse_uri does not infer it
			port = ssl ? 443 : 80;
		m_host = address ? address : "";
		m_path = "/";                           // lws_parse_uri strips the leading slash
		if ( path )
			m_path += path;

		struct lws_client_connect_info i;
		memset(&i, 0, sizeof(i));
		i.context = m_ctx;
		i.address = m_host.c_str();
		i.port = port;
		i.path = m_path.c_str();
		i.host = m_host.c_str();
		i.origin = m_host.c_str();
		i.protocol = "ws";                      // must match m_protocols[0].name
		i.pwsi = &m_wsi;
		i.userdata = this;
		if ( ssl )
			// Encrypt-only but keep SNI on (see http_client.hpp) - ALLOW_INSECURE
			// skips verification without disabling the SNI hostname that SNI-strict
			// servers require.
			i.ssl_connection = LCCSCF_USE_SSL | LCCSCF_ALLOW_INSECURE;
		// No i.method -> lws performs a WebSocket upgrade, not a plain HTTP request.

		// lws_client_connect_via_info may fire CLIENT_CONNECTION_ERROR synchronously
		// (socket/DNS setup failure); m_connecting tells the handler to defer that
		// callback instead of running it re-entrantly in the builtin.
		m_connecting = true;
		struct lws *w = lws_client_connect_via_info(&i);
		m_connecting = false;

		if ( !w )
		{
			if ( m_reconnect_ms == 0 )
				m_disconnect = true;
			m_pendingError = true;
			m_pendingErrorMsg = "Failed to connect";
			return false;
		}
		return true;
	}

	// External-poll drive (identical model to http_client.hpp). wait_ms is 0 on the
	// per-frame call (non-blocking) and >0 only for a shutdown drain that wants to
	// briefly wait for the CLOSE frame to flush.
	void service(int wait_ms)
	{
		if ( !m_ctx )
			return;

		if ( lws_service_adjust_timeout(m_ctx, 1, 0) == 0 )
			lws_service_tsi(m_ctx, -1, 0);

		if ( m_pollfds.empty() )
			return;

		int n = ::poll(m_pollfds.data(), (nfds_t)m_pollfds.size(), wait_ms);
		if ( n <= 0 )
			return;

		// A serviced fd may add/remove entries via the poll-fd callbacks, so iterate
		// a snapshot of the fds that had activity, not the live table.
		std::vector<struct pollfd> ready;
		for ( struct pollfd &p : m_pollfds )
			if ( p.revents )
				ready.push_back(p);
		for ( struct pollfd &p : ready )
		{
			struct lws_pollfd lp;
			lp.fd = p.fd;
			lp.events = p.events;
			lp.revents = p.revents;
			lws_service_fd(m_ctx, &lp);
		}
	}

	static int lws_cb(struct lws *wsi, enum lws_callback_reasons reason,
	                  void *user, void *in, size_t len)
	{
		(void)user;
		WebSocketClient *self = wsi ? (WebSocketClient *)lws_context_user(lws_get_context(wsi)) : NULL;

		switch ( reason )
		{
		// --- Embedded event-loop integration: watch/unwatch lws's sockets ---
		case LWS_CALLBACK_ADD_POLL_FD:
		{
			struct lws_pollargs *pa = (struct lws_pollargs *)in;
			if ( self && pa )
				self->add_pollfd(pa->fd, pa->events);
			break;
		}
		case LWS_CALLBACK_DEL_POLL_FD:
		{
			struct lws_pollargs *pa = (struct lws_pollargs *)in;
			if ( self && pa )
				self->del_pollfd(pa->fd);
			break;
		}
		case LWS_CALLBACK_CHANGE_MODE_POLL_FD:
		{
			struct lws_pollargs *pa = (struct lws_pollargs *)in;
			if ( self && pa )
				self->change_pollfd(pa->fd, pa->events);
			break;
		}

		case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
		{
			// Inject any extra handshake headers (same \r\n-separated format as HTTP).
			if ( self && !self->m_headers.empty() )
			{
				uint8_t **p = (uint8_t **)in;
				uint8_t *end = (*p) + len;
				const std::string &h = self->m_headers;
				size_t start = 0;
				while ( start < h.size() )
				{
					size_t nl = h.find("\r\n", start);
					std::string line = h.substr(start, nl == std::string::npos ? std::string::npos : nl - start);
					start = ( nl == std::string::npos ) ? h.size() : nl + 2;
					size_t colon = line.find(':');
					if ( colon == std::string::npos )
						continue;
					std::string name = line.substr(0, colon);
					std::string value = line.substr(colon + 1);
					while ( !value.empty() && ( value.front() == ' ' || value.front() == '\t' ) )
						value.erase(value.begin());
					if ( name.empty() )
						continue;
					name += ':';
					if ( lws_add_http_header_by_name(wsi, (const uint8_t *)name.c_str(),
					                                 (const uint8_t *)value.c_str(), (int)value.size(), p, end) )
						return -1;
				}
			}
			break;
		}

		case LWS_CALLBACK_CLIENT_ESTABLISHED:
		{
			if ( self )
			{
				self->m_connected = true;
				self->m_waitingPong = false;
				self->m_closing = false;
				if ( self->m_ping_interval_ms > 0 )
					self->m_nextPing = now_ms() + self->m_ping_interval_ms;
				if ( self->m_onOpen )
					self->m_onOpen();
			}
			break;
		}

		case LWS_CALLBACK_CLIENT_RECEIVE:
		{
			if ( !self )
				break;
			// TEXT only; ignore binary frames entirely.
			if ( lws_frame_is_binary(wsi) )
				break;

			// Bound reassembly: a persistent peer could otherwise stream an endless
			// "message". Beyond the deliverable limit we stop buffering and drop it.
			if ( !self->m_rxOversized )
			{
				if ( self->m_rxBuf.size() + len > GSC_MAX_STRING_BYTES )
				{
					self->m_rxOversized = true;
					self->m_rxBuf.clear();
				}
				else
					self->m_rxBuf.append((const char *)in, len);
			}

			if ( lws_is_final_fragment(wsi) && lws_remaining_packet_payload(wsi) == 0 )
			{
				// Oversized messages were never buffered past the cap; drop them
				// silently (delivering would exceed the script string limit anyway).
				if ( !self->m_rxOversized && self->m_onMessage )
					self->m_onMessage(self->m_rxBuf);
				self->m_rxBuf.clear();
				self->m_rxOversized = false;
			}
			break;
		}

		case LWS_CALLBACK_CLIENT_RECEIVE_PONG:
		{
			if ( self )
				self->m_waitingPong = false;
			break;
		}

		case LWS_CALLBACK_CLIENT_WRITEABLE:
		{
			if ( !self )
				break;

			// Flush queued TEXT first (one frame per writeable, re-arming for more),
			// then a pending PING, then a graceful CLOSE.
			if ( !self->m_sendQueue.empty() )
			{
				std::string msg = self->m_sendQueue.front();
				self->m_sendQueue.erase(self->m_sendQueue.begin());

				std::vector<unsigned char> buf(LWS_PRE + msg.size());
				if ( !msg.empty() )
					memcpy(&buf[LWS_PRE], msg.data(), msg.size());
				int wrote = lws_write(wsi, &buf[LWS_PRE], msg.size(), LWS_WRITE_TEXT);
				if ( wrote < (int)msg.size() )
					return -1;

				if ( !self->m_sendQueue.empty() || self->m_wantPing || self->m_wantClose )
					lws_callback_on_writable(wsi);
				break;
			}

			if ( self->m_wantPing )
			{
				self->m_wantPing = false;
				unsigned char buf[LWS_PRE + 8];
				lws_write(wsi, &buf[LWS_PRE], 0, LWS_WRITE_PING);
				self->m_waitingPong = ( self->m_pong_timeout_ms > 0 );
				if ( self->m_waitingPong )
					self->m_pongDeadline = now_ms() + self->m_pong_timeout_ms;
				if ( self->m_wantClose )
					lws_callback_on_writable(wsi);
				break;
			}

			if ( self->m_wantClose )
			{
				self->m_wantClose = false;
				lws_close_reason(wsi, LWS_CLOSE_STATUS_NORMAL, NULL, 0);
				return -1;   // send CLOSE and tear down; CLIENT_CLOSED follows
			}
			break;
		}

		case LWS_CALLBACK_WS_PEER_INITIATED_CLOSE:
		{
			// Peer sent a CLOSE frame; lws echoes it. Just note the closing state -
			// closedByRemote is derived from m_localClose in CLIENT_CLOSED.
			if ( self )
				self->m_closing = true;
			break;
		}

		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
		{
			if ( !self )
				break;
			if ( wsi == self->m_wsi )
				self->m_wsi = NULL;

			std::string msg = in ? std::string((const char *)in, len ? len : strlen((const char *)in))
			                     : std::string("connection error");

			if ( self->m_reconnect_ms == 0 )
				self->m_disconnect = true;
			else
				self->m_nextReconnect = now_ms() + self->m_reconnect_ms;

			if ( self->m_suppressError )
				self->m_suppressError = false;      // already reported by fail_connection()
			else if ( self->m_connecting )
			{
				self->m_pendingError = true;        // synchronous connect error -> defer
				self->m_pendingErrorMsg = msg;
			}
			else if ( self->m_onError )
				self->m_onError(msg);
			break;
		}

		case LWS_CALLBACK_CLIENT_CLOSED:
		{
			if ( !self )
				break;
			if ( wsi == self->m_wsi )
				self->m_wsi = NULL;

			bool closedByRemote = !self->m_localClose;
			bool wasConnected = self->m_connected;
			self->m_connected = false;
			self->m_closing = false;
			self->m_waitingPong = false;

			// Schedule a reconnect unless closed manually or auto-reconnect is off,
			// in which case the client is fully done.
			if ( !self->m_disconnect && !self->m_url.empty() && self->m_reconnect_ms > 0 )
				self->m_nextReconnect = now_ms() + self->m_reconnect_ms;
			else
				self->m_disconnect = true;

			if ( wasConnected && self->m_onClose )
				self->m_onClose(closedByRemote, self->m_disconnect);
			break;
		}

		default:
			break;
		}

		return 0;
	}

	// --- Embedded pollfd table, updated by the ADD/DEL/CHANGE_MODE callbacks ---
	void add_pollfd(int fd, int events)
	{
		for ( struct pollfd &p : m_pollfds )
		{
			if ( p.fd == fd )
			{
				p.events = (short)events;
				p.revents = 0;
				return;
			}
		}
		struct pollfd p;
		p.fd = fd;
		p.events = (short)events;
		p.revents = 0;
		m_pollfds.push_back(p);
	}

	void del_pollfd(int fd)
	{
		for ( size_t i = 0; i < m_pollfds.size(); i++ )
		{
			if ( m_pollfds[i].fd == fd )
			{
				m_pollfds.erase(m_pollfds.begin() + i);
				return;
			}
		}
	}

	void change_pollfd(int fd, int events)
	{
		for ( struct pollfd &p : m_pollfds )
		{
			if ( p.fd == fd )
			{
				p.events = (short)events;
				return;
			}
		}
	}

	struct lws_context *m_ctx = NULL;
	struct lws *m_wsi = NULL;
	struct lws_protocols m_protocols[2];
	std::vector<struct pollfd> m_pollfds;

	bool m_connected = false;
	bool m_disconnect = false;
	bool m_closing = false;
	bool m_localClose = false;
	bool m_connecting = false;
	bool m_pendingError = false;
	bool m_suppressError = false;
	std::string m_pendingErrorMsg;

	std::string m_url;
	std::string m_urlbuf;
	std::string m_host;
	std::string m_path;
	std::string m_headers;

	std::vector<std::string> m_sendQueue;
	bool m_wantPing = false;
	bool m_wantClose = false;

	std::string m_rxBuf;
	bool m_rxOversized = false;

	unsigned m_reconnect_ms;
	unsigned m_ping_interval_ms;
	unsigned m_pong_timeout_ms = 0;
	unsigned m_connectTimeoutMs = 10000;
	uint64_t m_nextReconnect = 0;
	uint64_t m_connectDeadline = 0;
	uint64_t m_nextPing = 0;
	bool m_waitingPong = false;
	uint64_t m_pongDeadline = 0;

	OnOpen m_onOpen;
	OnMessage m_onMessage;
	OnClose m_onClose;
	OnError m_onError;
};

#endif
