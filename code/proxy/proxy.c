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

#define TIMEOUT_SECONDS 240
#define MAX_BUFFER_SIZE 65536

char HEARTBEAT_MESSAGE[] = "\xFF\xFF\xFF\xFFheartbeat COD-2";
char HEARTBEAT_STOP_MESSAGE[] = "\xFF\xFF\xFF\xFFheartbeat flatline";
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

extern dvar_t *com_sv_running;
extern dvar_t *fs_game;
extern dvar_t *net_ip;
extern dvar_t *net_port;
extern dvar_t *sv_authorizePort;
extern dvar_t *sv_authorizeServer;
extern dvar_t *sv_logHeartbeats;
extern dvar_t *sv_masterPort;
extern dvar_t *sv_masterServer;
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
	"208.167.241.187",
	"108.61.78.150",
	"108.61.78.149",
	"149.28.43.230",
	"45.77.96.90",
	"159.69.0.99",
	"178.254.22.101",
	"63.239.170.80",
	"63.239.170.18",
	"63.239.170.8",
	"159.146.13.56",
	"217.131.98.94",
	"78.46.106.94"
};

unsigned long HashString(const char *str)
{
	unsigned long hash = 5381;
	int c;

	while ( ( c = *str++ ) )
		hash = ((hash << 5) + hash) + c; // djb2 algorithm

	return hash;
}

uint64_t CreateClientIdentifier(struct sockaddr_in *addr)
{
	char client_info[INET_ADDRSTRLEN + 6];
	uint32_t client_address = ntohl(addr->sin_addr.s_addr);
	uint16_t client_port = ntohs(addr->sin_port);

	snprintf(client_info, sizeof(client_info), "%u:%u", client_address, client_port);

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
	static char value[MAX_PROXIES][2][8192]; // Use two buffers so that
	// comparisons that call the function twice work without stomping on each
	// other's buffers - for each thread
	static int valueindex = 0; 
	int proxyindex = proxy - &proxies[0];
	char *o;

	if ( !s || !key )
	{
		return "";
	}

	if ( strlen( s ) >= 8192 )
	{
		Com_Error(ERR_FATAL, "InfoValueForKey: oversize infostring" );
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
	const char *proxyProtocolString = VaProxies(proxy, "\\protocol\\%d", proxy->version);
	const char *parentProtocolString = VaProxies(proxy, "\\protocol\\%d", proxy->parentVersion);
	char *offset = strstr(buffer, parentProtocolString);
	if ( offset != NULL )
		memcpy(offset, proxyProtocolString, 13);
}

void ReplaceShortversionString(char *buffer, proxy_t *proxy)
{
	const char *proxyShortversionString = VaProxies(proxy, "\\shortversion\\%s", proxy->versionString);
	const char *parentShortversionString = VaProxies(proxy, "\\shortversion\\%s", proxy->parentVersionString);
	char *offset = strstr(buffer, parentShortversionString);
	if ( offset != NULL )
		memcpy(offset, proxyShortversionString, 17);
}

qboolean Sys_IsProxyAddress(netadr_t from)
{
	if ( initialized )
	{
		int i;
		proxy_t *proxy;

		for ( i = 0; i < MAX_PROXIES; i++ )
		{
			proxy = &proxies[i];
			if ( proxy->enabled && proxy->started && NET_CompareBaseAdr(from, proxy->listenAdr) )
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
	memset(&proxies, 0, sizeof(proxy_t) * MAX_PROXIES);
}

void SV_ConfigureProxy(proxy_t *proxy, int version, const char *address, const char *forwardAddress, int parentVersion)
{
	int index = (proxy - &proxies[0]) + 1;
	const char *versionString = getShortVersionFromProtocol(version);

	Com_DPrintf("Proxy: Configuring proxy for version %s (protocol %i)\n", versionString, version);

	NET_StringToAdr(address, &proxy->listenAdr);
	if ( proxy->listenAdr.type == NA_BAD )
		Com_Error(ERR_FATAL, "\x15""Failed to parse listen address for proxy with version %s", versionString);

	NET_StringToAdr(forwardAddress, &proxy->forwardAdr);
	if ( proxy->forwardAdr.type == NA_BAD )
		Com_Error(ERR_FATAL, "\x15""Failed to parse forward address for proxy with version %s", versionString);

	proxy->lock = PTHREAD_MUTEX_INITIALIZER;
	proxy->numClients = 0;
	proxy->parentVersion = parentVersion;
	proxy->parentVersionString = getShortVersionFromProtocol(parentVersion);
	proxy->version = version;
	proxy->versionString = versionString;
	proxy->bucket = (outboundLeakyBucketIndex_t)index;
	proxy->enabled = qtrue;
}

void SV_ShutdownProxies()
{
	if ( initialized )
	{
		int i;
		proxy_t *proxy;

		for ( i = 0; i < MAX_PROXIES; i++ )
		{
			proxy = &proxies[i];
			if ( proxy->enabled && proxy->started )
			{
				printf("> [LIBCOD] Proxy: Shutting down proxy for version %s (protocol %i) on port %hu\n", proxy->versionString, proxy->version, BigShort(proxy->listenAdr.port));

				// Stop thread for announcements to master server
				if ( proxy->masterServerThread )
					pthread_cancel(*proxy->masterServerThread);

				// Tell the master server that we've gone offline. Should fix
				// a potential issue where this server might not show up in the
				// ingame browser list after being restarted
				if ( proxy->masterSockAdr )
				{
					// Delay to counter rate-limiting on the master server side
					sleep(1);

					sendto(proxy->socket, HEARTBEAT_STOP_MESSAGE, strlen(HEARTBEAT_STOP_MESSAGE), 0, (struct sockaddr *)proxy->masterSockAdr, sizeof(struct sockaddr_in));
				}

				// Stop main proxy thread that accepts new clients
				if ( proxy->mainThread )
					pthread_cancel(proxy->mainThread);

				// Cleanup main socket
				if ( proxy->socket )
					close(proxy->socket);
			}
		}
		SV_ResetProxiesInformation();
	}
}

void SV_SetupProxies()
{
	if ( initialized )
		return;

	Com_Printf("-----------------------------------\n");

	const char *forwardAddress = va("%s:%hu", net_ip->current.string, net_port->current.integer);

	sv_proxiesVisibleForTrackers = Dvar_RegisterBool("sv_proxiesVisibleForTrackers", qfalse, DVAR_ARCHIVE);
	sv_proxyAddress_1_0 = Dvar_RegisterString("sv_proxyAddress_1_0", "0.0.0.0:28960", DVAR_ARCHIVE);
	sv_proxyAddress_1_2 = Dvar_RegisterString("sv_proxyAddress_1_2", "0.0.0.0:28961", DVAR_ARCHIVE);
	sv_proxyAddress_1_3 = Dvar_RegisterString("sv_proxyAddress_1_3", "0.0.0.0:28962", DVAR_ARCHIVE);
	sv_proxyAddress_1_3_119 = Dvar_RegisterString("sv_proxyAddress_1_3_119", "0.0.0.0:28963", DVAR_ARCHIVE);
	sv_proxyEnable_1_0 = Dvar_RegisterBool("sv_proxyEnable_1_0", qfalse, DVAR_ARCHIVE);
	sv_proxyEnable_1_2 = Dvar_RegisterBool("sv_proxyEnable_1_2", qfalse, DVAR_ARCHIVE);
	sv_proxyEnable_1_3 = Dvar_RegisterBool("sv_proxyEnable_1_3", qfalse, DVAR_ARCHIVE);
	sv_proxyEnable_1_3_119 = Dvar_RegisterBool("sv_proxyEnable_1_3_119", qfalse, DVAR_ARCHIVE);
	sv_proxyForwardAddress_1_0 = Dvar_RegisterString("sv_proxyForwardAddress_1_0", forwardAddress, DVAR_ARCHIVE);
	sv_proxyForwardAddress_1_2 = Dvar_RegisterString("sv_proxyForwardAddress_1_2", forwardAddress, DVAR_ARCHIVE);
	sv_proxyForwardAddress_1_3 = Dvar_RegisterString("sv_proxyForwardAddress_1_3", forwardAddress, DVAR_ARCHIVE);
	sv_proxyForwardAddress_1_3_119 = Dvar_RegisterString("sv_proxyForwardAddress_1_3_119", forwardAddress, DVAR_ARCHIVE);

	SV_ResetProxiesInformation();

	switch ( getProtocolFromShortVersion(sv_version->current.string) )
	{
		case 115:
			if ( sv_proxyEnable_1_2->current.boolean )
				SV_ConfigureProxy(&proxies[0], 117, sv_proxyAddress_1_2->current.string, sv_proxyForwardAddress_1_2->current.string, 115);
			if ( sv_proxyEnable_1_3->current.boolean )
				SV_ConfigureProxy(&proxies[1], 118, sv_proxyAddress_1_3->current.string, sv_proxyForwardAddress_1_3->current.string, 115);
			if ( sv_proxyEnable_1_3_119->current.boolean )
				SV_ConfigureProxy(&proxies[2], 119, sv_proxyAddress_1_3_119->current.string, sv_proxyForwardAddress_1_3_119->current.string, 115);
			break;
		case 117:
			if ( sv_proxyEnable_1_0->current.boolean )		
				SV_ConfigureProxy(&proxies[0], 115, sv_proxyAddress_1_0->current.string, sv_proxyForwardAddress_1_0->current.string, 117);
			if ( sv_proxyEnable_1_3->current.boolean )
				SV_ConfigureProxy(&proxies[1], 118, sv_proxyAddress_1_3->current.string, sv_proxyForwardAddress_1_3->current.string, 117);
			if ( sv_proxyEnable_1_3_119->current.boolean )
				SV_ConfigureProxy(&proxies[2], 119, sv_proxyAddress_1_3_119->current.string, sv_proxyForwardAddress_1_3_119->current.string, 117);
			break;
		case 118:
			if ( sv_proxyEnable_1_0->current.boolean )
				SV_ConfigureProxy(&proxies[0], 115, sv_proxyAddress_1_0->current.string, sv_proxyForwardAddress_1_0->current.string, 118);
			if ( sv_proxyEnable_1_2->current.boolean )
				SV_ConfigureProxy(&proxies[1], 117, sv_proxyAddress_1_2->current.string, sv_proxyForwardAddress_1_2->current.string, 118);
			if ( sv_proxyEnable_1_3_119->current.boolean )
				SV_ConfigureProxy(&proxies[2], 119, sv_proxyAddress_1_3_119->current.string, sv_proxyForwardAddress_1_3_119->current.string, 118);
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
				Com_DPrintf("Proxy: Failed to start proxy thread for version %s (protocol %i) on port %hu\n", proxy->versionString, proxy->version, BigShort(proxy->listenAdr.port));
			}
			else
			{
				Com_DPrintf("Proxy: Started proxy thread for version %s (protocol %i) on port %hu\n", proxy->versionString, proxy->version, BigShort(proxy->listenAdr.port));
				pthread_detach(proxy->mainThread);
				proxy->started = qtrue;
			}
		}
	}

	initialized = qtrue;
}

void * SV_StartProxy(void *threadArgs)
{
	proxy_t *proxy = (proxy_t *)threadArgs;
	struct sockaddr_in listenerAddr;
	struct sockaddr_in forwarderAddr;
	int listenerSocket;
	pthread_t masterServerThread;

	memset(&listenerAddr, 0, sizeof(listenerAddr));
	NetadrToSockadr(&proxy->listenAdr, &listenerAddr);

	memset(&forwarderAddr, 0, sizeof(forwarderAddr));
	NetadrToSockadr(&proxy->forwardAdr, &forwarderAddr);

	// Open a new UDP socket for that proxy
	listenerSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if ( listenerSocket == -1 )
	{
		Com_Error(ERR_FATAL, "\x15Proxy: Failed to open socket on port %hu", BigShort(proxy->listenAdr.port));
	}
	if ( bind(listenerSocket, (struct sockaddr *)&listenerAddr, sizeof(listenerAddr)) == -1 )
	{
		close(listenerSocket);
		Com_Error(ERR_FATAL, "\x15Proxy: Failed to bind on socket on port %hu", BigShort(proxy->listenAdr.port));
	}
	proxy->socket = listenerSocket;

	if ( pthread_create(&masterServerThread, NULL, SV_ProxyMasterServerLoop, proxy) )
	{
		close(listenerSocket);
		Com_Error(ERR_FATAL, "\x15Proxy: Failed to create master server thread for version %s (protocol %i)", proxy->versionString, proxy->version);
	}
	else
	{
		pthread_detach(masterServerThread);
	}
	proxy->masterServerThread = &masterServerThread;

	Com_Printf("Proxy server listening on port %hu for version %s (protocol %i)\n", BigShort(proxy->listenAdr.port), proxy->versionString, proxy->version);

	proxyClientThreadInfo clientThreadInfo[65536];
	memset(clientThreadInfo, -1, sizeof(clientThreadInfo));

	while ( 1 )
	{
		char buffer[MAX_BUFFER_SIZE];
		char lowerCaseBuffer[MAX_BUFFER_SIZE];
		struct sockaddr_in addr;
		netadr_t adr;
		socklen_t addr_len = sizeof(addr);

		ssize_t bytes_received = recvfrom(listenerSocket, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&addr, &addr_len);
		if ( bytes_received >= 0 )
		{
			if ( (unsigned int)bytes_received < sizeof(buffer) - 1 )
				buffer[bytes_received] = '\0';
			else
			{
				Com_DPrintf("Proxy: Max. size exceeded at recvfrom on port %hu\n", BigShort(proxy->listenAdr.port));
				continue;
			}
		}
		else
		{
			Com_DPrintf("Proxy: No data from recvfrom on port %hu\n", BigShort(proxy->listenAdr.port));
			continue;
		}

		int activeClient = 1;
		char client_ip[INET_ADDRSTRLEN];

		inet_ntop(AF_INET, &addr.sin_addr, client_ip, sizeof(client_ip));
		SockadrToNetadr(&addr, &adr);
		strncpy(lowerCaseBuffer, buffer, MAX_BUFFER_SIZE);
		ToLowerCase(lowerCaseBuffer);

		if ( memcmp(lowerCaseBuffer, "\xFF\xFF\xFF\xFFgetstatus", 13) == 0 )
		{
			if ( SVC_ApplyStatusLimit(adr, proxy->bucket) )
				continue;

			activeClient = 0;
		}
		else if ( memcmp(lowerCaseBuffer, "\xFF\xFF\xFF\xFF""connect", 11) == 0 )
		{
			if ( SVC_ApplyConnectLimit(adr, proxy->bucket) )
				continue;

			// Prevent IP spoofing via userinfo IP client dvar
			if ( strlen(InfoValueForKey(lowerCaseBuffer + 11, "ip", proxy)) || strlen(InfoValueForKey(lowerCaseBuffer + 11, "port", proxy)) )
			{
				Com_Printf("Proxy: Potential IP spoofing attempt from %s\n", inet_ntoa(addr.sin_addr));

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
				sendto(listenerSocket, DISCONNECT_MESSAGE, sizeof(DISCONNECT_MESSAGE), 0, (struct sockaddr *)&addr, sizeof(addr));
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
				Com_Printf("Proxy: Not enough space in userinfo string for address from client %s\n", inet_ntoa(addr.sin_addr));
				continue;
			}

			// Insert data, starting at the closing quote
			size_t insert_position = current_len - 1;
			snprintf(buffer + insert_position, added_len + 2, "%s%s%s%s\"", ip_insertion, client_ip, port_insertion, client_port);
			buffer[insert_position + added_len + 2] = '\0';
			bytes_received = bytes_received + added_len;
		}
		else if ( memcmp(lowerCaseBuffer, "\xFF\xFF\xFF\xFFgetchallenge", 16) == 0 )
		{
			if ( SVC_ApplyChallengeLimit(adr, proxy->bucket) )
				continue;

			if ( memcmp(lowerCaseBuffer, "\xFF\xFF\xFF\xFFgetchallenge 0 \"", 20) != 0 )
				activeClient = 0;
		}
		else if ( memcmp(lowerCaseBuffer, "\xFF\xFF\xFF\xFFgetinfo", 11) == 0 )
		{
			if ( SVC_ApplyInfoLimit(adr, proxy->bucket) )
				continue;
		}
		else if ( memcmp(lowerCaseBuffer, "\xFF\xFF\xFF\xFFrcon", 8) == 0 )
		{
			// Block rcon entirely when sent to proxies
			continue;
		}

		// Check if a new client connection is to handle
		pthread_mutex_lock(&proxy->lock);

		uint64_t clientIdentifier = CreateClientIdentifier(&addr);
		uint32_t clientIndex = GetHash(clientIdentifier, sizeof(clientThreadInfo) / sizeof(clientThreadInfo[0]));

		if ( clientIndex >= 0 && clientIndex < sizeof(clientThreadInfo) / sizeof(clientThreadInfo[0]) )
		{
			if ( proxy->numClients == 0 || clientThreadInfo[clientIndex].s_client == -1 )
			{
				// New client connection to handle
				int s_client = socket(AF_INET, SOCK_DGRAM, 0);

				if ( s_client == -1 )
				{
					Com_DPrintf("Proxy: Socket error at port %hu\n", BigShort(proxy->listenAdr.port));
					pthread_mutex_unlock(&proxy->lock);
					continue;
				}

				struct timeval tv;
				tv.tv_sec = TIMEOUT_SECONDS;
				tv.tv_usec = 0;
				setsockopt(s_client, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

				proxyClientThreadInfo t_info;
				t_info.s_client = s_client;
				clientThreadInfo[clientIndex] = t_info;
				proxy->numClients++;

				if ( activeClient && com_sv_running->current.boolean )
					Com_DPrintf("Proxy: Client connecting from %s:%hu\n", client_ip, ntohs(addr.sin_port));

				sendto(s_client, buffer, bytes_received, 0, (struct sockaddr *)&forwarderAddr, sizeof(forwarderAddr));

				proxyClientThreadArgs *args = (proxyClientThreadArgs *)malloc(sizeof(proxyClientThreadArgs));
				args->addr = addr;
				args->s_client = &(clientThreadInfo[clientIndex].s_client);
				args->src_port = ntohs(addr.sin_port);
				args->activeClient = activeClient;
				args->proxy = proxy;

				pthread_t thread;
				if ( pthread_create(&thread, NULL, SV_ProxyClientThread, args) )
				{
					Com_DPrintf("Proxy: Failed to create new client thread at port %hu\n", BigShort(proxy->listenAdr.port));
					free(args);
					close(s_client);
					clientThreadInfo[clientIndex].s_client = -1;
					proxy->numClients--;
				}
				else
				{
					pthread_detach(thread);
				}
			}
			else
			{
				// Forward packets of known client to server
				sendto(clientThreadInfo[clientIndex].s_client, buffer, bytes_received, 0, (struct sockaddr *)&forwarderAddr, sizeof(forwarderAddr));
			}
		}
		else
			Com_DPrintf("Proxy: Invalid client address: %s\n", inet_ntoa(addr.sin_addr));
		
		pthread_mutex_unlock(&proxy->lock);
	}
	return NULL;
}

void * SV_ProxyMasterServerLoop(void *threadArgs)
{
	proxy_t *proxy = (proxy_t *)threadArgs;
	netadr_t authorizeAdr;
	struct sockaddr_in masterSockAdr;
	struct sockaddr_in authorizeSockAdr;
	qboolean startup = qtrue;
	int proxyindex = proxy - &proxies[0];

	Sys_StringToSockaddr(sv_masterServer->current.string, &masterSockAdr);
	masterSockAdr.sin_family = AF_INET;
	masterSockAdr.sin_port = htons(sv_masterPort->current.integer);
	proxy->masterSockAdr = &masterSockAdr;

	Sys_StringToSockaddr(sv_authorizeServer->current.string, &authorizeSockAdr);
	authorizeSockAdr.sin_family = AF_INET;
	authorizeSockAdr.sin_port = htons(sv_authorizePort->current.integer);
	SockadrToNetadr(&authorizeSockAdr, &authorizeAdr);

	while ( 1 )
	{
		// Delay on startup to counter rate-limiting on the master server side
		if ( startup )
		{
			sleep(proxyindex + 1);
			startup = qfalse;
		}

		// Do not continue heartbeats while the server is at rest (e.g., map
		// load error), but the process is still up
		while ( !com_sv_running->current.boolean )
			sleep(1);

		if ( sv_logHeartbeats->current.boolean )
			Com_Printf("Sending proxy heartbeat to %s for version %s (protocol %i)\n", sv_masterServer->current.string, proxy->versionString, proxy->version);

		sendto(proxy->socket, HEARTBEAT_MESSAGE, strlen(HEARTBEAT_MESSAGE), 0, (struct sockaddr*)&masterSockAdr, sizeof(masterSockAdr));

		if ( sv_logHeartbeats->current.boolean )
			Com_Printf("Sending proxy getIpAuthorize to %s for version %s (protocol %i)\n", sv_authorizeServer->current.string, proxy->versionString, proxy->version);

		char AUTHORIZE_SEND[MAX_BUFFER_SIZE];
		snprintf(AUTHORIZE_SEND, sizeof(AUTHORIZE_SEND), "%s %ld %i.%i.%i.%i \"%s\" 0",
			AUTHORIZE_MESSAGE,
			time(NULL),
			authorizeAdr.ip[0], authorizeAdr.ip[1], authorizeAdr.ip[2], authorizeAdr.ip[3],
			fs_game->current.string);

		sendto(proxy->socket, AUTHORIZE_SEND, strlen(AUTHORIZE_SEND), 0, (struct sockaddr*)&authorizeSockAdr, sizeof(authorizeSockAdr));
		
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

		ssize_t bytes_received = recvfrom(*(args->s_client), buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&r_addr, &r_len);

		inet_ntop(AF_INET, &args->addr.sin_addr, client_ip, sizeof(client_ip));

		if ( bytes_received >= 0 )
		{
			if ( (unsigned int)bytes_received < sizeof(buffer) - 1 )
				buffer[bytes_received] = '\0';
			else
			{
				Com_DPrintf("Proxy: Max. size exceeded at recvfrom in client thread\n");
				continue;
			}
		}
		else
		{
			if ( errno == EAGAIN || errno == EWOULDBLOCK )
				break;
			else
			{
				Com_DPrintf("Proxy: No data at recvfrom in client thread\n");
				continue;
			}
		}

		if ( memcmp(buffer, "\xFF\xFF\xFF\xFFstatusResponse", 18) == 0 || memcmp(buffer, "\xFF\xFF\xFF\xFFinfoResponse", 16) == 0 )
		{
			int is_blocked = 0;
			for ( unsigned int i = 0; i < sizeof(trackers) / sizeof(trackers[0]); i++ )
			{
				if ( strcmp(client_ip, trackers[i]) == 0 )
				{
					is_blocked = 1;
					break;
				}
			}

			if ( !is_blocked || sv_proxiesVisibleForTrackers->current.boolean )
			{
				if ( memcmp(buffer, "\xFF\xFF\xFF\xFFstatusResponse", 18) == 0 )
				{
					ReplaceProtocolString(buffer, proxy);
					ReplaceShortversionString(buffer, proxy);
				}
				else
				{
					ReplaceProtocolString(buffer, proxy);
				}
			}
			else
			{
				strcpy(buffer, "\xFF\xFF\xFF\xFF""disconnect");
				bytes_received = strlen(buffer);
				buffer[bytes_received] = '\0';
			}
		}

		// Forward packets to the client
		ssize_t sent_bytes = sendto(args->proxy->socket, buffer, bytes_received, 0, (struct sockaddr *)&args->addr, sizeof(args->addr));
		if ( sent_bytes == -1 )
			Com_DPrintf("Proxy: Error on sendto when forwarding packets to client\n");
	}

	if ( strlen(client_ip) && args->activeClient && com_sv_running->current.boolean )
		Com_DPrintf("Proxy: Client connection released for %s:%hu\n", client_ip, ntohs(args->addr.sin_port));

	pthread_mutex_lock(&proxy->lock);
	close(*(args->s_client));
	*(args->s_client) = -1;
	proxy->numClients--;
	pthread_mutex_unlock(&proxy->lock);

	free(args);
	return NULL;
}