#include "utils.hpp"

qboolean IsNullVector(float *vec)
{
	return vec[0] == 0.0 && vec[1] == 0.0 && vec[2] == 0.0;
}

void VectorClampLength(float *vec, double max)
{
	double length;
	double factor;
	
	length = VectorLength(vec);
	if ( length > max )
	{
		factor = length / max;
		vec[0] /= factor;
		vec[1] /= factor;
		vec[2] /= factor;
	}
}

time_t sys_timeBase = 0;
uint64_t Sys_Milliseconds64(void)
{
	struct timeval tp;

	gettimeofday(&tp, NULL);

	if ( !sys_timeBase )
	{
		sys_timeBase = tp.tv_sec;
		return tp.tv_usec / 1000;
	}

	return (tp.tv_sec - sys_timeBase) * 1000 + tp.tv_usec / 1000;
}

int getIndexForProtocol(int protocol)
{
	switch ( protocol )
	{
		case 115: return PROTOCOL_115;
		case 117: return PROTOCOL_117;
		case 118: return PROTOCOL_118;
		case 119: return PROTOCOL_119;
		default: return PROTOCOL_118;
	}
}

int getProtocolForIndex(int index)
{
	switch ( index )
	{
		case PROTOCOL_115: return 115;
		case PROTOCOL_117: return 117;
		case PROTOCOL_118: return 118;
		case PROTOCOL_119: return 119;
		default: return 118;
	}
}

int getProtocolFromShortVersion(const char *shortVersion)
{
	if ( strcmp(shortVersion, "1.0") == 0 )
		return 115; // 0x73
	else if ( strcmp(shortVersion, "1.2") == 0 )
		return 117; // 0x75
	else
		return 118; // 0x76, 1.3
	
	// Protocol 119 does not have a unique shortVersion
}

const char * getShortVersionFromProtocol(int protocol)
{
	switch ( protocol )
	{
		case 115: return "1.0"; // 0x73
		case 117: return "1.2"; // 0x75
		default: return "1.3"; // 0x76, 118 and 0x77, 119
	}
}

qboolean IsLocalIPAddress(unsigned char *ip)
{
    // As defined by IANA, plus link-local and loopback addresses
    if ( ip[0] == 10                                                     // Class A: 10.0.0.0 – 10.255.255.255
         || ( ip[0] == 172 && ( ip[1] >= 16 && ip[1] <= 31 ) )           // Class B: 172.16.0.0 – 172.31.255.255
         || ( ip[0] == 192 && ip[1] == 168 )                             // Class C: 192.168.0.0 – 192.168.255.255
		 || ( ip[0] == 169 && ip[1] == 254 )                             // Link local: 169.254.0.0 - 169.254.255.255
         || ( ip[0] == 127 && ip[1] == 0 && ip[2] == 0 && ip[3] == 1 ) ) // Localhost
    {
        return qtrue;
    }
    return qfalse;
}