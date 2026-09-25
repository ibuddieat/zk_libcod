# HTTP & WebSocket client (httpFetch, webSocket*)

`httpFetch` and `webSocket*` add non-blocking HTTP(S) and WebSocket clients to GSC.
They run on [libwebsockets](https://libwebsockets.org/) with
[mbedTLS](https://www.trustedfirmware.org/projects/mbed-tls/) for `https://` / `wss://`.
I/O is driven from the server frame, so callbacks fire on the main thread.

They are opt-in (see Building) and register no dvars. Full per-function docs live in
the **Network** category of the [script reference](script_reference/main.html).

---

## Building

Like MySQL and Speex, this is only compiled in when you ask for it. Both functions
share the same libwebsockets/mbedTLS build, so one flag turns on both:

```
./doit.sh http
```

The flag switches `COMPILE_HTTP` / `COMPILE_WEBSOCKET` on for that build and back off
afterwards, so `config.hpp` stays default-off.

### Requirements

Two packages on top of the normal libcod toolchain: cmake (libwebsockets builds
with cmake) and 32-bit mbedTLS (the statically-linked TLS backend).

```
sudo dpkg --add-architecture i386
sudo apt update
sudo apt install cmake libmbedtls-dev:i386
```

libwebsockets is a git submodule at `code/lib/libwebsockets`, pinned to release
`v4.5.8`. A plain `git clone` skips submodules, so either clone with `--recursive`
or just run `./doit.sh http` and it will fetch the submodule if it is missing.

The result is self-contained: mbedTLS is linked statically, so the `.so` loads no
`libssl` / `libcrypto` / `libwebsockets` / `libmbed*` at runtime and needs no CA file.

libwebsockets compiles once into `code/lib/libwebsockets/_build/` and is cached there,
so later builds skip it (that step takes a minute or two). If you bump the submodule
to a newer version, delete that folder to recompile it. It is gitignored, so it is
never committed.

### Platform support

| Platform            | cmake | `libmbedtls-dev:i386` |
|---------------------|-------|-----------------------|
| Ubuntu 24.04 LTS    | yes   | mbedTLS 2.28          |
| Debian 13 (Trixie)  | yes   | mbedTLS 3.6           |

---

## httpFetch

```
httpFetch(<url>, <method>, <data>, <extra headers>, <timeout seconds>, <onDone function>, <onError function>)
```

Makes one HTTP(S) request and closes the connection when the reply arrives. Either
`onDone` or `onError` fires once.

| Argument         | Notes |
|------------------|-------|
| `url`            | `http://` or `https://`. Missing port defaults to 80/443. |
| `method`         | `"GET"`, `"POST"`, ... (empty defaults to `GET`). |
| `data`           | Request body (e.g. a JSON string). Text only - a GSC string cannot hold a NUL, so no binary body. |
| `extra headers`  | Extra request headers separated by `\r\n`, e.g. `"Content-Type: application/json"`. |
| `timeout seconds`| Whole-request timeout; `0` disables it. |
| `onDone`         | `onDone(<status>, <body>, <headers>)`. |
| `onError`        | `onError(<error string>)`. |

```gsc
httpFetch( "https://api.example.com/v1/thing", "GET", "", "", 10, ::onDone, ::onError );

onDone( status, body, headers )
{
    // status  = HTTP status code (200, 404, 500, ...). 4xx/5xx come here, not onError.
    // body    = response body string
    // headers = array keyed by name, e.g. headers["content-type"]
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
webSocketConnect(<url>, <extra headers>, <onConnect function>, <onMessage function>, <onClose function>, <onError function>, [<reconnectDelayMs>], [<pingIntervalMs>])
webSocketSendText(<connectionId>, <message>)
webSocketClose(<connectionId>)
```

`webSocketConnect` opens a `ws://` or `wss://` connection and returns a connection id
(`0`-`15`), or `-1` on error. Up to 16 can be open at once.

| Argument           | Notes |
|--------------------|-------|
| `extra headers`    | Extra handshake headers, `\r\n`-separated (same format as httpFetch). |
| `onConnect`        | `onConnect()` - the handshake completed. |
| `onMessage`        | `onMessage(<text>)` - a TEXT frame arrived (binary frames are ignored). |
| `onClose`          | `onClose(<closedByRemote>, <fullyDisconnected>)`. |
| `onError`          | `onError(<error string>)`. |
| `reconnectDelayMs` | Delay before reconnecting after a drop. Default `2000`; `0` = connect once, no retry. |
| `pingIntervalMs`   | Keepalive ping interval. Default `15000`; `0` = off. |

```gsc
id = webSocketConnect( "wss://example.com/socket", "", ::onOpen, ::onMsg, ::onClose, ::onErr, 2000, 15000 );
...
sent = webSocketSendText( id, "hello" );   // true if queued, false if not connected
...
webSocketClose( id );                       // graceful close, stops auto-reconnect
```
