// Copyright (c) 2023, king-clan.com
// Copyright (c) 2024, iBuddie
// All rights reserved.

// This source code is licensed under the BSD-style license found in the
// LICENSE.md file located in the same directory as this file. 

#include "proxy.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

// Recv buffer
#define MAX_BUFFER_SIZE 65536

// Support at most 10 characters of challenge length (32-bit integer)
#define MAX_CHALLENGE_CHARS 10

char HEARTBEAT_MESSAGE[] = "\xFF\xFF\xFF\xFFheartbeat COD-2";
char HEARTBEAT_STOP_MESSAGE[] = "\xFF\xFF\xFF\xFFheartbeat flatline";
char GETINFO_MESSAGE[] = "\xFF\xFF\xFF\xFFgetinfo";
char GETSTATUS_MESSAGE[] = "\xFF\xFF\xFF\xFFgetstatus";
char AUTHORIZE_MESSAGE[] = "\xFF\xFF\xFF\xFFgetIpAuthorize";
char DISCONNECT_MESSAGE[] = "\xFF\xFF\xFF\xFF""error\nEXE_DISCONNECTED_FROM_SERVER";

dvar_t *sv_proxiesVisibleForTrackers;
dvar_t *sv_proxyAddress_1_0;
dvar_t *sv_proxyAddress_1_2;
dvar_t *sv_proxyAddress_1_3;
dvar_t *sv_proxyAddress_1_3_119;
dvar_t *sv_proxyEnable_1_0;
dvar_t *sv_proxyEnable_1_2;
dvar_t *sv_proxyEnable_1_3;
dvar_t *sv_proxyEnable_1_3_119;
dvar_t *sv_proxyForwardAddress_1_0;
dvar_t *sv_proxyForwardAddress_1_2;
dvar_t *sv_proxyForwardAddress_1_3;
dvar_t *sv_proxyForwardAddress_1_3_119;
dvar_t *sv_proxyQueryCacheRefreshTime;
dvar_t *sv_proxyTimeout;

extern dvar_t *com_sv_running;
extern dvar_t *fs_game;
extern dvar_t *net_ip;
extern dvar_t *net_port;
extern dvar_t *sv_authorizePort;
extern dvar_t *sv_authorizeServer;
extern dvar_t *sv_logHeartbeats;
extern dvar_t *sv_masterPort;
extern dvar_t *sv_masterServer;
extern dvar_t *sv_noMaster;
extern dvar_t *sv_timeout;
extern dvar_t *sv_version;

extern leakyBucket_t outboundLeakyBuckets[OUTBOUND_BUCKET_MAX];

// Proxies in addition to the main server port
proxy_t proxies[MAX_PROXIES];

// Flag stating whether SV_SetupProxies has been called, so that we know that
// proxy servers might have been started and that they need to be freed on
// server quit
qboolean initialized = qfalse;

// We do not reuse the stock va() buffers here for concurrency reasons
va_info_t proxies_va_info[MAX_PROXIES];

// These IP addresses are website server trackers
const char *trackers[] = {
	"159.69.0.99",      // KILLTUBE.org
	"78.46.106.94",     // cod.pm
	"155.138.163.54",   // GameTracker.com / GameServers.com
	"45.77.200.250",    // GameTracker.com / GameServers.com
	"5.180.252.202",    // codservers.net / neXus
	"51.89.216.249"     // cod2x.me / vps.freegamehosting.eu
};

// Master server socket address storage after hostname resolution
struct sockaddr_in masterSockAdr;

// Authorize server socket address storage after hostname resolution
struct sockaddr_in authorizeSockAdr;

// Main server address
netadr_t mainServerAdr;

unsigned long HashString(const char *str)
{
	unsigned long hash = 5381;
	int c;

	while ( ( c = *str++ ) )
		hash = ( ( hash << 5 ) + hash ) + c; // djb2 algorithm

	return hash;
}

uint64_t CreateClientIdentifier(struct sockaddr_in *addr)
{
	char client_info[INET_ADDRSTRLEN + 6];
	uint32_t client_address = ntohl(addr->sin_addr.s_addr);
	uint16_t client_port = ntohs(addr->sin_port);

	snprintf(
		client_info,
		sizeof(client_info),
		"%u:%u",
		client_address,
		client_port);

	return (uint64_t)HashString(client_info);
}

uint32_t GetHash(uint64_t identifier, uint32_t array_size)
{
	return (uint32_t)(identifier % array_size);
}

void ToLowerCase(char *str)
{
	for ( int i = 0; str[i]; i++ )
	{
		if ( str[i] >= 'A' && str[i] <= 'Z' )
			str[i] = str[i] + ('a' - 'A');
	}
}

char * VaProxies(proxy_t *proxy, const char *format, ...)
{
	va_info_t *info;
	int index;
	va_list va;

	info = &proxies_va_info[proxy - &proxies[0]];
	index = info->index;
	info->index = (info->index + 1) % MAX_VASTRINGS;

	va_start(va, format);
	Q_vsnprintf(info->va_string[index], sizeof(info->va_string[0]), format, va);
	va_end(va);

	info->va_string[index][MAX_STRINGLENGTH - 1] = 0;

	return info->va_string[index];
}

const char * InfoValueForKey(const char *s, const char *key, proxy_t *proxy)
{
	char pkey[8192];
	// Use two buffers so that comparisons that call the function twice work
	// without stomping on each other's buffers - for each thread
	static char value[MAX_PROXIES][2][8192];
	static int valueindex = 0; 
	int proxyindex = proxy - &proxies[0];
	char *o;

	if ( !s || !key )
	{
		return "";
	}

	if ( strlen( s ) >= 8192 )
	{
		Com_Error(ERR_FATAL, "InfoValueForKey: oversize infostring");
	}

	while ( *s && *s != '\\' )
		s++;

	if ( *s == '\\' )
		s++;

	while ( 1 )
	{
		o = pkey;
		while ( *s != '\\' )
		{
			if ( !*s )
				return "";
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value[proxyindex][valueindex];

		while ( *s != '\\' && *s )
		{
			*o++ = *s++;
		}
		*o = 0;

		if ( !I_stricmp(key, pkey) )
			return value[proxyindex][valueindex];

		if ( !*s )
			break;
		s++;
	}

	return "";
}

void ReplaceProtocolString(char *buffer, proxy_t *proxy)
{
	const char *proxyProtocolString = VaProxies(
		proxy,
		"\\protocol\\%d",
		proxy->version);
	const char *parentProtocolString = VaProxies(
		proxy,
		"\\protocol\\%d",
		proxy->parentVersion);
	char *offset = strstr(buffer, parentProtocolString);
	if ( offset != NULL )
		memcpy(offset, proxyProtocolString, 13);
}

void ReplaceShortversionString(char *buffer, proxy_t *proxy)
{
	const char *proxyShortversionString = VaProxies(
		proxy,
		"\\shortversion\\%s",
		proxy->versionString);
	const char *parentShortversionString = VaProxies(
		proxy,
		"\\shortversion\\%s",
		proxy->parentVersionString);
	char *offset = strstr(buffer, parentShortversionString);
	if ( offset != NULL )
		memcpy(offset, proxyShortversionString, 17);
}

qboolean Sys_IsTrackerAddress(char *address)
{
	for ( unsigned int i = 0; i < sizeof(trackers) / sizeof(trackers[0]); i++ )
	{
		if ( strcmp(address, trackers[i]) == 0 )
			return qtrue;
	}

	return qfalse;
}

qboolean Sys_IsMainServerAddress(netadr_t from)
{
	if ( net_port->current.integer != htons(from.port) )
		return qfalse;

	if ( NET_CompareBaseAdr(from, mainServerAdr) ||
	     ( from.ip[0] == 127 && from.ip[1] == 0 && from.ip[2] == 0 && from.ip[3] == 1 ) || 
		 ( from.ip[0] == 0 && from.ip[1] == 0 && from.ip[2] == 0 && from.ip[3] == 0 ) )
		return qtrue;

	return qfalse;
}

qboolean Sys_IsProxyAddress(netadr_t from)
{
	if ( initialized )
	{
		proxy_t *proxy;
		for ( int i = 0; i < MAX_PROXIES; i++ )
		{
			proxy = &proxies[i];
			if ( proxy->enabled &&
			     proxy->started &&
			     proxy->socket != -1 &&
			     NET_CompareBaseAdr(from, proxy->forwardAdr) )
				return qtrue;
		}
	}
	return qfalse;
}

qboolean SV_IsAnyProxyStarted()
{
	if ( initialized )
	{
		proxy_t *proxy;

		for ( int i = 0; i < MAX_PROXIES; i++ )
		{
			proxy = &proxies[i];
			if ( proxy->enabled && proxy->started )
					return qtrue;
		}
	}

	return qfalse;
}

void SV_ResetProxiesInformation()
{
	int i;
	int j;
	proxy_t *proxy;

	for ( i = 0; i < MAX_PROXIES; i++ )
	{
		proxy = &proxies[i];
		memset(proxy, 0, sizeof(proxy_t));
		for ( j = 0; j < MAX_PROXY_CLIENT_THREADS; j++ )
			proxy->clientThreadInfo[j].socket = -1;
	}
}

void SV_ConfigureProxy(proxy_t *proxy, int version, const char *address, const char *forwardAddress, int parentVersion)
{
	int index = (proxy - &proxies[0]) + 1;
	const char *versionString = GetShortVersionFromProtocol(version);

	Com_DPrintf(
		"Proxy: Configuring proxy for version %s (protocol %i)\n",
		versionString,
		version);

	NET_StringToAdr(address, &proxy->listenAdr);
	if ( proxy->listenAdr.type == NA_BAD )
	{
		Com_Error(
			ERR_FATAL,
			"\x15""Failed to parse listen address for proxy with version %s",
			versionString);
	}

	NET_StringToAdr(forwardAddress, &proxy->forwardAdr);
	if ( proxy->forwardAdr.type == NA_BAD )
	{
		Com_Error(
			ERR_FATAL,
			"\x15""Failed to parse forward address for proxy with version %s",
			versionString);
	}

	pthread_mutex_init(&proxy->lock, NULL);
	proxy->numClients = 0;
	proxy->masterServerThreadStarted = qfalse;
	memset(proxy->clientThreadInfo, 0, sizeof(proxyClientThreadInfo) * MAX_PROXY_CLIENT_THREADS);
	for ( int i = 0; i < MAX_PROXY_CLIENT_THREADS; i++ )
		proxy->clientThreadInfo[i].socket = -1;
	proxy->parentVersion = parentVersion;
	proxy->parentVersionString = GetShortVersionFromProtocol(parentVersion);
	proxy->version = version;
	proxy->versionString = versionString;
	proxy->bucket = (outboundLeakyBucketIndex_t)index;
	proxy->socket = -1;
	proxy->enabled = qtrue;
}

void SV_ShutdownProxies()
{
	if ( initialized )
	{
		int i;
		int j;
		proxy_t *proxy;
		qboolean stopped = qfalse;
		ssize_t bytes_sent;

		for ( i = 0; i < MAX_PROXIES; i++ )
		{
			proxy = &proxies[i];

			if ( proxy->enabled && proxy->started )
			{
				// Enable flag that states we had to stop a proxy
				stopped = qtrue;

				printf(
					"> [LIBCOD] Proxy: Shutting down proxy for version %s (protocol %i) on port %hu\n",
					proxy->versionString,
					proxy->version,
					BigShort(proxy->listenAdr.port));

				// Stop caching thread
				if ( proxy->queryCacheThreadStarted )
				{
					pthread_cancel(proxy->queryCacheThread);
					pthread_join(proxy->queryCacheThread, NULL);
					pthread_mutex_destroy(&proxy->queryCacheLock);
				}

				// Stop thread for announcements to master server
				if ( proxy->masterServerThreadStarted )
				{
					pthread_cancel(proxy->masterServerThread);
					pthread_join(proxy->masterServerThread, NULL);
				}

				// Tell the master server that we've gone offline. Should fix
				// a potential issue where this server might not show up in the
				// ingame browser list after being restarted
				if ( !sv_noMaster->current.integer && masterSockAdr.sin_family == AF_INET )
				{
					// Delay to counter rate-limiting on the master server side
					sleep(1);

					bytes_sent = sendto(
						proxy->socket,
						HEARTBEAT_STOP_MESSAGE,
						strlen(HEARTBEAT_STOP_MESSAGE),
						0,
						(struct sockaddr *)&masterSockAdr,
						sizeof(masterSockAdr));

					if ( bytes_sent == -1 )
						Com_DPrintf(
							"Proxy: Error %d on sendto when sending last heartbeat for port %hu: %s\n",
							errno,
							BigShort(proxy->listenAdr.port),
							strerror(errno));
				}

				// Prevent new clients from being accepted
				pthread_mutex_lock(&proxy->lock);
				proxy->stopped = qtrue;
				pthread_mutex_unlock(&proxy->lock);
				
				// Cleanup client sockets and threads
				for ( j = 0; j < MAX_PROXY_CLIENT_THREADS; j++ )
				{
					if ( proxy->clientThreadInfo[j].socket != -1 )
					{
						shutdown(proxy->clientThreadInfo[j].socket, SHUT_RDWR);
					}
				}

				// Stop main proxy thread that accepts new clients
				if ( proxy->mainThread )
				{
					pthread_cancel(proxy->mainThread);
					pthread_join(proxy->mainThread, NULL);
				}

				// Cleanup main socket
				if ( proxy->socket )
					close(proxy->socket);
			}
		}

		if ( stopped )
		{
			// Time for client threads to return after socket shutdown,
			// before data is nuked in SV_ResetProxiesInformation
			sleep(1);

			SV_ResetProxiesInformation();
		}
	}
}

void SV_ProxyStats_f()
{
	int i;
	int active = 0;
	proxy_t *proxy;
	
	for ( i = 0; i < MAX_PROXIES; i++ )
	{
		proxy = &proxies[i];

		pthread_mutex_lock(&proxy->lock);
		if ( proxy->enabled &&
		     proxy->started &&
		     proxy->socket != -1 )
		{
			active++;
			Com_Printf(
				"Proxy: Server %d listening on port %hu for version %s (protocol %i), %d client(s) connected\n",
				i+1,
				BigShort(proxy->listenAdr.port),
				proxy->versionString,
				proxy->version,
				proxy->numClients);
		}
		pthread_mutex_unlock(&proxy->lock);
	}
	if ( active == 0 )
		Com_Printf("No proxy servers active\n");
}

void SV_SetupProxies()
{
	const char *forwardAddress;

	if ( initialized )
		return;

	Com_Printf("-----------------------------------\n");

	if ( !sv_noMaster->current.integer )
	{
		// Resolve master server hostname, success required for server being listed
		if ( !Sys_StringToSockaddr(sv_masterServer->current.string, &masterSockAdr) )
		{
			Com_Printf("Proxy: Failed to resolve master server %s\n", sv_masterServer->current.string);
			masterSockAdr.sin_family = AF_UNSPEC;
		}
		else
		{
			masterSockAdr.sin_family = AF_INET;
			masterSockAdr.sin_port = htons(sv_masterPort->current.integer);
		}
	}

	// Resolve authorize server hostname, success required for server being listed
	if ( !Sys_StringToSockaddr(sv_authorizeServer->current.string, &authorizeSockAdr) )
	{
		Com_Printf("Proxy: Failed to resolve authorize server %s\n", sv_authorizeServer->current.string);
		authorizeSockAdr.sin_family = AF_UNSPEC;
	}
	else
	{
		authorizeSockAdr.sin_family = AF_INET;
		authorizeSockAdr.sin_port = htons(sv_authorizePort->current.integer);
	}

	// Automatically fill forward address and save it as main server address
	if ( I_strncmp(net_ip->current.string, "0.0.0.0", 8) == 0 )
		forwardAddress = va("127.0.0.1:%hu", net_port->current.integer);
	else
		forwardAddress = va("%s:%hu", net_ip->current.string, net_port->current.integer);

	NET_StringToAdr(forwardAddress, &mainServerAdr);
	Com_DPrintf("Proxy: Main server socket address is %s:%hu\n", net_ip->current.string, net_port->current.integer);
	Com_DPrintf("Proxy: Internal forwarding address is %s\n", forwardAddress);

	sv_proxiesVisibleForTrackers = Dvar_RegisterBool("sv_proxiesVisibleForTrackers", qfalse, DVAR_ARCHIVE);
	sv_proxyAddress_1_0 = Dvar_RegisterString("sv_proxyAddress_1_0", "0.0.0.0:28960", DVAR_ARCHIVE);
	sv_proxyAddress_1_2 = Dvar_RegisterString("sv_proxyAddress_1_2", "0.0.0.0:28961", DVAR_ARCHIVE);
	sv_proxyAddress_1_3 = Dvar_RegisterString("sv_proxyAddress_1_3", "0.0.0.0:28962", DVAR_ARCHIVE);
	sv_proxyAddress_1_3_119 = Dvar_RegisterString("sv_proxyAddress_1_3_119", "0.0.0.0:28963", DVAR_ARCHIVE);
	sv_proxyEnable_1_0 = Dvar_RegisterBool("sv_proxyEnable_1_0", qfalse, DVAR_ARCHIVE);
	sv_proxyEnable_1_2 = Dvar_RegisterBool("sv_proxyEnable_1_2", qfalse, DVAR_ARCHIVE);
	sv_proxyEnable_1_3 = Dvar_RegisterBool("sv_proxyEnable_1_3", qfalse, DVAR_ARCHIVE);
	sv_proxyEnable_1_3_119 = Dvar_RegisterBool("sv_proxyEnable_1_3_119", qfalse, DVAR_ARCHIVE);
	sv_proxyForwardAddress_1_0 = Dvar_RegisterString("sv_proxyForwardAddress_1_0", forwardAddress, DVAR_NOFLAG);
	sv_proxyForwardAddress_1_2 = Dvar_RegisterString("sv_proxyForwardAddress_1_2", forwardAddress, DVAR_NOFLAG);
	sv_proxyForwardAddress_1_3 = Dvar_RegisterString("sv_proxyForwardAddress_1_3", forwardAddress, DVAR_NOFLAG);
	sv_proxyForwardAddress_1_3_119 = Dvar_RegisterString("sv_proxyForwardAddress_1_3_119", forwardAddress, DVAR_NOFLAG);
	sv_proxyQueryCacheRefreshTime = Dvar_RegisterInt("sv_proxyQueryCacheRefreshTime", 1000, 100, 60000, DVAR_ARCHIVE);
	sv_proxyTimeout = Dvar_RegisterInt("sv_proxyTimeout", 240, 0, 1800, DVAR_ARCHIVE);

	SV_ResetProxiesInformation();

	switch ( GetProtocolFromShortVersion(sv_version->current.string) )
	{
		case 115:
			if ( sv_proxyEnable_1_2->current.boolean )
				SV_ConfigureProxy(
					&proxies[0],
					117,
					sv_proxyAddress_1_2->current.string,
					sv_proxyForwardAddress_1_2->current.string,
					115);
			if ( sv_proxyEnable_1_3->current.boolean )
				SV_ConfigureProxy(
					&proxies[1], 
					118,
					sv_proxyAddress_1_3->current.string,
					sv_proxyForwardAddress_1_3->current.string,
					115);
			if ( sv_proxyEnable_1_3_119->current.boolean )
				SV_ConfigureProxy(
					&proxies[2],
					119,
					sv_proxyAddress_1_3_119->current.string,
					sv_proxyForwardAddress_1_3_119->current.string,
					115);
			break;
		case 117:
			if ( sv_proxyEnable_1_0->current.boolean )		
				SV_ConfigureProxy(
					&proxies[0],
					115,
					sv_proxyAddress_1_0->current.string,
					sv_proxyForwardAddress_1_0->current.string,
					117);
			if ( sv_proxyEnable_1_3->current.boolean )
				SV_ConfigureProxy(
					&proxies[1],
					118,
					sv_proxyAddress_1_3->current.string,
					sv_proxyForwardAddress_1_3->current.string,
					117);
			if ( sv_proxyEnable_1_3_119->current.boolean )
				SV_ConfigureProxy(
					&proxies[2],
					119,
					sv_proxyAddress_1_3_119->current.string,
					sv_proxyForwardAddress_1_3_119->current.string,
					117);
			break;
		case 118:
			if ( sv_proxyEnable_1_0->current.boolean )
				SV_ConfigureProxy(
					&proxies[0],
					115,
					sv_proxyAddress_1_0->current.string,
					sv_proxyForwardAddress_1_0->current.string,
					118);
			if ( sv_proxyEnable_1_2->current.boolean )
				SV_ConfigureProxy(
					&proxies[1],
					117,
					sv_proxyAddress_1_2->current.string,
					sv_proxyForwardAddress_1_2->current.string,
					118);
			if ( sv_proxyEnable_1_3_119->current.boolean )
				SV_ConfigureProxy(
					&proxies[2],
					119,
					sv_proxyAddress_1_3_119->current.string,
					sv_proxyForwardAddress_1_3_119->current.string,
					118);
			break;
	}

	int i;
	proxy_t *proxy;

	for ( i = 0; i < MAX_PROXIES; i++ )
	{
		proxy = &proxies[i];
		if ( proxy->enabled )
		{
			if ( pthread_create(&proxy->mainThread, NULL, SV_StartProxy, proxy) )
			{
				Com_DPrintf(
					"Proxy: Failed to start proxy thread for version %s (protocol %i) on port %hu\n",
					proxy->versionString,
					proxy->version,
					BigShort(proxy->listenAdr.port));
			}
			else
			{
				Com_DPrintf(
					"Proxy: Started proxy thread for version %s (protocol %i) on port %hu\n",
					proxy->versionString,
					proxy->version,
					BigShort(proxy->listenAdr.port));

				proxy->started = qtrue;
			}
		}
	}

	Cmd_AddCommand("proxyStats", SV_ProxyStats_f);

	initialized = qtrue;
}

void * SV_StartProxy(void *threadArgs)
{
	proxy_t *proxy = (proxy_t *)threadArgs;
	struct sockaddr_in listenerAddr;
	struct sockaddr_in forwarderAddr;
	int listenerSocket;

	NetadrToSockadr(&proxy->listenAdr, &listenerAddr);
	NetadrToSockadr(&proxy->forwardAdr, &forwarderAddr);

	// Open a new UDP socket for that proxy
	listenerSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if ( listenerSocket == -1 )
	{
		Com_Error(
			ERR_FATAL,
			"\x15Proxy: Failed to open socket on port %hu",
			BigShort(proxy->listenAdr.port));
	}
	if ( bind(listenerSocket, (struct sockaddr *)&listenerAddr, sizeof(listenerAddr)) == -1 )
	{
		close(listenerSocket);
		Com_Error(
			ERR_FATAL,
			"\x15Proxy: Failed to bind on socket on port %hu",
			BigShort(proxy->listenAdr.port));
	}
	proxy->socket = listenerSocket;

	Com_Printf(
		"Proxy: Server listening on port %hu for version %s (protocol %i)\n",
		BigShort(proxy->listenAdr.port),
		proxy->versionString,
		proxy->version);

	// Startup info/status request caching thread
	if ( sv_noMaster->current.integer < 2 )
	{
		pthread_mutex_init(&proxy->queryCacheLock, NULL);
		if ( pthread_create(&proxy->queryCacheThread, NULL, SV_ProxyQueryCacheLoop, proxy) )
		{
			close(listenerSocket);
			pthread_cancel(proxy->masterServerThread);
			pthread_join(proxy->masterServerThread, NULL);
			Com_Error(
				ERR_FATAL,
				"\x15Proxy: Failed to create caching thread for version %s (protocol %i)",
				proxy->versionString,
				proxy->version);
		}
		proxy->queryCacheThreadStarted = qtrue;
	}

	// Announce server to master list
	if ( !sv_noMaster->current.integer && masterSockAdr.sin_family == AF_INET && authorizeSockAdr.sin_family == AF_INET )
	{
		if ( pthread_create(&proxy->masterServerThread, NULL, SV_ProxyMasterServerLoop, proxy) )
		{
			close(listenerSocket);
			Com_Error(
				ERR_FATAL,
				"\x15Proxy: Failed to create master server thread for version %s (protocol %i)",
				proxy->versionString,
				proxy->version);
		}
		proxy->masterServerThreadStarted = qtrue;
	}

	while ( 1 )
	{
		ssize_t bytes_received;
		char buffer[MAX_BUFFER_SIZE];
		struct sockaddr_in addr;
		socklen_t addr_len = sizeof(addr);
		char client_ip[INET_ADDRSTRLEN];
		int activeClient;
		netadr_t adr;
		char lowerCaseBuffer[MAX_BUFFER_SIZE];
		ssize_t bytes_sent;

		bytes_received = recvfrom(
			listenerSocket,
			buffer,
			sizeof(buffer) - 1,
			0,
			(struct sockaddr *)&addr,
			&addr_len);

		inet_ntop(AF_INET, &addr.sin_addr, client_ip, sizeof(client_ip));

		if ( bytes_received >= 0 )
		{
			if ( (unsigned int)bytes_received < sizeof(buffer) - 1 )
			{
				buffer[bytes_received] = '\0';
			}
			else
			{
				Com_DPrintf(
					"Proxy: Max. size exceeded at recvfrom on port %hu\n",
					BigShort(proxy->listenAdr.port));
				continue;
			}
		}
		else
		{
			Com_DPrintf(
				"Proxy: No data from recvfrom on port %hu\n",
				BigShort(proxy->listenAdr.port));
			continue;
		}

		activeClient = 0;
		SockadrToNetadr(&addr, &adr);
		strncpy(lowerCaseBuffer, buffer, MAX_BUFFER_SIZE);
		ToLowerCase(lowerCaseBuffer);

		// Messages that lead to an active connection
		if ( memcmp(lowerCaseBuffer, "\xFF\xFF\xFF\xFF""connect", 11) == 0 )
		{
			if ( SVC_ApplyConnectLimit(adr, proxy->bucket) )
				continue;

			// Prevent IP spoofing via userinfo IP client dvar
			if ( strlen(InfoValueForKey(lowerCaseBuffer + 11, "ip", proxy)) ||
			     strlen(InfoValueForKey(lowerCaseBuffer + 11, "port", proxy)) )
			{
				Com_Printf(
					"Proxy: Potential IP spoofing attempt from %s\n",
					inet_ntoa(addr.sin_addr));

				// Prevent excess outbound bandwidth usage when being flooded inbound
				if ( SVC_RateLimit(&outboundLeakyBuckets[proxy->bucket], 10, 100) )
				{
					// In theory, clients stay in the "Awaiting challenge..."
					// screen, but the disconnect messages that make it through
					// every now and then will still cause a normal disconnect
					Com_DPrintf("Proxy: IP spoofing disconnect rate limit exceeded, dropping response\n");
					continue;
				}

				// Cause disconnect on the client side
				bytes_sent = sendto(
					listenerSocket,
					DISCONNECT_MESSAGE,
					sizeof(DISCONNECT_MESSAGE),
					0,
					(struct sockaddr *)&addr,
					sizeof(addr));

				if ( bytes_sent == -1 )
					Com_DPrintf(
						"Proxy: Error %d on sendto when sending disconnect to %s: %s\n",
						errno,
						inet_ntoa(addr.sin_addr),
						strerror(errno));

				continue;
			}

			// Prepare insertion of public client address into userinfo string
			// so that the getIP() script function can return that IP although
			// the server only sees the proxy as peer, and status requests will
			// yield the external IP:Port combinations
			char ip_insertion[] = "\\ip\\";
			char port_insertion[] = "\\port\\";
			char client_port[6];
			snprintf(client_port, sizeof(client_port), "%hu", ntohs(addr.sin_port));
			size_t current_len = strlen(buffer);
			size_t added_len = strlen(ip_insertion) + strlen(client_ip) + strlen(port_insertion) + strlen(client_port);

			// Buffer size validation
			if ( current_len + added_len + 1 >= sizeof(buffer) )
			{
				Com_Printf(
					"Proxy: Not enough space in userinfo string for address from client %s\n",
					inet_ntoa(addr.sin_addr));
				continue;
			}

			// Insert data, starting at the closing quote
			size_t insert_position = current_len - 1;
			snprintf(
				buffer + insert_position,
				added_len + 2,
				"%s%s%s%s\"",
				ip_insertion,
				client_ip,
				port_insertion,
				client_port);
			buffer[insert_position + added_len + 2] = '\0';
			bytes_received = bytes_received + added_len;

			// Player appears to be connecting (direct connect)
			activeClient = 1;
		}
		else if ( memcmp(lowerCaseBuffer, "\xFF\xFF\xFF\xFFgetchallenge", 16) == 0 )
		{
			if ( SVC_ApplyChallengeLimit(adr, proxy->bucket) )
				continue;

			if ( memcmp(lowerCaseBuffer, "\xFF\xFF\xFF\xFFgetchallenge 0 \"", 20) == 0 )
			{
				// Player appears to be connecting
				activeClient = 1;
			}
			else
			{
				// Master server communication
			}
		}
		// Stateless messages
		else if ( memcmp(lowerCaseBuffer, "\xFF\xFF\xFF\xFFgetstatus", 13) == 0 )
		{
			if ( !SVC_ApplyStatusLimit(adr, proxy->bucket) &&
			     sv_noMaster->current.integer < 2 && 
			     ( !Sys_IsTrackerAddress(client_ip) || sv_proxiesVisibleForTrackers->current.boolean ) )
			{
				SV_SendCachedStatusResponse(proxy, buffer, bytes_received, &addr);
			}

			continue;
		}
		else if ( memcmp(lowerCaseBuffer, "\xFF\xFF\xFF\xFFgetinfo", 11) == 0 )
		{
			if ( !SVC_ApplyInfoLimit(adr, proxy->bucket) && 
			     sv_noMaster->current.integer < 2 &&
			     ( !Sys_IsTrackerAddress(client_ip) || sv_proxiesVisibleForTrackers->current.boolean ) )
			{
				SV_SendCachedInfoResponse(proxy, buffer, bytes_received, &addr);
			}
			
			continue;
		}
		else if ( memcmp(lowerCaseBuffer, "\xFF\xFF\xFF\xFFrcon", 8) == 0 )
		{
			// Block rcon entirely when sent to proxies
			continue;
		}

		// Check if a new client connection is to handle
		pthread_mutex_lock(&proxy->lock);

		if ( proxy->stopped )
		{
			pthread_mutex_unlock(&proxy->lock);
			break;
		}

		uint64_t clientIdentifier = CreateClientIdentifier(&addr);
		uint32_t clientIndex = GetHash(clientIdentifier, MAX_PROXY_CLIENT_THREADS);

		if ( clientIndex >= 0 && clientIndex < MAX_PROXY_CLIENT_THREADS )
		{
			proxyClientThreadInfo *clientThreadInfo = &proxy->clientThreadInfo[clientIndex];
			qboolean slotMatches = clientThreadInfo->socket != -1 && clientThreadInfo->identifier == clientIdentifier;
			if ( !slotMatches )
			{
				// New client connection to handle
				int s = socket(AF_INET, SOCK_DGRAM, 0);
				if ( s == -1 )
				{
					Com_DPrintf(
						"Proxy: Socket error %d at proxy port %hu: %s\n",
						errno,
						BigShort(proxy->listenAdr.port),
						strerror(errno));
					pthread_mutex_unlock(&proxy->lock);
					continue;
				}

				// Set client connection socket timeout. By default, use the
				// stock sv_timeout dvar to be in sync with the game, but with
				// a minimum timeout of 10s. Otherwise, if set to a value
				// greater than 0, the sv_proxyTimeout dvar value is used
				struct timeval tv;
				if ( sv_proxyTimeout->current.integer > 0 )
				{
					tv.tv_sec = sv_proxyTimeout->current.integer;
				}
				else
				{
					if ( sv_timeout->current.integer < 10 )
						tv.tv_sec = 10;
					else
						tv.tv_sec = sv_timeout->current.integer;
				}
				tv.tv_usec = 0;
				setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

				clientThreadInfo->clientAddr = addr;
				clientThreadInfo->generation++;
				clientThreadInfo->identifier = clientIdentifier;
				clientThreadInfo->socket = s;

				if ( activeClient )
				{
					proxy->numClients++;
					if ( com_sv_running->current.boolean )
						Com_DPrintf("Proxy: Client connecting from %s:%hu\n",
							client_ip,
							ntohs(addr.sin_port));
				}

				// Forward packet of new client to server
				if ( bytes_received > 0 )
				{
					bytes_sent = sendto(
						s,
						buffer,
						bytes_received,
						0,
						(struct sockaddr *)&forwarderAddr,
						sizeof(forwarderAddr));

					if ( bytes_sent == -1 )
						Com_DPrintf(
							"Proxy: Error %d on sendto when forwarding packet (from new client) to main server: %s\n",
							errno,
							strerror(errno));
				}

				proxyClientThreadArgs *args = (proxyClientThreadArgs *)Z_MallocInternal(sizeof(proxyClientThreadArgs));
				uint32_t generation = clientThreadInfo->generation;

				args->activeClient = activeClient;
				args->addr = addr;
				args->clientIndex = clientIndex;
				args->forwardAdr = proxy->forwardAdr;
				args->generation = generation;
				args->proxy = proxy;
				args->socket = s;

				int pthreadCreate = pthread_create(&clientThreadInfo->thread, NULL, SV_ProxyClientThread, args);
				if ( pthreadCreate )
				{
					Com_DPrintf(
						"Proxy: Failed to create new client thread at port %hu, error %d\n",
						BigShort(proxy->listenAdr.port),
						pthreadCreate);
					Z_FreeInternal(args);
					close(s);
					clientThreadInfo->socket = -1;
					if ( activeClient )
						proxy->numClients--;
				}
				else
				{
					pthread_detach(clientThreadInfo->thread);
				}
			}
			else
			{
				// Forward packets of known client to server
				if ( bytes_received > 0 )
				{
					bytes_sent = sendto(
						clientThreadInfo->socket,
						buffer,
						bytes_received,
						0,
						(struct sockaddr *)&forwarderAddr,
						sizeof(forwarderAddr));

					if ( bytes_sent == -1 )
						Com_DPrintf(
							"Proxy: Error %d on sendto when forwarding packet (from known client) to main server: %s\n",
							errno,
							strerror(errno));
				}
			}
		}
		else
			Com_DPrintf("Proxy: Invalid client address: %s\n", inet_ntoa(addr.sin_addr));
		
		pthread_mutex_unlock(&proxy->lock);
	}
	return NULL;
}

void * SV_ProxyQueryCacheLoop(void *threadArgs)
{
	proxy_t *proxy = (proxy_t *)threadArgs;
	int s;
	struct sockaddr_in forwarderAddr;

	// Socket for getting getinfo/getstatus data from main server
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if ( s == -1 )
	{
		Com_Error(ERR_FATAL, "\x15Proxy: Failed to open cache query socket");
		return NULL;
	}

	// Recv timeout of 1s
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));

	NetadrToSockadr(&proxy->forwardAdr, &forwarderAddr);

	while ( !proxy->stopped )
	{
		if ( com_sv_running->current.boolean )
		{
			// Only update the status cache if updating the info cache
			// succeeded to cover the case where one of those two runs into the
			// socket timeout of 1s. This can happen during map load where one
			// of the requests would be answered too late, thus putting the
			// response into the buffer meant of the subsequent request. We
			// keep the short timeout to have a quick cache update once the
			// server responds again
			if ( SV_RefreshInfoCache(proxy, s, &forwarderAddr) )
			{
				SV_RefreshStatusCache(proxy, s, &forwarderAddr);
			}
		}

		usleep(sv_proxyQueryCacheRefreshTime->current.integer * 1000);
	}

	close(s);

	return NULL;
}

qboolean SV_RefreshInfoCache(proxy_t *proxy, int s, struct sockaddr_in *forwarderAddr)
{
	char buffer[MAX_INFO_STRING];
	char cache_buffer[MAX_INFO_STRING];
	struct sockaddr_in r_addr;
	socklen_t r_len;
	int bytes_received;
	ssize_t bytes_sent;
	netadr_t sender_adr;
	char client_ip[INET_ADDRSTRLEN];

	// Request infoResponse from main server
	bytes_sent = sendto(
		s,
		GETINFO_MESSAGE,
		sizeof(GETINFO_MESSAGE) - 1,
		0,
		(struct sockaddr *)forwarderAddr,
		sizeof(*forwarderAddr));

	if ( bytes_sent == -1 )
	{
		Com_DPrintf("Proxy: No data sent at info cache refresh\n");
		return qfalse;
	}

	r_len = sizeof(r_addr);

	bytes_received = recvfrom(
		s,
		buffer,
		sizeof(buffer) - 1,
		0,
		(struct sockaddr *)&r_addr,
		&r_len);

	if ( bytes_received <= 0 )
	{
		Com_DPrintf("Proxy: No data at recvfrom at info cache refresh, check the sv_proxyForwardAddress dvars for validity at runtime\n");
		return qfalse;
	}

	buffer[bytes_received] = '\0';

	SockadrToNetadr(
		&r_addr,
		&sender_adr);

	// Check source
	if ( !Sys_IsMainServerAddress(sender_adr) )
	{
		if ( inet_ntop(AF_INET, &r_addr.sin_addr, client_ip, sizeof(client_ip)) )
		{
			Com_DPrintf(
				"Proxy: Dropping packet from %s:%hu at info cache refresh\n",
				client_ip,
				ntohs(r_addr.sin_port));
		}
		return qfalse;
	}

	// Check response header
	if ( memcmp(buffer, "\xFF\xFF\xFF\xFFinfoResponse", 16) != 0 )
	{
		if ( inet_ntop(AF_INET, &r_addr.sin_addr, client_ip, sizeof(client_ip)) )
		{
			Com_DPrintf(
				"Proxy: Dropping packet from %s, invalid header at info cache refresh\n",
				client_ip);
		}
		return qfalse;
	}

	// Work on a copy
	I_strncpyz(cache_buffer, buffer, sizeof(cache_buffer));

	// Remove challenge if present
	Info_RemoveKey(cache_buffer, "challenge");

	// Response version rewriting
	ReplaceProtocolString(cache_buffer, proxy);
	ReplaceShortversionString(cache_buffer, proxy);

	// Save to cache
	pthread_mutex_lock(&proxy->queryCacheLock);
	I_strncpyz(proxy->queryCache.infoResponse, cache_buffer, sizeof(proxy->queryCache.infoResponse));
	proxy->queryCache.infoResponseLen = strlen(proxy->queryCache.infoResponse);
	proxy->queryCache.infoResponseValid = qtrue;
	pthread_mutex_unlock(&proxy->queryCacheLock);

	return qtrue;
}

qboolean SV_RefreshStatusCache(proxy_t *proxy, int s, struct sockaddr_in *forwarderAddr)
{
	char buffer[BIG_INFO_STRING];
	char cache_buffer[BIG_INFO_STRING];
	struct sockaddr_in r_addr;
	socklen_t r_len;
	int bytes_received;
	ssize_t bytes_sent;
	netadr_t sender_adr;
	char client_ip[INET_ADDRSTRLEN];

	// Request statusResponse from main server
	bytes_sent = sendto(
		s,
		GETSTATUS_MESSAGE,
		sizeof(GETSTATUS_MESSAGE) - 1,
		0,
		(struct sockaddr *)forwarderAddr,
		sizeof(*forwarderAddr));

	if ( bytes_sent == -1 )
	{
		Com_DPrintf("Proxy: No data sent at status cache refresh\n");
		return qfalse;
	}

	r_len = sizeof(r_addr);

	bytes_received = recvfrom(
		s,
		buffer,
		sizeof(buffer) - 1,
		0,
		(struct sockaddr *)&r_addr,
		&r_len);

	if ( bytes_received <= 0 )
	{
		Com_DPrintf("Proxy: No data at recvfrom at status cache refresh, check the sv_proxyForwardAddress dvars for validity at runtime\n");
		return qfalse;
	}

	buffer[bytes_received] = '\0';

	SockadrToNetadr(
		&r_addr,
		&sender_adr);

	// Check source
	if ( !Sys_IsMainServerAddress(sender_adr) )
	{
		if ( inet_ntop(AF_INET, &r_addr.sin_addr, client_ip, sizeof(client_ip)) )
		{
			Com_DPrintf(
				"Proxy: Dropping packet from %s:%hu at status cache refresh\n",
				client_ip,
				ntohs(r_addr.sin_port));
		}
		return qfalse;
	}

	// Check response header
	if ( memcmp(buffer, "\xFF\xFF\xFF\xFFstatusResponse", 18) != 0 )
	{
		if ( inet_ntop(AF_INET, &r_addr.sin_addr, client_ip, sizeof(client_ip)) )
		{
			Com_DPrintf(
				"Proxy: Dropping packet from %s, invalid header at status cache refresh\n",
				client_ip);
		}
		return qfalse;
	}

	// Work on a copy
	I_strncpyz(cache_buffer, buffer, sizeof(cache_buffer));

	// Remove challenge if present
	Info_RemoveKey_Big(cache_buffer, "challenge");

	// Response version rewriting
	ReplaceProtocolString(cache_buffer, proxy);
	ReplaceShortversionString(cache_buffer, proxy);

	// Save to cache
	pthread_mutex_lock(&proxy->queryCacheLock);
	I_strncpyz(proxy->queryCache.statusResponse, cache_buffer, sizeof(proxy->queryCache.statusResponse));
	proxy->queryCache.statusResponseLen = strlen(proxy->queryCache.statusResponse);
	proxy->queryCache.statusResponseValid = qtrue;
	pthread_mutex_unlock(&proxy->queryCacheLock);

	return qtrue;
}

void SV_SendCachedInfoResponse(proxy_t *proxy, const char *requestBuffer, int requestLen, struct sockaddr_in *addr)
{
	proxyQueryCache_t cache;
	char out[MAX_LEGACY_MSGLEN]; // Message size with legacy client compatibility
	char challenge[MAX_CHALLENGE_CHARS + 1];
	char *infoString;
	int packetLen;
	ssize_t bytes_sent;

	if ( !com_sv_running->current.boolean )
		return;

	// Take challenge from request, if there is
	challenge[0] = '\0';
	SV_ExtractChallenge(requestBuffer, requestLen, strlen(GETINFO_MESSAGE), challenge);

	// Copy and validate cache
	pthread_mutex_lock(&proxy->queryCacheLock);
	cache = proxy->queryCache;
	pthread_mutex_unlock(&proxy->queryCacheLock);
	if ( !cache.infoResponseValid )
	{
		Com_DPrintf("Proxy: Cached info response requested but cache is not valid\n");
		return;
	}
	memcpy(out, cache.infoResponse, cache.infoResponseLen + 1);
	infoString = strchr(out, '\\');
	if ( !infoString )
	{
		Com_DPrintf("Proxy: No info string in cache\n");
		return;
	}

	// Add challenge, if there is
	if ( challenge[0] )
	{
		Info_SetValueForKey(infoString, "challenge", challenge);
	}

	// Send back to client
	packetLen = strlen(out);
	bytes_sent = sendto(
		proxy->socket,
		out,
		packetLen,
		0,
		(struct sockaddr *)addr,
		sizeof(*addr));

	if ( bytes_sent == -1 )
	{
		Com_DPrintf("Proxy: Error %d on sendto for cached info response: %s\n", errno, strerror(errno));
		return;
	}
}

void SV_SendCachedStatusResponse(proxy_t *proxy, const char *requestBuffer, int requestLen, struct sockaddr_in *addr)
{
	proxyQueryCache_t cache;
	char out[MAX_LEGACY_MSGLEN];
	char challenge[MAX_CHALLENGE_CHARS + 1];
	char *statusString;
	int packetLen;
	ssize_t bytes_sent;

	if ( !com_sv_running->current.boolean )
		return;

	// Take challenge from request, if there is
	challenge[0] = '\0';
	SV_ExtractChallenge(requestBuffer, requestLen, strlen(GETSTATUS_MESSAGE), challenge);

	// Copy and validate cache
	pthread_mutex_lock(&proxy->queryCacheLock);
	cache = proxy->queryCache;
	pthread_mutex_unlock(&proxy->queryCacheLock);
	if ( !cache.statusResponseValid )
	{
		Com_DPrintf("Proxy: Cached status response requested but cache is not valid\n");
		return;
	}

	memcpy(out, cache.statusResponse, cache.statusResponseLen + 1);
	statusString = strchr(out, '\\');
	if ( !statusString )
	{
		Com_DPrintf("Proxy: No status string in cache\n");
		return;
	}

	// Add challenge, if there is
	if ( challenge[0] )
	{
		Info_SetValueForKey(statusString, "challenge", challenge);
	}

	// Send back to client
	packetLen = strlen(out);
	bytes_sent = sendto(
		proxy->socket,
		out,
		packetLen,
		0,
		(struct sockaddr *)addr,
		sizeof(*addr));

	if ( bytes_sent == -1 )
	{
		Com_DPrintf(
			"Proxy: Error %d on sendto for cached status response: %s\n",
			errno,
			strerror(errno));
		return;
	}
}

void SV_ExtractChallenge(const char *request, int requestLen, int offset, char *challenge)
{
    int i;
    int start;

    challenge[0] = '\0';

	// Skip header
    if ( requestLen <= offset )
    {
        return;
    }

	// Challenge is expected after header + space char(s)
    start = offset;
    while ( start < requestLen && request[start] == ' ' )
    {
        start++;
    }

    if ( start >= requestLen )
    {
        return;
    }

	// Copy at most MAX_CHALLENGE_CHARS chars, NULL-terminate
    for ( i = 0; i < MAX_CHALLENGE_CHARS && start + i < requestLen; i++ )
    {
        challenge[i] = request[start + i];
    }
    challenge[i] = '\0';
}

void * SV_ProxyMasterServerLoop(void *threadArgs)
{
	proxy_t *proxy = (proxy_t *)threadArgs;
	netadr_t authorizeAdr;
	qboolean startup = qtrue;
	int proxyIndex = proxy - &proxies[0];
	ssize_t bytes_sent;

	SockadrToNetadr(&authorizeSockAdr, &authorizeAdr);

	while ( 1 )
	{
		// Delay on startup to counter rate-limiting on the master server side
		if ( startup )
		{
			sleep(proxyIndex + 1);
			startup = qfalse;
		}

		// Do not continue heartbeats while the server is at rest (e.g., map
		// load error), but the process is still up. Also wait until the info
		// and status request buffers contain valid data
		while ( !com_sv_running->current.boolean ||
		        !proxy->queryCache.infoResponseValid || 
		        !proxy->queryCache.statusResponseValid )
		{

			sleep(1);
		}

		if ( sv_logHeartbeats->current.boolean )
		{
			Com_Printf(
				"Sending proxy heartbeat to %s for version %s (protocol %i)\n",
				sv_masterServer->current.string,
				proxy->versionString,
				proxy->version);
		}

		bytes_sent = sendto(
			proxy->socket,
			HEARTBEAT_MESSAGE,
			strlen(HEARTBEAT_MESSAGE),
			0,
			(struct sockaddr*)&masterSockAdr,
			sizeof(masterSockAdr));
		
		if ( bytes_sent == -1 )
		{
			Com_DPrintf(
				"Proxy: Error on sendto when sending heartbeat to %s for version %s (protocol %i)\n",
				sv_masterServer->current.string,
				proxy->versionString,
				proxy->version);
		}

		if ( sv_logHeartbeats->current.boolean )
		{
			Com_Printf(
				"Sending proxy getIpAuthorize to %s for version %s (protocol %i)\n",
				sv_authorizeServer->current.string,
				proxy->versionString,
				proxy->version);
		}

		char AUTHORIZE_SEND[MAX_BUFFER_SIZE];
		snprintf(
			AUTHORIZE_SEND,
			sizeof(AUTHORIZE_SEND),
			"%s %ld %i.%i.%i.%i \"%s\" 0",
			AUTHORIZE_MESSAGE,
			time(NULL),
			authorizeAdr.ip[0],
			authorizeAdr.ip[1],
			authorizeAdr.ip[2],
			authorizeAdr.ip[3],
			fs_game->current.string);

		bytes_sent = sendto(
			proxy->socket,
			AUTHORIZE_SEND,
			strlen(AUTHORIZE_SEND),
			0,
			(struct sockaddr*)&authorizeSockAdr,
			sizeof(authorizeSockAdr));

		if ( bytes_sent == -1 )
		{
			Com_DPrintf(
				"Proxy: Error on sendto when sending getIpAuthorize to %s for version %s (protocol %i)\n",
				sv_masterServer->current.string,
				proxy->versionString,
				proxy->version);
		}
		
		// Stock heartbeats are sent at least every 180 seconds
		sleep(60);
	}
	return NULL;
}

void * SV_ProxyClientThread(void *threadArgs)
{
	proxyClientThreadArgs *args = (proxyClientThreadArgs *)threadArgs;
	proxy_t *proxy = args->proxy;
	char client_ip[INET_ADDRSTRLEN];

	while ( 1 )
	{
		char buffer[MAX_BUFFER_SIZE];
		struct sockaddr_in r_addr;
		socklen_t r_len = sizeof(r_addr);
		ssize_t bytes_received;
		ssize_t bytes_sent;
		netadr_t senderAdr;
		
		bytes_received = recvfrom(
			args->socket,
			buffer,
			sizeof(buffer) - 1,
			0,
			(struct sockaddr *)&r_addr,
			&r_len);

		// Make sure that only the main server sends stuff to the client socket
		SockadrToNetadr(&r_addr, &senderAdr);
		if ( !Sys_IsMainServerAddress(senderAdr) )
		{
			if ( bytes_received >= 0 )
			{
				if ( inet_ntop(AF_INET, &r_addr.sin_addr, client_ip, sizeof(client_ip)) )
				{
					Com_DPrintf("Proxy: Dropping packet from %s:%hu: %s\n", client_ip, ntohs(r_addr.sin_port), buffer);
				}
			}
			continue;
		}

		inet_ntop(AF_INET, &args->addr.sin_addr, client_ip, sizeof(client_ip));

		if ( bytes_received >= 0 )
		{
			if ( (unsigned int)bytes_received < sizeof(buffer) - 1 )
			{
				buffer[bytes_received] = '\0';
			}
			else
			{
				Com_DPrintf("Proxy: Max. size exceeded at recvfrom in client thread\n");
				continue;
			}
		}
		else
		{
			if ( errno == EAGAIN || errno == EWOULDBLOCK )
			{
				break;
			}
			else
			{
				Com_DPrintf("Proxy: No data at recvfrom in client thread\n");
				continue;
			}
		}

		// Adjust version strings in response buffer
		if ( memcmp(buffer, "\xFF\xFF\xFF\xFFstatusResponse", 18) == 0 )
		{
			ReplaceProtocolString(buffer, proxy);
			ReplaceShortversionString(buffer, proxy);
		}
		else if ( memcmp(buffer, "\xFF\xFF\xFF\xFFinfoResponse", 16) == 0 )
		{
			ReplaceProtocolString(buffer, proxy);
		}

		// Forward packets to the client
		if ( bytes_received > 0 )
		{
			bytes_sent = sendto(
				args->proxy->socket,
				buffer,
				bytes_received,
				0,
				(struct sockaddr *)&args->addr,
				sizeof(args->addr));

			if ( bytes_sent == -1 )
			{
				if ( errno == EBADF )
					break;
				else
					Com_DPrintf(
						"Proxy: Error %d on sendto when forwarding packets to client: %s\n",
						errno,
						strerror(errno));
			}
		}
	}

	pthread_mutex_lock(&proxy->lock);
	close(args->socket);
	proxyClientThreadInfo *slot = &proxy->clientThreadInfo[args->clientIndex];
	if ( slot->generation == args->generation && slot->socket == args->socket )
	{
		slot->socket = -1;
		slot->identifier = 0;
		memset(&slot->clientAddr, 0, sizeof(slot->clientAddr));
	}

	if ( args->activeClient )
	{
		proxy->numClients--;
		if ( strlen(client_ip) && com_sv_running->current.boolean )
		{
			Com_DPrintf(
				"Proxy: Client connection released for %s:%hu\n",
				client_ip,
				ntohs(args->addr.sin_port));
		}
	}
	pthread_mutex_unlock(&proxy->lock);

	Z_FreeInternal(args);
	
	return NULL;
}