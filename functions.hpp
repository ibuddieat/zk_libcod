#ifndef _FUNCTIONS_HPP_
#define _FUNCTIONS_HPP_

/* MAKE FUNCTIONS STATIC, SO THEY CAN BE IN EVERY FILE */

typedef int (*GetVariableName_t)(int a1);
#if COD_VERSION == COD2_1_0
static const GetVariableName_t GetVariableName = (GetVariableName_t)0x0807CA72;
#elif COD_VERSION == COD2_1_2
static const GetVariableName_t GetVariableName = (GetVariableName_t)0x0807CFF6;
#elif COD_VERSION == COD2_1_3
static const GetVariableName_t GetVariableName = (GetVariableName_t)0x0807D0C2;
#endif

typedef int (*GetNextVariable_t)(int a1);
#if COD_VERSION == COD2_1_0
static const GetNextVariable_t GetNextVariable = (GetNextVariable_t)0x0807C9CE;
#elif COD_VERSION == COD2_1_2
static const GetNextVariable_t GetNextVariable = (GetNextVariable_t)0x0807CF52;
#elif COD_VERSION == COD2_1_3
static const GetNextVariable_t GetNextVariable = (GetNextVariable_t)0x0807D01E;
#endif

typedef int (*GetArraySize_t)(int a1);
#if COD_VERSION == COD2_1_0
static const GetArraySize_t GetArraySize = (GetArraySize_t)0x0807C9AC;
#elif COD_VERSION == COD2_1_2
static const GetArraySize_t GetArraySize = (GetArraySize_t)0x0807CF30;
#elif COD_VERSION == COD2_1_3
static const GetArraySize_t GetArraySize = (GetArraySize_t)0x0807CFFC;
#endif

typedef char * (*SL_ConvertToString_t)(unsigned int index);
#if COD_VERSION == COD2_1_0
static const SL_ConvertToString_t SL_ConvertToString = (SL_ConvertToString_t)0x08078896;
#elif COD_VERSION == COD2_1_2
static const SL_ConvertToString_t SL_ConvertToString = (SL_ConvertToString_t)0x08078E1A;
#elif COD_VERSION == COD2_1_3
static const SL_ConvertToString_t SL_ConvertToString = (SL_ConvertToString_t)0x08078EE6;
#endif

typedef int (*Scr_GetFunctionHandle_t)(const char* scriptName, const char* labelName, int isNeeded);
#if COD_VERSION == COD2_1_0
static const Scr_GetFunctionHandle_t Scr_GetFunctionHandle = (Scr_GetFunctionHandle_t)0x0810DD70;
#elif COD_VERSION == COD2_1_2
static const Scr_GetFunctionHandle_t Scr_GetFunctionHandle = (Scr_GetFunctionHandle_t)0x081100AC;
#elif COD_VERSION == COD2_1_3
static const Scr_GetFunctionHandle_t Scr_GetFunctionHandle = (Scr_GetFunctionHandle_t)0x08110208;
#endif

typedef char * (*Cmd_Argv_t)(int arg);
#if COD_VERSION == COD2_1_0
static const Cmd_Argv_t Cmd_Argv = (Cmd_Argv_t)0x0806001C;
#elif COD_VERSION == COD2_1_2
static const Cmd_Argv_t Cmd_Argv = (Cmd_Argv_t)0x08060228;
#elif COD_VERSION == COD2_1_3
static const Cmd_Argv_t Cmd_Argv = (Cmd_Argv_t)0x08060220;
#endif

typedef int (*trap_Argc_t)();
#if COD_VERSION == COD2_1_0
static const trap_Argc_t trap_Argc = (trap_Argc_t)0x0805FFDC;
#elif COD_VERSION == COD2_1_2
static const trap_Argc_t trap_Argc = (trap_Argc_t)0x080601E8;
#elif COD_VERSION == COD2_1_3
static const trap_Argc_t trap_Argc = (trap_Argc_t)0x080601E0;
#endif

typedef int (*Scr_GetNumParam_t)();
#if COD_VERSION == COD2_1_0
static const Scr_GetNumParam_t Scr_GetNumParam = (Scr_GetNumParam_t)0x08084AEE;
#elif COD_VERSION == COD2_1_2
static const Scr_GetNumParam_t Scr_GetNumParam = (Scr_GetNumParam_t)0x0808506A;
#elif COD_VERSION == COD2_1_3
static const Scr_GetNumParam_t Scr_GetNumParam = (Scr_GetNumParam_t)0x08085136;
#endif

typedef void (*trap_Argv_t)(unsigned int param, char *buf, int bufLen);
#if COD_VERSION == COD2_1_0
static const trap_Argv_t trap_Argv = (trap_Argv_t)0x08060074;
#elif COD_VERSION == COD2_1_2
static const trap_Argv_t trap_Argv = (trap_Argv_t)0x08060280;
#elif COD_VERSION == COD2_1_3
static const trap_Argv_t trap_Argv = (trap_Argv_t)0x08060278;
#endif

typedef void (*Com_Printf_t)(const char *format, ...);
#if COD_VERSION == COD2_1_0
static const Com_Printf_t Com_Printf = (Com_Printf_t)0x08060B2C;
#elif COD_VERSION == COD2_1_2
static const Com_Printf_t Com_Printf = (Com_Printf_t)0x08060DF2;
#elif COD_VERSION == COD2_1_3
static const Com_Printf_t Com_Printf = (Com_Printf_t)0x08060DEA;
#endif

typedef void (*Com_DPrintf_t)(const char *format, ...);
#if COD_VERSION == COD2_1_0
static const Com_DPrintf_t Com_DPrintf = (Com_DPrintf_t)0x08060B7C;
#elif COD_VERSION == COD2_1_2
static const Com_DPrintf_t Com_DPrintf = (Com_DPrintf_t)0x08060E42;
#elif COD_VERSION == COD2_1_3
static const Com_DPrintf_t Com_DPrintf = (Com_DPrintf_t)0x08060E3A;
#endif

typedef int (*Com_sprintf_t)(char *dest, int size, const char *format, ...);
#if COD_VERSION == COD2_1_0
static const Com_sprintf_t Com_sprintf = (Com_sprintf_t)0x080B5932;
#elif COD_VERSION == COD2_1_2
static const Com_sprintf_t Com_sprintf = (Com_sprintf_t)0x080B7DC6;
#elif COD_VERSION == COD2_1_3
static const Com_sprintf_t Com_sprintf = (Com_sprintf_t)0x080B7F0A;
#endif

typedef void (*Cmd_ExecuteString_t)(const char *cmd);
#if COD_VERSION == COD2_1_0
static const Cmd_ExecuteString_t Cmd_ExecuteString = (Cmd_ExecuteString_t)0x08060754;
#elif COD_VERSION == COD2_1_2
static const Cmd_ExecuteString_t Cmd_ExecuteString = (Cmd_ExecuteString_t)0x080609D4;
#elif COD_VERSION == COD2_1_3
static const Cmd_ExecuteString_t Cmd_ExecuteString = (Cmd_ExecuteString_t)0x080609CC;
#endif

typedef void (*ClientCommand_t)(int clientNum);
#if COD_VERSION == COD2_1_0
static const ClientCommand_t ClientCommand = (ClientCommand_t)0x080FE998;
#elif COD_VERSION == COD2_1_2
static const ClientCommand_t ClientCommand = (ClientCommand_t)0x08100D1E;
#elif COD_VERSION == COD2_1_3
static const ClientCommand_t ClientCommand = (ClientCommand_t)0x08100E62;
#endif

typedef int (*FS_ReadFile_t)(const char *qpath, void **buffer);
#if COD_VERSION == COD2_1_0
static const FS_ReadFile_t FS_ReadFile = (FS_ReadFile_t)0x0809E892;
#elif COD_VERSION == COD2_1_2
static const FS_ReadFile_t FS_ReadFile = (FS_ReadFile_t)0x080A0958;
#elif COD_VERSION == COD2_1_3
static const FS_ReadFile_t FS_ReadFile = (FS_ReadFile_t)0x080A0A9C;
#endif

typedef void (*FS_LoadDir_t)(char *path, char *dir);
#if COD_VERSION == COD2_1_0
static const FS_LoadDir_t FS_LoadDir = (FS_LoadDir_t)0x080A01A4;
#elif COD_VERSION == COD2_1_2
static const FS_LoadDir_t FS_LoadDir = (FS_LoadDir_t)0x080A22D8;
#elif COD_VERSION == COD2_1_3
static const FS_LoadDir_t FS_LoadDir = (FS_LoadDir_t)0x080A241C;
#endif

typedef enum
{
	CS_FREE,		// can be reused for a new connection
	CS_ZOMBIE,		// client has been disconnected, but don't reuse connection for a couple seconds
	CS_CONNECTED,	// has been assigned to a client_t, but no gamestate yet
	CS_PRIMED,		// gamestate has been sent, but client hasn't sent a usercmd
	CS_ACTIVE		// client is fully in game
} clientState_t;

typedef enum
{
	NA_BOT = 0,
	NA_BAD = 0,
	NA_LOOPBACK = 2,
	NA_BROADCAST = 3,
	NA_IP = 4,
	NA_IPX = 5,
	NA_BROADCAST_IPX = 6
} netadrtype_t;

typedef struct
{
	netadrtype_t type;
	unsigned char ip[4];
	char ipx[10];
	unsigned short port;
} netadr_t;

typedef enum
{
	NS_CLIENT,
	NS_SERVER
} netsrc_t;

typedef struct
{
	bool overflowed; // 0
	char *data; // 4
	int maxsize; // 8
	int cursize; // 12
	int readcount; // 16
	int bit;
} msg_t; // 0x18

typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];

typedef unsigned char byte;

typedef union
{
	int i;
	byte rgba[4];
} ucolor_t;

typedef struct cvar_s
{
	char *name;
	char *description;
	short int flags;
	byte type;
	byte modified;
	union
	{
		float floatval;
		float value;
		int integer;
		char* string;
		byte boolean;
		vec2_t vec2;
		vec3_t vec3;
		vec4_t vec4;
		ucolor_t color;
	};
	union
	{
		float latchedFloatval;
		int latchedInteger;
		char* latchedString;
		byte latchedBoolean;
		vec2_t latchedVec2;
		vec3_t latchedVec3;
		vec4_t latchedVec4;
		ucolor_t latchedColor;
	};
	union
	{
		float resetFloatval;
		int resetInteger;
		char* resetString;
		byte resetBoolean;
		vec2_t resetVec2;
		vec3_t resetVec3;
		vec4_t resetVec4;
		ucolor_t resetColor;
	};
	union
	{
		int imin;
		float fmin;
	};
	union
	{
		int imax;
		float fmax;
		const char** enumStr;
	};
	struct cvar_s *next;
	struct cvar_s *hashNext;
} cvar_t;

//defines Cvarflags
#define	CVAR_ARCHIVE		1	// set to cause it to be saved to vars.rc
// used for system variables, not for player
// specific configurations
#define	CVAR_USERINFO		2	// sent to server on connect or change
#define	CVAR_SERVERINFO		4	// sent in response to front end requests
#define	CVAR_SYSTEMINFO		8	// these cvars will be duplicated on all clients
#define	CVAR_INIT		16	// don't allow change from console at all,
// but can be set from the command line
#define	CVAR_LATCH		32	// will only change when C code next does
// a Cvar_Get(), so it can't be changed
// without proper initialization.  modified
// will be set, even though the value hasn't
// changed yet
#define	CVAR_ROM		64	// display only, cannot be set by user at all
#define CVAR_CHEAT		128	// can not be changed if cheats are disabled
#define	CVAR_TEMP		256	// can be set even when cheats are disabled, but is not archived
#define CVAR_NORESTART		1024	// do not clear when a cvar_restart is issued
#define	CVAR_USER_CREATED	16384	// created by a set command

struct VariableStackBuffer
{
	const char *pos;
	u_int16_t size;
	u_int16_t bufLen;
	u_int16_t localId;
	char time;
	char buf[1];
};

union VariableUnion
{
	int intValue;
	float floatValue;
	unsigned int stringValue;
	const float *vectorValue;
	const char *codePosValue;
	unsigned int pointerValue;
	struct VariableStackBuffer *stackValue;
	unsigned int entityOffset;
};

typedef struct
{
	union VariableUnion u;
	int type;
} VariableValue;

typedef struct
{
	const char *fieldBuffer;
	u_int16_t canonicalStrCount;
	byte developer;
	byte developer_script;
	byte evaluate;
	byte pad[3];
	const char *error_message;
	int error_index;
	unsigned int time;
	unsigned int timeArrayId;
	unsigned int pauseArrayId;
	unsigned int levelId;
	unsigned int gameId;
	unsigned int animId;
	unsigned int freeEntList;
	unsigned int tempVariable;
	byte bInited;
	byte pad2;
	u_int16_t savecount;
	unsigned int checksum;
	unsigned int entId;
	unsigned int entFieldName;
	struct HunkUser *programHunkUser;
	const char *programBuffer;
	const char *endScriptBuffer;
	u_int16_t saveIdMap[24574];
	u_int16_t saveIdMapRev[24574];
} scrVarPub_t;

struct function_stack_t
{
	const char *pos;
	unsigned int localId;
	unsigned int localVarCount;
	VariableValue *top;
	VariableValue *startTop;
};

struct function_frame_t
{
	struct function_stack_t fs;
	int topType;
};

typedef struct
{
	unsigned int *localVars;
	VariableValue *maxstack;
	int function_count;
	struct function_frame_t *function_frame;
	VariableValue *top;
	byte debugCode;
	byte abort_on_error;
	byte terminal_error;
	byte pad;
	unsigned int inparamcount;
	unsigned int outparamcount;
	struct function_frame_t function_frame_start[32];
	VariableValue stack[2048];
} scrVmPub_t;

typedef int	fileHandle_t;

typedef void (*xfunction_t)();

typedef struct scr_function_s
{
	char			*name;
	xfunction_t		call;
	int				developer;
} scr_function_t;

typedef void (*xmethod_t)(int);

typedef struct scr_method_s
{
	char			*name;
	xmethod_t		call;
	int				developer;
} scr_method_t;

typedef enum
{
	EV_NONE = 0,
	EV_FOOTSTEP_RUN_DEFAULT,
	EV_FOOTSTEP_RUN_BARK,
	EV_FOOTSTEP_RUN_BRICK,
	EV_FOOTSTEP_RUN_CARPET,
	EV_FOOTSTEP_RUN_CLOTH,
	EV_FOOTSTEP_RUN_CONCRETE,
	EV_FOOTSTEP_RUN_DIRT,
	EV_FOOTSTEP_RUN_FLESH,
	EV_FOOTSTEP_RUN_FOLIAGE,
	EV_FOOTSTEP_RUN_GLASS,
	EV_FOOTSTEP_RUN_GRASS,
	EV_FOOTSTEP_RUN_GRAVEL,
	EV_FOOTSTEP_RUN_ICE,
	EV_FOOTSTEP_RUN_METAL,
	EV_FOOTSTEP_RUN_MUD,
	EV_FOOTSTEP_RUN_PAPER,
	EV_FOOTSTEP_RUN_PLASTER,
	EV_FOOTSTEP_RUN_ROCK,
	EV_FOOTSTEP_RUN_SAND,
	EV_FOOTSTEP_RUN_SNOW,
	EV_FOOTSTEP_RUN_WATER,
	EV_FOOTSTEP_RUN_WOOD,
	EV_FOOTSTEP_RUN_ASPHALT,
	EV_FOOTSTEP_WALK_DEFAULT,
	EV_FOOTSTEP_WALK_BARK,
	EV_FOOTSTEP_WALK_BRICK,
	EV_FOOTSTEP_WALK_CARPET,
	EV_FOOTSTEP_WALK_CLOTH,
	EV_FOOTSTEP_WALK_CONCRETE,
	EV_FOOTSTEP_WALK_DIRT,
	EV_FOOTSTEP_WALK_FLESH,
	EV_FOOTSTEP_WALK_FOLIAGE,
	EV_FOOTSTEP_WALK_GLASS,
	EV_FOOTSTEP_WALK_GRASS,
	EV_FOOTSTEP_WALK_GRAVEL,
	EV_FOOTSTEP_WALK_ICE,
	EV_FOOTSTEP_WALK_METAL,
	EV_FOOTSTEP_WALK_MUD,
	EV_FOOTSTEP_WALK_PAPER,
	EV_FOOTSTEP_WALK_PLASTER,
	EV_FOOTSTEP_WALK_ROCK,
	EV_FOOTSTEP_WALK_SAND,
	EV_FOOTSTEP_WALK_SNOW,
	EV_FOOTSTEP_WALK_WATER,
	EV_FOOTSTEP_WALK_WOOD,
	EV_FOOTSTEP_WALK_ASPHALT,
	EV_FOOTSTEP_PRONE_DEFAULT,
	EV_FOOTSTEP_PRONE_BARK,
	EV_FOOTSTEP_PRONE_BRICK,
	EV_FOOTSTEP_PRONE_CARPET,
	EV_FOOTSTEP_PRONE_CLOTH,
	EV_FOOTSTEP_PRONE_CONCRETE,
	EV_FOOTSTEP_PRONE_DIRT,
	EV_FOOTSTEP_PRONE_FLESH,
	EV_FOOTSTEP_PRONE_FOLIAGE,
	EV_FOOTSTEP_PRONE_GLASS,
	EV_FOOTSTEP_PRONE_GRASS,
	EV_FOOTSTEP_PRONE_GRAVEL,
	EV_FOOTSTEP_PRONE_ICE,
	EV_FOOTSTEP_PRONE_METAL,
	EV_FOOTSTEP_PRONE_MUD,
	EV_FOOTSTEP_PRONE_PAPER,
	EV_FOOTSTEP_PRONE_PLASTER,
	EV_FOOTSTEP_PRONE_ROCK,
	EV_FOOTSTEP_PRONE_SAND,
	EV_FOOTSTEP_PRONE_SNOW,
	EV_FOOTSTEP_PRONE_WATER,
	EV_FOOTSTEP_PRONE_WOOD,
	EV_FOOTSTEP_PRONE_ASPHALT,
	EV_JUMP_DEFAULT,
	EV_JUMP_BARK,
	EV_JUMP_BRICK,
	EV_JUMP_CARPET,
	EV_JUMP_CLOTH,
	EV_JUMP_CONCRETE,
	EV_JUMP_DIRT,
	EV_JUMP_FLESH,
	EV_JUMP_FOLIAGE,
	EV_JUMP_GLASS,
	EV_JUMP_GRASS,
	EV_JUMP_GRAVEL,
	EV_JUMP_ICE,
	EV_JUMP_METAL,
	EV_JUMP_MUD,
	EV_JUMP_PAPER,
	EV_JUMP_PLASTER,
	EV_JUMP_ROCK,
	EV_JUMP_SAND,
	EV_JUMP_SNOW,
	EV_JUMP_WATER,
	EV_JUMP_WOOD,
	EV_JUMP_ASPHALT,
	EV_LANDING_DEFAULT,
	EV_LANDING_BARK,
	EV_LANDING_BRICK,
	EV_LANDING_CARPET,
	EV_LANDING_CLOTH,
	EV_LANDING_CONCRETE,
	EV_LANDING_DIRT,
	EV_LANDING_FLESH,
	EV_LANDING_FOLIAGE,
	EV_LANDING_GLASS,
	EV_LANDING_GRASS,
	EV_LANDING_GRAVEL,
	EV_LANDING_ICE,
	EV_LANDING_METAL,
	EV_LANDING_MUD,
	EV_LANDING_PAPER,
	EV_LANDING_PLASTER,
	EV_LANDING_ROCK,
	EV_LANDING_SAND,
	EV_LANDING_SNOW,
	EV_LANDING_WATER,
	EV_LANDING_WOOD,
	EV_LANDING_ASPHALT,
	EV_LANDING_PAIN_DEFAULT,
	EV_LANDING_PAIN_BARK,
	EV_LANDING_PAIN_BRICK,
	EV_LANDING_PAIN_CARPET,
	EV_LANDING_PAIN_CLOTH,
	EV_LANDING_PAIN_CONCRETE,
	EV_LANDING_PAIN_DIRT,
	EV_LANDING_PAIN_FLESH,
	EV_LANDING_PAIN_FOLIAGE,
	EV_LANDING_PAIN_GLASS,
	EV_LANDING_PAIN_GRASS,
	EV_LANDING_PAIN_GRAVEL,
	EV_LANDING_PAIN_ICE,
	EV_LANDING_PAIN_METAL,
	EV_LANDING_PAIN_MUD,
	EV_LANDING_PAIN_PAPER,
	EV_LANDING_PAIN_PLASTER,
	EV_LANDING_PAIN_ROCK,
	EV_LANDING_PAIN_SAND,
	EV_LANDING_PAIN_SNOW,
	EV_LANDING_PAIN_WATER,
	EV_LANDING_PAIN_WOOD,
	EV_LANDING_PAIN_ASPHALT,
	EV_FOLIAGE_SOUND,
	EV_STANCE_FORCE_STAND,
	EV_STANCE_FORCE_CROUCH,
	EV_STANCE_FORCE_PRONE,
	EV_STEP_VIEW,
	EV_ITEM_PICKUP,
	EV_AMMO_PICKUP,
	EV_NOAMMO,
	EV_EMPTYCLIP,
	EV_EMPTY_OFFHAND,
	EV_RESET_ADS,
	EV_RELOAD,
	EV_RELOAD_FROM_EMPTY,
	EV_RELOAD_START,
	EV_RELOAD_END,
	EV_RAISE_WEAPON,
	EV_PUTAWAY_WEAPON,
	EV_WEAPON_ALT,
	EV_PULLBACK_WEAPON,
	EV_FIRE_WEAPON,
	EV_FIRE_WEAPONB,
	EV_FIRE_WEAPON_LASTSHOT,
	EV_RECHAMBER_WEAPON,
	EV_EJECT_BRASS,
	EV_MELEE_SWIPE,
	EV_FIRE_MELEE,
	EV_PREP_OFFHAND,
	EV_USE_OFFHAND,
	EV_SWITCH_OFFHAND,
	EV_BINOCULAR_ENTER,
	EV_BINOCULAR_EXIT,
	EV_BINOCULAR_FIRE,
	EV_BINOCULAR_RELEASE,
	EV_BINOCULAR_DROP,
	EV_MELEE_HIT,
	EV_MELEE_MISS,
	EV_FIRE_WEAPON_MG42,
	EV_FIRE_QUADBARREL_1,
	EV_FIRE_QUADBARREL_2,
	EV_BULLET_TRACER,
	EV_SOUND_ALIAS,
	EV_SOUND_ALIAS_AS_MASTER,
	EV_BULLET_HIT_SMALL,
	EV_BULLET_HIT_LARGE,
	EV_SHOTGUN_HIT,
	EV_BULLET_HIT_AP,
	EV_BULLET_HIT_CLIENT_SMALL,
	EV_BULLET_HIT_CLIENT_LARGE,
	EV_GRENADE_BOUNCE,
	EV_GRENADE_EXPLODE,
	EV_ROCKET_EXPLODE,
	EV_ROCKET_EXPLODE_NOMARKS,
	EV_CUSTOM_EXPLODE,
	EV_CUSTOM_EXPLODE_NOMARKS,
	EV_BULLET,
	EV_PLAY_FX,
	EV_PLAY_FX_ON_TAG,
	EV_EARTHQUAKE,
	EV_GRENADE_SUICIDE,
	EV_OBITUARY
} entity_event_t;

typedef enum
{
	TRACE_HITTYPE_NONE = 0x0,
	TRACE_HITTYPE_ENTITY = 0x1,
	TRACE_HITTYPE_DYNENT_MODEL = 0x2,
	TRACE_HITTYPE_DYNENT_BRUSH = 0x3,
	TRACE_HITTYPE_GLASS = 0x4
} TraceHitType;

typedef struct trace_s
{
	float fraction;
	vec3_t normal;
	int surfaceFlags;
	int contents;
	const char *material;
	TraceHitType hitType;
	u_int16_t hitId;
	u_int16_t modelIndex;
	u_int16_t partName;
	u_int16_t partGroup;
	byte allsolid;
	byte startsolid;
	byte walkable;
	byte padding;
} trace_t;

typedef struct leakyBucket_s leakyBucket_t;
struct leakyBucket_s
{
	netadrtype_t type;
	unsigned char adr[4];
	int	lastTime;
	signed char	burst;
	long hash;

	leakyBucket_t *prev, *next;
};

typedef xfunction_t (*Scr_GetFunction_t)(const char** v_function, int* v_developer);
#if COD_VERSION == COD2_1_0
static const Scr_GetFunction_t Scr_GetFunction = (Scr_GetFunction_t)0x08115824;
#elif COD_VERSION == COD2_1_2
static const Scr_GetFunction_t Scr_GetFunction = (Scr_GetFunction_t)0x08117B56;
#elif COD_VERSION == COD2_1_3
static const Scr_GetFunction_t Scr_GetFunction = (Scr_GetFunction_t)0x08117CB2;
#endif

typedef xmethod_t (*Scr_GetMethod_t)(const char** v_method, int* v_developer);
#if COD_VERSION == COD2_1_0
static const Scr_GetMethod_t Scr_GetMethod = (Scr_GetMethod_t)0x0811595C;
#elif COD_VERSION == COD2_1_2
static const Scr_GetMethod_t Scr_GetMethod = (Scr_GetMethod_t)0x08117C8E;
#elif COD_VERSION == COD2_1_3
static const Scr_GetMethod_t Scr_GetMethod = (Scr_GetMethod_t)0x08117DEA;
#endif

typedef cvar_t* (*Cvar_FindVar_t)(const char *var_name);
#if COD_VERSION == COD2_1_0
static const Cvar_FindVar_t Cvar_FindVar = (Cvar_FindVar_t)0x080B0AE4;
#elif COD_VERSION == COD2_1_2
static const Cvar_FindVar_t Cvar_FindVar = (Cvar_FindVar_t)0x080B2D94;
#elif COD_VERSION == COD2_1_3
static const Cvar_FindVar_t Cvar_FindVar = (Cvar_FindVar_t)0x080B2ED8;
#endif

typedef cvar_t* (*Cvar_RegisterBool_t)(const char *var_name, bool var_value, unsigned short flags);
#if COD_VERSION == COD2_1_0
static const Cvar_RegisterBool_t Cvar_RegisterBool = (Cvar_RegisterBool_t)0x080B1B72;
#elif COD_VERSION == COD2_1_2
static const Cvar_RegisterBool_t Cvar_RegisterBool = (Cvar_RegisterBool_t)0x080B3E8E;
#elif COD_VERSION == COD2_1_3
static const Cvar_RegisterBool_t Cvar_RegisterBool = (Cvar_RegisterBool_t)0x080B3FD2;
#endif

typedef cvar_t* (*Cvar_RegisterString_t)(const char *var_name, const char *var_value, unsigned short flags);
#if COD_VERSION == COD2_1_0
static const Cvar_RegisterString_t Cvar_RegisterString = (Cvar_RegisterString_t)0x080B1DD2;
#elif COD_VERSION == COD2_1_2
static const Cvar_RegisterString_t Cvar_RegisterString = (Cvar_RegisterString_t)0x080B40EE;
#elif COD_VERSION == COD2_1_3
static const Cvar_RegisterString_t Cvar_RegisterString = (Cvar_RegisterString_t)0x080B4232;
#endif

typedef void (*SV_ConnectionlessPacket_t)(netadr_t from, msg_t *msg);
#if COD_VERSION == COD2_1_0
static const SV_ConnectionlessPacket_t SV_ConnectionlessPacket = (SV_ConnectionlessPacket_t)0x08093F1E;
#elif COD_VERSION == COD2_1_2
static const SV_ConnectionlessPacket_t SV_ConnectionlessPacket = (SV_ConnectionlessPacket_t)0x08095894;
#elif COD_VERSION == COD2_1_3
static const SV_ConnectionlessPacket_t SV_ConnectionlessPacket = (SV_ConnectionlessPacket_t)0x0809594E;
#endif

typedef void (*NET_OutOfBandPrint_t)(netsrc_t net_socket, netadr_t adr, const char *format, ...);
#if COD_VERSION == COD2_1_0
static const NET_OutOfBandPrint_t NET_OutOfBandPrint = (NET_OutOfBandPrint_t)0x0806C40C;
#elif COD_VERSION == COD2_1_2
static const NET_OutOfBandPrint_t NET_OutOfBandPrint = (NET_OutOfBandPrint_t)0x0806C8D4;
#elif COD_VERSION == COD2_1_3
static const NET_OutOfBandPrint_t NET_OutOfBandPrint = (NET_OutOfBandPrint_t)0x0806C8CC;
#endif

typedef void (*SV_GameSendServerCommand_t)(int clientnum, int svscmd_type, const char *text);
#if COD_VERSION == COD2_1_0
static const SV_GameSendServerCommand_t SV_GameSendServerCommand = (SV_GameSendServerCommand_t)0x0808FE96;
#elif COD_VERSION == COD2_1_2
static const SV_GameSendServerCommand_t SV_GameSendServerCommand = (SV_GameSendServerCommand_t)0x080916A6;
#elif COD_VERSION == COD2_1_3
static const SV_GameSendServerCommand_t SV_GameSendServerCommand = (SV_GameSendServerCommand_t)0x080917AA;
#endif

typedef void (*SV_DropClient_t)(int a1, char* reason);
#if COD_VERSION == COD2_1_0
static const SV_DropClient_t SV_DropClient = (SV_DropClient_t)0x0808DC8C;
#elif COD_VERSION == COD2_1_2
static const SV_DropClient_t SV_DropClient = (SV_DropClient_t)0x0808EF9A;
#elif COD_VERSION == COD2_1_3
static const SV_DropClient_t SV_DropClient = (SV_DropClient_t)0x0808F02E;
#endif

typedef void (*ClientUserinfoChanged_t)(int a1);
#if COD_VERSION == COD2_1_0
static const ClientUserinfoChanged_t ClientUserinfoChanged = (ClientUserinfoChanged_t)0x080F6506;
#elif COD_VERSION == COD2_1_2
static const ClientUserinfoChanged_t ClientUserinfoChanged = (ClientUserinfoChanged_t)0x080F8B1A;
#elif COD_VERSION == COD2_1_3
static const ClientUserinfoChanged_t ClientUserinfoChanged = (ClientUserinfoChanged_t)0x080F8C5E;
#endif

typedef int (*SV_ClientHasClientMuted_t)(int a1, int a2);
#if COD_VERSION == COD2_1_0
static const SV_ClientHasClientMuted_t SV_ClientHasClientMuted = (SV_ClientHasClientMuted_t)0x0809A01E;
#elif COD_VERSION == COD2_1_2
static const SV_ClientHasClientMuted_t SV_ClientHasClientMuted = (SV_ClientHasClientMuted_t)0x0809C0B6;
#elif COD_VERSION == COD2_1_3
static const SV_ClientHasClientMuted_t SV_ClientHasClientMuted = (SV_ClientHasClientMuted_t)0x0809C1FA;
#endif

typedef void (*Info_SetValueForKey_t)(char *s, const char *key, const char *value);
#if COD_VERSION == COD2_1_0
static const Info_SetValueForKey_t Info_SetValueForKey = (Info_SetValueForKey_t)0x080B5FF6;
#elif COD_VERSION == COD2_1_2
static const Info_SetValueForKey_t Info_SetValueForKey = (Info_SetValueForKey_t)0x080B848A;
#elif COD_VERSION == COD2_1_3
static const Info_SetValueForKey_t Info_SetValueForKey = (Info_SetValueForKey_t)0x080B85CE;
#endif

typedef char* (*Info_ValueForKey_t)(char *s, const char *key);
#if COD_VERSION == COD2_1_0
static const Info_ValueForKey_t Info_ValueForKey = (Info_ValueForKey_t)0x080B5B30;
#elif COD_VERSION == COD2_1_2
static const Info_ValueForKey_t Info_ValueForKey = (Info_ValueForKey_t)0x080B7FC4;
#elif COD_VERSION == COD2_1_3
static const Info_ValueForKey_t Info_ValueForKey = (Info_ValueForKey_t)0x080B8108;
#endif

typedef short (*Scr_ExecThread_t)(int callback, int params);
#if COD_VERSION == COD2_1_0
static const Scr_ExecThread_t Scr_ExecThread = (Scr_ExecThread_t)0x0808398E;
#elif COD_VERSION == COD2_1_2
static const Scr_ExecThread_t Scr_ExecThread = (Scr_ExecThread_t)0x08083F0A;
#elif COD_VERSION == COD2_1_3
static const Scr_ExecThread_t Scr_ExecThread = (Scr_ExecThread_t)0x08083FD6;
#endif

typedef short (*Scr_ExecEntThread_t)(int entity, int callback, int params);
#if COD_VERSION == COD2_1_0
static const Scr_ExecEntThread_t Scr_ExecEntThread = (Scr_ExecEntThread_t)0x08118DF4;
#elif COD_VERSION == COD2_1_2
static const Scr_ExecEntThread_t Scr_ExecEntThread = (Scr_ExecEntThread_t)0x0811B128;
#elif COD_VERSION == COD2_1_3
static const Scr_ExecEntThread_t Scr_ExecEntThread = (Scr_ExecEntThread_t)0x0811B284;
#endif

typedef short (*Scr_FreeThread_t)(short thread_id);
#if COD_VERSION == COD2_1_0
static const Scr_FreeThread_t Scr_FreeThread = (Scr_FreeThread_t)0x08083B8E;
#elif COD_VERSION == COD2_1_2
static const Scr_FreeThread_t Scr_FreeThread = (Scr_FreeThread_t)0x0808410A;
#elif COD_VERSION == COD2_1_3
static const Scr_FreeThread_t Scr_FreeThread = (Scr_FreeThread_t)0x080841D6;
#endif

typedef void (*SVC_RemoteCommand_t)(netadr_t from, msg_t *msg);
#if COD_VERSION == COD2_1_0
static const SVC_RemoteCommand_t SVC_RemoteCommand = (SVC_RemoteCommand_t)0x080951B4;
#elif COD_VERSION == COD2_1_2
static const SVC_RemoteCommand_t SVC_RemoteCommand = (SVC_RemoteCommand_t)0x080970CC;
#elif COD_VERSION == COD2_1_3
static const SVC_RemoteCommand_t SVC_RemoteCommand = (SVC_RemoteCommand_t)0x08097188;
#endif

typedef void (*SV_GetChallenge_t)(netadr_t from);
#if COD_VERSION == COD2_1_0
static const SV_GetChallenge_t SV_GetChallenge = (SV_GetChallenge_t)0x0808BE54;
#elif COD_VERSION == COD2_1_2
static const SV_GetChallenge_t SV_GetChallenge = (SV_GetChallenge_t)0x0808D0C2;
#elif COD_VERSION == COD2_1_3
static const SV_GetChallenge_t SV_GetChallenge = (SV_GetChallenge_t)0x0808D18E;
#endif

typedef void (*SVC_Info_t)(netadr_t from);
#if COD_VERSION == COD2_1_0
static const SVC_Info_t SVC_Info = (SVC_Info_t)0x08093980;
#elif COD_VERSION == COD2_1_2
static const SVC_Info_t SVC_Info = (SVC_Info_t)0x080952C4;
#elif COD_VERSION == COD2_1_3
static const SVC_Info_t SVC_Info = (SVC_Info_t)0x0809537C;
#endif

typedef void (*SVC_Status_t)(netadr_t from);
#if COD_VERSION == COD2_1_0
static const SVC_Status_t SVC_Status = (SVC_Status_t)0x08093288;
#elif COD_VERSION == COD2_1_2
static const SVC_Status_t SVC_Status = (SVC_Status_t)0x08094BCC;
#elif COD_VERSION == COD2_1_3
static const SVC_Status_t SVC_Status = (SVC_Status_t)0x08094C84;
#endif

typedef const char* (*NET_AdrToString_t)(netadr_t a);
#if COD_VERSION == COD2_1_0
static const NET_AdrToString_t NET_AdrToString = (NET_AdrToString_t)0x0806AD14;
#elif COD_VERSION == COD2_1_2
static const NET_AdrToString_t NET_AdrToString = (NET_AdrToString_t)0x0806B1DC;
#elif COD_VERSION == COD2_1_3
static const NET_AdrToString_t NET_AdrToString = (NET_AdrToString_t)0x0806B1D4;
#endif

typedef void (*Scr_Error_t)(const char *a1);
#if COD_VERSION == COD2_1_0
static const Scr_Error_t Scr_Error = (Scr_Error_t)0x08084DB4;
#elif COD_VERSION == COD2_1_2
static const Scr_Error_t Scr_Error = (Scr_Error_t)0x08085330;
#elif COD_VERSION == COD2_1_3
static const Scr_Error_t Scr_Error = (Scr_Error_t)0x080853FC;
#endif

typedef int (*Sys_MilliSeconds_t)(void);
#if COD_VERSION == COD2_1_0
static const Sys_MilliSeconds_t Sys_MilliSeconds = (Sys_MilliSeconds_t)0x080D3728;
#elif COD_VERSION == COD2_1_2
static const Sys_MilliSeconds_t Sys_MilliSeconds = (Sys_MilliSeconds_t)0x080D5C54;
#elif COD_VERSION == COD2_1_3
static const Sys_MilliSeconds_t Sys_MilliSeconds = (Sys_MilliSeconds_t)0x080D5D98;
#endif

typedef long double (*calc_player_speed_t)(int a1, int a2);
#if COD_VERSION == COD2_1_0
static const calc_player_speed_t calc_player_speed = (calc_player_speed_t)0x080DF534;
#elif COD_VERSION == COD2_1_2
static const calc_player_speed_t calc_player_speed = (calc_player_speed_t)0x080E1B14;
#elif COD_VERSION == COD2_1_3
static const calc_player_speed_t calc_player_speed = (calc_player_speed_t)0x080E1C58;
#endif

typedef void (*calc_client_speed_t)(int client);
#if COD_VERSION == COD2_1_0
static const calc_client_speed_t calc_client_speed = (calc_client_speed_t)0x0811FB7A;
#elif COD_VERSION == COD2_1_2
static const calc_client_speed_t calc_client_speed = (calc_client_speed_t)0x08121EAE;
#elif COD_VERSION == COD2_1_3
static const calc_client_speed_t calc_client_speed = (calc_client_speed_t)0x0812200A;
#endif

typedef int (*G_FindConfigstringIndex_t)(const char *a1, int a2, int a3, int a4, const char *a5);
#if COD_VERSION == COD2_1_0
static const G_FindConfigstringIndex_t G_FindConfigstringIndex = (G_FindConfigstringIndex_t)0x0811AE70;
#elif COD_VERSION == COD2_1_2
static const G_FindConfigstringIndex_t G_FindConfigstringIndex = (G_FindConfigstringIndex_t)0x0811D1A4;
#elif COD_VERSION == COD2_1_3
static const G_FindConfigstringIndex_t G_FindConfigstringIndex = (G_FindConfigstringIndex_t)0x0811D300;
#endif

typedef char * (*SV_GetConfigstringConst_t)(int index);
#if COD_VERSION == COD2_1_0
static const SV_GetConfigstringConst_t SV_GetConfigstringConst = (SV_GetConfigstringConst_t)0x08091108;
#elif COD_VERSION == COD2_1_2
static const SV_GetConfigstringConst_t SV_GetConfigstringConst = (SV_GetConfigstringConst_t)0x08092918;
#elif COD_VERSION == COD2_1_3
static const SV_GetConfigstringConst_t SV_GetConfigstringConst = (SV_GetConfigstringConst_t)0x08092A1C;
#endif

typedef void (*SV_SetConfigstring_t)(int index, const char *val);
#if COD_VERSION == COD2_1_0
static const SV_SetConfigstring_t SV_SetConfigstring = (SV_SetConfigstring_t)0x08090E6C;
#elif COD_VERSION == COD2_1_2
static const SV_SetConfigstring_t SV_SetConfigstring = (SV_SetConfigstring_t)0x0809267C;
#elif COD_VERSION == COD2_1_3
static const SV_SetConfigstring_t SV_SetConfigstring = (SV_SetConfigstring_t)0x08092780;
#endif

typedef void * (*Z_MallocInternal_t)(int size);
#if COD_VERSION == COD2_1_0
static const Z_MallocInternal_t Z_MallocInternal = (Z_MallocInternal_t)0x080A92FA;
#elif COD_VERSION == COD2_1_2
static const Z_MallocInternal_t Z_MallocInternal = (Z_MallocInternal_t)0x080AB51A;
#elif COD_VERSION == COD2_1_3
static const Z_MallocInternal_t Z_MallocInternal = (Z_MallocInternal_t)0x080AB65E;
#endif

typedef int (*FS_Read_t)(void *buffer, int len, fileHandle_t f);
#if COD_VERSION == COD2_1_0
static const FS_Read_t FS_Read = (FS_Read_t)0x0809E328;
#elif COD_VERSION == COD2_1_2
static const FS_Read_t FS_Read = (FS_Read_t)0x080A03E6;
#elif COD_VERSION == COD2_1_3
static const FS_Read_t FS_Read = (FS_Read_t)0x080A052A;
#endif

typedef void (*MSG_WriteByte_t)(msg_t *msg, int c);
#if COD_VERSION == COD2_1_0
static const MSG_WriteByte_t MSG_WriteByte = (MSG_WriteByte_t)0x08067B4C;
#elif COD_VERSION == COD2_1_2
static const MSG_WriteByte_t MSG_WriteByte = (MSG_WriteByte_t)0x08068014;
#elif COD_VERSION == COD2_1_3
static const MSG_WriteByte_t MSG_WriteByte = (MSG_WriteByte_t)0x0806800C;
#endif

typedef void (*MSG_WriteShort_t)(msg_t *msg, int c);
#if COD_VERSION == COD2_1_0
static const MSG_WriteShort_t MSG_WriteShort = (MSG_WriteShort_t)0x08067BDA;
#elif COD_VERSION == COD2_1_2
static const MSG_WriteShort_t MSG_WriteShort = (MSG_WriteShort_t)0x080680A2;
#elif COD_VERSION == COD2_1_3
static const MSG_WriteShort_t MSG_WriteShort = (MSG_WriteShort_t)0x0806809A;
#endif

typedef void (*MSG_WriteLong_t)(msg_t *msg, int c);
#if COD_VERSION == COD2_1_0
static const MSG_WriteLong_t MSG_WriteLong = (MSG_WriteLong_t)0x08067C2A;
#elif COD_VERSION == COD2_1_2
static const MSG_WriteLong_t MSG_WriteLong = (MSG_WriteLong_t)0x080680F2;
#elif COD_VERSION == COD2_1_3
static const MSG_WriteLong_t MSG_WriteLong = (MSG_WriteLong_t)0x080680EA;
#endif

typedef void (*MSG_WriteString_t)(msg_t *msg, const char *s);
#if COD_VERSION == COD2_1_0
static const MSG_WriteString_t MSG_WriteString = (MSG_WriteString_t)0x08067CE4;
#elif COD_VERSION == COD2_1_2
static const MSG_WriteString_t MSG_WriteString = (MSG_WriteString_t)0x080681AC;
#elif COD_VERSION == COD2_1_3
static const MSG_WriteString_t MSG_WriteString = (MSG_WriteString_t)0x080681A4;
#endif

typedef void (*MSG_WriteData_t)(msg_t *buf, const void *data, int length);
#if COD_VERSION == COD2_1_0
static const MSG_WriteData_t MSG_WriteData = (MSG_WriteData_t)0x08067B84;
#elif COD_VERSION == COD2_1_2
static const MSG_WriteData_t MSG_WriteData = (MSG_WriteData_t)0x0806804C;
#elif COD_VERSION == COD2_1_3
static const MSG_WriteData_t MSG_WriteData = (MSG_WriteData_t)0x08068044;
#endif

typedef long (*FS_SV_FOpenFileRead_t)(const char *filename, fileHandle_t *fp);
#if COD_VERSION == COD2_1_0
static const FS_SV_FOpenFileRead_t FS_SV_FOpenFileRead = (FS_SV_FOpenFileRead_t)0x08064100;
#elif COD_VERSION == COD2_1_2
static const FS_SV_FOpenFileRead_t FS_SV_FOpenFileRead = (FS_SV_FOpenFileRead_t)0x08064560;
#elif COD_VERSION == COD2_1_3
static const FS_SV_FOpenFileRead_t FS_SV_FOpenFileRead = (FS_SV_FOpenFileRead_t)0x08064558;
#endif

typedef int (*FS_iwIwd_t)(char *a1, char *a2);
#if COD_VERSION == COD2_1_0
static const FS_iwIwd_t FS_iwIwd = (FS_iwIwd_t)0x08064ECC;
#elif COD_VERSION == COD2_1_2
static const FS_iwIwd_t FS_iwIwd = (FS_iwIwd_t)0x0806532C;
#elif COD_VERSION == COD2_1_3
static const FS_iwIwd_t FS_iwIwd = (FS_iwIwd_t)0x08065324;
#endif

typedef signed int (*SV_WWWRedirectClient_t)(int a1, msg_t *msg);
#if COD_VERSION == COD2_1_0
static const SV_WWWRedirectClient_t SV_WWWRedirectClient = (SV_WWWRedirectClient_t)0x0; // Does not exist in 1.0
#elif COD_VERSION == COD2_1_2
static const SV_WWWRedirectClient_t SV_WWWRedirectClient = (SV_WWWRedirectClient_t)0x0808FB78;
#elif COD_VERSION == COD2_1_3
static const SV_WWWRedirectClient_t SV_WWWRedirectClient = (SV_WWWRedirectClient_t)0x0808FC0C;
#endif

typedef int (*BG_WeaponDefs_t)(int a1);
#if COD_VERSION == COD2_1_0
static const BG_WeaponDefs_t BG_WeaponDefs = (BG_WeaponDefs_t)0x080E9270;
#elif COD_VERSION == COD2_1_2
static const BG_WeaponDefs_t BG_WeaponDefs = (BG_WeaponDefs_t)0x080EB860;
#elif COD_VERSION == COD2_1_3
static const BG_WeaponDefs_t BG_WeaponDefs = (BG_WeaponDefs_t)0x080EB9A4;
#endif

typedef int (*BG_GetNumWeapons_t)();
#if COD_VERSION == COD2_1_0
static const BG_GetNumWeapons_t BG_GetNumWeapons = (BG_GetNumWeapons_t)0x080E9322;
#elif COD_VERSION == COD2_1_2
static const BG_GetNumWeapons_t BG_GetNumWeapons = (BG_GetNumWeapons_t)0x080EB912;
#elif COD_VERSION == COD2_1_3
static const BG_GetNumWeapons_t BG_GetNumWeapons = (BG_GetNumWeapons_t)0x080EBA56;
#endif

typedef signed int (*sub_80E9758_t)(int a1);
#if COD_VERSION == COD2_1_0
static const sub_80E9758_t sub_80E9758 = (sub_80E9758_t)0x080E9758;
#elif COD_VERSION == COD2_1_2
static const sub_80E9758_t sub_80E9758 = (sub_80E9758_t)0x080EBD48;
#elif COD_VERSION == COD2_1_3
static const sub_80E9758_t sub_80E9758 = (sub_80E9758_t)0x080EBE8C;
#endif

typedef int (*sub_80D9E84_t)(int a1, signed int a2);
#if COD_VERSION == COD2_1_0
static const sub_80D9E84_t sub_80D9E84 = (sub_80D9E84_t)0x080D9E84;
#elif COD_VERSION == COD2_1_2
static const sub_80D9E84_t sub_80D9E84 = (sub_80D9E84_t)0x080DC464;
#elif COD_VERSION == COD2_1_3
static const sub_80D9E84_t sub_80D9E84 = (sub_80D9E84_t)0x080DC5A8;
#endif

typedef void (*scriptError_t)(int a1, int a2, int a3, void *a4);
#if COD_VERSION == COD2_1_0
static const scriptError_t scriptError = (scriptError_t)0x08078282;
#elif COD_VERSION == COD2_1_2
static const scriptError_t scriptError = (scriptError_t)0x08078806;
#elif COD_VERSION == COD2_1_3
static const scriptError_t scriptError = (scriptError_t)0x080788D2;
#endif

typedef void (*runtimeError_t)(int a1, int a2, int a3, int a4);
#if COD_VERSION == COD2_1_0
static const runtimeError_t runtimeError = (runtimeError_t)0x0807818C;
#elif COD_VERSION == COD2_1_2
static const runtimeError_t runtimeError = (runtimeError_t)0x08078710;
#elif COD_VERSION == COD2_1_3
static const runtimeError_t runtimeError = (runtimeError_t)0x080787DC;
#endif

typedef int (*Touch_Item_Auto_t)(int a1, int a2, int a3);
#if COD_VERSION == COD2_1_0
static const Touch_Item_Auto_t Touch_Item_Auto = (Touch_Item_Auto_t)0x081037F0;
#elif COD_VERSION == COD2_1_2
static const Touch_Item_Auto_t Touch_Item_Auto = (Touch_Item_Auto_t)0x08105B24;
#elif COD_VERSION == COD2_1_3
static const Touch_Item_Auto_t Touch_Item_Auto = (Touch_Item_Auto_t)0x08105C80;
#endif

typedef int (*BG_AnimationIndexForString_t)(char *src);
#if COD_VERSION == COD2_1_0
static const BG_AnimationIndexForString_t BG_AnimationIndexForString = (BG_AnimationIndexForString_t)0x080D46AC;
#elif COD_VERSION == COD2_1_2
static const BG_AnimationIndexForString_t BG_AnimationIndexForString = (BG_AnimationIndexForString_t)0x080D6C8C;
#elif COD_VERSION == COD2_1_3
static const BG_AnimationIndexForString_t BG_AnimationIndexForString = (BG_AnimationIndexForString_t)0x080D6DD0;
#endif

typedef int (*Sys_IsLANAddress_t)(netadr_t adr);
#if COD_VERSION == COD2_1_0
static const Sys_IsLANAddress_t Sys_IsLANAddress = (Sys_IsLANAddress_t)0x080D2FC8;
#elif COD_VERSION == COD2_1_2
static const Sys_IsLANAddress_t Sys_IsLANAddress = (Sys_IsLANAddress_t)0x080D54F4;
#elif COD_VERSION == COD2_1_3
static const Sys_IsLANAddress_t Sys_IsLANAddress = (Sys_IsLANAddress_t)0x080D5638;
#endif

typedef void (*LookAtKiller_t)(int self, int inflictor, int attacker);
#if COD_VERSION == COD2_1_0
static const LookAtKiller_t LookAtKiller = (LookAtKiller_t)0x080FF17A;
#elif COD_VERSION == COD2_1_2
static const LookAtKiller_t LookAtKiller = (LookAtKiller_t)0x081014AE;
#elif COD_VERSION == COD2_1_3
static const LookAtKiller_t LookAtKiller = (LookAtKiller_t)0x0810160A;
#endif

typedef int (*Sys_GetValue_t)(int key);
#if COD_VERSION == COD2_1_0
static const Sys_GetValue_t Sys_GetValue = (Sys_GetValue_t)0x080D44A4;
#elif COD_VERSION == COD2_1_2
static const Sys_GetValue_t Sys_GetValue = (Sys_GetValue_t)0x080D6A7C;
#elif COD_VERSION == COD2_1_3
static const Sys_GetValue_t Sys_GetValue = (Sys_GetValue_t)0x080D6BC0;
#endif

typedef int (*Scr_GetEntity_t)(int a1);
#if COD_VERSION == COD2_1_0
static const Scr_GetEntity_t Scr_GetEntity = (Scr_GetEntity_t)0x0810E03A;
#elif COD_VERSION == COD2_1_2
static const Scr_GetEntity_t Scr_GetEntity = (Scr_GetEntity_t)0x08110376;
#elif COD_VERSION == COD2_1_3
static const Scr_GetEntity_t Scr_GetEntity = (Scr_GetEntity_t)0x081104D2;
#endif

typedef int (*Scr_IsSystemActive_t)();
#if COD_VERSION == COD2_1_0
static const Scr_IsSystemActive_t Scr_IsSystemActive = (Scr_IsSystemActive_t)0x08084030;
#elif COD_VERSION == COD2_1_2
static const Scr_IsSystemActive_t Scr_IsSystemActive = (Scr_IsSystemActive_t)0x080845AC;
#elif COD_VERSION == COD2_1_3
static const Scr_IsSystemActive_t Scr_IsSystemActive = (Scr_IsSystemActive_t)0x08084678;
#endif

typedef void (*Scr_AddInt_t)(int value);
#if COD_VERSION == COD2_1_0
static const Scr_AddInt_t Scr_AddInt = (Scr_AddInt_t)0x08084B1C;
#elif COD_VERSION == COD2_1_2
static const Scr_AddInt_t Scr_AddInt = (Scr_AddInt_t)0x08085098;
#elif COD_VERSION == COD2_1_3
static const Scr_AddInt_t Scr_AddInt = (Scr_AddInt_t)0x08085164;
#endif

typedef void (*Scr_AddFloat_t)(float);
#if COD_VERSION == COD2_1_0
static const Scr_AddFloat_t Scr_AddFloat = (Scr_AddFloat_t)0x08084B40;
#elif COD_VERSION == COD2_1_2
static const Scr_AddFloat_t Scr_AddFloat = (Scr_AddFloat_t)0x080850BC;
#elif COD_VERSION == COD2_1_3
static const Scr_AddFloat_t Scr_AddFloat = (Scr_AddFloat_t)0x08085188;
#endif

typedef void (*Scr_AddString_t)(const char *string);
#if COD_VERSION == COD2_1_0
static const Scr_AddString_t Scr_AddString = (Scr_AddString_t)0x08084C1A;
#elif COD_VERSION == COD2_1_2
static const Scr_AddString_t Scr_AddString = (Scr_AddString_t)0x08085196;
#elif COD_VERSION == COD2_1_3
static const Scr_AddString_t Scr_AddString = (Scr_AddString_t)0x08085262;
#endif

typedef void (*Scr_AddUndefined_t)(void);
#if COD_VERSION == COD2_1_0
static const Scr_AddUndefined_t Scr_AddUndefined = (Scr_AddUndefined_t)0x08084B88;
#elif COD_VERSION == COD2_1_2
static const Scr_AddUndefined_t Scr_AddUndefined = (Scr_AddUndefined_t)0x08085104;
#elif COD_VERSION == COD2_1_3
static const Scr_AddUndefined_t Scr_AddUndefined = (Scr_AddUndefined_t)0x080851D0;
#endif

typedef void (*Scr_AddVector_t)(vec3_t vec);
#if COD_VERSION == COD2_1_0
static const Scr_AddVector_t Scr_AddVector = (Scr_AddVector_t)0x08084CBE;
#elif COD_VERSION == COD2_1_2
static const Scr_AddVector_t Scr_AddVector = (Scr_AddVector_t)0x0808523A;
#elif COD_VERSION == COD2_1_3
static const Scr_AddVector_t Scr_AddVector = (Scr_AddVector_t)0x08085306;
#endif

typedef void (*Scr_AddEntity_t)(int entity);
#if COD_VERSION == COD2_1_0
static const Scr_AddEntity_t Scr_AddEntity = (Scr_AddEntity_t)0x08118CC0;
#elif COD_VERSION == COD2_1_2
static const Scr_AddEntity_t Scr_AddEntity = (Scr_AddEntity_t)0x0811AFF4;
#elif COD_VERSION == COD2_1_3
static const Scr_AddEntity_t Scr_AddEntity = (Scr_AddEntity_t)0x08117F50;
#endif

typedef void (*Scr_MakeArray_t)(void);
#if COD_VERSION == COD2_1_0
static const Scr_MakeArray_t Scr_MakeArray = (Scr_MakeArray_t)0x08084CF0;
#elif COD_VERSION == COD2_1_2
static const Scr_MakeArray_t Scr_MakeArray = (Scr_MakeArray_t)0x0808526C;
#elif COD_VERSION == COD2_1_3
static const Scr_MakeArray_t Scr_MakeArray = (Scr_MakeArray_t)0x08085338;
#endif

typedef void (*Scr_AddArray_t)(void);
#if COD_VERSION == COD2_1_0
static const Scr_AddArray_t Scr_AddArray = (Scr_AddArray_t)0x08084D1C;
#elif COD_VERSION == COD2_1_2
static const Scr_AddArray_t Scr_AddArray = (Scr_AddArray_t)0x08085298;
#elif COD_VERSION == COD2_1_3
static const Scr_AddArray_t Scr_AddArray = (Scr_AddArray_t)0x08085364;
#endif

typedef int (*G_TempEntity_t)(vec3_t origin, int event);
#if COD_VERSION == COD2_1_0
static const G_TempEntity_t G_TempEntity = (G_TempEntity_t)0x0811CB34;
#elif COD_VERSION == COD2_1_2
static const G_TempEntity_t G_TempEntity = (G_TempEntity_t)0x0811EE68;
#elif COD_VERSION == COD2_1_3
static const G_TempEntity_t G_TempEntity = (G_TempEntity_t)0x0811EFC4;
#endif

typedef int (*DirToByte_t)(vec3_t dir);
#if COD_VERSION == COD2_1_0
static const DirToByte_t DirToByte = (DirToByte_t)0x080A1C2A;
#elif COD_VERSION == COD2_1_2
static const DirToByte_t DirToByte = (DirToByte_t)0x080A3E4A;
#elif COD_VERSION == COD2_1_3
static const DirToByte_t DirToByte = (DirToByte_t)0x080A3F8E;
#endif

typedef void (*G_LocationalTrace_t)(trace_t *results, const vec3_t start, const vec3_t end, int passEntityNum, int contentmask);
#if COD_VERSION == COD2_1_0
static const G_LocationalTrace_t G_LocationalTrace = (G_LocationalTrace_t)0x08108134;
#elif COD_VERSION == COD2_1_2
static const G_LocationalTrace_t G_LocationalTrace = (G_LocationalTrace_t)0x0810A470;
#elif COD_VERSION == COD2_1_3
static const G_LocationalTrace_t G_LocationalTrace = (G_LocationalTrace_t)0x0810A5CC;
#endif

typedef void (*G_AddEvent_t)(int ent, int event, int eventParm);
#if COD_VERSION == COD2_1_0
static const G_AddEvent_t G_AddEvent = (G_AddEvent_t)0x0811CDA2;
#elif COD_VERSION == COD2_1_2
static const G_AddEvent_t G_AddEvent = (G_AddEvent_t)0x0811F0D6;
#elif COD_VERSION == COD2_1_3
static const G_AddEvent_t G_AddEvent = (G_AddEvent_t)0x0811F232;
#endif

#endif
