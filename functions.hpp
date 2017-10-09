#ifndef _FUNCTIONS_HPP_
#define _FUNCTIONS_HPP_

/* MAKE FUNCTIONS STATIC, SO THEY CAN BE IN EVERY FILE */

typedef unsigned short (*GetVariableName_t)(unsigned short a1);
#if COD_VERSION == COD2_1_0
static const GetVariableName_t GetVariableName = (GetVariableName_t)0x0807CA72;
#elif COD_VERSION == COD2_1_2
static const GetVariableName_t GetVariableName = (GetVariableName_t)0x0807CFF6;
#elif COD_VERSION == COD2_1_3
static const GetVariableName_t GetVariableName = (GetVariableName_t)0x0807D0C2;
#endif

typedef unsigned short (*GetNextVariable_t)(unsigned short a1);
#if COD_VERSION == COD2_1_0
static const GetNextVariable_t GetNextVariable = (GetNextVariable_t)0x0807C9CE;
#elif COD_VERSION == COD2_1_2
static const GetNextVariable_t GetNextVariable = (GetNextVariable_t)0x0807CF52;
#elif COD_VERSION == COD2_1_3
static const GetNextVariable_t GetNextVariable = (GetNextVariable_t)0x0807D01E;
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

typedef struct
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char alpha;
} ucolor_t;

typedef struct cvar_s
{
	char *name;
	char *description;
	short int flags;
	unsigned char type;
	unsigned char modified;
	union
	{
		float floatval;
		float value;
		int integer;
		char* string;
		unsigned char boolean;
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
		unsigned char latchedBoolean;
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
		unsigned char resetBoolean;
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

struct VariableStackBuffer
{
	const char *pos;
	unsigned short size;
	unsigned short bufLen;
	unsigned short localId;
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
	unsigned char debugCode;
	unsigned char abort_on_error;
	unsigned char terminal_error;
	unsigned char pad;
	unsigned int inparamcount;
	unsigned int outparamcount;
	struct function_frame_t function_frame_start[32];
	VariableValue stack[2048];
} scrVmPub_t;

typedef struct
{
	const char *fieldBuffer;
	unsigned short canonicalStrCount;
	unsigned char developer;
	unsigned char developer_script;
	unsigned char evaluate;
	unsigned char pad[3];
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
	unsigned char bInited;
	unsigned char pad2;
	unsigned short savecount;
	unsigned int checksum;
	unsigned int entId;
	unsigned int entFieldName;
	struct HunkUser *programHunkUser;
	const char *programBuffer;
	const char *endScriptBuffer;
	unsigned short saveIdMap[24574];
	unsigned short saveIdMapRev[24574];
} scrVarPub_t;

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

typedef char * (*SV_BeginDownload_f_t)(int a1);
#if COD_VERSION == COD2_1_0
static const SV_BeginDownload_f_t SV_BeginDownload_f = (SV_BeginDownload_f_t)0x0808E508;
#elif COD_VERSION == COD2_1_2
static const SV_BeginDownload_f_t SV_BeginDownload_f = (SV_BeginDownload_f_t)0x0808F8AC;
#elif COD_VERSION == COD2_1_3
static const SV_BeginDownload_f_t SV_BeginDownload_f = (SV_BeginDownload_f_t)0x0808F940;
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

#endif
