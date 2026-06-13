#ifndef _GSC_WEBSOCKET_HPP_
#define _GSC_WEBSOCKET_HPP_

#include "gsc.hpp"

#if COMPILE_HTTP == 1

void gsc_websocket_connect();
void gsc_websocket_sendtext();
void gsc_websocket_close();
void gsc_websocket_poll();
void gsc_websocket_shutdown();

#endif

#endif
