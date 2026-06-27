#ifndef _GSC_HPP_
#define _GSC_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <dlfcn.h> // dlcall
#include <stdarg.h> // va_args
#include <unistd.h> // link, unlink, usleep
#include <dirent.h> // dir stuff
#include <sys/mman.h> // mprotect
#include <execinfo.h> // stacktrace
#include <stddef.h> // offsetof
#include <ctype.h> // toupper
#include <sys/time.h> // milliseconds
#include <sys/stat.h> // fsize
#include <time.h>  // getsystemtime

#include "config.hpp"
#include "declarations.hpp"
#include "cracking.hpp"
#include "functions.hpp"

#if COMPILE_BOTS == 1
#include "gsc_bots.hpp"
#endif

#if COMPILE_BSP == 1
#include "bsp.hpp"
#endif

#if COMPILE_ENTITY == 1
#include "gsc_entity.hpp"
#endif

#if COMPILE_EXEC == 1
#include "gsc_exec.hpp"
#endif

#if COMPILE_JUMP == 1
#include "jump.hpp"
#endif

#if COMPILE_LEVEL == 1
#include "gsc_level.hpp"
#endif

#if COMPILE_MEMORY == 1
#include "gsc_memory.hpp"
#endif

#if COMPILE_MYSQL_DEFAULT == 1
#include "gsc_mysql.hpp"
#endif

#if COMPILE_MYSQL_VORON == 1
#include "gsc_mysql_voron.hpp"
#endif

#if COMPILE_PLAYER == 1
#include "gsc_player.hpp"
#endif

#if COMPILE_UTILS == 1
#include "gsc_utils.hpp"
#endif

#if COMPILE_WEAPONS == 1
#include "gsc_weapons.hpp"
#endif

#include "gsc_extra.hpp"

void Scr_AddFunc(const char *value);

void Scr_PushConstString(unsigned int value);
void Scr_PushString(const char *value);

#define stackPushUndefined Scr_AddUndefined
#define stackPushBool Scr_AddBool
#define stackPushInt Scr_AddInt
#define stackPushFloat Scr_AddFloat
#define stackPushString Scr_AddString
#define stackPushVector Scr_AddVector
#define stackPushEntity Scr_AddEntity
#define stackPushArray Scr_MakeArray
#define stackPushArrayLast Scr_AddArray
#define stackPushObject Scr_AddObject
#define stackPushFunc Scr_AddFunc

const char * getParamTypeAsString(int type);
const char * stackGetParamTypeAsString(int param);

int stackGetParamType(int param);
int stackGetParams(const char *params, ...);

int stackGetParamInt(int param, int *value);
int stackGetParamFunction(int param, int *value);
int stackGetParamString(int param, const char **value);
int stackGetParamConstString(int param, unsigned int *value);
int stackGetParamLocalizedString(int param, const char **value);
int stackGetParamVector(int param, vec3_t value);
int stackGetParamFloat(int param, float *value);
int stackGetParamObject(int param, unsigned int *value);

void stackError(const char *format, ...);
int stackPrintParam(int param);

xfunction_t Scr_GetCustomFunction(const char **fname, qboolean *fdev);
xmethod_t Scr_GetCustomMethod(const char **fname, qboolean *fdev);

#endif