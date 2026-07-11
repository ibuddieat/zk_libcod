#ifndef HTTP_CLIENT_HPP
#define HTTP_CLIENT_HPP

#include <libwebsockets.h>

#include <functional>
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include <cstdint>
#include <cctype>
#include "gsc_maxstring.hpp"

// The server frame drives lws in embedded (external-poll) mode: lws hands us the
// sockets to watch via ADD/DEL/CHANGE_MODE_POLL_FD, and we run a non-blocking
// poll() each frame and feed ready sockets back with lws_service_fd(). This keeps
// lws off its own blocking event loop (lws_service sleeps until an event, which
// would freeze the game thread) while all callbacks still fire on this thread.
#undef poll
#include <poll.h>
#undef poll

/*
 * Asynchronous HTTP(S) client on the libwebsockets event loop. One connection
 * per request, closed once the response is received. poll() must be called
 * regularly (once per server frame) to drive I/O; callbacks fire from within
 * poll(), on the calling thread.
 *
 * TLS uses mbedTLS in encrypt-only mode: the connection is encrypted but the
 * server certificate is not verified (no CA trust store), so the .so stays fully
 * self-contained - it needs no system cert file at runtime. This matches round-1
 * Mongoose's built-in TLS. libwebsockets and mbedTLS are both permissively
 * licensed (MIT / Apache-2.0).
 */
class HttpClient
{
  public:
	struct Response
	{
		int status = 0;
		std::map<std::string, std::string> headers;
		std::string body;
	};
	using Callback = std::function<void(const Response &)>;
	using ErrorCallback = std::function<void(const std::string &error)>;

	HttpClient()
	{
		lws_set_log_level(0, NULL);

		memset(protocols, 0, sizeof(protocols));
		protocols[0].name = "http";
		protocols[0].callback = &HttpClient::ev_handler;

		struct lws_context_creation_info info;
		memset(&info, 0, sizeof(info));
		info.port = CONTEXT_PORT_NO_LISTEN;
		info.protocols = protocols;
		info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
		// No CA trust store: TLS is encrypt-only (see request(), where the client
		// connection flags skip certificate verification). This keeps the .so fully
		// self-contained - no system cert file - matching the round-1 Mongoose build.
		//
		// Advertise no ALPN. lws otherwise auto-fills the HTTP client's ALPN with
		// "http/1.1"; with mbedTLS 2.28 that extension is malformed enough that peers
		// reject the TLS handshake with a fatal alert (https fails while wss - which
		// sends no ALPN - works). We are HTTP/1.1 only and never need it. An empty
		// (non-NULL) string overrides the auto-discovered default (context.c:1094).
		info.alpn = "";
		info.user = this;

		ctx = lws_create_context(&info);
	}

	~HttpClient()
	{
		if ( ctx )
			lws_context_destroy(ctx);
	}

	/*
	 * Drives the event loop. Delivers errors deferred out of the synchronous
	 * connect window first (frame boundary), enforces per-request timeouts, then
	 * services libwebsockets and reaps finished requests.
	 */
	void poll(int wait_time_ms = 0)
	{
		//   CoD2rev Scr_ExecThread precondition (VM must be idle):
		//     https://github.com/voron00/CoD2rev_Server/blob/abf692f1fd5697585ffd083e3114b2850c13a1f4/src/script/scr_vm.cpp#L1687
		if ( !deferred_errors.empty() )
		{
			std::vector<std::pair<ErrorCallback, std::string>> pending;
			pending.swap(deferred_errors);
			for ( auto &e : pending )
				if ( e.first )
					e.first(e.second);
		}

		if ( !ctx )
			return;

		uint64_t now = (uint64_t)(lws_now_usecs() / 1000);
		for ( RequestContext *rc : active )
		{
			if ( rc->done || !rc->wsi )
				continue;
			bool expired = false;
			if ( !rc->connected && rc->connect_timeout_ms > 0 && now > rc->connect_deadline )
				expired = true;
			else if ( rc->timeout_ms > 0 && now > rc->deadline )
				expired = true;
			if ( expired )
			{
				rc->timed_out = true;
				// Force the connection closed; the resulting CLOSED_CLIENT_HTTP
				// reports the timeout and marks the request done.
				lws_set_timeout(rc->wsi, (enum pending_timeout)1, LWS_TO_KILL_ASYNC);
			}
		}

		// Run any scheduled/forced lws work (connection timeouts, retries) without
		// sleeping: adjust_timeout returns 0 when something needs forced service, and
		// lws_service_tsi(-1) then services only those and returns immediately.
		if ( lws_service_adjust_timeout(ctx, 1, 0) == 0 )
			lws_service_tsi(ctx, -1, 0);

		// Poll lws's sockets and feed the ready ones back to lws. wait_time_ms is 0 on
		// the per-frame call (non-blocking) and >0 only when a shutdown drain wants to
		// briefly wait for in-flight I/O to flush. A serviced fd may add/remove entries
		// (via the poll-fd callbacks), so iterate a snapshot of the active fds.
		if ( !pollfds.empty() )
		{
			int n = ::poll(pollfds.data(), (nfds_t)pollfds.size(), wait_time_ms);
			if ( n > 0 )
			{
				std::vector<struct pollfd> ready;
				for ( struct pollfd &p : pollfds )
					if ( p.revents )
						ready.push_back(p);
				for ( struct pollfd &p : ready )
				{
					struct lws_pollfd lp;
					lp.fd = p.fd;
					lp.events = p.events;
					lp.revents = p.revents;
					lws_service_fd(ctx, &lp);
				}
			}
		}

		// Reap finished requests. This is the SINGLE cleanup point - callbacks
		// only set done=true, never delete, so a wsi that fires both an error and
		// a close cannot double-free.
		for ( size_t i = 0; i < active.size(); )
		{
			if ( active[i]->done )
			{
				delete active[i];
				active.erase(active.begin() + i);
			}
			else
				i++;
		}
	}

	/*
	 * Sends an HTTP request. headers are extra request headers separated by \r\n
	 * (e.g. "Content-Type: application/json"). timeout_ms bounds the whole
	 * request; connect_timeout_ms bounds the initial connect; timeout_ms <= 0
	 * disables both. Exactly one of onDone/onError is invoked exactly once.
	 */
	void request(const char *method, const char *url, const char *data, size_t data_length,
	             const char *headers, Callback onDone, ErrorCallback onError,
	             int timeout_ms = 60000, int connect_timeout_ms = 5000)
	{
		if ( !is_valid_url(url) )
		{
			if ( onError )
				deferred_errors.emplace_back(std::move(onError), "Invalid URL");
			return;
		}

		RequestContext *rc = new RequestContext();
		rc->owner = this;
		rc->method = ( method && *method ) ? method : "GET";
		rc->headers = ( headers && *headers ) ? headers : "";
		if ( data_length > 0 )
			rc->body_out.assign(data, data + data_length);
		rc->onDone = std::move(onDone);
		rc->onError = std::move(onError);
		if ( timeout_ms <= 0 )
			connect_timeout_ms = 0;
		rc->timeout_ms = timeout_ms;
		rc->connect_timeout_ms = connect_timeout_ms;

		// lws_parse_uri mutates its buffer, so parse a private copy.
		rc->urlbuf = url;
		const char *prot = NULL, *address = NULL, *path = NULL;
		int port = 0;
		if ( lws_parse_uri(&rc->urlbuf[0], &prot, &address, &port, &path) )
		{
			if ( rc->onError )
				deferred_errors.emplace_back(rc->onError, "Failed to parse URL");
			delete rc;
			return;
		}
		bool ssl = ( prot && ( strcmp(prot, "https") == 0 || strcmp(prot, "wss") == 0 ) );
		rc->host = address ? address : "";
		rc->path = "/";                 // lws_parse_uri strips the leading slash
		if ( path )
			rc->path += path;

		// lws_parse_uri does not infer the port from the scheme, so a URL without an
		// explicit :port yields port 0. Default it or the connect fails immediately.
		if ( port == 0 )
			port = ssl ? 443 : 80;

		uint64_t now = (uint64_t)(lws_now_usecs() / 1000);
		rc->deadline = now + rc->timeout_ms;
		rc->connect_deadline = now + rc->connect_timeout_ms;

		struct lws_client_connect_info i;
		memset(&i, 0, sizeof(i));
		i.context = ctx;
		i.address = rc->host.c_str();
		i.port = port;
		i.path = rc->path.c_str();
		i.host = rc->host.c_str();
		i.origin = rc->host.c_str();
		i.method = rc->method.c_str();
		i.protocol = "http";
		i.userdata = rc;
		i.pwsi = &rc->wsi;
		if ( ssl )
			// Encrypt-only: ALLOW_INSECURE sets verify=NONE (no cert check) but,
			// unlike SKIP_SERVER_CERT_HOSTNAME_CHECK, still lets lws set the TLS SNI
			// hostname - which SNI-strict servers (Cloudflare etc.) require or they
			// abort the handshake with a fatal alert.
			i.ssl_connection = LCCSCF_USE_SSL | LCCSCF_ALLOW_INSECURE;

		active.push_back(rc);

		// lws_client_connect_via_info can fire CLIENT_CONNECTION_ERROR
		// synchronously (socket/DNS setup failure), so any error raised in this
		// window is deferred (report_error checks `connecting`) rather than
		// delivered re-entrantly into the httpFetch builtin.
		connecting = true;
		struct lws *w = lws_client_connect_via_info(&i);
		connecting = false;

		if ( !w && !rc->error_reported )
		{
			// No wsi was created and no synchronous error fired: defer the error
			// and mark the request done so poll() reaps it (no callback will).
			if ( rc->onError )
				deferred_errors.emplace_back(rc->onError, "Failed to connect");
			rc->done = true;
		}
	}

	static bool is_valid_url(const char *url)
	{
		if ( !url || *url == '\0' )
			return false;

		const char *p = url;

		// Scheme: a letter followed by letters/digits/+/-/. and "://"
		if ( !isalpha((unsigned char)*p) )
			return false;
		p++;
		while ( *p && ( isalnum((unsigned char)*p) || *p == '+' || *p == '-' || *p == '.' ) )
			p++;
		if ( strncmp(p, "://", 3) != 0 )
			return false;
		p += 3;
		if ( *p == '\0' )
			return false;

		while ( *p )
		{
			unsigned char c = (unsigned char)*p;
			if ( c <= 32 || c > 126 )
				return false;
			if ( c == '"' || c == '`' || c == '<' || c == '>' || c == '\\' )
				return false;
			p++;
		}

		return true;
	}

  private:
	struct RequestContext
	{
		HttpClient *owner = NULL;
		struct lws *wsi = NULL;
		std::string urlbuf;             // mutable copy for lws_parse_uri
		std::string method;
		std::string host;
		std::string path;
		std::string headers;
		std::vector<char> body_out;     // request body
		size_t body_written = 0;
		Response resp;
		Callback onDone;
		ErrorCallback onError;
		int timeout_ms = 0;
		int connect_timeout_ms = 0;
		uint64_t deadline = 0;
		uint64_t connect_deadline = 0;
		bool connected = false;
		bool finished = false;
		bool error_reported = false;
		bool timed_out = false;
		bool oversized = false;         // body exceeded the deliverable cap
		bool done = false;              // set by callbacks; reaped in poll()
	};

	// Reports an error exactly once. Inside the synchronous connect window the
	// callback is deferred to the next poll() so it never runs re-entrantly.
	void report_error(RequestContext *rc, const std::string &msg)
	{
		if ( rc->error_reported || rc->finished )
			return;
		rc->error_reported = true;
		if ( !rc->onError )
			return;
		if ( connecting )
			deferred_errors.emplace_back(rc->onError, msg);
		else
			rc->onError(msg);
	}

	// Collect the response status line + headers once the server responds.
	static void collect_headers(struct lws *wsi, Response &resp)
	{
		resp.status = (int)lws_http_client_http_response(wsi);

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

		for ( unsigned t = 0; t < sizeof(tokens) / sizeof(tokens[0]); t++ )
		{
			int n = lws_hdr_total_length(wsi, tokens[t]);
			if ( n <= 0 )
				continue;

			std::string value(n + 1, '\0');
			if ( lws_hdr_copy(wsi, &value[0], n + 1, tokens[t]) < 0 )
				continue;
			value.resize(strlen(value.c_str()));

			const char *name = (const char *)lws_token_to_string(tokens[t]);
			if ( !name )
				continue;
			std::string key(name);
			if ( !key.empty() && key.back() == ':' )   // lws returns "content-type:"
				key.pop_back();

			resp.headers[key] = value;
		}
	}

	static int ev_handler(struct lws *wsi, enum lws_callback_reasons reason,
	                      void *user, void *in, size_t len)
	{
		RequestContext *rc = (RequestContext *)user;

		switch ( reason )
		{
		case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
		{
			// Inject extra request headers and, for a body, announce it.
			uint8_t **p = (uint8_t **)in;
			uint8_t *end = (*p) + len;

			if ( rc && !rc->headers.empty() )
			{
				size_t start = 0;
				const std::string &h = rc->headers;
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

					name += ':';   // lws wants the trailing colon on the name
					if ( lws_add_http_header_by_name(wsi, (const uint8_t *)name.c_str(),
					                                 (const uint8_t *)value.c_str(), (int)value.size(), p, end) )
						return -1;
				}
			}

			if ( rc && !rc->body_out.empty() )
			{
				if ( lws_add_http_header_content_length(wsi, rc->body_out.size(), p, end) )
					return -1;
				lws_client_http_body_pending(wsi, 1);
				lws_callback_on_writable(wsi);
			}
			break;
		}

		case LWS_CALLBACK_CLIENT_HTTP_WRITEABLE:
		{
			if ( !rc || rc->body_out.empty() || rc->body_written >= rc->body_out.size() )
				break;

			size_t remaining = rc->body_out.size() - rc->body_written;
			size_t chunk = remaining < 4096 ? remaining : 4096;

			std::vector<uint8_t> buf(LWS_PRE + chunk);
			memcpy(&buf[LWS_PRE], rc->body_out.data() + rc->body_written, chunk);
			rc->body_written += chunk;

			bool last = ( rc->body_written >= rc->body_out.size() );
			enum lws_write_protocol wp = last ? LWS_WRITE_HTTP_FINAL : LWS_WRITE_HTTP;

			if ( lws_write(wsi, &buf[LWS_PRE], chunk, wp) != (int)chunk )
				return -1;

			if ( last )
				lws_client_http_body_pending(wsi, 0);
			else
				lws_callback_on_writable(wsi);
			break;
		}

		case LWS_CALLBACK_ESTABLISHED_CLIENT_HTTP:
		{
			if ( rc )
			{
				rc->connected = true;
				collect_headers(wsi, rc->resp);
			}
			break;
		}

		case LWS_CALLBACK_RECEIVE_CLIENT_HTTP:
		{
			// Pump available body through to RECEIVE_CLIENT_HTTP_READ.
			char buffer[2048 + LWS_PRE];
			char *px = buffer + LWS_PRE;
			int lenx = (int)sizeof(buffer) - LWS_PRE;
			if ( lws_http_client_read(wsi, &px, &lenx) < 0 )
				return -1;
			break;
		}

		case LWS_CALLBACK_RECEIVE_CLIENT_HTTP_READ:
		{
			if ( rc && in && len )
			{
				// A body larger than the max deliverable script string cannot be
				// passed to onDone anyway. Stop and fail fast the moment the cap is
				// crossed, so a huge (or endless) reply cannot balloon memory in
				// this 32-bit process before a post-hoc size check. Mirrors the WS
				// client's reassembly cap.
				if ( rc->resp.body.size() + len > GSC_MAX_STRING_BYTES )
				{
					rc->oversized = true;
					rc->owner->report_error(rc, "response body too large");
					rc->done = true;
					return -1;   // close the connection
				}
				rc->resp.body.append((const char *)in, len);
			}
			break;
		}

		case LWS_CALLBACK_COMPLETED_CLIENT_HTTP:
		{
			if ( rc && !rc->finished && !rc->error_reported )
			{
				rc->finished = true;
				if ( rc->onDone )
					rc->onDone(rc->resp);
			}
			if ( rc )
				rc->done = true;
			return -1;   // close the connection
		}

		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
		{
			if ( rc )
			{
				rc->owner->report_error(rc, in ? std::string((const char *)in, len ? len : strlen((const char *)in))
				                                : std::string("connection error"));
				rc->wsi = NULL;
				rc->done = true;
			}
			break;
		}

		case LWS_CALLBACK_CLOSED_CLIENT_HTTP:
		{
			if ( rc )
			{
				if ( !rc->finished && !rc->error_reported )
					rc->owner->report_error(rc, rc->timed_out ? "Timeout" : "Connection closed unexpectedly");
				rc->wsi = NULL;
				rc->done = true;
			}
			break;
		}

		// Embedded event-loop integration: lws asks us to watch/unwatch its sockets.
		// We keep our own pollfd table and service it each frame in poll().
		case LWS_CALLBACK_ADD_POLL_FD:
		{
			struct lws_pollargs *pa = (struct lws_pollargs *)in;
			HttpClient *self = (HttpClient *)lws_context_user(lws_get_context(wsi));
			if ( self && pa )
				self->add_pollfd(pa->fd, pa->events);
			break;
		}

		case LWS_CALLBACK_DEL_POLL_FD:
		{
			struct lws_pollargs *pa = (struct lws_pollargs *)in;
			HttpClient *self = (HttpClient *)lws_context_user(lws_get_context(wsi));
			if ( self && pa )
				self->del_pollfd(pa->fd);
			break;
		}

		case LWS_CALLBACK_CHANGE_MODE_POLL_FD:
		{
			struct lws_pollargs *pa = (struct lws_pollargs *)in;
			HttpClient *self = (HttpClient *)lws_context_user(lws_get_context(wsi));
			if ( self && pa )
				self->change_pollfd(pa->fd, pa->events);
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
		for ( struct pollfd &p : pollfds )
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
		pollfds.push_back(p);
	}

	void del_pollfd(int fd)
	{
		for ( size_t i = 0; i < pollfds.size(); i++ )
		{
			if ( pollfds[i].fd == fd )
			{
				pollfds.erase(pollfds.begin() + i);
				return;
			}
		}
	}

	void change_pollfd(int fd, int events)
	{
		for ( struct pollfd &p : pollfds )
		{
			if ( p.fd == fd )
			{
				p.events = (short)events;
				return;
			}
		}
	}

	struct lws_context *ctx = NULL;
	struct lws_protocols protocols[2];

	// True only while lws_client_connect_via_info() runs inside request(); tells
	// report_error to defer a synchronous error instead of firing it re-entrantly.
	bool connecting = false;

	std::vector<RequestContext *> active;
	std::vector<std::pair<ErrorCallback, std::string>> deferred_errors;

	// Sockets lws asked us to watch, serviced non-blocking each frame in poll().
	std::vector<struct pollfd> pollfds;
};

#endif
