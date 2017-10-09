#include "gsc_utils.hpp"

#if COMPILE_UTILS == 1

#include <dirent.h> // dir stuff
#include <ctype.h> // toupper
#include <time.h>  // getsystemtime
#include <sys/stat.h> // fsize

//thanks to riicchhaarrd/php
unsigned short Scr_GetArray(int param)
{
	if (param >= Scr_GetNumParam())
	{
		stackError("Scr_GetArray() one parameter is required");
		return 0;
	}

	VariableValue *var;
	var = &scrVmPub.top[-param];

	if (var->type == STACK_OBJECT)
		return *(unsigned short*)var;

	stackError("Scr_GetArray() the parameter must be an array");
	return 0;
}

void gsc_utils_getarraykeys()
{
	unsigned short arrIndex = Scr_GetArray(0);
	stackPushArray();

	if (arrIndex == 0)
		return; // we didn't find a valid array

	unsigned short i;
	for(i = GetNextVariable(arrIndex); i != 0;)
	{
		stackPushString(SL_ConvertToString(GetVariableName(i)));
		stackPushArrayLast();
		i = GetNextVariable(i);
	}
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
void Sys_AnsiColorPrint( const char *msg )
{
	static char buffer[ MAXPRINTMSG ];
	int         length = 0;
	static int  q3ToAnsi[ 8 ] =
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

	while( *msg )
	{
		if( Q_IsColorString( msg ) || *msg == '\n' )
		{
			// First empty the buffer
			if( length > 0 )
			{
				buffer[ length ] = '\0';
				fputs( buffer, stdout );
				length = 0;
			}

			if( *msg == '\n' )
			{
				// Issue a reset and then the newline
				fputs( "\033[0m\n", stdout );
				msg++;
			}
			else
			{
				// Print the color code
				snprintf( buffer, sizeof( buffer ), "\033[1;%dm", q3ToAnsi[ ColorIndex( *( msg + 1 ) ) ] );
				fputs( buffer, stdout );
				msg += 2;
			}
		}
		else
		{
			if( length >= MAXPRINTMSG - 1 )
				break;

			buffer[ length ] = *msg;
			length++;
			msg++;
		}
	}

	// Empty anything still left in the buffer
	if( length > 0 )
	{
		buffer[ length ] = '\0';
		fputs( buffer, stdout );
		// Issue a reset at the end
		fputs( "\033[0m", stdout );
	}
}

extern cvar_t *colored_prints;
int stackPrintParam(int param)
{
	if (param >= Scr_GetNumParam())
		return 0;

	switch (stackGetParamType(param))
	{
	case STACK_STRING:
		char *str;
		stackGetParamString(param, &str); // no error checking, since we know it's a string
		if (colored_prints->boolean)
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

	stackPushInt(1);
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
			if(str[i + 1] == '%')
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

void gsc_utils_getAscii()
{
	char *str;
	if ( ! stackGetParams("s", &str) || strlen(str) == 0)
	{
		stackError("gsc_utils_getAscii() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	stackPushInt(str[0]);
}

void gsc_utils_toupper()
{
	char *str;
	int offset = 0;
	int len = 0;
	if ( ! stackGetParams("s", &str) || strlen(str) == 0)
	{
		stackError("gsc_utils_toupper() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackGetParamInt(1, &offset);
	if(offset < 0)
		offset = 0;
	if(!stackGetParamInt(2, &len) || len == 0)
		len = strlen(str);
	if((len - offset) > int(strlen(str)))
		len = strlen(str) - offset;
	if(len <= 0)
	{
		stackPushString("");
		return;
	}

	int maxlen = strlen(str);
	char result[maxlen+1];
	strcpy(result, str);

	for (int i = offset; i < len; i++)
	{
		result[i] = toupper(str[i]);
	}
	result[maxlen] = '\0';

	stackPushString(result);
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

void gsc_utils_getsystemtime()
{
	time_t timer;
	stackPushInt( time(&timer) );
}

void gsc_utils_getlocaltime()
{
	time_t timer;
	struct tm *timeinfo;

	time(&timer);
	timeinfo = localtime(&timer);

	stackPushString( asctime(timeinfo) );
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

void gsc_utils_file_link()
{
	char *source, *dest;
	if ( ! stackGetParams("ss",  &source, &dest))
	{
		stackError("gsc_utils_file_link() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	char cmd[COD2_MAX_STRINGLENGTH];
	snprintf(cmd, sizeof(cmd), "ln -sfn %s %s", source, dest);
	int link_failed = system(cmd);
	stackPushInt( link_failed ); // 0 == success
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
	stackPushInt( unlink(file) ); // 0 == success
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
	stackPushInt( ! (access(filename, F_OK) == -1) );
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
	stackPushInt(1);
}

void gsc_utils_getType()
{
	if (Scr_GetNumParam() == 0)
	{
		stackError("gsc_utils_getType() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	stackPushString( stackGetParamTypeAsString(0) );
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

// rundll("print.so", "test_print")
void gsc_utils_rundll()
{
	char *arg_library, *arg_function;
	if ( ! stackGetParams("ss", &arg_library, &arg_function))
	{
		stackError("gsc_utils_rundll() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	Com_DPrintf("lib=%s func=%s\n", arg_library, arg_function);
	//void *handle = dlopen(arg_library, RTLD_GLOBAL); // crashes
	//void *handle = dlopen(arg_library, RTLD_LOCAL); // crashes
	//void *handle = dlopen(arg_library, RTLD_NOW); // crashes
	void *handle = dlopen(arg_library, RTLD_LAZY);
	if ( ! handle)
	{
		stackError("dlopen(\"%s\") failed", arg_library);
		stackPushInt(0);
		return;
	}
	Com_DPrintf("dlopen(\"%s\") returned: %.8x\n", arg_library, (unsigned int)handle);
	void (*func)();
	//*((void *)&func) = dlsym(handle, arg_function);
	*(int *)&func = (int)dlsym(handle, arg_function);
	if (!func)
	{
		stackError("dlsym(\"%s\") failed", arg_function);
		stackPushInt(0);
		return;
	}
	Com_DPrintf("function-name=%s -> address=%.8x\n", arg_function, (unsigned int)func);
	func();
	dlclose(handle);
	stackPushInt(1);
}

void gsc_utils_ExecuteString()
{
	char *str;
	if ( ! stackGetParams("s", &str))
	{
		stackError("gsc_utils_ExecuteString() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	Cmd_ExecuteString(str);
	stackPushInt(1);
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
	stackPushInt(1);
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

// http://code.metager.de/source/xref/RavenSoftware/jediacademy/code/game/g_utils.cpp#36
void gsc_G_FindConfigstringIndexOriginal()
{
	char *name;
	int min, max, create;
	if ( ! stackGetParams("siii", &name, &min, &max, &create))
	{
		stackError("gsc_G_FindConfigstringIndexOriginal() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushInt( G_FindConfigstringIndex(name, min, max, create, "G_FindConfigstringIndex() from GSC") );
}

// simple version, without crash
void gsc_G_FindConfigstringIndex()
{
	char *name;
	int min, max;
	if ( ! stackGetParams("sii", &name, &min, &max))
	{
		stackError("gsc_G_FindConfigstringIndex() one or more arguments is undefined or has a wrong type");
		return;
	}

	for (int i = 1; i < max; i++)
	{
		char *curitem = SV_GetConfigstringConst(min + i);

		if ( ! *curitem)
			break;

		if ( ! strcasecmp(name, curitem))
		{
			stackPushInt(i + min);
			return;
		}
	}

	stackPushInt(0);
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

	char *string = SV_GetConfigstringConst(index);
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

	SV_SetConfigstring(index, string);
	stackPushInt(1);
}

void gsc_call_function_raw()
{
	int func_address;
	char *args;
	unsigned char *data;
	if ( ! stackGetParams("isi", &func_address, &args, &data))
	{
		stackError("gsc_call_function_raw() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	stackPushInt(cracking_call_function(func_address, args, data));
}

void gsc_dlopen()
{
	char *arg_library;
	if ( ! stackGetParams("s", &arg_library))
	{
		stackError("gsc_dlopen() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	int handle = (int)dlopen(arg_library, RTLD_LAZY);
	if ( ! handle)
	{
		stackError("dlopen(\"%s\") failed! Error: %s", arg_library, dlerror());
		stackPushInt(0);
		return;
	}
	stackPushInt(handle);
}

void gsc_dlsym()
{
	int handle;
	char *arg_function;
	if ( ! stackGetParams("is", &handle, &arg_function))
	{
		stackError("gsc_dlsym() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	int func = (int)dlsym((void *)handle, arg_function);
	if (!func)
	{
		stackError("dlsym(\"%s\") failed! Error: %s", arg_function, dlerror());
		stackPushInt(0);
		return;
	}
	stackPushInt(func);
}

void gsc_dlclose()
{
	int handle;
	if ( ! stackGetParams("i", &handle))
	{
		stackError("gsc_dlclose() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	int ret = dlclose((void *)handle);
	if (ret != 0)
	{
		stackError("dlclose(%d) failed! Error: %s", handle, dlerror());
		stackPushInt(0);
		return;
	}
	stackPushInt(ret);
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

void gsc_utils_sqrtInv()
{
	float x;
	if ( ! stackGetParams("f", &x))
	{
		stackError("gsc_utils_sqrtInv() argument is undefined or has a wrong type");
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

#endif
