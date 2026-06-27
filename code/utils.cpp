#include "utils.hpp"

qboolean IsNullVector(float *vec)
{
	return vec[0] == 0.0 && vec[1] == 0.0 && vec[2] == 0.0;
}

void VectorClampLength(float *vec, float max)
{
	float length;
	float factor;
	
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

int GetIndexForProtocol(int protocol)
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

int GetProtocolForIndex(int index)
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

int GetProtocolFromShortVersion(const char *shortVersion)
{
	if ( strcmp(shortVersion, "1.0") == 0 )
		return 115; // 0x73
	else if ( strcmp(shortVersion, "1.2") == 0 )
		return 117; // 0x75
	else
		return 118; // 0x76, 1.3
	
	// Protocol 119 does not have a unique shortVersion
}

const char * GetShortVersionFromProtocol(int protocol)
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

weapSlot_t GetCurrentWeaponSlot(int clientNum)
{
	playerState_t *ps = SV_GameClientNum(clientNum);

	if ( ps->weapon == 0 )
	{
		return WEAPSLOT_NONE;
	} 
	else if ( (byte)ps->weapon == ps->weaponslots[1] )
	{
		return WEAPSLOT_PRIMARY;
	}
	else if ( (byte)ps->weapon == ps->weaponslots[2] )
	{
		return WEAPSLOT_PRIMARYB;
	}
	else
	{
		return WEAPSLOT_NONE;
	}
}

int GetAnimationId(const char *string)
{
	int i;
	int hash;
	animation_t *anim;

	hash = BG_StringHashValue(string);

	for ( i = 0; i < globalScriptData->numAnimations; i++ )
	{
		anim = &globalScriptData->animations[i];
		if ( ( hash == anim->nameHash ) && !I_stricmp(string, anim->name) )
		{
			return i;
		}
	}

	return -1;
}

qboolean IsValidWeaponId(int id)
{
	int weapons = BG_GetNumWeapons();

	if ( id > weapons || id <= 0 || weapons == 0 )
		return qfalse;

	return qtrue;
}

/*
=================
Sys_AnsiColorPrint
Transform Q3 colour codes to ANSI escape sequences
=================
*/
#define MAXPRINTMSG 1024
#define ColorIndex(c)	(((c) - '0') & 0x07)
#define Q_COLOR_ESCAPE	'^'
#define Q_IsColorString(p)	((p) && *(p) == Q_COLOR_ESCAPE && *((p)+1) && isdigit(*((p)+1))) // ^[0-9]
void Sys_AnsiColorPrint(const char *msg)
{
	static char buffer[ MAXPRINTMSG ];
	int         length = 0;
	static int  q3ToAnsi[8] =
	{
		30, // COLOR_BLACK
		31, // COLOR_RED
		32, // COLOR_GREEN
		33, // COLOR_YELLOW
		34, // COLOR_BLUE
		36, // COLOR_CYAN
		35, // COLOR_MAGENTA
		0   // COLOR_WHITE
	};

	while ( *msg )
	{
		if ( Q_IsColorString(msg) || *msg == '\n' )
		{
			// First empty the buffer
			if ( length > 0 )
			{
				buffer[length] = '\0';
				fputs(buffer, stdout);
				length = 0;
			}

			if ( *msg == '\n' )
			{
				// Issue a reset and then the newline
				fputs("\033[0m\n", stdout);
				msg++;
			}
			else
			{
				// Print the color code
				snprintf(buffer, sizeof(buffer), "\033[1;%dm", q3ToAnsi[ColorIndex(*(msg + 1))]);
				fputs(buffer, stdout);
				msg += 2;
			}
		}
		else
		{
			if ( length >= MAXPRINTMSG - 1 )
				break;

			buffer[length] = *msg;
			length++;
			msg++;
		}
	}

	// Empty anything still left in the buffer
	if ( length > 0 )
	{
		buffer[length] = '\0';
		fputs(buffer, stdout);
		// Issue a reset at the end
		fputs("\033[0m", stdout);
	}
}