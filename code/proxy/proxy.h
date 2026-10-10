#ifndef _GSC_PROXY_H_
#define _GSC_PROXY_H_

#include "gsc.hpp"
#include "ratelimiter.hpp"

qboolean Sys_IsProxyAddress(netadr_t from);
qboolean SV_IsAnyProxyStarted();
void SV_SetupProxies();
void SV_ShutdownProxies();
void * SV_StartProxy(void *threadArgs);
void * SV_ProxyQueryCacheLoop(void *threadArgs);
qboolean SV_RefreshInfoCache(proxy_t *proxy, int s, struct sockaddr_in *forwarderAddr);
qboolean SV_RefreshStatusCache(proxy_t *proxy, int s, struct sockaddr_in *forwarderAddr);
void SV_SendCachedInfoResponse(proxy_t *proxy, const char *requestBuffer, int requestLen, struct sockaddr_in *addr);
void SV_SendCachedStatusResponse(proxy_t *proxy, const char *requestBuffer, int requestLen, struct sockaddr_in *addr);
void SV_ExtractChallenge(const char *request, int requestLen, int offset, char *challenge);
void * SV_ProxyMasterServerLoop(void *threadArgs);
void * SV_ProxyClientThread(void *threadArgs);

#endif