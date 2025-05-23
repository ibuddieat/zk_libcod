#ifndef _GSC_PROXY_H_
#define _GSC_PROXY_H_

#include "gsc.hpp"
#include "ratelimiter.hpp"

qboolean Sys_IsProxyAddress(netadr_t from);
qboolean SV_IsAnyProxyStarted();
void SV_SetupProxies();
void SV_ShutdownProxies();
void * SV_StartProxy(void *threadArgs);
void * SV_ProxyMasterServerLoop(void *threadArgs);
void * SV_ProxyClientThread(void *threadArgs);

#endif