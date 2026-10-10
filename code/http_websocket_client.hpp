#ifndef HTTP_WEBSOCKET_CLIENT_HPP
#define HTTP_WEBSOCKET_CLIENT_HPP

/*
 * HTTP(S) and WebSocket client behind httpFetch and webSocket*.
 * One libwebsockets (lws) context, checked once per server frame without waiting.
 *
 * - lws callbacks only save state and queue events. poll() runs the script
 *   callbacks later, outside lws, with a byte limit per frame.
 * - A connection is freed only after LWS_CALLBACK_WSI_DESTROY, the last callback
 *   lws sends for it. Each result is reported once.
 * - level_changed() drops everything meant for the old level's scripts.
 */

#include <libwebsockets.h>

#include <deque>
#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <strings.h>
#include <unistd.h>

#define GSC_MAX_STRING_BYTES 65000 // Largest string passed to scripts; the engine errors out near 64 KB
#define LWS_MAX_WEBSOCKETS 16

class HttpWsClient
{
  public:
	struct Response
	{
		int status = 0;
		std::map<std::string, std::string> headers;
		std::string body;
	};
	using OnDone = std::function<void(const Response &)>;
	using OnText = std::function<void(const std::string &)>;
	using OnOpen = std::function<void()>;
	using OnClose = std::function<void(bool closedByRemote, bool fullyDisconnected)>;

	struct WsCallbacks
	{
		OnOpen open;
		OnText message;
		OnClose close;
		OnText error;
	};

	static HttpWsClient &get()
	{
		static HttpWsClient client;
		return client;
	}

	// Console print, set by the GSC side (lws itself is kept silent)
	void (*log)(const char *fmt, ...) = NULL;

	/*
	 * Starts an HTTP request. Exactly one of onDone/onError is queued, unless the
	 * level changes first. timeout_ms bounds the whole request; 0 leaves only lws's
	 * own per-phase limits. Redirects are not followed: onDone gets the 3xx.
	 */
	void fetch(const char *method, const char *url, const std::string &body, const char *headers,
	           bool insecure, int timeout_ms, OnDone onDone, OnText onError)
	{
		Req *r = new Req();
		r->onDone = std::move(onDone);
		r->onError = std::move(onError);
		r->gen = gen;
		r->method = ( method && *method ) ? method : "GET";
		r->body = body;

		if ( !parse_url(url, false, r->url) )
			r->err = "Invalid URL";
		else if ( !is_token(r->method) )
			r->err = "Invalid method";
		else if ( !parse_headers(headers, r->headers, r->err) )
			;
		else if ( !ensure_ctx() )
			r->err = "HTTP client not initialised";

		if ( !r->err.empty() )
		{
			finish(r);
			delete r;
			return;
		}

		if ( timeout_ms > 0 )
			r->deadline = now_ms() + (uint64_t)timeout_ms;

		reqs.push_back(r);
		connect(r, r->url, r->method.c_str(), insecure);
	}

	// Opens a WebSocket in a free slot; returns the slot id, or -1 if none is free
	int ws_open(const char *url, const char *headers, unsigned reconnect_ms, unsigned ping_ms,
	            bool insecure, WsCallbacks cb)
	{
		int id = 0;
		while ( id < LWS_MAX_WEBSOCKETS && slots[id] )
			id++;
		if ( id == LWS_MAX_WEBSOCKETS )
			return -1;

		Ws *w = new Ws();
		w->cb = std::move(cb);
		w->reconnect_ms = reconnect_ms;
		w->ping_ms = ping_ms;
		w->insecure = insecure;
		slots[id] = w;
		sockets.push_back(w);

		if ( !parse_url(url, true, w->url) )
			w->err = "Invalid URL";
		else if ( !parse_headers(headers, w->headers, w->err) )
			;
		else if ( !ensure_ctx() )
			w->err = "WebSocket client not initialised";

		if ( !w->err.empty() )
		{
			// Retrying cannot fix these; report once and free the slot
			w->reconnect_ms = 0;
			ws_ended(w);
		}
		else
			ws_connect(w);
		return id;
	}

	// Queues a TEXT frame; false if not connected or the send queue is full
	bool ws_send(int id, const std::string &text)
	{
		Ws *w = slot(id);
		if ( !w || !w->wsi || !w->connected || w->closing )
			return false;
		if ( w->sendq_bytes + text.size() > WS_SENDQ_MAX )
			return false;
		w->sendq.push_back(text);
		w->sendq_bytes += text.size();
		lws_callback_on_writable(w->wsi);
		return true;
	}

	// Graceful close; no reconnect, no further onError. False if the id is unknown.
	bool ws_close(int id)
	{
		Ws *w = slot(id);
		if ( !w )
			return false;
		ws_shut(w);
		return true;
	}

	// Called once per server frame
	void poll()
	{
		if ( ctx && ( !reqs.empty() || !sockets.empty() ) )
		{
			// -1 = don't wait (lws unix-service.c:107). Sockets before timers, so a reply that already arrived wins.
			lws_service(ctx, -1);
			timers();
		}
		deliver();
		reap();
	}

	// New level or map_restart: old script handles are dead, drop all of it
	void level_changed()
	{
		gen++;
		events.clear();
		ws_rx_bytes = 0;
		for ( int i = 0; i < LWS_MAX_WEBSOCKETS; i++ )
		{
			if ( !slots[i] )
				continue;
			slots[i]->detached = true;
			ws_shut(slots[i]);
			slots[i] = NULL;
		}
	}

	// Server quit: flush close frames and in-flight requests (max ~1 s), then free all
	void shutdown()
	{
		level_changed();
		quitting = true;
		for ( int i = 0; i < 100 && ctx && ( !reqs.empty() || !sockets.empty() ); i++ )
		{
			poll();
			usleep(10000);
		}
		if ( ctx )
			lws_context_destroy(ctx);
		ctx = NULL;
		// lws sends nothing after this; free what it never reported (e.g. a request still in DNS)
		for ( Req *r : reqs )
			delete r;
		for ( Ws *w : sockets )
			delete w;
		reqs.clear();
		sockets.clear();
		events.clear();
	}

  private:
	typedef std::vector<std::pair<std::string, std::string>> Headers;

	static const size_t FRAME_BUDGET = 128 * 1024;      // script-visible bytes per frame
	static const size_t EVENT_COST = 256;               // so empty events count too
	static const size_t RX_QUEUE_MAX = 4 * 1024 * 1024; // undelivered WS message bytes before a WS is killed
	static const size_t WS_SENDQ_MAX = 1024 * 1024;
	static const size_t HEADERS_MAX = 8192;             // pt_serv_buf_size below holds them
	static const unsigned WS_CONNECT_TIMEOUT_MS = 10000;
	static const unsigned WS_CLOSE_TIMEOUT_MS = 5000;    // a peer that stops reading never lets the close frame out

	struct Url
	{
		bool tls = false;
		std::string host;   // what we connect to
		std::string hosthdr; // Host header: brackets for IPv6, port if not default
		int port = 0;
		std::string path;
	};

	struct Conn
	{
		explicit Conn(bool is_ws) : ws(is_ws) {}
		bool ws;
		bool live = false;   // lws still holds this connection
		bool killed = false;
		struct lws *wsi = NULL;
		std::string err;     // reported when the connection ends
		Url url;
		Headers headers;
	};

	struct Req : Conn
	{
		Req() : Conn(false) {}
		OnDone onDone;
		OnText onError;
		unsigned gen = 0;
		std::string method;
		std::string body;
		size_t body_sent = 0;
		Response resp;
		uint64_t deadline = 0;
		bool reported = false;
	};

	struct Ws : Conn
	{
		Ws() : Conn(true) {}
		WsCallbacks cb;
		unsigned reconnect_ms = 0;
		unsigned ping_ms = 0;
		bool insecure = false;
		bool connected = false;
		bool closing = false;   // script or level asked to close
		bool detached = false;  // level changed: report nothing
		bool finished = false;  // no reconnect coming; reap when the wsi is gone
		std::deque<std::string> sendq;
		size_t sendq_bytes = 0;
		bool want_ping = false;
		bool want_close = false;
		bool waiting_pong = false;
		std::string rx;
		bool rx_drop = false;
		uint64_t next_reconnect = 0;
		uint64_t connect_deadline = 0;
		uint64_t close_deadline = 0;
		uint64_t next_ping = 0;
		uint64_t pong_deadline = 0;
	};

	struct Event
	{
		size_t bytes;
		bool ws_message;
		std::function<void()> fn;
	};

	HttpWsClient() { memset(slots, 0, sizeof(slots)); }

	static uint64_t now_ms() { return (uint64_t)(lws_now_usecs() / 1000); }

	bool ensure_ctx()
	{
		if ( ctx )
			return true;
		if ( quitting )
			return false;

		// CA bundle paths of the common distros (Debian/Ubuntu, Fedora/RHEL, openSUSE, OpenELEC, CentOS, Alpine)
		static const char *bundles[] = {
			"/etc/ssl/certs/ca-certificates.crt",
			"/etc/pki/tls/certs/ca-bundle.crt",
			"/etc/ssl/ca-bundle.pem",
			"/etc/pki/tls/cacert.pem",
			"/etc/pki/ca-trust/extracted/pem/tls-ca-bundle.pem",
			"/etc/ssl/cert.pem",
		};
		const char *ca = NULL;
		for ( const char *b : bundles )
		{
			if ( access(b, R_OK) == 0 )
			{
				ca = b;
				break;
			}
		}

		lws_set_log_level(0, NULL);
		memset(protocols, 0, sizeof(protocols));
		protocols[0].name = "lws";
		protocols[0].callback = &HttpWsClient::callback;

		struct lws_context_creation_info info;
		memset(&info, 0, sizeof(info));
		info.port = CONTEXT_PORT_NO_LISTEN;
		info.protocols = protocols;
		info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
		info.max_http_header_data = 16384;   // reply headers (lws default 4096 is too small)
		info.pt_serv_buf_size = 16384;       // request headers are built in this buffer
		if ( ca )
			info.client_ssl_ca_filepath = ca;
		else if ( log )
			log("http: no CA bundle found (e.g. /etc/ssl/certs/ca-certificates.crt), https:// and wss:// certificate checks will fail\n");

		ctx = lws_create_context(&info);
		if ( !ctx && log )
			log("http: could not start libwebsockets\n");
		return ctx != NULL;
	}

	// scheme://host[:port][/path]. Scheme is case-insensitive; no userinfo.
	static bool parse_url(const char *s, bool ws, Url &u)
	{
		if ( !s )
			return false;
		for ( const char *q = s; *q; q++ )
		{
			unsigned char ch = (unsigned char)*q;
			if ( ch <= 32 || ch > 126 || strchr("\"`<>\\", ch) )
				return false;
		}

		const char *p = strstr(s, "://");
		if ( !p )
			return false;
		std::string scheme(s, p - s);
		for ( char &ch : scheme )
			ch = (char)tolower((unsigned char)ch);
		if ( scheme == ( ws ? "ws" : "http" ) )
			u.tls = false;
		else if ( scheme == ( ws ? "wss" : "https" ) )
			u.tls = true;
		else
			return false;
		p += 3;

		bool v6 = ( *p == '[' );
		const char *h = v6 ? p + 1 : p;
		const char *e = h;
		while ( *e && ( v6 ? *e != ']' : !strchr(":/?#", *e) ) )
		{
			if ( !( isalnum((unsigned char)*e) || strchr(v6 ? ":." : ".-_", *e) ) )
				return false;
			e++;
		}
		if ( e == h || ( v6 && *e != ']' ) )
			return false;
		u.host.assign(h, e - h);
		p = v6 ? e + 1 : e;

		int def = u.tls ? 443 : 80;
		u.port = def;
		if ( *p == ':' )
		{
			long n = 0;
			const char *d = ++p;
			while ( isdigit((unsigned char)*p) && n <= 65535 )
				n = n * 10 + ( *p++ - '0' );
			if ( p == d || n < 1 || n > 65535 )
				return false;
			u.port = (int)n;
		}

		if ( *p && !strchr("/?#", *p) )
			return false;
		u.path = ( *p == '/' ) ? p : std::string("/") + p;
		size_t frag = u.path.find('#');
		if ( frag != std::string::npos )
			u.path.resize(frag);

		u.hosthdr = v6 ? "[" + u.host + "]" : u.host;
		if ( u.port != def )
			u.hosthdr += ":" + std::to_string(u.port);
		return true;
	}

	// RFC 9110 token: methods and header names
	static bool is_token(const std::string &s)
	{
		if ( s.empty() )
			return false;
		for ( char c : s )
			if ( !isalnum((unsigned char)c) && !strchr("!#$%&'*+-.^_`|~", c) )
				return false;
		return true;
	}

	// Headers lws writes itself; a second copy would conflict (two Content-Length values, a broken upgrade)
	static bool is_reserved_header(std::string name)
	{
		for ( char &ch : name )
			ch = (char)tolower((unsigned char)ch);
		return name == "host" || name == "content-length" || name == "transfer-encoding" ||
		       name == "connection" || name == "upgrade" || name.compare(0, 14, "sec-websocket-") == 0;
	}

	// "Name: value" lines split by \r\n. A stray CR, LF or control byte is an error (header injection).
	static bool parse_headers(const char *s, Headers &out, std::string &err)
	{
		size_t total = 0;
		std::string all = s ? s : "";
		size_t start = 0;
		while ( start < all.size() )
		{
			size_t nl = all.find("\r\n", start);
			std::string line = all.substr(start, nl == std::string::npos ? std::string::npos : nl - start);
			start = ( nl == std::string::npos ) ? all.size() : nl + 2;

			for ( char c : line )
			{
				if ( ( (unsigned char)c < 32 && c != '\t' ) || c == 127 )
				{
					err = "Invalid headers";
					return false;
				}
			}
			size_t colon = line.find(':');
			if ( colon == std::string::npos || colon == 0 )
				continue;
			std::string name = line.substr(0, colon);
			if ( !is_token(name) )
			{
				err = "Invalid headers";
				return false;
			}
			if ( is_reserved_header(name) )
			{
				err = "Header not allowed: " + name;
				return false;
			}
			size_t v = line.find_first_not_of(" \t", colon + 1);
			std::string value = ( v == std::string::npos ) ? "" : line.substr(v);
			total += name.size() + value.size() + 4;
			out.emplace_back(name + ":", value);   // lws wants the colon on the name
		}
		if ( total > HEADERS_MAX )
		{
			err = "Request headers too large";
			return false;
		}
		return true;
	}

	void connect(Conn *c, const Url &u, const char *method, bool insecure)
	{
		struct lws_client_connect_info i;
		memset(&i, 0, sizeof(i));
		i.context = ctx;
		i.address = u.host.c_str();
		i.port = u.port;
		i.path = u.path.c_str();
		i.host = u.hosthdr.c_str();
		i.origin = u.hosthdr.c_str();
		for ( const auto &h : c->headers )
			if ( strcasecmp(h.first.c_str(), "origin:") == 0 )
				i.origin = NULL;   // the script sets its own
		i.method = method;                // NULL = WebSocket upgrade
		i.local_protocol_name = "lws";    // bind our callback; no Sec-WebSocket-Protocol
		i.opaque_user_data = c;           // read back in callback()
		i.ssl_connection = LCCSCF_HTTP_NO_FOLLOW_REDIRECT;
		if ( u.tls )
			i.ssl_connection |= LCCSCF_USE_SSL | ( insecure ? LCCSCF_ALLOW_INSECURE : 0 );

		// On failure lws may already have sent WSI_DESTROY, which clears live
		c->live = true;
		c->killed = false;
		c->wsi = lws_client_connect_via_info(&i);
		if ( !c->wsi && c->live )
		{
			if ( c->err.empty() )
				c->err = "Failed to connect";
			ended(c);
		}
	}

	void ws_connect(Ws *w)
	{
		w->next_reconnect = 0;
		w->connect_deadline = now_ms() + WS_CONNECT_TIMEOUT_MS;
		connect(w, w->url, NULL, w->insecure);
	}

	// Asks lws to close the wsi on its next service, outside any callback
	void kill(Conn *c, const char *msg)
	{
		if ( !c->wsi || c->killed )
			return;
		c->killed = true;
		if ( msg && c->err.empty() )
			c->err = msg;
		lws_set_timeout(c->wsi, PENDING_TIMEOUT_USER_OK, LWS_TO_KILL_ASYNC);
	}

	void ws_shut(Ws *w)
	{
		w->closing = true;
		if ( !w->wsi )
			w->finished = true;
		else if ( w->connected )
		{
			w->want_close = true;
			if ( !w->close_deadline )   // a repeated close must not push it back
				w->close_deadline = now_ms() + WS_CLOSE_TIMEOUT_MS;
			lws_callback_on_writable(w->wsi);
		}
		else
			kill(w, NULL);
	}

	Ws *slot(int id) { return ( id >= 0 && id < LWS_MAX_WEBSOCKETS ) ? slots[id] : NULL; }

	void queue(size_t bytes, std::function<void()> fn, bool ws_message = false)
	{
		if ( quitting )
			return;
		if ( ws_message )
			ws_rx_bytes += bytes;
		events.push_back(Event{ bytes, ws_message, std::move(fn) });
	}

	// Reports a request's outcome once
	void finish(Req *r)
	{
		if ( r->reported )
			return;
		r->reported = true;
		if ( r->gen != gen )
			return;
		if ( r->err.empty() )
		{
			// Size first: argument order is unspecified, the move may run before size()
			size_t bytes = r->resp.body.size();
			if ( r->onDone )
				queue(bytes, [cb = std::move(r->onDone), res = std::move(r->resp)]() { cb(res); });
		}
		else if ( r->onError )
			queue(r->err.size(), [cb = std::move(r->onError), msg = r->err]() { cb(msg); });
	}

	// The wsi is gone (WSI_DESTROY, or a connect that failed inside lws)
	void ended(Conn *c)
	{
		c->live = false;
		c->wsi = NULL;
		if ( c->ws )
			ws_ended((Ws *)c);
		else
		{
			if ( c->err.empty() )
				c->err = "Connection closed unexpectedly";
			finish((Req *)c);
		}
	}

	void ws_ended(Ws *w)
	{
		bool was = w->connected;
		bool byRemote = !w->closing && !w->killed;
		bool again = !w->closing && w->reconnect_ms > 0;

		w->connected = false;
		w->sendq.clear();
		w->sendq_bytes = 0;
		w->rx.clear();
		w->rx_drop = false;
		w->want_ping = w->want_close = w->waiting_pong = false;
		if ( again )
			w->next_reconnect = now_ms() + w->reconnect_ms;
		else
			w->finished = true;

		if ( !w->detached )
		{
			// After the script closed, it hears nothing but the close
			if ( !w->err.empty() && !w->closing && w->cb.error )
				queue(w->err.size(), [cb = w->cb.error, msg = w->err]() { cb(msg); });
			if ( was && w->cb.close )
				queue(0, [cb = w->cb.close, byRemote, fully = w->finished]() { cb(byRemote, fully); });
		}
		w->err.clear();
	}

	void timers()
	{
		uint64_t now = now_ms();

		for ( Req *r : reqs )
			if ( r->deadline && now >= r->deadline )
				kill(r, "Timeout");

		for ( Ws *w : sockets )
		{
			if ( w->wsi )
			{
				if ( w->close_deadline && now >= w->close_deadline )
					kill(w, NULL);
				else if ( !w->connected )
				{
					if ( now >= w->connect_deadline )
						kill(w, "Connect timeout");
				}
				else if ( w->waiting_pong && now >= w->pong_deadline )
					kill(w, "Ping timeout (no pong received within time limit)");
				else if ( w->ping_ms && !w->waiting_pong && !w->closing && now >= w->next_ping )
				{
					w->want_ping = true;
					w->next_ping = now + w->ping_ms;
					lws_callback_on_writable(w->wsi);
				}
			}
			else if ( !w->live && !w->finished && w->next_reconnect && now >= w->next_reconnect )
				ws_connect(w);
		}
	}

	void deliver()
	{
		size_t spent = 0;
		while ( !events.empty() && ( spent == 0 || spent + events.front().bytes + EVENT_COST <= FRAME_BUDGET ) )
		{
			// The script may queue more, or change the level, while this runs
			Event e = std::move(events.front());
			events.pop_front();
			if ( e.ws_message )
				ws_rx_bytes -= e.bytes;
			spent += e.bytes + EVENT_COST;
			e.fn();
		}
	}

	void reap()
	{
		for ( size_t i = 0; i < reqs.size(); )
		{
			if ( !reqs[i]->live && reqs[i]->reported )
			{
				delete reqs[i];
				reqs.erase(reqs.begin() + i);
			}
			else
				i++;
		}
		for ( size_t i = 0; i < sockets.size(); )
		{
			Ws *w = sockets[i];
			if ( !w->live && w->finished )
			{
				for ( int s = 0; s < LWS_MAX_WEBSOCKETS; s++ )
					if ( slots[s] == w )
						slots[s] = NULL;
				delete w;
				sockets.erase(sockets.begin() + i);
			}
			else
				i++;
		}
	}

	int append_headers(Conn *c, struct lws *wsi, void *in, size_t len)
	{
		uint8_t **p = (uint8_t **)in;
		uint8_t *end = *p + len;
		for ( const auto &h : c->headers )
		{
			if ( lws_add_http_header_by_name(wsi, (const uint8_t *)h.first.c_str(),
			                                 (const uint8_t *)h.second.c_str(), (int)h.second.size(), p, end) )
			{
				c->err = "Request headers too large";
				return -1;
			}
		}
		if ( c->ws )
			return 0;

		// An empty POST/PUT/PATCH still says Content-Length: 0, or some servers answer 411 (RFC 9110 8.6)
		Req *r = (Req *)c;
		bool sendsBody = r->method == "POST" || r->method == "PUT" || r->method == "PATCH";
		if ( r->body.empty() && !sendsBody )
			return 0;
		if ( lws_add_http_header_content_length(wsi, r->body.size(), p, end) )
		{
			c->err = "Request headers too large";
			return -1;
		}
		if ( !r->body.empty() )
		{
			lws_client_http_body_pending(wsi, 1);
			lws_callback_on_writable(wsi);
		}
		return 0;
	}

	static void collect_headers(struct lws *wsi, Response &resp)
	{
		static const enum lws_token_indexes tokens[] = {
			WSI_TOKEN_HTTP_CONTENT_TYPE,
			WSI_TOKEN_HTTP_CONTENT_LENGTH,
			WSI_TOKEN_HTTP_CONTENT_ENCODING,
			WSI_TOKEN_HTTP_SERVER,
			WSI_TOKEN_HTTP_DATE,
			WSI_TOKEN_HTTP_ETAG,
			WSI_TOKEN_HTTP_LAST_MODIFIED,
			WSI_TOKEN_HTTP_LOCATION,
			WSI_TOKEN_HTTP_SET_COOKIE,
			WSI_TOKEN_HTTP_CACHE_CONTROL,
			WSI_TOKEN_CONNECTION,
			WSI_TOKEN_HTTP_TRANSFER_ENCODING,
		};

		resp.status = (int)lws_http_client_http_response(wsi);
		for ( enum lws_token_indexes t : tokens )
		{
			int n = lws_hdr_total_length(wsi, t);
			const char *name = (const char *)lws_token_to_string(t);
			if ( n <= 0 || !name )
				continue;
			std::string value(n + 1, '\0');
			if ( lws_hdr_copy(wsi, &value[0], n + 1, t) < 0 )
				continue;
			value.resize(strlen(value.c_str()));
			std::string key(name);
			if ( !key.empty() && key.back() == ':' )   // lws gives "content-type:"
				key.pop_back();
			resp.headers[key] = value;
		}
	}

	int http_event(Req *r, struct lws *wsi, enum lws_callback_reasons reason, void *in, size_t len)
	{
		switch ( reason )
		{
		case LWS_CALLBACK_CLIENT_HTTP_WRITEABLE:
		{
			size_t left = r->body.size() - r->body_sent;
			if ( !left )
				break;
			size_t chunk = left < 4096 ? left : 4096;
			std::vector<uint8_t> buf(LWS_PRE + chunk);
			memcpy(&buf[LWS_PRE], r->body.data() + r->body_sent, chunk);
			r->body_sent += chunk;
			bool last = ( r->body_sent == r->body.size() );
			if ( lws_write(wsi, &buf[LWS_PRE], chunk, last ? LWS_WRITE_HTTP_FINAL : LWS_WRITE_HTTP) != (int)chunk )
				return -1;
			if ( last )
				lws_client_http_body_pending(wsi, 0);
			else
				lws_callback_on_writable(wsi);
			break;
		}

		case LWS_CALLBACK_ESTABLISHED_CLIENT_HTTP:
			collect_headers(wsi, r->resp);
			// lws takes the first status line as the final one, so a 1xx would pass as the reply
			if ( r->resp.status >= 100 && r->resp.status < 200 )
			{
				r->err = "Unsupported interim response " + std::to_string(r->resp.status);
				return -1;
			}
			if ( r->resp.status < 100 || r->resp.status > 999 )
			{
				r->err = "Invalid status line";
				return -1;
			}
			break;

		case LWS_CALLBACK_RECEIVE_CLIENT_HTTP:
		{
			// Makes lws hand the body to RECEIVE_CLIENT_HTTP_READ
			char buffer[2048 + LWS_PRE];
			char *px = buffer + LWS_PRE;
			int lenx = (int)sizeof(buffer) - LWS_PRE;
			if ( lws_http_client_read(wsi, &px, &lenx) < 0 )
				return -1;
			break;
		}

		case LWS_CALLBACK_RECEIVE_CLIENT_HTTP_READ:
			// Fail as soon as the body passes what a script string can hold
			if ( r->resp.body.size() + len > GSC_MAX_STRING_BYTES )
			{
				r->err = "Response body too large";
				return -1;
			}
			r->resp.body.append((const char *)in, len);
			break;

		case LWS_CALLBACK_COMPLETED_CLIENT_HTTP:
			finish(r);
			/*
			 * A reply with no body (204, Content-Length: 0, HEAD) completes while lws
			 * is still reading headers, where it ignores -1. Close it on the next service.
			 */
			lws_set_timeout(wsi, PENDING_TIMEOUT_USER_OK, LWS_TO_KILL_ASYNC);
			return -1;

		default:
			break;
		}
		return 0;
	}

	int ws_event(Ws *w, struct lws *wsi, enum lws_callback_reasons reason, void *in, size_t len)
	{
		switch ( reason )
		{
		case LWS_CALLBACK_CLIENT_ESTABLISHED:
			if ( w->closing )   // closed during the handshake
				return -1;
			w->connected = true;
			w->next_ping = now_ms() + w->ping_ms;
			if ( !w->detached && w->cb.open )
				queue(0, [cb = w->cb.open]() { cb(); });
			break;

		case LWS_CALLBACK_CLIENT_RECEIVE:
			if ( lws_frame_is_binary(wsi) )   // TEXT only
				break;
			if ( !w->rx_drop )
			{
				if ( w->rx.size() + len > GSC_MAX_STRING_BYTES )
				{
					w->rx_drop = true;
					w->rx.clear();
				}
				else
					w->rx.append((const char *)in, len);
			}
			if ( lws_is_final_fragment(wsi) && lws_remaining_packet_payload(wsi) == 0 )
			{
				if ( w->rx_drop )
				{
					if ( log )
						log("webSocket: dropped a text message over %d bytes\n", GSC_MAX_STRING_BYTES);
				}
				else if ( !w->detached && w->cb.message )
				{
					size_t bytes = w->rx.size();   // before the move, see finish()
					queue(bytes, [cb = w->cb.message, msg = std::move(w->rx)]() { cb(msg); }, true);
				}
				w->rx.clear();
				w->rx_drop = false;
				// Only WS messages count: a burst of HTTP replies must not drop a quiet socket
				if ( ws_rx_bytes > RX_QUEUE_MAX )
					kill(w, "Receive queue full");
			}
			break;

		case LWS_CALLBACK_CLIENT_RECEIVE_PONG:
			w->waiting_pong = false;
			break;

		case LWS_CALLBACK_CLIENT_WRITEABLE:
			// One TEXT frame per writeable, then a PING, then the CLOSE
			if ( !w->sendq.empty() )
			{
				std::string msg = std::move(w->sendq.front());
				w->sendq.pop_front();
				w->sendq_bytes -= msg.size();
				std::vector<unsigned char> buf(LWS_PRE + msg.size());
				memcpy(&buf[LWS_PRE], msg.data(), msg.size());
				if ( lws_write(wsi, &buf[LWS_PRE], msg.size(), LWS_WRITE_TEXT) < (int)msg.size() )
					return -1;
			}
			else if ( w->want_ping )
			{
				unsigned char buf[LWS_PRE + 1];
				w->want_ping = false;
				lws_write(wsi, &buf[LWS_PRE], 0, LWS_WRITE_PING);
				// Half the ping interval for the pong, at least 1 s
				w->waiting_pong = true;
				w->pong_deadline = now_ms() + ( w->ping_ms / 2 > 1000 ? w->ping_ms / 2 : 1000 );
			}
			else if ( w->want_close )
			{
				w->want_close = false;
				lws_close_reason(wsi, LWS_CLOSE_STATUS_NORMAL, NULL, 0);
				return -1;
			}
			if ( !w->sendq.empty() || w->want_ping || w->want_close )
				lws_callback_on_writable(wsi);
			break;

		default:
			break;
		}
		return 0;
	}

	// Turns an lws connect failure into one short line
	static std::string conn_error_text(const std::string &msg)
	{
		if ( msg.compare(0, 9, "Timed out") == 0 )
			return "Timeout";
		// lws v5 gives no DNS error of its own (connect3.c:492)
		if ( msg == "closed before established" )
			return "Failed to connect (host not found or unreachable)";
		if ( msg.compare(0, 4, "  ! ") != 0 )
			return msg;

		// mbedTLS cert verify text: one "  ! reason\n" line per problem (mbedtls-client.c:194)
		std::string out;
		size_t pos = 0;
		while ( pos < msg.size() )
		{
			size_t end = msg.find('\n', pos);
			if ( end == std::string::npos )
				end = msg.size();
			std::string line = msg.substr(pos, end - pos);
			pos = end + 1;
			if ( line.compare(0, 4, "  ! ") == 0 )
				line.erase(0, 4);
			if ( line.empty() )
				continue;
			out += out.empty() ? "TLS: " : "; ";
			out += line;
		}
		return out;
	}

	static int callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
	{
		(void)user;
		HttpWsClient &self = get();
		Conn *c = wsi ? (Conn *)lws_get_opaque_user_data(wsi) : NULL;
		if ( !c )
			return 0;

		switch ( reason )
		{
		case LWS_CALLBACK_WSI_DESTROY:
			self.ended(c);
			return 0;

		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
			if ( c->err.empty() )
				c->err = in ? conn_error_text(std::string((const char *)in, len ? len : strlen((const char *)in))) : "Connection error";
			return 0;

		case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
			return self.append_headers(c, wsi, in, len);

		default:
		{
			int n = c->ws ? self.ws_event((Ws *)c, wsi, reason, in, len)
			              : self.http_event((Req *)c, wsi, reason, in, len);
			// lws v5 never closes on -1 from a writeable callback (ops-ws.c:1082); 1 does
			if ( n < 0 && ( reason == LWS_CALLBACK_CLIENT_WRITEABLE || reason == LWS_CALLBACK_CLIENT_HTTP_WRITEABLE ) )
				n = 1;
			return n;
		}
		}
	}

	struct lws_context *ctx = NULL;
	struct lws_protocols protocols[2];
	std::vector<Req *> reqs;
	std::vector<Ws *> sockets;           // owns every Ws, slotted or closing
	Ws *slots[LWS_MAX_WEBSOCKETS];       // script ids
	std::deque<Event> events;
	size_t ws_rx_bytes = 0;              // undelivered WS message bytes
	unsigned gen = 0;
	bool quitting = false;
};

#endif
