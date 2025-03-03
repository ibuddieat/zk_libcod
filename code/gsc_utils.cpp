#include "gsc_utils.hpp"

#if COMPILE_CUSTOM_VOICE == 1
#include <speex/speex.h>
#include <pthread.h>

extern loadSoundFileResult_t loadSoundFileResults[MAX_THREAD_RESULTS_BUFFER];
extern int loadSoundFileResultsIndex;
extern dvar_t *fs_debug;
extern dvar_t *fs_homepath;

struct encoder_async_task
{
	encoder_async_task *prev;
	encoder_async_task *next;
	char filePath[MAX_STRINGLENGTH];
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

extern dvar_t *con_coloredPrints;
int stackPrintParam(int param)
{
	if ( param >= Scr_GetNumParam() )
		return 0;

	switch ( stackGetParamType(param) )
	{
	case STACK_STRING:
		char *str;
		stackGetParamString(param, &str); // No error checking, since we know it's a string
		if ( con_coloredPrints->current.boolean )
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
		printf("%.3f", tmp_float); // Need a way to define precision
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

extern snd_alias_build_s *customSoundAliasInfo;
void gsc_utils_getsoundaliasesfromfile()
{
	char *inputfilename;

	if ( !stackGetParams("s", &inputfilename) )
	{
		stackError("gsc_utils_getsoundaliasesfromfile() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( strlen(inputfilename) > 59 )
	{
		stackError("gsc_utils_getsoundaliasesfromfile() file name is too long");
		stackPushUndefined();
		return;
	}

	// Append .csv extension
	char inputfilenamecsv[64];
	Com_sprintf(inputfilenamecsv, 64, "%s.csv", inputfilename);

	Scr_MakeArray();
	for ( snd_alias_build_s *build = customSoundAliasInfo; build; build = build->pNext )
	{
		if ( !strcmp(inputfilenamecsv, build->szSourceFile) )
		{
			Scr_AddString(build->szAliasName);
			Scr_AddArray();
		}
	}
}

void gsc_utils_getsoundduration()
{
	char *soundalias;

	if ( !stackGetParams("s", &soundalias) )
	{
		stackError("gsc_utils_getsoundduration() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	snd_alias_list_t *list;

	list = Com_FindSoundAlias(soundalias);
	if ( !list )
	{
		stackError("gsc_utils_getsoundduration() unknown sound alias");
		stackPushUndefined();
		return;
	}

	char filename[MAX_ZPATH];
	searchpath_t *search;

	for ( search = fs_searchpaths; search; search = search->next )
	{
		iwd_t *iwd;
		unzFile uf;
		int err;
		unz_global_info gi;
		unsigned int i;
		unz_file_info file_info;
		char filename_inzip[MAX_ZPATH];
		void *buffer;
		int read;
		FILE *tempFile;

		// Find the specified file in the loaded iwds
		iwd = search->iwd;
		if ( iwd && iwd->numFiles )
		{
			uf = iwd->handle;
			err = unzGetGlobalInfo(uf, &gi);
			if ( err )
			{
				continue;
			}

			unzGoToFirstFile(uf);
			for ( i = 0; i < gi.number_entry; i++ )
			{
				err = unzGetCurrentFileInfo(uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);
				if ( err )
				{
					Com_DPrintf("gsc_utils_getsoundduration() error at unzGetCurrentFileInfo\n");
					break;
				}
				Com_sprintf(filename, MAX_ZPATH, "sound/%s", list->head->soundFile->soundName);
				if ( !FS_FilenameCompare(filename_inzip, filename) )
				{
					if ( unzOpenCurrentFile(iwd->handle) )
					{
						Com_DPrintf("gsc_utils_getsoundduration() error at unzOpenCurrentFile\n");
						break;
					}

					buffer = Z_MallocInternal(file_info.uncompressed_size);
					read = unzReadCurrentFile(iwd->handle, buffer, file_info.uncompressed_size);
					if ( read < 0 )
					{
						Com_DPrintf("gsc_utils_getsoundduration() error at unzReadCurrentFile\n");
						unzCloseCurrentFile(iwd->handle);
						Z_FreeInternal(buffer);
						break;
					}

					/* Decided against using a faster in-memory reader via
					 libavcodec as that causes symbol name overlaps that
					 cannot be fixed without refactoring the whole project.
					 We now depend on ffprobe instead of libavcodec-dev:i386
					 and potentially also libavformat-dev:i386.
					*/

					// Check if ffprobe is installed
					if ( system("which ffprobe > /dev/null 2>&1") )
					{
						stackError("gsc_utils_getsoundduration() ffprobe not installed");
						stackPushUndefined();
						unzCloseCurrentFile(iwd->handle);
						Z_FreeInternal(buffer);
						return;
					}

					// Write our buffer from within the iwd to a temporary file
					tempFile = fopen("getSoundDuration.tmp", "wb");
					if ( tempFile )
					{
						fwrite(buffer, 1, file_info.uncompressed_size, tempFile);
						fclose(tempFile);
					}
					else
					{
						stackError("gsc_utils_getsoundduration() could not create temporary file");
						stackPushUndefined();
						unzCloseCurrentFile(iwd->handle);
						Z_FreeInternal(buffer);
						return;
					}

					// Execute ffprobe on the file, delete the temporary file
					tempFile = popen("ffprobe -show_entries stream=duration -of compact=p=0:nk=1 -v fatal getSoundDuration.tmp; rm getSoundDuration.tmp", "r");
					if ( !tempFile )
					{
						stackError("gsc_utils_getsoundduration() could not execute ffprobe");
						stackPushUndefined();
						unzCloseCurrentFile(iwd->handle);
						Z_FreeInternal(buffer);
						return;
					}

					char c;
					int curpos = 0;
					char content[MAX_STRINGLENGTH];

					while ( (c = getc(tempFile)) != EOF )
					{
						if ( c == '\n' || curpos == MAX_STRINGLENGTH - 1 )
						{
							content[curpos] = '\0';
							break;
						}
						else
						{
							content[curpos] = c;
							curpos++;
						}
					}
					content[curpos] = '\0';

					// Free stuff
					pclose(tempFile);
					unzCloseCurrentFile(iwd->handle);
					Z_FreeInternal(buffer);

					// No output if input file is in invalid format or somehow corrupt
					if ( !*content )
					{
						stackError("gsc_utils_getsoundduration() input file is corrupt");
						stackPushUndefined();
						return;
					}

					// Reset infinite loop detection counter
					Scr_ResetTimeout();

					// Return seconds as float
					stackPushFloat(atof(content));
					return;
				}
				unzGoToNextFile(uf);
			}
		}
	}

	stackError("gsc_utils_getsoundduration() could not open or process file");
	stackPushUndefined();
}

extern customStringIndex_t custom_scr_const;
void gsc_utils_getsoundinfo()
{
	char *soundalias;

	if ( !stackGetParams("s", &soundalias) )
	{
		stackError("gsc_utils_getsoundinfo() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	snd_alias_list_t *list;

	list = Com_FindSoundAlias(soundalias);
	if ( !list )
	{
		stackError("gsc_utils_getsoundinfo() unknown sound alias");
		stackPushUndefined();
		return;
	}

	char filename[MAX_ZPATH];
	searchpath_t *search;

	for ( search = fs_searchpaths; search; search = search->next )
	{
		iwd_t *iwd;
		unzFile uf;
		int err;
		unz_global_info gi;
		unsigned int i;
		unz_file_info file_info;
		char filename_inzip[MAX_ZPATH];
		void *buffer;
		int read;
		FILE *tempFile;

		// Find the specified file in the loaded iwds
		iwd = search->iwd;
		if ( iwd && iwd->numFiles )
		{
			uf = iwd->handle;
			err = unzGetGlobalInfo(uf, &gi);
			if ( err )
			{
				continue;
			}

			unzGoToFirstFile(uf);
			for ( i = 0; i < gi.number_entry; i++ )
			{
				err = unzGetCurrentFileInfo(uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);
				if ( err )
				{
					Com_DPrintf("gsc_utils_getsoundinfo() error at unzGetCurrentFileInfo\n");
					break;
				}
				Com_sprintf(filename, MAX_ZPATH, "sound/%s", list->head->soundFile->soundName);
				if ( !FS_FilenameCompare(filename_inzip, filename) )
				{
					if ( unzOpenCurrentFile(iwd->handle) )
					{
						Com_DPrintf("gsc_utils_getsoundinfo() error at unzOpenCurrentFile\n");
						break;
					}

					buffer = Z_MallocInternal(file_info.uncompressed_size);
					read = unzReadCurrentFile(iwd->handle, buffer, file_info.uncompressed_size);
					if ( read < 0 )
					{
						Com_DPrintf("gsc_utils_getsoundinfo() error at unzReadCurrentFile\n");
						unzCloseCurrentFile(iwd->handle);
						Z_FreeInternal(buffer);
						break;
					}

					/* Decided against using a faster in-memory reader via
					 libavcodec as that causes symbol name overlaps that
					 cannot be fixed without refactoring the whole project.
					 We now depend on ffprobe instead of libavcodec-dev:i386
					 and potentially also libavformat-dev:i386.
					*/

					// Check if ffprobe is installed
					if ( system("which ffprobe > /dev/null 2>&1") )
					{
						stackError("gsc_utils_getsoundinfo() ffprobe not installed");
						stackPushUndefined();
						unzCloseCurrentFile(iwd->handle);
						Z_FreeInternal(buffer);
						return;
					}

					// Write our buffer from within the iwd to a temporary file
					tempFile = fopen("getSoundInfo.tmp", "wb");
					if ( tempFile )
					{
						fwrite(buffer, 1, file_info.uncompressed_size, tempFile);
						fclose(tempFile);
					}
					else
					{
						stackError("gsc_utils_getsoundinfo() could not create temporary file");
						stackPushUndefined();
						unzCloseCurrentFile(iwd->handle);
						Z_FreeInternal(buffer);
						return;
					}

					// Execute ffprobe on the file, delete the temporary file
					tempFile = popen("ffprobe -loglevel error -show_entries format_tags=title,artist -of default=noprint_wrappers=1:nokey=1 getSoundInfo.tmp; rm getSoundInfo.tmp", "r");
					if ( !tempFile )
					{
						stackError("gsc_utils_getsoundinfo() could not execute ffprobe");
						stackPushUndefined();
						unzCloseCurrentFile(iwd->handle);
						Z_FreeInternal(buffer);
						return;
					}

					char c;
					int curpos = 0;
					int curfield = 0;
					char info[2][MAX_STRINGLENGTH];

					memset(info, 0, MAX_STRINGLENGTH * 2);

					while ( (c = getc(tempFile)) != EOF && curfield < 2 )
					{
						if ( c == '\n' || curpos == MAX_STRINGLENGTH - 1 )
						{
							info[curfield][curpos] = '\0';
							curfield++;
							curpos = 0;
						}
						else
						{
							info[curfield][curpos] = c;
							curpos++;
						}
					}
					info[curfield][curpos] = '\0';

					// Free stuff
					pclose(tempFile);
					unzCloseCurrentFile(iwd->handle);
					Z_FreeInternal(buffer);

					// No output if input file is in invalid format or somehow corrupt
					if ( strstr(info[0], "Invalid data found when processing input") != NULL )
					{
						stackError("gsc_utils_getsoundinfo() input file is corrupt");
						stackPushUndefined();
						return;
					}

					// Reset infinite loop detection counter
					Scr_ResetTimeout();

					// Return string-indexed array
					Scr_MakeArray();
					Scr_AddString(info[0]);
					Scr_AddArrayStringIndexed(custom_scr_const.title);
					Scr_AddString(info[1]);
					Scr_AddArrayStringIndexed(custom_scr_const.artist);
					return;
				}
				unzGoToNextFile(uf);
			}
		}
	}

	stackError("gsc_utils_getsoundinfo() could not open or process file");
	stackPushUndefined();
}

void gsc_utils_processremotecommand()
{
	char *sFrom;
	int pointerMsg;

	if ( !stackGetParams("si", &sFrom, &pointerMsg) )
	{
		stackError("gsc_utils_processremotecommand() one or more arguments is undefined or has a wrong type");
		return;
	}

	netadr_t from;
	msg_t *msg = (msg_t *)pointerMsg;

	NET_StringToAdr(sFrom, &from);
	custom_SVC_RemoteCommand(from, msg, qtrue);
}

void gsc_utils_executecommand()
{
	char *str;

	if ( !stackGetParams("s", &str) )
	{
		stackError("gsc_utils_executecommand() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	Cmd_ExecuteString(str);
	stackPushBool(qtrue);
}

void gsc_utils_sendcommandtoclient()
{
	int clientNum;
	char *message;

	if ( !stackGetParams("is", &clientNum, &message) )
	{
		stackError("gsc_utils_sendcommandtoclient() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	SV_GameSendServerCommand(clientNum, SV_CMD_CAN_IGNORE, message);
	stackPushBool(qtrue);
}

void gsc_utils_system()
{
	char *cmd;

	if ( !stackGetParams("s",  &cmd) )
	{
		stackError("gsc_utils_system() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushInt(system(cmd));
}

void gsc_utils_printf()
{
	char *str;

	if ( !stackGetParams("s", &str) )
	{
		stackError("gsc_utils_printf() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int param = 1; // Maps to first %
	int len = strlen(str);

	for ( int i = 0; i < len; i++ )
	{
		if ( str[i] == '%' )
		{
			if ( str[i + 1] == '%' )
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
	char result[MAX_STRINGLENGTH];
	char *str;

	if ( !stackGetParams("s", &str) )
	{
		stackError("gsc_utils_sprintf() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int param = 1; // Maps to first %
	int len = strlen(str);
	int num = 0;

	for ( int i = 0; i < len; i++ )
	{
		if ( num > ( MAX_STRINGLENGTH - 2 ) )
		{
			num = MAX_STRINGLENGTH - 1;
			break;
		}

		if ( str[i] == '%' )
		{
			if ( str[i + 1] == '%' )
			{
				result[num++] = '%';
				i++;
			}
			else
			{
				if ( param >= Scr_GetNumParam() )
					continue;

				switch ( stackGetParamType(param) )
				{
				case STACK_STRING:
					char *tmp_str;
					stackGetParamString(param, &tmp_str); // No error checking, since we know it's a string
					num += snprintf(&(result[num]), MAX_STRINGLENGTH - num, "%s", tmp_str);
					break;

				case STACK_VECTOR:
					float tmp_vec[3];
					stackGetParamVector(param, tmp_vec);
					num += snprintf(&(result[num]), MAX_STRINGLENGTH - num, "(%.2f, %.2f, %.2f)", tmp_vec[0], tmp_vec[1], tmp_vec[2]);
					break;

				case STACK_FLOAT:
					float tmp_float;
					stackGetParamFloat(param, &tmp_float);
					num += snprintf(&(result[num]), MAX_STRINGLENGTH - num, "%.3f", tmp_float); // Need a way to define precision
					break;

				case STACK_INT:
					int tmp_int;
					stackGetParamInt(param, &tmp_int);
					num += snprintf(&(result[num]), MAX_STRINGLENGTH - num, "%d", tmp_int);
					break;
				}

				param++;
			}
		}
		else
		{
			result[num++] = str[i];
		}
	}

	result[num] = '\0';
	stackPushString(result);
}

void gsc_utils_sendpacket()
{
	char *address;
	char *msg;

	if ( !stackGetParams("ss", &address, &msg) )
	{
		stackError("gsc_utils_sendpacket() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	netadr_t to;

	if ( NET_StringToAdr(address, &to) )
	{
		if ( NET_OutOfBandPrint(NS_SERVER, to, msg) )
		{
			stackPushBool(qtrue);
		}
		else
		{
			stackError("gsc_utils_sendpacket() failed to send packet");
			stackPushUndefined();
			return;
		}
	}
	else
	{
		stackError("gsc_utils_sendpacket() invalid address");
		stackPushUndefined();
		return;
	}
}


void gsc_utils_putchar()
{
	int val;

	if ( !stackGetParams("i",  &val) )
	{
		stackError("gsc_utils_putchar() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( val < -127 || val > 127 )
	{
		stackError("gsc_utils_putchar() character index is out of range");
		stackPushUndefined();
		return;
	}

	char s[2];

	s[0] = val;
	s[1] = '\0';

	stackPushString(s);
}

void gsc_utils_logprintconsole()
{
	char *str;

	if ( !stackGetParams("s", &str) )
	{
		stackError("gsc_utils_logprintconsole() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	Com_Printf("%s", str);

	stackPushBool(qtrue);
}

extern char consolePrefix[MAX_CONSOLE_PREFIX_LENGTH];
void gsc_utils_setconsoleprefix()
{
	char *str;

	if ( !stackGetParams("s", &str) )
	{
		stackError("gsc_utils_setconsoleprefix() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	Com_sprintf(consolePrefix, MAX_CONSOLE_PREFIX_LENGTH, "%s", str);

	stackPushString(consolePrefix);
}

void gsc_utils_getarraykeys()
{
	unsigned int arrIndex;

	if ( !stackGetParamObject(0, &arrIndex) )
	{
		stackError("gsc_utils_getarraykeys() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int arraysize = GetArraySize(arrIndex);

	if ( !arraysize )
	{
		stackError("gsc_utils_getarraykeys() got an empty or invalid array");
		stackPushUndefined();
		return;
	}

	unsigned int index = arrIndex;
	unsigned int name;

	stackPushArray();

	for ( int i = 0; i < arraysize; i++ )
	{
		index = GetNextVariable(index);
		name = GetVariableName(index);

		if ( name < 0x10000 )
		{
			stackPushString(SL_ConvertToString(name));
			stackPushArrayLast();
		}
	}
}

void gsc_utils_getascii()
{
	char *str;

	if ( !stackGetParams("s", &str) )
	{
		stackError("gsc_utils_getascii() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( !strlen(str) )
	{
		stackError("gsc_utils_getascii() string length is 0");
		stackPushUndefined();
		return;
	}

	stackPushInt(str[0]);
}

extern const char * stackGetPrevCodePosFileName(const char *codePos, unsigned int index);
extern int stackGetPrevCodePosLineNumber(const char *codePos, unsigned int index);
void gsc_utils_getcallstack()
{
	int i, j;

	stackPushArray();
	stackPushString(stackGetPrevCodePosFileName(scrVmPub.function_frame->fs.pos, 0));
	stackPushArrayLast();
	stackPushInt(stackGetPrevCodePosLineNumber(scrVmPub.function_frame->fs.pos, 0));
	stackPushArrayLast();
	i = scrVmPub.function_count;
	if ( scrVmPub.function_count )
	{
		while ( j = i - 1, 0 < j )
		{
			stackPushString(stackGetPrevCodePosFileName(scrVmPub.function_frame_start[i - 1].fs.pos, scrVmPub.function_frame_start[i - 1].fs.localId == 0));
			stackPushArrayLast();
			stackPushInt(stackGetPrevCodePosLineNumber(scrVmPub.function_frame_start[i - 1].fs.pos, scrVmPub.function_frame_start[i - 1].fs.localId == 0));
			stackPushArrayLast();
			i = j;
		}
		stackPushString(stackGetPrevCodePosFileName(scrVmPub.function_frame_start[0].fs.pos, 1));
		stackPushArrayLast();
		stackPushInt(stackGetPrevCodePosLineNumber(scrVmPub.function_frame_start[0].fs.pos, 1));
		stackPushArrayLast();
	}
}

void gsc_utils_toupper()
{
	char *str;

	if ( !stackGetParams("s", &str) )
	{
		stackError("gsc_utils_toupper() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( !strlen(str) )
	{
		stackError("gsc_utils_toupper() string length is 0");
		stackPushUndefined();
		return;
	}

	stackPushString(I_strupr(str));
}

void gsc_utils_file_link()
{
	char *source, *dest;

	if ( !stackGetParams("ss",  &source, &dest) )
	{
		stackError("gsc_utils_file_link() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int link_success = symlink(source, dest) == 0;
	stackPushInt(link_success);
}

void gsc_utils_file_unlink()
{
	char *file;

	if ( !stackGetParams("s",  &file) )
	{
		stackError("gsc_utils_file_unlink() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int unlink_success = unlink(file) == 0;
	stackPushInt(unlink_success);
}

void gsc_utils_file_exists()
{
	char *filename;

	if ( !stackGetParams("s", &filename) )
	{
		stackError("gsc_utils_file_exists() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int file_exists = access(filename, F_OK) != -1;
	stackPushInt(file_exists);
}

void gsc_utils_loaddir()
{
	char *path, *dir;

	if ( !stackGetParams("ss", &path, &dir) )
	{
		stackError("gsc_utils_loaddir() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	FS_AddIwdFilesForGameDirectory(path, dir);
	stackPushBool(qtrue);
}

void gsc_utils_scandir()
{
	char *dirname;

	if ( !stackGetParams("s", &dirname) )
	{
		stackError("gsc_utils_scandir() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	DIR *dir;
	struct dirent *dir_ent;

	dir = opendir(dirname);

	if ( !dir )
	{
		stackPushUndefined();
		return;
	}

	stackPushArray();

	while ( ( dir_ent = readdir(dir) ) != NULL )
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

	if ( !stackGetParams("ss", &filename, &mode) )
	{
		stackError("gsc_utils_fopen() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	file = fopen(filename, mode);

	if ( !file )
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

	if ( !stackGetParams("i", &file) )
	{
		stackError("gsc_utils_fread() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( !file )
	{
		stackError("gsc_utils_fread() returned a error");
		stackPushUndefined();
		return;
	}

	char buffer[256];
	int ret = fread(buffer, 1, 255, file);

	if ( !ret )
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

	if ( !stackGetParams("is", &file, &buffer) )
	{
		stackError("gsc_utils_fwrite() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( !file )
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

	if ( !stackGetParams("i", &file) )
	{
		stackError("gsc_utils_fclose() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( !file )
	{
		stackError("gsc_utils_fclose() returned a error");
		stackPushUndefined();
		return;
	}

	stackPushInt(fclose(file));
}

void gsc_utils_fsize()
{
	FILE *file;

	if ( !stackGetParams("i", &file) )
	{
		stackError("gsc_utils_fsize() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( !file )
	{
		stackError("gsc_utils_fsize() returned a error");
		stackPushUndefined();
		return;
	}

	struct stat buf;
	fstat(fileno(file), &buf);

	stackPushInt(buf.st_size);
}

void gsc_utils_fremove()
{
	char *file;
	
	if ( !stackGetParams("s", &file) )
	{
		stackError("gsc_utils_fremove() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	
	stackPushInt(remove(file));
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

	stackPushString(stripped_time);
}

void gsc_utils_getmicroseconds()
{
	struct timeval tv;
	unsigned long microseconds;

	gettimeofday(&tv, NULL);
	microseconds = 1000000 * tv.tv_sec + tv.tv_usec;
	stackPushInt(int(microseconds) & 0x7FFFFFFF);
}

void gsc_utils_getmilliseconds()
{
	stackPushInt(Sys_Milliseconds() & 0x7FFFFFFF);
}

static int starttime = time(NULL);
void gsc_utils_getserverstarttime()
{
	stackPushInt(starttime);
}

void gsc_utils_getsystemtime()
{
	time_t timer;
	stackPushInt(time(&timer));
}

// http://code.metager.de/source/xref/RavenSoftware/jediacademy/code/game/g_utils.cpp#36
void gsc_utils_findconfigstringindexoriginal()
{
	char *name;
	int min, max, create;

	if ( !stackGetParams("siii", &name, &min, &max, &create) )
	{
		stackError("gsc_utils_findconfigstringindexoriginal() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
    
	if ( min < 0 || max >= MAX_CONFIGSTRINGS )
	{
		stackError("gsc_utils_findconfigstringindexoriginal() configstring index is out of range");
		stackPushUndefined();
		return;
	}

	stackPushInt(G_FindConfigstringIndex(name, min, max, create, "G_FindConfigstringIndex() from GSC"));
}

// Simple version, without crash
void gsc_utils_findconfigstringindex()
{
	char *name;
	int min, max;

	if ( !stackGetParams("sii", &name, &min, &max) )
	{
		stackError("gsc_utils_findconfigstringindex() one or more arguments is undefined or has a wrong type");
		return;
	}
    
    if ( min < 0 || max >= MAX_CONFIGSTRINGS )
	{
		stackError("gsc_utils_findconfigstringindex() configstring index is out of range");
		stackPushUndefined();
		return;
	}

	for ( int i = 1; i < max; i++ )
	{
		const char *curitem = SV_GetConfigstringConst(min + i);

		if ( !*curitem )
			break;

		if ( !strcasecmp(name, curitem) )
		{
			stackPushInt(i + min);
			return;
		}
	}

	stackPushBool(qtrue);
}

void gsc_utils_getconfigstring()
{
	int index;

	if ( !stackGetParams("i", &index) )
	{
		stackError("gsc_utils_getconfigstring() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
    
    if ( index < 0 || index >= MAX_CONFIGSTRINGS )
	{
		stackError("gsc_utils_getconfigstring() configstring index is out of range");
		stackPushUndefined();
		return;
	}

	const char *string = SV_GetConfigstringConst(index);

	if ( !*string )
		stackPushUndefined();
	else
		stackPushString(string);
}

void gsc_utils_setconfigstring()
{
	int index;
	char *string;

	if ( !stackGetParams("is", &index, &string) )
	{
		stackError("gsc_utils_setconfigstring() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
    
	if ( index < 0 || index >= MAX_CONFIGSTRINGS )
	{
		stackError("gsc_utils_setconfigstring() configstring index is out of range");
		stackPushUndefined();
		return;
	}

	SV_SetConfigstring(index, string);
	stackPushBool(qtrue);
}

void gsc_utils_makelocalizedstring()
{
	char *str;

	if ( !stackGetParams("s", &str) )
	{
		stackError("gsc_utils_makelocalizedstring() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushString(str);

	VariableValue *var;
	int param = 0;

	var = &scrVmPub.top[-param];
	var->type = STACK_LOCALIZED_STRING;
}

void gsc_utils_makeclientlocalizedstring()
{
	char *input;

	if ( !stackGetParams("s", &input) )
	{
		stackError("gsc_utils_makeclientlocalizedstring() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( !strlen(input) || strlen(input) > MAX_STRINGLENGTH - 3 )
	{
		stackError("gsc_utils_makeclientlocalizedstring() invalid string length");
		stackPushUndefined();
		return;
	}

	char output[MAX_STRINGLENGTH];
	Com_sprintf(output, MAX_STRINGLENGTH,"\x14%s\x15", input);

	stackPushString(output);
}

void gsc_utils_makestring()
{
	char *str;

	if ( !stackGetParams("l", &str) )
	{
		stackError("gsc_utils_makestring() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushString(str);

	VariableValue *var;
	int param = 0;

	var = &scrVmPub.top[-param];
	var->type = STACK_STRING;
}

void gsc_utils_float()
{
	if ( Scr_GetNumParam() == 0 )
	{
		stackError("gsc_utils_float() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	switch ( stackGetParamType(0) )
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

void gsc_utils_pow()
{
	float basis;
	float exponent;

	if ( !stackGetParams("ff", &basis, &exponent) )
	{
		stackError("gsc_utils_pow() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushFloat(pow(basis, exponent));
}

void gsc_utils_round()
{
	float val;

	if ( !stackGetParams("f",  &val) )
	{
		stackError("gsc_utils_round() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushFloat(roundf(val * 100) / 100);
}

void gsc_utils_roundto()
{
	float val;
	float precision;

	if ( !stackGetParams("ff",  &val, &precision) )
	{
		stackError("gsc_utils_roundto() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushFloat(roundf(val * precision) / precision);
}

void gsc_utils_sqrt()
{
	float x;

	if ( !stackGetParams("f", &x) )
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

	if ( !stackGetParams("f", &x) )
	{
		stackError("gsc_utils_sqrtinv() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	// http://www.beyond3d.com/content/articles/8/
	float xhalf = 0.5f * x;
	int i = *(int*)&x;
	i = 0x5F3759DF - (i >> 1);
	x = *(float*)&i;
	x = x * (1.5f - xhalf * x * x);

	stackPushFloat(x);
}

void gsc_utils_chr()
{
	int input;

	if ( !stackGetParams("i", &input) )
	{
		stackError("gsc_utils_chr() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	char output[2];
	if ( input == 0x25 )
	{
		output[0] = '.';
		output[1] = '\0';
	}
	else
	{
		Com_sprintf(output, 2, "%c", input & 0xFF);
	}

	stackPushString(output);
}

void gsc_utils_ord()
{
	char *input;

	if ( !stackGetParams("s", &input) )
	{
		stackError("gsc_utils_ord() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( strlen(input) != 1 )
	{
		stackError("gsc_utils_ord() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushInt((unsigned char)*input);
}

void gsc_utils_fromhex()
{
	char *input;

	if ( !stackGetParams("s", &input) )
	{
		stackError("gsc_utils_fromhex() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int output;

	if ( sscanf(input, "%X", &output) != 1 )
	{
		stackError("gsc_utils_fromhex() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushInt(output);
}

void gsc_utils_tohex()
{
	int input;

	if ( !stackGetParams("i", &input) )
	{
		stackError("gsc_utils_tohex() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	char output[11];
	Com_sprintf(output, 11, "0x%X", input);

	stackPushString(output);
}

void gsc_utils_vectorscale()
{
	vec3_t vector;
	float scale;

	if ( !stackGetParams("vf", &vector, &scale) )
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
	int offset = 0x083E2F0C;

	stackPushInt(*(int *)offset);
}

void gsc_utils_bullethiteffect()
{
	vec3_t origin;
	vec3_t normal;

	if ( !stackGetParams("vv", &origin, &normal) )
	{
		stackError("gsc_utils_bullethiteffect() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	gentity_t *entity = G_TempEntity(origin, EV_SHOTGUN_HIT);
	entity->s.eventParm = DirToByte(normal);

	trace_t trace;

	vec3_t end_origin = { origin[0] - (normal[0] * 10), origin[1] - (normal[1] * 10), origin[2] - (normal[2] * 10) };
	G_LocationalTrace(&trace, origin, end_origin, 1023, 1, NULL);

	entity->s.surfType = (trace.surfaceFlags >> 20) & 0x1F;

	stackPushBool(qtrue);
}

void gsc_utils_getsurfacename()
{
	int index;

	if ( !stackGetParams("i", &index) )
	{
		stackError("gsc_utils_getsurfacename() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushString(Com_SurfaceTypeToName(index));
}

void gsc_utils_gettype()
{
	if ( Scr_GetNumParam() == 0 )
	{
		stackError("gsc_utils_gettype() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushString(stackGetParamTypeAsString(0));
}

void gsc_utils_getdvarflags()
{
	const char *dvarName;
	dvar_t *dvar;

	dvarName = Scr_GetString(0);
	dvar = Dvar_FindVar(dvarName);
	if ( !dvar )
	{
		stackError("gsc_utils_getdvarflags() dvar '%s' does not exist", dvarName);
		stackPushUndefined();
		return;
	}

	stackPushInt(dvar->flags);
}

void gsc_utils_error()
{
	int terminal;
	const char *message;

	message = Scr_GetString(0);
	terminal = 0;
	if ( Scr_GetNumParam() > 1 )
		terminal = Scr_GetInt(1);

	if ( !terminal )
		Scr_Error(message);
	else
		Com_Error(ERR_SCRIPT_DROP, "\x15%s\n", message);
}

#endif


#if COMPILE_CUSTOM_VOICE == 1

extern VoicePacket_t voiceDataStore[MAX_CUSTOMSOUNDS][MAX_STOREDVOICEPACKETS];
extern dvar_t *sv_voiceQuality;
encoder_async_task *first_encoder_async_task = NULL;
extern int currentMaxSoundIndex;

void Encode_SetOptions(void *encoder)
{
	int g_encoder_samplerate = 8192;
	int g_encoder_quality = sv_voiceQuality->current.integer;
	int enabled = 0;
	speex_encoder_ctl(encoder, SPEEX_SET_SAMPLING_RATE /* 24 */, &g_encoder_samplerate);
	speex_encoder_ctl(encoder, SPEEX_SET_QUALITY /* 4 */, &g_encoder_quality);
	speex_encoder_ctl(encoder, SPEEX_SET_VAD /* 30 */, &enabled); // Voice Activity Detection (VAD) status
	speex_encoder_ctl(encoder, SPEEX_SET_DTX /* 34 */, &enabled); // Discontinuous Transmission (DTX)
}

void *encode_async(void *newtask)
{
	encoder_async_task *task = (encoder_async_task*)newtask;
	int result = 0;

	// Reset sound data for this slot
	memset(&voiceDataStore[task->soundIndex - 1], 0, sizeof(voiceDataStore[0]));

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

	// (Re)Open input file
	FILE *file = fopen(task->filePath, "r");

	// Create a new encoder state in narrowband mode
	g_encoder = speex_encoder_init(&speex_nb_mode);
	speex_bits_init(&encodeBits);
	Encode_SetOptions(g_encoder);

	for ( packetIndex = 0; packetIndex <= MAX_STOREDVOICEPACKETS; packetIndex++ )
	{
		if ( packetIndex == MAX_STOREDVOICEPACKETS )
		{
			// Sound file too long, end encoding here
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
		voicePacket = &voiceDataStore[task->soundIndex - 1][packetIndex];
		memcpy(voicePacket->data, data, dataLen);
		voicePacket->dataLen = dataLen;
	}
	// Reset subsequent voice packet (in case a long sound is replaced by a shorter one)
	if ( packetIndex != MAX_STOREDVOICEPACKETS )
		memset(&voiceDataStore[task->soundIndex - 1][packetIndex], 0, sizeof(voiceDataStore[0][0]));

	speex_encoder_destroy(g_encoder);
	speex_bits_destroy(&encodeBits);
	fclose(file);

	if ( Scr_IsSystemActive() )
	{
		Sys_EnterCriticalSection(CRITSECT_LOAD_SOUND_FILE);
		if ( loadSoundFileResultsIndex < MAX_THREAD_RESULTS_BUFFER )
		{
			loadSoundFileResults[loadSoundFileResultsIndex].result = result;
			loadSoundFileResults[loadSoundFileResultsIndex].soundIndex = task->soundIndex;
			loadSoundFileResults[loadSoundFileResultsIndex].callback = task->callback;
			loadSoundFileResults[loadSoundFileResultsIndex].levelId = task->levelId;
			loadSoundFileResultsIndex++;
		}
		/* No message on error excess here since that might introduce another
		 concurrency issue. Instead, we warn in the main thread if the buffer
		 is full */
		Sys_LeaveCriticalSection(CRITSECT_LOAD_SOUND_FILE);
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

	char ospath[MAX_OSPATH];
    FS_BuildOSPath(fs_homepath->current.string, filePath, "", ospath);
    ospath[strlen(ospath) - 1] = '\0';
    if ( fs_debug->current.integer )
		Com_Printf("gsc_utils_getsoundfileduration (fs_homepath) : %s\n", ospath);

	if ( strstr(ospath, "..") )
	{
		stackError("gsc_utils_getsoundfileduration() invalid file path");
		stackPushUndefined();
		return;
	}

	FILE *file = fopen(ospath, "rb");
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
		stackPushUndefined();
		return;
	}

	if ( !stackGetParamFunction(1, &callback) )
	{
		stackError("gsc_utils_loadsoundfile() requires a callback function as second argument");
		stackPushUndefined();
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

	if ( soundIndex < 1 || soundIndex > MAX_CUSTOMSOUNDS )
	{
		stackError("gsc_utils_loadsoundfile() invalid sound index, valid range is 1-%d", MAX_CUSTOMSOUNDS);
		stackPushUndefined();
		return;
	}

	char ospath[MAX_OSPATH];
    FS_BuildOSPath(fs_homepath->current.string, filePath, "", ospath);
    ospath[strlen(ospath) - 1] = '\0';
    if ( fs_debug->current.integer )
		Com_Printf("gsc_utils_loadsoundfile (fs_homepath) : %s\n", ospath);

	if ( strstr(ospath, "..") )
	{
		stackError("gsc_utils_loadsoundfile() invalid file path");
		stackPushUndefined();
		return;
	}

	FILE *file = fopen(ospath, "r");
	if ( !file )
	{
		stackError("gsc_utils_loadsoundfile() input file could not be opened");
		stackPushUndefined();
		return;
	}
	else
	{
		fclose(file);
	}

	encoder_async_task *current = first_encoder_async_task;

	while ( current != NULL && current->next != NULL )
		current = current->next;

	encoder_async_task *newtask = new encoder_async_task;

	strncpy(newtask->filePath, filePath, MAX_STRINGLENGTH - 1);
	newtask->prev = current;
	newtask->next = NULL;
	newtask->filePath[MAX_STRINGLENGTH - 1] = '\0';
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
		stackPushUndefined();
		return;
	}

	if ( pthread_detach(encoder_doer) != 0 )
	{
		stackError("gsc_utils_loadsoundfile() error detaching encoder async handler thread");
		stackPushUndefined();
		return;
	}

	stackPushInt(soundIndex);
}

void gsc_utils_loadspeexfile()
{
	char *filePath;
	int soundIndex;

	if ( !stackGetParamString(0, &filePath) )
	{
		stackError("gsc_utils_loadspeexfile() requires a file path (string) as first argument");
		stackPushBool(qfalse);
		return;
	}

	if ( !stackGetParamInt(1, &soundIndex) )
	{
		stackError("gsc_utils_loadspeexfile() requires a sound index (integer) as second argument");
		stackPushBool(qfalse);
		return;
	}

	if ( soundIndex < 1 || soundIndex > MAX_CUSTOMSOUNDS )
	{
		stackError("gsc_utils_loadspeexfile() invalid sound index, valid range is 1-%d", MAX_CUSTOMSOUNDS);
		stackPushBool(qfalse);
		return;
	}

	char ospath[MAX_OSPATH];
    FS_BuildOSPath(fs_homepath->current.string, filePath, "", ospath);
    ospath[strlen(ospath) - 1] = '\0';
    if ( fs_debug->current.integer )
		Com_Printf("gsc_utils_loadspeexfile (fs_homepath) : %s\n", ospath);

	if ( strstr(ospath, "..") )
	{
		stackError("gsc_utils_loadspeexfile() invalid file path");
		stackPushUndefined();
		return;
	}

	FILE *file = fopen(ospath, "rb");
	if ( !file )
	{
		stackError("gsc_utils_loadspeexfile() could not open the specified file");
		stackPushBool(qfalse);
		return;
	}

	int packetIndex;
	VoicePacket_t packet;
	for ( packetIndex = 0; packetIndex <= MAX_STOREDVOICEPACKETS; packetIndex++ )
	{
		if ( packetIndex == MAX_STOREDVOICEPACKETS )
		{
			// Speex file overly long, end reading here
			break;
		}

		// Read a single voice packet at a time
		fread(&packet, sizeof(packet), 1, file);
		if ( feof(file) )
			break;
		
		memcpy(&voiceDataStore[soundIndex - 1][packetIndex], &packet, sizeof(packet));
	}
	// Reset subsequent voice packet (in case a long sound is replaced by a shorter one)
	if ( packetIndex != MAX_STOREDVOICEPACKETS )
		memset(&voiceDataStore[soundIndex - 1][packetIndex], 0, sizeof(packet));

	fclose(file);

	stackPushBool(qtrue);
}

void gsc_utils_savespeexfile()
{
	int soundIndex;
	char *filePath;

	if ( !stackGetParamInt(0, &soundIndex) )
	{
		stackError("gsc_utils_savespeexfile() requires a sound index (integer) as first argument");
		stackPushBool(qfalse);
		return;
	}

	if ( soundIndex < 1 || soundIndex > MAX_CUSTOMSOUNDS )
	{
		stackError("gsc_utils_savespeexfile() invalid sound index, valid range is 1-%d", MAX_CUSTOMSOUNDS);
		stackPushBool(qfalse);
		return;
	}

	if ( !stackGetParamString(1, &filePath) )
	{
		stackError("gsc_utils_savespeexfile() requires a file path (string) as second argument");
		stackPushBool(qfalse);
		return;
	}

	VoicePacket_t *packet;
	packet = &voiceDataStore[soundIndex - 1][0];
	if ( !packet->dataLen )
	{
		stackError("gsc_utils_savespeexfile() no sound data in the specified slot");
		stackPushBool(qfalse);
		return;
	}

	char ospath[MAX_OSPATH];
    FS_BuildOSPath(fs_homepath->current.string, filePath, "", ospath);
    ospath[strlen(ospath) - 1] = '\0';
    if ( fs_debug->current.integer )
		Com_Printf("gsc_utils_savespeexfile (fs_homepath) : %s\n", ospath);

	if ( strstr(ospath, "..") )
	{
		stackError("gsc_utils_savespeexfile() invalid file path");
		stackPushUndefined();
		return;
	}

	FILE *file = fopen(ospath, "wb");
	if ( !file )
	{
		stackError("gsc_utils_savespeexfile() could not open the specified file");
		stackPushBool(qfalse);
		return;
	}

	for ( int packetIndex = 0; packetIndex < MAX_STOREDVOICEPACKETS; packetIndex++ )
	{
		packet = &voiceDataStore[soundIndex - 1][packetIndex];
		if ( !packet->dataLen )
			break;

		fwrite(packet, sizeof(VoicePacket_t), 1, file);
	}
	fclose(file);

	stackPushBool(qtrue);
}

#endif