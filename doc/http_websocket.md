# HTTP & WebSocket client (httpFetch, webSocket*)

`httpFetch` and `webSocket*` add non-blocking HTTP(S) and WebSocket clients to GSC.
They run on [libwebsockets](https://libwebsockets.org/) with
[mbedTLS](https://www.trustedfirmware.org/projects/mbed-tls/) for `https://` / `wss://`.
I/O is driven from the server frame, so callbacks fire on the main thread.
Callbacks hand scripts at most about 128 KB of bodies and messages per frame (two
65000-byte replies); the rest wait for the next frames.

They are opt-in (see Building) and register no dvars. Full per-function docs live in
the **Network** category of the [script reference](script_reference/main.html).

---

## Building

Like MySQL and Speex, this is only compiled in when you ask for it. Both functions
share the same libwebsockets/mbedTLS build, so one flag turns on both:

```
./doit.sh http
```

The flag switches `COMPILE_HTTP` on for that build and back off afterwards, so
`config.hpp` stays default-off.

### Requirements

On top of the normal libcod toolchain: cmake, plus python3 with jinja2 and
jsonschema (mbedTLS generates some of its sources at build time).

```
sudo apt install cmake python3 python3-jinja2 python3-jsonschema
```

Both libraries are git submodules, built statically:

| Submodule | Path | Version |
|---|---|---|
| libwebsockets | `code/lib/libwebsockets` | `v5.0-stable` |
| mbedTLS | `code/lib/mbedtls` | `v4.1.1` |

`./doit.sh http` checks out the submodules itself and builds them once (a few
minutes); later builds reuse them. The `.so` needs no TLS or websocket library
installed at runtime.

### Certificates

`https://` and `wss://` verify the server certificate against the system CA bundle,
the first one found of `/etc/ssl/certs/ca-certificates.crt` (Debian, Ubuntu, Arch),
`/etc/pki/tls/certs/ca-bundle.crt` (Fedora, RHEL), `/etc/ssl/ca-bundle.pem` (openSUSE),
`/etc/pki/tls/cacert.pem`, `/etc/pki/ca-trust/extracted/pem/tls-ca-bundle.pem` (CentOS)
and `/etc/ssl/cert.pem` (Alpine). Install the `ca-certificates` package if none exists.
Without one, verified connections fail with a TLS error and a one-time
message is printed. Pass `allowInsecure` to skip verification for one request or
connection (the traffic is still encrypted).

### Platform support

| Platform            | Builds |
|---------------------|--------|
| Ubuntu 24.04 LTS    | yes    |
| Debian 13 (Trixie)  | yes    |

---

## httpFetch

```
httpFetch(<url>, <method>, <data>, <extra headers>, <timeout seconds>, <onDone function>, <onError function>, [<allowInsecure>])
```

Makes one HTTP(S) request and closes the connection when the reply arrives. Either
`onDone` or `onError` fires once.

| Argument         | Notes |
|------------------|-------|
| `url`            | `http://` or `https://`. Missing port defaults to 80/443. No `user:pass@`, spaces or non-ASCII: percent-encode them. |
| `method`         | `"GET"`, `"POST"`, ... (empty defaults to `GET`). |
| `data`           | Request body (e.g., a JSON string). Text only - a GSC string cannot hold a NUL, so no binary body. |
| `extra headers`  | Extra request headers separated by `\r\n`, e.g., `"Content-Type: application/json"`. At most 8 KB. `Host`, `Content-Length`, `Transfer-Encoding`, `Connection`, `Upgrade` and `Sec-WebSocket-*` are set by the client and refused here; an `Origin` header replaces the default one. A line without a name before its `:` is skipped, as before. |
| `timeout seconds`| Whole-request timeout. `0` = none of our own; libwebsockets still gives up on a stalled connect, TLS handshake or reply. |
| `onDone`         | `onDone(<status>, <body>, <headers>)`. |
| `onError`        | `onError(<error string>)`. |
| `allowInsecure`  | Skip the certificate check. Default `false`. |

- Any reply goes to `onDone`, 4xx/5xx included.
- A `POST`, `PUT` or `PATCH` with empty `data` sends `Content-Length: 0`.
- Redirects are not followed: a 3xx comes to `onDone` with `headers["location"]`.
- `headers` holds the common response headers (content-type, content-length, location, set-cookie, cache-control, ...). A repeated header is joined with `", "`.
- A body over 65000 bytes fails with `"Response body too large"`.
- A reply whose status line has no valid code fails with `"Invalid status line"`. An interim 1xx reply (such as `100 Continue`) fails with `"Unsupported interim response <code>"`.
- Other `onError` strings include `"Timeout"`, `"Invalid URL"`, `"Invalid method"`, `"Invalid headers"` (a CR or LF inside a header line, or a bad header name), `"Header not allowed: <name>"`, `"Request headers too large"`, `"Failed to connect (host not found or unreachable)"`, `"TLS: <reason>"` for a rejected certificate, plus other connect errors from libwebsockets.
- A request keeps running across a map change or `map_restart`, so a POST at the end of a map still goes out. Its callback is dropped, because the script that asked for it is gone.

```gsc
httpFetch( "https://api.example.com/v1/thing", "GET", "", "", 10, ::onDone, ::onError );

onDone( status, body, headers )
{
    // status  = HTTP status code (200, 404, 500, ...). 4xx/5xx come here, not onError.
    // body    = response body string
    // headers = array keyed by name, e.g., headers["content-type"]
    data = json_parse( body );
}

onError( error )
{
    // network / DNS / TLS failure, timeout, invalid URL, or oversized body
}
```

---

## webSocket*

```
webSocketConnect(<url>, <extra headers>, <onConnect function>, <onMessage function>, <onClose function>, <onError function>, [<reconnectDelayMs>], [<pingIntervalMs>], [<allowInsecure>])
webSocketSendText(<connectionId>, <message>)
webSocketClose(<connectionId>)
```

`webSocketConnect` opens a `ws://` or `wss://` connection and returns a connection id
(`0`-`15`). Up to 16 can be open at once. When all 16 are in use it raises a script
error and returns `-1`.

| Argument           | Notes |
|--------------------|-------|
| `extra headers`    | Extra handshake headers, `\r\n`-separated (same format as httpFetch). |
| `onConnect`        | `onConnect()` - the handshake completed. |
| `onMessage`        | `onMessage(<text>)` - a TEXT message arrived (binary frames are ignored). |
| `onClose`          | `onClose(<closedByRemote>, <fullyDisconnected>)`. |
| `onError`          | `onError(<error string>)`. |
| `reconnectDelayMs` | Delay before reconnecting after a drop. Default `2000`; `0` = connect once, no retry. |
| `pingIntervalMs`   | Keepalive ping interval. Default `15000`; `0` = off. |
| `allowInsecure`    | Skip the certificate check. Default `false`. |

- `fullyDisconnected` is `false` when a reconnect is coming.
- An invalid URL or oversized headers fails once with `onError` and is not retried.
- No pong within half the ping interval (at least 1 s) fails with `"Ping timeout ..."` and the link is dropped (and retried, if enabled).
- An incoming message over 65000 bytes is dropped, with a line in the server log.
- If more than 4 MB of messages are waiting for scripts, a socket that receives another one fails with `"Receive queue full"` and is dropped (and retried, if enabled).
- `webSocketClose` sends a close frame and stops reconnecting. `onClose(false, true)` follows on a later frame. If the close frame cannot go out (the server stopped reading), the connection is dropped after 5 seconds.
- Map change and `map_restart` close every connection without calling back. Open them again from the new level's init.

```gsc
id = webSocketConnect( "wss://example.com/socket", "", ::onOpen, ::onMsg, ::onClose, ::onErr, 2000, 15000 );
...
sent = webSocketSendText( id, "hello" );   // true if queued, false if not connected
...
webSocketClose( id );                       // graceful close, stops auto-reconnect
```
