#ifndef _GSC_HPP_
#define _GSC_HPP_

#ifdef __cplusplus
extern "C" {
#endif

#define COD2_1_0 210
#define COD2_1_2 212
#define COD2_1_3 213

#define COD2_MAX_STRINGLENGTH 1024

/* default stuff */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dlfcn.h> // dlcall
#include <stdarg.h> // va_args
#include <unistd.h> //link, unlink, usleep
#include <dirent.h> // dir stuff
#include <sys/mman.h> // mprotect
#include <execinfo.h> // stacktrace
#include <stddef.h> // offsetof

#include "config.hpp"
#include "cracking.hpp"
#include "functions.hpp"

#if COMPILE_BOTS == 1
#include "gsc_bots.hpp"
#endif

#if COMPILE_EXEC == 1
#include "gsc_exec.hpp"
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

#ifdef EXTRA_INCLUDES_INC
#include "extra/includes.hpp"
#endif

#define STACK_UNDEFINED 0
#define STACK_OBJECT 1
#define STACK_STRING 2
#define STACK_LOCALIZED_STRING 3
#define STACK_VECTOR 4
#define STACK_FLOAT 5
#define STACK_INT 6
#define STACK_CODEPOS 7
#define STACK_PRECODEPOS 8
#define STACK_FUNCTION 9
#define STACK_STACK 10
#define STACK_ANIMATION 11
#define STACK_DEVELOPER_CODEPOS 12
#define STACK_INCLUDE_CODEPOS 13
#define STACK_THREAD_LIST 14
#define STACK_THREAD_1 15
#define STACK_THREAD_2 16
#define STACK_THREAD_3 17
#define STACK_THREAD_4 18
#define STACK_STRUCT 19
#define STACK_REMOVED_ENTITY 20
#define STACK_ENTITY 21
#define STACK_ARRAY 22
#define STACK_REMOVED_THREAD 23

#if COD_VERSION == COD2_1_0
static const int varpub_offset = 0x08394000;
#elif COD_VERSION == COD2_1_2
static const int varpub_offset = 0x08396480;
#elif COD_VERSION == COD2_1_3
static const int varpub_offset = 0x08397500;
#endif

#if COD_VERSION == COD2_1_0
static const int vmpub_offset = 0x083D7600;
#elif COD_VERSION == COD2_1_2
static const int vmpub_offset = 0x083D7A00;
#elif COD_VERSION == COD2_1_3
static const int vmpub_offset = 0x083D8A80;
#endif

#define scrVarPub (*((scrVarPub_t*)( varpub_offset )))
#define scrVmPub (*((scrVmPub_t*)( vmpub_offset )))

#if COD_VERSION == COD2_1_0
static const int playerStates = 0x086F1480;
static const int sizeOfPlayer = 0x28A4;
#elif COD_VERSION == COD2_1_2
static const int playerStates = 0x08705480;
static const int sizeOfPlayer = 0x28A4;
#elif COD_VERSION == COD2_1_3
static const int playerStates = 0x087a2500;
static const int sizeOfPlayer = 0x28A4;
#endif

#if COD_VERSION == COD2_1_0
static const int gentities = 0x08665480;
static const int gentities_size = 560;
#elif COD_VERSION == COD2_1_2
static const int gentities = 0x08679380;
static const int gentities_size = 560;
#elif COD_VERSION == COD2_1_3
static const int gentities = 0x08716400;
static const int gentities_size = 560;
#endif

#if COD_VERSION == COD2_1_0
static const int playerinfo_base = 0x0841FB0C;
static const int playerinfo_size = 0x78F14;
#elif COD_VERSION == COD2_1_2
static const int playerinfo_base = 0x0842200C;
static const int playerinfo_size = 0x79064;
#elif COD_VERSION == COD2_1_3
static const int playerinfo_base = 0x0842308C;
static const int playerinfo_size = 0xB1064;
#endif

#define PLAYERBASE(playerid) (*(int *)(playerinfo_base) + playerid * playerinfo_size)
#define PLAYERSTATE(playerid) (playerStates + playerid * sizeOfPlayer)
#define G_ENTITY(playerid) (gentities + gentities_size * playerid)

#define VALID_ENTITY(entity) (*(int *)(entity + 1))

#define PLAYERBASE_ID(address) ((address - *(int *)playerinfo_base) / playerinfo_size)
#define PLAYERSTATE_ID(address) ((address - playerStates) / sizeOfPlayer)
#define G_ENTITY_ID(address) ((address - gentities) / gentities_size)

#if COD_VERSION == COD2_1_0
static const int addresstype_offset = 0x6E5C4;
#elif COD_VERSION == COD2_1_2
static const int addresstype_offset = 0x6E6D4;
#elif COD_VERSION == COD2_1_3
static const int addresstype_offset = 0x6E6D4;
#endif

#define ADDRESSTYPE(playerid) (*(int *)(PLAYERBASE(playerid) + addresstype_offset))
#define CLIENTSTATE(playerid) (*(int *)(PLAYERBASE(playerid)))

#if COD_VERSION == COD2_1_0
static const int svstime_offset = 0x0841FB04;
#elif COD_VERSION == COD2_1_2
static const int svstime_offset = 0x08422004;
#elif COD_VERSION == COD2_1_3
static const int svstime_offset = 0x08423084;
#endif

#define SVS_TIME (*(int *)svstime_offset)

#define stackPushUndefined Scr_AddUndefined
#define stackPushInt Scr_AddInt
#define stackPushFloat Scr_AddFloat
#define stackPushString Scr_AddString
#define stackPushVector Scr_AddVector
#define stackPushEntity Scr_AddEntity
#define stackPushArray Scr_MakeArray
#define stackPushArrayLast Scr_AddArray

void stackError(char *format, ...);

char *stackGetParamTypeAsString(int param);

int stackGetParamInt(int param, int *value);
int stackGetParamFunction(int param, int *value);
int stackGetParamString(int param, char **value);
int stackGetParamVector(int param, float value[3]);
int stackGetParamFloat(int param, float *value);
int stackGetParamType(int param);
int stackGetParams(char *params, ...);

xfunction_t Scr_GetCustomFunction(const char **fname, int *fdev);
xmethod_t Scr_GetCustomMethod(const char **fname, int *fdev);

#ifdef __cplusplus
}
#endif

#endif