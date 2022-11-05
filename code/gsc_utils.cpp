#include "gsc_utils.hpp"

#if COMPILE_CUSTOM_VOICE == 1
#include <speex/speex.h>
#include <pthread.h>

struct encoder_async_task
{
	encoder_async_task *prev;
	encoder_async_task *next;
	char filePath[COD2_MAX_STRINGLENGTH];
	int callback;
	float volume;
	int soundIndex;
	unsigned int levelId;
};
#endif

#if COMPILE_UTILS == 1

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

extern cvar_t *con_coloredPrints;
int stackPrintParam(int param)
{
	if (param >= Scr_GetNumParam())
		return 0;

	switch (stackGetParamType(param))
	{
	case STACK_STRING:
		char *str;
		stackGetParamString(param, &str); // no error checking, since we know it's a string
		if ( con_coloredPrints->boolean )
			Sys_AnsiColorPrint(str);
		else
			printf("%s", str);
		return 1;

	case STACK_VECTOR:
		float vec[3];
		stackGetParamVector(param, vec);
		printf("(%.2f, %.2f, %.2f)", vec[0], vec[1], vec[2]);
		return 1;

	case STACK_FLOAT:
		float tmp_float;
		stackGetParamFloat(param, &tmp_float);
		printf("%.3f", tmp_float); // need a way to define precision
		return 1;

	case STACK_INT:
		int tmp_int;
		stackGetParamInt(param, &tmp_int);
		printf("%d", tmp_int);
		return 1;
	}

	printf("(%s)", stackGetParamTypeAsString(param));
	return 0;
}

void gsc_utils_remotecommand()
{
	char * sFrom;
	int pointerMsg;

	if (!stackGetParams("si", &sFrom, &pointerMsg))
	{
		stackError("gsc_utils_remotecommand() one or more arguments is undefined or has a wrong type");
		return;
	}

	netadr_t from;

	msg_t * msg = (msg_t *)pointerMsg;
	NET_StringToAdr(sFrom, &from);

#if COD_VERSION == COD2_1_0
	int lasttime_offset = 0x0848B674;
#elif COD_VERSION == COD2_1_2
	int lasttime_offset = 0x0849EB74;
#elif COD_VERSION == COD2_1_3
	int lasttime_offset = 0x0849FBF4;
#endif

	*(int *)lasttime_offset = 0;

	SVC_RemoteCommand(from, msg);
}

void gsc_utils_executestring()
{
	char *str;

	if ( ! stackGetParams("s", &str))
	{
		stackError("gsc_utils_executestring() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	Cmd_ExecuteString(str);
	stackPushBool(qtrue);
}

void gsc_utils_sendgameservercommand()
{
	int clientNum;
	char *message;

	if ( ! stackGetParams("is", &clientNum, &message))
	{
		stackError("gsc_utils_sendgameservercommand() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	SV_GameSendServerCommand(clientNum, 0, message);
	stackPushBool(qtrue);
}

void gsc_utils_system()
{
	char *cmd;

	if ( ! stackGetParams("s",  &cmd))
	{
		stackError("gsc_utils_system() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushInt( system(cmd) );
}

void gsc_utils_printf()
{
	char *str;

	if ( ! stackGetParams("s", &str))
	{
		stackError("gsc_utils_printf() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int param = 1; // maps to first %
	int len = strlen(str);

	for (int i = 0; i < len; i++)
	{
		if (str[i] == '%')
		{
			if(str[i + 1] == '%')
			{
				putchar('%');
				i++;
			}
			else
				stackPrintParam(param++);
		}
		else
			putchar(str[i]);
	}

	stackPushBool(qtrue);
}

void gsc_utils_sprintf()
{
	char result[COD2_MAX_STRINGLENGTH];
	char *str;

	if (!stackGetParams("s", &str))
	{
		stackError("gsc_utils_sprintf() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int param = 1; // maps to first %
	int len = strlen(str);
	int num = 0;

	for (int i = 0; i < len; i++)
	{
		if (str[i] == '%')
		{
			if (str[i + 1] == '%')
			{
				result[num++] = '%';
				i++;
			}
			else
			{
				if(param >= Scr_GetNumParam())
					continue;

				switch (stackGetParamType(param))
				{
				case STACK_STRING:
					char *tmp_str;
					stackGetParamString(param, &tmp_str); // no error checking, since we know it's a string
					num += sprintf(&(result[num]), "%s", tmp_str);
					break;

				case STACK_VECTOR:
					float vec[3];
					stackGetParamVector(param, vec);
					num += sprintf(&(result[num]), "(%.2f, %.2f, %.2f)", vec[0], vec[1], vec[2]);
					break;

				case STACK_FLOAT:

					float tmp_float;
					stackGetParamFloat(param, &tmp_float);
					num += sprintf(&(result[num]), "%.3f", tmp_float); // need a way to define precision
					break;

				case STACK_INT:
					int tmp_int;
					stackGetParamInt(param, &tmp_int);
					num += sprintf(&(result[num]), "%d", tmp_int);
					break;
				}

				param++;
			}
		}
		else
			result[num++] = str[i];
	}

	result[num] = '\0';
	stackPushString(result);
}

void gsc_utils_outofbandprint()
{
	char * address;
	char * msg;

	if (!stackGetParams("ss", &address, &msg))
	{
		stackError("gsc_utils_outofbandprint() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	netadr_t from;
	NET_StringToAdr(address, &from);
	NET_OutOfBandPrint(NS_SERVER, from, msg);
}


void gsc_utils_putchar()
{
	int val;

	if ( ! stackGetParams("i",  &val))
	{
		stackError("gsc_utils_putchar() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (val < -127 || val > 127)
	{
		stackError("gsc_utils_putchar() character index is out of range");
		stackPushUndefined();
		return;
	}

	char s[2];

	s[0] = val;
	s[1] = '\0';

	stackPushString( s );
}

void gsc_utils_logprintconsole()
{
	char *str;

	if ( ! stackGetParams("s", &str))
	{
		stackError("gsc_utils_logprintconsole() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!strlen(str) || strlen(str) > COD2_MAX_STRINGLENGTH)
	{
		stackError("gsc_utils_logprintconsole() invalid string length");
		stackPushUndefined();
		return;
	}

	Com_Printf("%s", str);

	stackPushBool(qtrue);
}

void gsc_utils_getarraykeys()
{
	unsigned int arrIndex;

	if (!stackGetParamObject(0, &arrIndex))
	{
		stackError("gsc_utils_getarraykeys() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int arraysize = GetArraySize(arrIndex);

	if (!arraysize)
	{
		stackError("gsc_utils_getarraykeys() got an empty or invalid array");
		stackPushUndefined();
		return;
	}

	unsigned int index = arrIndex;
	unsigned int name;

	stackPushArray();

	for (int i = 0; i < arraysize; i++)
	{
		index = GetNextVariable(index);
		name = GetVariableName(index);

		if (name < 0x10000)
		{
			stackPushString(SL_ConvertToString(name));
			stackPushArrayLast();
		}
	}
}

void gsc_utils_getascii()
{
	char *str;

	if ( ! stackGetParams("s", &str))
	{
		stackError("gsc_utils_getascii() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!strlen(str))
	{
		stackError("gsc_utils_getascii() string length is 0");
		stackPushUndefined();
		return;
	}

	stackPushInt(str[0]);
}

void gsc_utils_toupper()
{
	char *str;

	if ( ! stackGetParams("s", &str))
	{
		stackError("gsc_utils_toupper() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!strlen(str))
	{
		stackError("gsc_utils_toupper() string length is 0");
		stackPushUndefined();
		return;
	}

	stackPushString( I_strupr(str) );
}

void gsc_utils_file_link()
{
	char *source, *dest;

	if ( ! stackGetParams("ss",  &source, &dest))
	{
		stackError("gsc_utils_file_link() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int link_success = symlink(source, dest) == 0;
	stackPushInt( link_success );
}

void gsc_utils_file_unlink()
{
	char *file;

	if ( ! stackGetParams("s",  &file))
	{
		stackError("gsc_utils_file_unlink() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int unlink_success = unlink(file) == 0;
	stackPushInt( unlink_success );
}

void gsc_utils_file_exists()
{
	char *filename;

	if ( ! stackGetParams("s", &filename))
	{
		stackError("gsc_utils_file_exists() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int file_exists = access(filename, F_OK) != -1;
	stackPushInt( file_exists );
}

void gsc_utils_FS_LoadDir()
{
	char *path, *dir;

	if ( ! stackGetParams("ss", &path, &dir))
	{
		stackError("gsc_utils_FS_LoadDir() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	FS_LoadDir(path, dir);
	stackPushBool(qtrue);
}

void gsc_utils_scandir()
{
	char *dirname;

	if ( ! stackGetParams("s", &dirname))
	{
		stackError("gsc_utils_scandir() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	DIR *dir;
	struct dirent *dir_ent;

	dir = opendir(dirname);

	if ( ! dir)
	{
		stackPushUndefined();
		return;
	}

	stackPushArray();

	while ( (dir_ent = readdir(dir)) != NULL)
	{
		stackPushString(dir_ent->d_name);
		stackPushArrayLast();
	}

	closedir(dir);
}

void gsc_utils_fopen()
{
	FILE *file;
	char *filename, *mode;

	if ( ! stackGetParams("ss", &filename, &mode))
	{
		stackError("gsc_utils_fopen() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	file = fopen(filename, mode);

	if (!file)
	{
		stackError("gsc_utils_fopen() returned a error");
		stackPushUndefined();
		return;
	}

	stackPushInt((int)file);
}

void gsc_utils_fread()
{
	FILE *file;

	if ( ! stackGetParams("i", &file))
	{
		stackError("gsc_utils_fread() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!file)
	{
		stackError("gsc_utils_fread() returned a error");
		stackPushUndefined();
		return;
	}

	char buffer[256];
	int ret = fread(buffer, 1, 255, file);

	if ( ! ret)
	{
		stackPushUndefined();
		return;
	}

	buffer[ret] = '\0';
	stackPushString(buffer);
}

void gsc_utils_fwrite()
{
	FILE *file;
	char *buffer;

	if ( ! stackGetParams("is", &file, &buffer))
	{
		stackError("gsc_utils_fwrite() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!file)
	{
		stackError("gsc_utils_fwrite() returned a error");
		stackPushUndefined();
		return;
	}

	stackPushInt(fwrite(buffer, 1, strlen(buffer), file));
}

void gsc_utils_fclose()
{
	FILE *file;

	if ( ! stackGetParams("i", &file))
	{
		stackError("gsc_utils_fclose() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!file)
	{
		stackError("gsc_utils_fclose() returned a error");
		stackPushUndefined();
		return;
	}

	stackPushInt( fclose(file) );
}

void gsc_utils_fsize()
{
	FILE *file;

	if ( ! stackGetParams("i", &file))
	{
		stackError("gsc_utils_fsize() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!file)
	{
		stackError("gsc_utils_fsize() returned a error");
		stackPushUndefined();
		return;
	}

	struct stat buf;
	fstat(fileno(file), &buf);

	stackPushInt( buf.st_size );
}

void gsc_utils_fremove()
{
	char * file;
	
	if (!stackGetParams("s", &file))
	{
		stackError("gsc_utils_fremove() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	
	stackPushInt(remove( file ));
}

void gsc_utils_getsystemtime()
{
	time_t timer;
	stackPushInt( time(&timer) );
}

static int starttime = time(NULL);
void gsc_utils_getserverstarttime()
{
	stackPushInt( starttime );
}

void gsc_utils_getlocaltime()
{
	time_t timer;
	struct tm *timeinfo;

	time(&timer);
	timeinfo = localtime(&timer);

	const char *timestring = asctime(timeinfo);
	char stripped_time[128];

	strncpy(stripped_time, timestring, sizeof(stripped_time));
	stripped_time[strlen(timestring) - 1] = '\0';

	stackPushString( stripped_time );
}

// http://code.metager.de/source/xref/RavenSoftware/jediacademy/code/game/g_utils.cpp#36
void gsc_g_findconfigstringindexoriginal()
{
	char *name;
	int min, max, create;

	if ( ! stackGetParams("siii", &name, &min, &max, &create))
	{
		stackError("gsc_g_findconfigstringindexoriginal() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
    
	if (min < 0 || max >= MAX_CONFIGSTRINGS)
	{
		stackError("gsc_g_findconfigstringindexoriginal() configstring index is out of range");
		stackPushUndefined();
		return;
	}

	stackPushInt( G_FindConfigstringIndex(name, min, max, create, "G_FindConfigstringIndex() from GSC") );
}

// simple version, without crash
void gsc_g_findconfigstringindex()
{
	char *name;
	int min, max;

	if ( ! stackGetParams("sii", &name, &min, &max))
	{
		stackError("gsc_g_findconfigstringindex() one or more arguments is undefined or has a wrong type");
		return;
	}
    
    if (min < 0 || max >= MAX_CONFIGSTRINGS)
	{
		stackError("gsc_g_findconfigstringindex() configstring index is out of range");
		stackPushUndefined();
		return;
	}

	for (int i = 1; i < max; i++)
	{
		const char *curitem = SV_GetConfigstringConst(min + i);

		if ( ! *curitem)
			break;

		if ( ! strcasecmp(name, curitem))
		{
			stackPushInt(i + min);
			return;
		}
	}

	stackPushBool(qtrue);
}

void gsc_get_configstring()
{
	int index;

	if ( ! stackGetParams("i", &index))
	{
		stackError("gsc_get_configstring() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
    
    if (index < 0 || index >= MAX_CONFIGSTRINGS)
	{
		stackError("gsc_get_configstring() configstring index is out of range");
		stackPushUndefined();
		return;
	}

	const char *string = SV_GetConfigstringConst(index);

	if ( ! *string )
		stackPushUndefined();
	else
		stackPushString(string);
}

void gsc_set_configstring()
{
	int index;
	char *string;

	if ( ! stackGetParams("is", &index, &string))
	{
		stackError("gsc_set_configstring() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
    
	if (index < 0 || index >= MAX_CONFIGSTRINGS)
	{
		stackError("gsc_set_configstring() configstring index is out of range");
		stackPushUndefined();
		return;
	}

	SV_SetConfigstring(index, string);
	stackPushBool(qtrue);
}

void gsc_make_localized_string()
{
	char *str;

	if ( ! stackGetParams("s", &str))
	{
		stackError("gsc_make_localized_string() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushString(str);

	VariableValue *var;
	int param = 0;

	var = &scrVmPub.top[-param];
	var->type = STACK_LOCALIZED_STRING;
}

void gsc_utils_float()
{
	if (Scr_GetNumParam() == 0)
	{
		stackError("gsc_utils_float() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	switch (stackGetParamType(0))
	{
	case STACK_STRING:
		char *asstring;
		stackGetParamString(0, &asstring);
		stackPushFloat( atof(asstring) );
		return;

	case STACK_FLOAT:
		float asfloat;
		stackGetParamFloat(0, &asfloat);
		stackPushFloat( asfloat );
		return;

	case STACK_INT:
		int asinteger;
		stackGetParamInt(0, &asinteger);
		stackPushFloat( float(asinteger) );
		return;

	default:
		stackError("gsc_utils_float() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
}

void gsc_utils_exponent()
{
	float basis;
	float exponent;

	if ( ! stackGetParams("ff", &basis, &exponent))
	{
		stackError("gsc_utils_exponent() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushFloat( pow(basis, exponent) );
}

void gsc_utils_round()
{
	float val;

	if ( ! stackGetParams("f",  &val))
	{
		stackError("gsc_utils_round() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushFloat( roundf(val * 100) / 100 );
}

void gsc_utils_sqrt()
{
	float x;

	if ( ! stackGetParams("f", &x))
	{
		stackError("gsc_utils_sqrt() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushFloat(sqrt(x));
}

void gsc_utils_sqrtinv()
{
	float x;

	if ( ! stackGetParams("f", &x))
	{
		stackError("gsc_utils_sqrtinv() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	// http://www.beyond3d.com/content/articles/8/
	float xhalf = 0.5f*x;
	int i = *(int*)&x;
	i = 0x5f3759df - (i>>1);
	x = *(float*)&i;
	x = x*(1.5f - xhalf*x*x);

	stackPushFloat(x);
}

void gsc_utils_vectorscale()
{
	vec3_t vector;
	float scale;

	if ( ! stackGetParams("vf", &vector, &scale))
	{
		stackError("gsc_utils_vectorscale() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	vec3_t out;

	VectorScale(vector, scale, out);
	stackPushVector(out);
}

void gsc_utils_getlasttestclientnumber()
{
#if COD_VERSION == COD2_1_0
	int offset = 0x083DF9EC;
#elif COD_VERSION == COD2_1_2
	int offset = 0x083E1E8C;
#elif COD_VERSION == COD2_1_3
	int offset = 0x083E2F0C;
#endif

	stackPushInt(*(int *)offset);
}

void gsc_utils_bullethiteffect()
{
	vec3_t origin;
	vec3_t normal;

	if ( ! stackGetParams("vv", &origin, &normal))
	{
		stackError("gsc_utils_bullethiteffect() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	gentity_t *entity = G_TempEntity(&origin, EV_SHOTGUN_HIT);
	entity->s.eventParm = DirToByte(normal);

	trace_t trace;

	vec3_t end_origin = { origin[0] - (normal[0] * 10), origin[1] - (normal[1] * 10), origin[2] - (normal[2] * 10) };
	G_LocationalTrace(&trace, origin, end_origin, 1023, 1, NULL);

	entity->s.surfType = (trace.surfaceFlags >> 20) & 0x1F;

	stackPushBool(qtrue);
}

void gsc_utils_gettype()
{
	if (Scr_GetNumParam() == 0)
	{
		stackError("gsc_utils_gettype() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushString( stackGetParamTypeAsString(0) );
}
#endif


#if COMPILE_CUSTOM_VOICE == 1

extern VoicePacket_t voiceDataStore[MAX_CUSTOMSOUNDS][MAX_STOREDVOICEPACKETS];
extern cvar_t *sv_voiceQuality;
encoder_async_task *first_encoder_async_task = NULL;
extern int currentMaxSoundIndex;

void Encode_SetOptions(void *encoder)
{
	int g_encoder_samplerate = 8192;
	int g_encoder_quality = sv_voiceQuality->integer;
	int enabled = 0;
	speex_encoder_ctl(encoder, SPEEX_SET_SAMPLING_RATE /* 24 */, &g_encoder_samplerate);
	speex_encoder_ctl(encoder, SPEEX_SET_QUALITY /* 4 */, &g_encoder_quality);
	speex_encoder_ctl(encoder, SPEEX_SET_VAD /* 30 */, &enabled); // Voice Activity Detection (VAD) status
	speex_encoder_ctl(encoder, SPEEX_SET_DTX /* 34 */, &enabled); // Discontinuous Transmission (DTX)
}

void *encode_async(void *newtask)
{
	encoder_async_task *task = (encoder_async_task*)newtask;
	FILE *file = fopen(task->filePath, "r");
	int result = 0;

	if ( file != NULL )    
	{
		// Reset song data for this slot
		memset(&voiceDataStore[task->soundIndex], 0, sizeof(voiceDataStore[0]));

		/*
		The following encoding procedure is based on:
		https://gitlab.xiph.org/xiph/speex/-/blob/Speex-1.1.9/doc/sampleenc.c

		License:
		Copyright 2002-2004 Xiph.org Foundation, Jean-Marc Valin, David Rowe, EpicGames

		Redistribution and use in source and binary forms, with or without
		modification, are permitted provided that the following conditions
		are met:

		- Redistributions of source code must retain the above copyright
		notice, this list of conditions and the following disclaimer.

		- Redistributions in binary form must reproduce the above copyright
		notice, this list of conditions and the following disclaimer in the
		documentation and/or other materials provided with the distribution.

		- Neither the name of the Xiph.org Foundation nor the names of its
		contributors may be used to endorse or promote products derived from
		this software without specific prior written permission.

		THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
		``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
		LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
		A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
		CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
		EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
		PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
		PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
		LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
		NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
		SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
		*/
		short in[MAX_VOICEFRAMESIZE];
		float input[MAX_VOICEFRAMESIZE];
		char data[MAX_VOICEPACKETDATALEN];
		int dataLen;
		void *g_encoder;
		SpeexBits encodeBits;
		int i, packetIndex;
		VoicePacket_t *voicePacket;

		// Create a new encoder state in narrowband mode
		g_encoder = speex_encoder_init(&speex_nb_mode);
		speex_bits_init(&encodeBits);
		Encode_SetOptions(g_encoder);

		for ( packetIndex = 0; packetIndex <= MAX_STOREDVOICEPACKETS; packetIndex++ )
		{
			if ( packetIndex == MAX_STOREDVOICEPACKETS )
			{
				// Song file too long, end encoding here
				result = 1;
				break;
			}

			// Read a 16 bits/sample audio frame
			fread(in, sizeof(short), MAX_VOICEFRAMESIZE, file);
			if ( feof(file) )
				break;

			for ( i = 0; i < MAX_VOICEFRAMESIZE; i++ )
				input[i] = in[i] * task->volume;

			speex_bits_reset(&encodeBits);
			speex_encode(g_encoder, input, &encodeBits);
			dataLen = speex_bits_write(&encodeBits, data, MAX_VOICEPACKETDATALEN);
			voicePacket = &voiceDataStore[task->soundIndex][packetIndex];
			memcpy(voicePacket->data, data, dataLen);
			voicePacket->dataLen = dataLen;
		}
		speex_encoder_destroy(g_encoder);
		speex_bits_destroy(&encodeBits);
		fclose(file);
	}
	else
	{
		stackError("gsc_utils_loadsoundfile() input file could not be opened");
		result = 2;
	}

	if ( Scr_IsSystemActive() )
	{
		stackPushInt(result);
		stackPushInt(task->soundIndex);
		short ret = Scr_ExecThread(task->callback, 2);
		Scr_FreeThread(ret);
	}

	if ( task->next != NULL )
		task->next->prev = task->prev;

	if ( task->prev != NULL )
		task->prev->next = task->next;
	else
		first_encoder_async_task = task->next;

	delete task;
	return NULL;
}

void gsc_utils_getsoundfileduration()
{
	char *filePath;
	int overrideLimit;

	if ( !stackGetParamString(0, &filePath) )
	{
		stackError("gsc_utils_getsoundfileduration() requires a file path (string) as argument");
		stackPushUndefined();
		return;
	}

	if ( !stackGetParamInt(1, &overrideLimit) )
	{
		overrideLimit = 0;
	}

	FILE *file = fopen(filePath, "rb");
	if ( file != NULL )    
	{
		if ( fseek(file, 0, SEEK_END) == 0 )
		{
			int size = ftell(file);
			if ( size != -1 )
			{
				fclose(file);
				size -= size % (sizeof(short) * MAX_VOICEFRAMESIZE);
				float duration = size / ((((1.0 / FRAMETIME) * 1000) * MAX_VOICEPACKETSPERFRAME) * (sizeof(short) * MAX_VOICEFRAMESIZE));
				if ( overrideLimit && duration > (60 * MAX_CUSTOMSOUNDDURATION) )
					duration = 60 * MAX_CUSTOMSOUNDDURATION;

				stackPushFloat(duration);
			}
			else
			{
				fclose(file);
				stackError("gsc_utils_getsoundfileduration() error at ftell");
				stackPushUndefined();
			}
		}
		else
		{
			fclose(file);
			stackError("gsc_utils_getsoundfileduration() error at fseek");
			stackPushUndefined();
		}
	}
	else
	{
		stackError("gsc_utils_getsoundfileduration() file could not be opened");
		stackPushUndefined();
	}
}

void gsc_utils_loadsoundfile()
{
	char *filePath;
	int callback;
	float volume;
	int soundIndex;

	if ( !stackGetParamString(0, &filePath) )
	{
		stackError("gsc_utils_loadsoundfile() requires a file path (string) as first argument");
		stackPushInt(0);
		return;
	}

	if ( !stackGetParamFunction(1, &callback) )
	{
		stackError("gsc_utils_loadsoundfile() requires a callback function as second argument");
		stackPushInt(0);
		return;
	}

	if ( !stackGetParamFloat(2, &volume) )
	{
		volume = 1.0;
	}
	if ( volume > 1.0)
		volume = 1.0;
	
	if ( volume < 0.0 )
		volume = 0.0;

	if ( !stackGetParamInt(3, &soundIndex) )
	{
		soundIndex = ++currentMaxSoundIndex;
	}

	if ( !soundIndex || soundIndex >= MAX_CUSTOMSOUNDS )
	{
		stackError("gsc_utils_loadsoundfile() invalid sound index, valid range is 0-%d", MAX_CUSTOMSOUNDS-1);
		stackPushInt(0);
		return;
	}

	encoder_async_task *current = first_encoder_async_task;

	while ( current != NULL && current->next != NULL )
		current = current->next;

	encoder_async_task *newtask = new encoder_async_task;

	strncpy(newtask->filePath, filePath, COD2_MAX_STRINGLENGTH - 1);
	newtask->prev = current;
	newtask->next = NULL;
	newtask->filePath[COD2_MAX_STRINGLENGTH - 1] = '\0';
	newtask->callback = callback;
	newtask->volume = volume;
	newtask->soundIndex = soundIndex;
	newtask->levelId = scrVarPub.levelId;

	if ( current != NULL )
		current->next = newtask;
	else
		first_encoder_async_task = newtask;
	
	pthread_t encoder_doer;

	if ( pthread_create(&encoder_doer, NULL, encode_async, newtask) != 0 )
	{
		stackError("gsc_utils_loadsoundfile() error creating encoder async handler thread");
		stackPushInt(0);
		return;
	}

	if ( pthread_detach(encoder_doer) != 0 )
	{
		stackError("gsc_utils_loadsoundfile() error detaching encoder async handler thread");
		stackPushInt(0);
		return;
	}

	stackPushInt(currentMaxSoundIndex);
}
#endif