long SVC_HashForAddress(netadr_t address);
leakyBucket_t * SVC_BucketForAddress(netadr_t address, int burst, int period);
bool SVC_RateLimit(leakyBucket_t *bucket, int burst, int period);
bool SVC_RateLimitAddress(netadr_t from, int burst, int period);

bool SVC_ApplyConnectLimit(netadr_t from, outboundLeakyBucketIndex_t index);
bool SVC_ApplyRconLimit(netadr_t from, qboolean badRconPassword);
bool SVC_ApplyStatusLimit(netadr_t from, outboundLeakyBucketIndex_t index);
bool SVC_ApplyChallengeLimit(netadr_t from, outboundLeakyBucketIndex_t index);
bool SVC_ApplyAuthorizeIpPacketLimit(netadr_t from, outboundLeakyBucketIndex_t index);
bool SVC_ApplyInfoLimit(netadr_t from, outboundLeakyBucketIndex_t index);