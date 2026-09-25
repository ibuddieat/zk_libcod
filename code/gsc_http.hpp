#ifndef _GSC_HTTP_HPP_
#define _GSC_HTTP_HPP_

#include "gsc.hpp"

#if COMPILE_HTTP == 1

void gsc_http_fetch();
void gsc_http_poll();
void gsc_http_shutdown();

#endif

#endif
