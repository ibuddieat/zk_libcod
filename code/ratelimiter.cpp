// ioquake3 rate limit connectionless requests
// https://github.com/ioquake/ioq3/blob/master/code/server/sv_main.c

#include "gsc.hpp"
#include "libcod.hpp"
#include "utils.hpp"

// This is deliberately quite large to make it more of an effort to DoS
#define MAX_BUCKETS	16384
#define MAX_HASHES 1024

leakyBucket_t buckets[MAX_BUCKETS];
leakyBucket_t* bucketHashes[MAX_HASHES];
leakyBucket_t outboundLeakyBuckets[OUTBOUND_BUCKET_MAX]; // One for each game version/server

long SVC_HashForAddress(netadr_t address)
{
	unsigned char *ip = address.ip;
	int	i;
	long hash = 0;

	for ( i = 0; i < 4; i++ )
	{
		hash += (long)( ip[i] ) * ( i + 119 );
	}

	hash = ( hash ^ ( hash >> 10 ) ^ ( hash >> 20 ) );
	hash &= ( MAX_HASHES - 1 );

	return hash;
}

leakyBucket_t * SVC_BucketForAddress(netadr_t address, int burst, int period)
{
	leakyBucket_t *bucket = NULL;
	int i;
	long hash = SVC_HashForAddress(address);
	uint64_t now = Sys_Milliseconds64();

	for ( bucket = bucketHashes[hash]; bucket; bucket = bucket->next )
	{
		if ( memcmp(bucket->adr, address.ip, 4) == 0 )
			return bucket;
	}

	for ( i = 0; i < MAX_BUCKETS; i++ )
	{
		int interval;

		bucket = &buckets[i];
		interval = now - bucket->lastTime;

		// Reclaim expired buckets
		if ( bucket->lastTime > 0 && ( interval > ( burst * period ) ||
									   interval < 0 ) )
		{
			if ( bucket->prev != NULL )
				bucket->prev->next = bucket->next;
			else
				bucketHashes[bucket->hash] = bucket->next;

			if ( bucket->next != NULL )
				bucket->next->prev = bucket->prev;

			memset(bucket, 0, sizeof(leakyBucket_t));
		}

		if ( bucket->type == 0 )
		{
			bucket->type = address.type;
			memcpy(bucket->adr, address.ip, 4);

			bucket->lastTime = now;
			bucket->burst = 0;
			bucket->hash = hash;

			// Add to the head of the relevant hash chain
			bucket->next = bucketHashes[hash];
			if ( bucketHashes[hash] != NULL )
				bucketHashes[hash]->prev = bucket;

			bucket->prev = NULL;
			bucketHashes[hash] = bucket;

			return bucket;
		}
	}

	// Couldn't allocate a bucket for this address
	return NULL;
}

bool SVC_RateLimit(leakyBucket_t *bucket, int burst, int period)
{
	if ( bucket != NULL )
	{
		uint64_t now = Sys_Milliseconds64();
		int interval = now - bucket->lastTime;
		int expired = interval / period;
		int expiredRemainder = interval % period;

		if ( expired > bucket->burst || interval < 0 )
		{
			bucket->burst = 0;
			bucket->lastTime = now;
		}
		else
		{
			bucket->burst -= expired;
			bucket->lastTime = now - expiredRemainder;
		}

		if ( bucket->burst < burst )
		{
			bucket->burst++;
			return false;
		}
	}

	return true;
}

extern proxy_t proxies[MAX_PROXIES];
bool IsProxySource(netadr_t from)
{
	// Requests directed to the proxy ports will pass through this twice, once
	// when talking to the proxy, and once again when the proxy talks to the
	// main server port/socket.
	//
	// Also, here a remark on the OUTBOUND_BUCKET_MAIN checks before calling
	// SVC_SpamCallback: Do not call script callbacks from the proxy threads
	// (or any other threads other than the main game loop thread), as they're
	// currently not thread-safe.
	//
	// As a result, CodeCallback_CLSpam in _callbacksetup.gsc is not called
	// when the proxy ports are attacked, but rate-limiting still applies,
	// and rcon is protected.

	proxy_t *proxy = &proxies[0];
	for ( int i = 0; i < MAX_PROXIES; i++, proxy++ ) {
		if ( proxy->started && !memcmp(from.ip, proxy->listenAdr.ip, 4) )
			return qtrue;
	}
	return qfalse;
}

bool SVC_RateLimitAddress(netadr_t from, int burst, int period)
{
	leakyBucket_t *bucket = SVC_BucketForAddress(from, burst, period);

	return SVC_RateLimit(bucket, burst, period);
}

bool SVC_ApplyConnectLimit(netadr_t from, outboundLeakyBucketIndex_t index)
{
	// Avoid rate-limiting proxied connections twice
	if ( IsProxySource(from) )
		return false;

	// Prevent using connect as an amplifier
	if ( SVC_RateLimitAddress(from, 10, 1000) )
	{
		if ( index != OUTBOUND_BUCKET_MAIN || !SVC_SpamCallback("CONNECT:ADDRESS", NET_AdrToString(from)) )
			Com_DPrintf("SV_DirectConnect: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
		return true;
	}

	// Allow connect to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit(&outboundLeakyBuckets[index], 10, 100) )
	{
		Com_DPrintf("SV_DirectConnect: rate limit exceeded, dropping request\n");
		return true;
	}

	return false;
}

bool SVC_ApplyRconLimit(netadr_t from, qboolean badRconPassword)
{
	// No proxy check needed here as we block rcon traffic from proxies, see
	// proxy.c

	// Prevent using rcon as an amplifier and make dictionary attacks impractical
	if ( SVC_RateLimitAddress(from, 10, 1000) )
	{
		if ( !SVC_SpamCallback("RCON:ADDRESS", NET_AdrToString(from)) )
			Com_DPrintf("SVC_RemoteCommand: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
		return true;
	}

    // Outbound rcon limit is only applied when a wrong rcon password is
    // provided so that legitimate requests are not limited outbound-wise.
    //
    // However, please note that this could be abused by attackers that know
    // the rcon password, allowing them to generate a lot of useless outbound
    // traffic (e.g., by sending numerous status requests from a large number
    // of spoofed source IP addresses)
	if ( badRconPassword )
	{
		static leakyBucket_t bucket;

		// Make DoS via rcon impractical
		if ( SVC_RateLimit(&bucket, 10, 1000) )
		{
			if ( !SVC_SpamCallback("RCON:GLOBAL", NET_AdrToString(from)) )
				Com_DPrintf("SVC_RemoteCommand: rate limit exceeded, dropping request\n");
			return true;
		}
	}

	return false;
}

bool SVC_ApplyStatusLimit(netadr_t from, outboundLeakyBucketIndex_t index)
{
	// Avoid rate-limiting proxied connections twice
	if ( IsProxySource(from) )
		return false;

	// Prevent using getstatus as an amplifier
	if ( SVC_RateLimitAddress(from, 10, 1000) )
	{
		if ( index != OUTBOUND_BUCKET_MAIN || !SVC_SpamCallback("STATUS:ADDRESS", NET_AdrToString(from)) )
			Com_DPrintf("SVC_Status: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
		return true;
	}

	// Allow getstatus to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit(&outboundLeakyBuckets[index], 10, 100) )
	{
		if ( index != OUTBOUND_BUCKET_MAIN || !SVC_SpamCallback("STATUS:GLOBAL", NET_AdrToString(from)) )
			Com_DPrintf("SVC_Status: rate limit exceeded, dropping request\n");
		return true;
	}

	return false;
}

bool SVC_ApplyChallengeLimit(netadr_t from, outboundLeakyBucketIndex_t index)
{
	// Avoid rate-limiting proxied connections twice
	if ( IsProxySource(from) )
		return false;

	// Prevent using getchallenge as an amplifier
	if ( SVC_RateLimitAddress(from, 10, 1000) )
	{
		if ( index != OUTBOUND_BUCKET_MAIN || !SVC_SpamCallback("CHALLENGE:ADDRESS", NET_AdrToString(from)) )
			Com_DPrintf("SV_GetChallenge: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
		return true;
	}

	// Allow getchallenge to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit(&outboundLeakyBuckets[index], 10, 100) )
	{
		if ( index != OUTBOUND_BUCKET_MAIN || !SVC_SpamCallback("CHALLENGE:GLOBAL", NET_AdrToString(from)) )
			Com_DPrintf("SV_GetChallenge: rate limit exceeded, dropping request\n");
		return true;
	}

	return false;
}

bool SVC_ApplyAuthorizeIpPacketLimit(netadr_t from, outboundLeakyBucketIndex_t index)
{
	// Avoid rate-limiting proxied connections twice
	if ( IsProxySource(from) )
		return false;

	// Prevent ipAuthorize log spam DoS
	if ( SVC_RateLimitAddress(from, 20, 1000) )
	{
		Com_DPrintf("SV_AuthorizeIpPacket: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
		return true;
	}

	// Allow ipAuthorize to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit(&outboundLeakyBuckets[index], 10, 100) )
	{
		Com_DPrintf("SV_AuthorizeIpPacket: rate limit exceeded, dropping request\n");
		return true;
	}

    return false;
}

bool SVC_ApplyInfoLimit(netadr_t from, outboundLeakyBucketIndex_t index)
{
	// Avoid rate-limiting proxied connections twice
	if ( IsProxySource(from) )
		return false;

	// Prevent using getinfo as an amplifier
	if ( SVC_RateLimitAddress(from, 10, 1000) )
	{
		if ( index != OUTBOUND_BUCKET_MAIN || !SVC_SpamCallback("INFO:ADDRESS", NET_AdrToString(from)) )
			Com_DPrintf("SVC_Info: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
		return true;
	}

	// Allow getinfo to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit(&outboundLeakyBuckets[index], 10, 100) )
	{
		if ( index != OUTBOUND_BUCKET_MAIN || !SVC_SpamCallback("INFO:GLOBAL", NET_AdrToString(from)) )
			Com_DPrintf("SVC_Info: rate limit exceeded, dropping request\n");
		return true;
	}

	return false;
}