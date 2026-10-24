#ifndef _GSC_HTTP_WS_HPP_
#define _GSC_HTTP_WS_HPP_

#include "gsc.hpp"

#if COMPILE_HTTP == 1

void gsc_http_fetch();
void gsc_websocket_connect();
void gsc_websocket_sendtext();
void gsc_websocket_close();

void gsc_http_poll();
void gsc_http_level_changed();
void gsc_http_shutdown();

#endif

#endif
