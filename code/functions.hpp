#ifndef _FUNCTIONS_HPP_
#define _FUNCTIONS_HPP_

/* MAKE FUNCTIONS STATIC, SO THEY CAN BE IN EVERY FILE */

typedef unsigned int (*GetVariableName_t)(unsigned int a1);
#if COD_VERSION == COD2_1_0
static const GetVariableName_t GetVariableName = (GetVariableName_t)0x0807CA72;
#elif COD_VERSION == COD2_1_2
static const GetVariableName_t GetVariableName = (GetVariableName_t)0x0807CFF6;
#elif COD_VERSION == COD2_1_3
static const GetVariableName_t GetVariableName = (GetVariableName_t)0x0807D0C2;
#endif

typedef unsigned int (*GetNextVariable_t)(unsigned int a1);
#if COD_VERSION == COD2_1_0
static const GetNextVariable_t GetNextVariable = (GetNextVariable_t)0x0807C9CE;
#elif COD_VERSION == COD2_1_2
static const GetNextVariable_t GetNextVariable = (GetNextVariable_t)0x0807CF52;
#elif COD_VERSION == COD2_1_3
static const GetNextVariable_t GetNextVariable = (GetNextVariable_t)0x0807D01E;
#endif

typedef int (*GetArraySize_t)(unsigned int a1);
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

typedef unsigned int (*SL_GetString_t)(const char *str, int type);
#if COD_VERSION == COD2_1_0
static const SL_GetString_t SL_GetString = (SL_GetString_t)0x08079290;
#elif COD_VERSION == COD2_1_2
static const SL_GetString_t SL_GetString = (SL_GetString_t)0x08079814;
#elif COD_VERSION == COD2_1_3
static const SL_GetString_t SL_GetString = (SL_GetString_t)0x080798E0;
#endif

typedef void (*SL_RemoveRefToString_t)(unsigned int);
#if COD_VERSION == COD2_1_0
static const SL_RemoveRefToString_t SL_RemoveRefToString = (SL_RemoveRefToString_t)0x080796A2;
#elif COD_VERSION == COD2_1_2
static const SL_RemoveRefToString_t SL_RemoveRefToString = (SL_RemoveRefToString_t)0x08079C26;
#elif COD_VERSION == COD2_1_3
static const SL_RemoveRefToString_t SL_RemoveRefToString = (SL_RemoveRefToString_t)0x08079CF2;
#endif

typedef int (*Scr_GetFunctionHandle_t)(const char* scriptName, const char* labelName, int isNeeded);
#if COD_VERSION == COD2_1_0
static const Scr_GetFunctionHandle_t Scr_GetFunctionHandle = (Scr_GetFunctionHandle_t)0x0810DD70;
#elif COD_VERSION == COD2_1_2
static const Scr_GetFunctionHandle_t Scr_GetFunctionHandle = (Scr_GetFunctionHandle_t)0x081100AC;
#elif COD_VERSION == COD2_1_3
static const Scr_GetFunctionHandle_t Scr_GetFunctionHandle = (Scr_GetFunctionHandle_t)0x08110208;
#endif

typedef int (*Scr_GetNumParam_t)(void);
#if COD_VERSION == COD2_1_0
static const Scr_GetNumParam_t Scr_GetNumParam = (Scr_GetNumParam_t)0x08084AEE;
#elif COD_VERSION == COD2_1_2
static const Scr_GetNumParam_t Scr_GetNumParam = (Scr_GetNumParam_t)0x0808506A;
#elif COD_VERSION == COD2_1_3
static const Scr_GetNumParam_t Scr_GetNumParam = (Scr_GetNumParam_t)0x08085136;
#endif

typedef void (*Cmd_Score_f_t)(gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const Cmd_Score_f_t Cmd_Score_f = (Cmd_Score_f_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Cmd_Score_f_t Cmd_Score_f = (Cmd_Score_f_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Cmd_Score_f_t Cmd_Score_f = (Cmd_Score_f_t)0x080FE0EE;
#endif

typedef char * (*Cmd_Argv_t)(int arg);
#if COD_VERSION == COD2_1_0
static const Cmd_Argv_t Cmd_Argv = (Cmd_Argv_t)0x0806001C;
#elif COD_VERSION == COD2_1_2
static const Cmd_Argv_t Cmd_Argv = (Cmd_Argv_t)0x08060228;
#elif COD_VERSION == COD2_1_3
static const Cmd_Argv_t Cmd_Argv = (Cmd_Argv_t)0x08060220;
#endif

typedef int (*Cmd_Argc_t)();
#if COD_VERSION == COD2_1_0
static const Cmd_Argc_t Cmd_Argc = (Cmd_Argc_t)0x0805FFDC;
#elif COD_VERSION == COD2_1_2
static const Cmd_Argc_t Cmd_Argc = (Cmd_Argc_t)0x080601E8;
#elif COD_VERSION == COD2_1_3
static const Cmd_Argc_t Cmd_Argc = (Cmd_Argc_t)0x080601E0;
#endif

typedef int (*SV_Cmd_Argc_t)(void);
#if COD_VERSION == COD2_1_0
static const SV_Cmd_Argc_t SV_Cmd_Argc = (SV_Cmd_Argc_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_Cmd_Argc_t SV_Cmd_Argc = (SV_Cmd_Argc_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_Cmd_Argc_t SV_Cmd_Argc = (SV_Cmd_Argc_t)0x080601EA;
#endif

typedef char * (*SV_Cmd_Argv_t)(unsigned int arg);
#if COD_VERSION == COD2_1_0
static const SV_Cmd_Argv_t SV_Cmd_Argv = (SV_Cmd_Argv_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_Cmd_Argv_t SV_Cmd_Argv = (SV_Cmd_Argv_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_Cmd_Argv_t SV_Cmd_Argv = (SV_Cmd_Argv_t)0x08060220;
#endif

typedef void (*SV_Cmd_ArgvBuffer_t)(int arg, char *buffer, int bufferLength);
#if COD_VERSION == COD2_1_0
static const SV_Cmd_ArgvBuffer_t SV_Cmd_ArgvBuffer = (SV_Cmd_ArgvBuffer_t)0x08060074;
#elif COD_VERSION == COD2_1_2
static const SV_Cmd_ArgvBuffer_t SV_Cmd_ArgvBuffer = (SV_Cmd_ArgvBuffer_t)0x08060280;
#elif COD_VERSION == COD2_1_3
static const SV_Cmd_ArgvBuffer_t SV_Cmd_ArgvBuffer = (SV_Cmd_ArgvBuffer_t)0x08060278;
#endif

typedef void (*SV_Cmd_ExecuteString_t)(const char *cmd);
#if COD_VERSION == COD2_1_0
static const SV_Cmd_ExecuteString_t SV_Cmd_ExecuteString = (SV_Cmd_ExecuteString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_Cmd_ExecuteString_t SV_Cmd_ExecuteString = (SV_Cmd_ExecuteString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_Cmd_ExecuteString_t SV_Cmd_ExecuteString = (SV_Cmd_ExecuteString_t)0x080609CC;
#endif

typedef void (*SV_Cmd_TokenizeString_t)(const char *text_in);
#if COD_VERSION == COD2_1_0
static const SV_Cmd_TokenizeString_t SV_Cmd_TokenizeString = (SV_Cmd_TokenizeString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_Cmd_TokenizeString_t SV_Cmd_TokenizeString = (SV_Cmd_TokenizeString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_Cmd_TokenizeString_t SV_Cmd_TokenizeString = (SV_Cmd_TokenizeString_t)0x0806064C;
#endif

typedef void (*SV_CreateBaseline_t)(void);
#if COD_VERSION == COD2_1_0
static const SV_CreateBaseline_t SV_CreateBaseline = (SV_CreateBaseline_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_CreateBaseline_t SV_CreateBaseline = (SV_CreateBaseline_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_CreateBaseline_t SV_CreateBaseline = (SV_CreateBaseline_t)0x08092C86;
#endif

typedef void (*SV_InitArchivedSnapshot_t)(void);
#if COD_VERSION == COD2_1_0
static const SV_InitArchivedSnapshot_t SV_InitArchivedSnapshot = (SV_InitArchivedSnapshot_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_InitArchivedSnapshot_t SV_InitArchivedSnapshot = (SV_InitArchivedSnapshot_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_InitArchivedSnapshot_t SV_InitArchivedSnapshot = (SV_InitArchivedSnapshot_t)0x0809338E;
#endif

typedef void (*SV_InitGameProgs_t)(int savepersist);
#if COD_VERSION == COD2_1_0
static const SV_InitGameProgs_t SV_InitGameProgs = (SV_InitGameProgs_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_InitGameProgs_t SV_InitGameProgs = (SV_InitGameProgs_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_InitGameProgs_t SV_InitGameProgs = (SV_InitGameProgs_t)0x080926B0;
#endif

typedef void (*SV_RunFrame_t)(void);
#if COD_VERSION == COD2_1_0
static const SV_RunFrame_t SV_RunFrame = (SV_RunFrame_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_RunFrame_t SV_RunFrame = (SV_RunFrame_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_RunFrame_t SV_RunFrame = (SV_RunFrame_t)0x08096752;
#endif

typedef void (*Sys_Error_t)(const char *fmt, ...);
#if COD_VERSION == COD2_1_0
static const Sys_Error_t Sys_Error = (Sys_Error_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Sys_Error_t Sys_Error = (Sys_Error_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Sys_Error_t Sys_Error = (Sys_Error_t)0x080D3B3C;
#endif

typedef void (*Sys_EnterCriticalSectionInternal_t)(int section);
#if COD_VERSION == COD2_1_0
static const Sys_EnterCriticalSectionInternal_t Sys_EnterCriticalSectionInternal = (Sys_EnterCriticalSectionInternal_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Sys_EnterCriticalSectionInternal_t Sys_EnterCriticalSectionInternal = (Sys_EnterCriticalSectionInternal_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Sys_EnterCriticalSectionInternal_t Sys_EnterCriticalSectionInternal = (Sys_EnterCriticalSectionInternal_t)0x080D6842;
#endif

typedef void * (*Sys_GetValue_t)(int key);
#if COD_VERSION == COD2_1_0
static const Sys_GetValue_t Sys_GetValue = (Sys_GetValue_t)0x080D44A4;
#elif COD_VERSION == COD2_1_2
static const Sys_GetValue_t Sys_GetValue = (Sys_GetValue_t)0x080D6A7C;
#elif COD_VERSION == COD2_1_3
static const Sys_GetValue_t Sys_GetValue = (Sys_GetValue_t)0x080D6BC0;
#endif

typedef int (*Sys_IsLANAddress_t)(netadr_t adr);
#if COD_VERSION == COD2_1_0
static const Sys_IsLANAddress_t Sys_IsLANAddress = (Sys_IsLANAddress_t)0x080D2FC8;
#elif COD_VERSION == COD2_1_2
static const Sys_IsLANAddress_t Sys_IsLANAddress = (Sys_IsLANAddress_t)0x080D54F4;
#elif COD_VERSION == COD2_1_3
static const Sys_IsLANAddress_t Sys_IsLANAddress = (Sys_IsLANAddress_t)0x080D5638;
#endif

typedef void (*Sys_LeaveCriticalSectionInternal_t)(int section);
#if COD_VERSION == COD2_1_0
static const Sys_LeaveCriticalSectionInternal_t Sys_LeaveCriticalSectionInternal = (Sys_LeaveCriticalSectionInternal_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Sys_LeaveCriticalSectionInternal_t Sys_LeaveCriticalSectionInternal = (Sys_LeaveCriticalSectionInternal_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Sys_LeaveCriticalSectionInternal_t Sys_LeaveCriticalSectionInternal = (Sys_LeaveCriticalSectionInternal_t)0x080D6864;
#endif

typedef int (*Sys_Milliseconds_t)(void);
#if COD_VERSION == COD2_1_0
static const Sys_Milliseconds_t Sys_Milliseconds = (Sys_Milliseconds_t)0x080D3728;
#elif COD_VERSION == COD2_1_2
static const Sys_Milliseconds_t Sys_Milliseconds = (Sys_Milliseconds_t)0x080D5C54;
#elif COD_VERSION == COD2_1_3
static const Sys_Milliseconds_t Sys_Milliseconds = (Sys_Milliseconds_t)0x080D5D98;
#endif

typedef int (*Sys_MillisecondsRaw_t)(void);
#if COD_VERSION == COD2_1_0
static const Sys_MillisecondsRaw_t Sys_MillisecondsRaw = (Sys_MillisecondsRaw_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Sys_MillisecondsRaw_t Sys_MillisecondsRaw = (Sys_MillisecondsRaw_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Sys_MillisecondsRaw_t Sys_MillisecondsRaw = (Sys_MillisecondsRaw_t)0x080D5E20;
#endif

typedef void (*Sys_Print_t)(const char *msg);
#if COD_VERSION == COD2_1_0
static const Sys_Print_t Sys_Print = (Sys_Print_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Sys_Print_t Sys_Print = (Sys_Print_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Sys_Print_t Sys_Print = (Sys_Print_t)0x080D4AE0;
#endif

typedef void (*Sys_SetValue_t)(int key, void *value);
#if COD_VERSION == COD2_1_0
static const Sys_SetValue_t Sys_SetValue = (Sys_SetValue_t)0x080D4492;
#elif COD_VERSION == COD2_1_2
static const Sys_SetValue_t Sys_SetValue = (Sys_SetValue_t)0x080D6A6A;
#elif COD_VERSION == COD2_1_3
static const Sys_SetValue_t Sys_SetValue = (Sys_SetValue_t)0x080D6BAE;
#endif

typedef unsigned int (*Com_AddToString_t)(const char* string, char* buffer, unsigned int current, unsigned int length, byte escapeSpaces);
#if COD_VERSION == COD2_1_0
static const Com_AddToString_t Com_AddToString = (Com_AddToString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Com_AddToString_t Com_AddToString = (Com_AddToString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Com_AddToString_t Com_AddToString = (Com_AddToString_t)0x080629CE;
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

typedef void (*Com_PrintMessage_t)(int channel, const char *message);
#if COD_VERSION == COD2_1_0
static const Com_PrintMessage_t Com_PrintMessage = (Com_PrintMessage_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Com_PrintMessage_t Com_PrintMessage = (Com_PrintMessage_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Com_PrintMessage_t Com_PrintMessage = (Com_PrintMessage_t)0x08060C20;
#endif

typedef void (*Com_BeginRedirect_t)(char *buffer, int buffersize, void (*flush)(char *));
#if COD_VERSION == COD2_1_0
static const Com_BeginRedirect_t Com_BeginRedirect = (Com_BeginRedirect_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Com_BeginRedirect_t Com_BeginRedirect = (Com_BeginRedirect_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Com_BeginRedirect_t Com_BeginRedirect = (Com_BeginRedirect_t)0x08060BA2;
#endif

typedef void (*Com_EndRedirect_t)(void);
#if COD_VERSION == COD2_1_0
static const Com_EndRedirect_t Com_EndRedirect = (Com_EndRedirect_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Com_EndRedirect_t Com_EndRedirect = (Com_EndRedirect_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Com_EndRedirect_t Com_EndRedirect = (Com_EndRedirect_t)0x08060BDC;
#endif

typedef void (*Com_LoadBsp_t)(const char *filename);
#if COD_VERSION == COD2_1_0
static const Com_LoadBsp_t Com_LoadBsp = (Com_LoadBsp_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Com_LoadBsp_t Com_LoadBsp = (Com_LoadBsp_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Com_LoadBsp_t Com_LoadBsp = (Com_LoadBsp_t)0x08051E88;
#endif

typedef void (*Com_UnloadBsp_t)(void);
#if COD_VERSION == COD2_1_0
static const Com_UnloadBsp_t Com_UnloadBsp = (Com_UnloadBsp_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Com_UnloadBsp_t Com_UnloadBsp = (Com_UnloadBsp_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Com_UnloadBsp_t Com_UnloadBsp = (Com_UnloadBsp_t)0x08052054;
#endif

typedef void (*Com_LoadSoundAliases_t)(char *src, const char *loadspec, snd_alias_system_t system);
#if COD_VERSION == COD2_1_0
static const Com_LoadSoundAliases_t Com_LoadSoundAliases = (Com_LoadSoundAliases_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Com_LoadSoundAliases_t Com_LoadSoundAliases = (Com_LoadSoundAliases_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Com_LoadSoundAliases_t Com_LoadSoundAliases = (Com_LoadSoundAliases_t)0x080AD0AA;
#endif

typedef void (*Com_MakeSoundAliasesPermanent_t)(snd_alias_list_t *aliasList, SoundFileInfo *fileInfo);
#if COD_VERSION == COD2_1_0
static const Com_MakeSoundAliasesPermanent_t Com_MakeSoundAliasesPermanent = (Com_MakeSoundAliasesPermanent_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Com_MakeSoundAliasesPermanent_t Com_MakeSoundAliasesPermanent = (Com_MakeSoundAliasesPermanent_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Com_MakeSoundAliasesPermanent_t Com_MakeSoundAliasesPermanent = (Com_MakeSoundAliasesPermanent_t)0x080AF224;
#endif

typedef snd_alias_list_t * (*Com_FindSoundAlias_t)(const char *name);
#if COD_VERSION == COD2_1_0
static const Com_FindSoundAlias_t Com_FindSoundAlias = (Com_FindSoundAlias_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Com_FindSoundAlias_t Com_FindSoundAlias = (Com_FindSoundAlias_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Com_FindSoundAlias_t Com_FindSoundAlias = (Com_FindSoundAlias_t)0x080ACC42;
#endif

typedef void (*Cmd_AddCommand_t)(const char *cmd_name, xcommand_t function);
#if COD_VERSION == COD2_1_0
static const Cmd_AddCommand_t Cmd_AddCommand = (Cmd_AddCommand_t)0x080604B2;
#elif COD_VERSION == COD2_1_2
static const Cmd_AddCommand_t Cmd_AddCommand = (Cmd_AddCommand_t)0x080606BE;
#elif COD_VERSION == COD2_1_3
static const Cmd_AddCommand_t Cmd_AddCommand = (Cmd_AddCommand_t)0x080606B6;
#endif

typedef void (*Cmd_ExecuteString_t)(const char *cmd);
#if COD_VERSION == COD2_1_0
static const Cmd_ExecuteString_t Cmd_ExecuteString = (Cmd_ExecuteString_t)0x08060754;
#elif COD_VERSION == COD2_1_2
static const Cmd_ExecuteString_t Cmd_ExecuteString = (Cmd_ExecuteString_t)0x080609D4;
#elif COD_VERSION == COD2_1_3
static const Cmd_ExecuteString_t Cmd_ExecuteString = (Cmd_ExecuteString_t)0x080609CC; // That's actually SV_Cmd_ExecuteString, but doesn't matter
#endif

typedef void (*ClientCommand_t)(int clientNum);
#if COD_VERSION == COD2_1_0
static const ClientCommand_t ClientCommand = (ClientCommand_t)0x080FE998;
#elif COD_VERSION == COD2_1_2
static const ClientCommand_t ClientCommand = (ClientCommand_t)0x08100D1E;
#elif COD_VERSION == COD2_1_3
static const ClientCommand_t ClientCommand = (ClientCommand_t)0x08100E62;
#endif

typedef char (*FS_Initialized_t)(void);
#if COD_VERSION == COD2_1_0
static const FS_Initialized_t FS_Initialized = (FS_Initialized_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const FS_Initialized_t FS_Initialized = (FS_Initialized_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const FS_Initialized_t FS_Initialized = (FS_Initialized_t)0x0809E620;
#endif

typedef long (*FS_HashFileName_t)(const char *fname, int hashSize);
#if COD_VERSION == COD2_1_0
static const FS_HashFileName_t FS_HashFileName = (FS_HashFileName_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const FS_HashFileName_t FS_HashFileName = (FS_HashFileName_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const FS_HashFileName_t FS_HashFileName = (FS_HashFileName_t)0x0809E740;
#endif

typedef int (*FS_FilenameCompare_t)(const char *s1, const char *s2);
#if COD_VERSION == COD2_1_0
static const FS_FilenameCompare_t FS_FilenameCompare = (FS_FilenameCompare_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const FS_FilenameCompare_t FS_FilenameCompare = (FS_FilenameCompare_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const FS_FilenameCompare_t FS_FilenameCompare = (FS_FilenameCompare_t)0x0809F32C;
#endif

typedef const char * (*FS_LoadedIwdChecksums_t)(void);
#if COD_VERSION == COD2_1_0
static const FS_LoadedIwdChecksums_t FS_LoadedIwdChecksums = (FS_LoadedIwdChecksums_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const FS_LoadedIwdChecksums_t FS_LoadedIwdChecksums = (FS_LoadedIwdChecksums_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const FS_LoadedIwdChecksums_t FS_LoadedIwdChecksums = (FS_LoadedIwdChecksums_t)0x0806590A;
#endif

typedef const char * (*FS_LoadedIwdNames_t)(void);
#if COD_VERSION == COD2_1_0
static const FS_LoadedIwdNames_t FS_LoadedIwdNames = (FS_LoadedIwdNames_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const FS_LoadedIwdNames_t FS_LoadedIwdNames = (FS_LoadedIwdNames_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const FS_LoadedIwdNames_t FS_LoadedIwdNames = (FS_LoadedIwdNames_t)0x08065982;
#endif

typedef const char * (*FS_ReferencedIwdChecksums_t)(void);
#if COD_VERSION == COD2_1_0
static const FS_ReferencedIwdChecksums_t FS_ReferencedIwdChecksums = (FS_ReferencedIwdChecksums_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const FS_ReferencedIwdChecksums_t FS_ReferencedIwdChecksums = (FS_ReferencedIwdChecksums_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const FS_ReferencedIwdChecksums_t FS_ReferencedIwdChecksums = (FS_ReferencedIwdChecksums_t)0x08065A86;
#endif

typedef const char * (*FS_ReferencedIwdNames_t)(void);
#if COD_VERSION == COD2_1_0
static const FS_ReferencedIwdNames_t FS_ReferencedIwdNames = (FS_ReferencedIwdNames_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const FS_ReferencedIwdNames_t FS_ReferencedIwdNames = (FS_ReferencedIwdNames_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const FS_ReferencedIwdNames_t FS_ReferencedIwdNames = (FS_ReferencedIwdNames_t)0x08065B32;
#endif

typedef int (*FS_ReadFile_t)(const char *qpath, void **buffer);
#if COD_VERSION == COD2_1_0
static const FS_ReadFile_t FS_ReadFile = (FS_ReadFile_t)0x0809E892;
#elif COD_VERSION == COD2_1_2
static const FS_ReadFile_t FS_ReadFile = (FS_ReadFile_t)0x080A0958;
#elif COD_VERSION == COD2_1_3
static const FS_ReadFile_t FS_ReadFile = (FS_ReadFile_t)0x080A0A9C;
#endif

typedef void (*FS_Restart_t)(int checksumFeed);
#if COD_VERSION == COD2_1_0
static const FS_Restart_t FS_Restart = (FS_Restart_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const FS_Restart_t FS_Restart = (FS_Restart_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const FS_Restart_t FS_Restart = (FS_Restart_t)0x080A337A;
#endif

typedef void (*FS_AddIwdFilesForGameDirectory_t)(const char *path, const char *dir);
#if COD_VERSION == COD2_1_0
static const FS_AddIwdFilesForGameDirectory_t FS_AddIwdFilesForGameDirectory = (FS_AddIwdFilesForGameDirectory_t)0x080A01A4;
#elif COD_VERSION == COD2_1_2
static const FS_AddIwdFilesForGameDirectory_t FS_AddIwdFilesForGameDirectory = (FS_AddIwdFilesForGameDirectory_t)0x080A22D8;
#elif COD_VERSION == COD2_1_3
static const FS_AddIwdFilesForGameDirectory_t FS_AddIwdFilesForGameDirectory = (FS_AddIwdFilesForGameDirectory_t)0x080A241C;
#endif

typedef void (*FS_Printf_t)(fileHandle_t h, const char *fmt, ...);
#if COD_VERSION == COD2_1_0
static const FS_Printf_t FS_Printf = (FS_Printf_t)0x0809E52A;
#elif COD_VERSION == COD2_1_2
static const FS_Printf_t FS_Printf = (FS_Printf_t)0x080A05E8;
#elif COD_VERSION == COD2_1_3
static const FS_Printf_t FS_Printf = (FS_Printf_t)0x080A072C;
#endif

typedef int (*FS_Write_t)(const void *buffer, int len, fileHandle_t h);
#if COD_VERSION == COD2_1_0
static const FS_Write_t FS_Write = (FS_Write_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const FS_Write_t FS_Write = (FS_Write_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const FS_Write_t FS_Write = (FS_Write_t)0x080A0656;
#endif

typedef void (*FS_Flush_t)(fileHandle_t h);
#if COD_VERSION == COD2_1_0
static const FS_Flush_t FS_Flush = (FS_Flush_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const FS_Flush_t FS_Flush = (FS_Flush_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const FS_Flush_t FS_Flush = (FS_Flush_t)0x080A36E2;
#endif

typedef fileHandle_t (*FS_FOpenFileWrite_t)(const char *filename);
#if COD_VERSION == COD2_1_0
static const FS_FOpenFileWrite_t FS_FOpenFileWrite = (FS_FOpenFileWrite_t)0x0809CDDE;
#elif COD_VERSION == COD2_1_2
static const FS_FOpenFileWrite_t FS_FOpenFileWrite = (FS_FOpenFileWrite_t)0x0809EE9C;
#elif COD_VERSION == COD2_1_3
static const FS_FOpenFileWrite_t FS_FOpenFileWrite = (FS_FOpenFileWrite_t)0x0809EFE0;
#endif

typedef long (*FS_SV_FOpenFileRead_t)(const char *filename, fileHandle_t *fp);
#if COD_VERSION == COD2_1_0
static const FS_SV_FOpenFileRead_t FS_SV_FOpenFileRead = (FS_SV_FOpenFileRead_t)0x08064100;
#elif COD_VERSION == COD2_1_2
static const FS_SV_FOpenFileRead_t FS_SV_FOpenFileRead = (FS_SV_FOpenFileRead_t)0x08064560;
#elif COD_VERSION == COD2_1_3
static const FS_SV_FOpenFileRead_t FS_SV_FOpenFileRead = (FS_SV_FOpenFileRead_t)0x08064558;
#endif

typedef void (*FS_FCloseFile_t)(fileHandle_t h);
#if COD_VERSION == COD2_1_0
static const FS_FCloseFile_t FS_FCloseFile = (FS_FCloseFile_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const FS_FCloseFile_t FS_FCloseFile = (FS_FCloseFile_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const FS_FCloseFile_t FS_FCloseFile = (FS_FCloseFile_t)0x0809EE54;
#endif

typedef qboolean (*FS_iwIwd_t)(char *iwd, const char *base);
#if COD_VERSION == COD2_1_0
static const FS_iwIwd_t FS_iwIwd = (FS_iwIwd_t)0x08064ECC;
#elif COD_VERSION == COD2_1_2
static const FS_iwIwd_t FS_iwIwd = (FS_iwIwd_t)0x0806532C;
#elif COD_VERSION == COD2_1_3
static const FS_iwIwd_t FS_iwIwd = (FS_iwIwd_t)0x08065324;
#endif

typedef qboolean (*FS_svrIwd_t)(char *iwd); // Guessed function name
#if COD_VERSION == COD2_1_0
static const FS_svrIwd_t FS_svrIwd = (FS_svrIwd_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const FS_svrIwd_t FS_svrIwd = (FS_svrIwd_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const FS_svrIwd_t FS_svrIwd = (FS_svrIwd_t)0x08065450;
#endif

typedef void (*FS_ClearIwdReferences_t)(void);
#if COD_VERSION == COD2_1_0
static const FS_ClearIwdReferences_t FS_ClearIwdReferences = (FS_ClearIwdReferences_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const FS_ClearIwdReferences_t FS_ClearIwdReferences = (FS_ClearIwdReferences_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const FS_ClearIwdReferences_t FS_ClearIwdReferences = (FS_ClearIwdReferences_t)0x080A3222;
#endif

typedef void (*FS_Shutdown_t)(void);
#if COD_VERSION == COD2_1_0
static const FS_Shutdown_t FS_Shutdown = (FS_Shutdown_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const FS_Shutdown_t FS_Shutdown = (FS_Shutdown_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const FS_Shutdown_t FS_Shutdown = (FS_Shutdown_t)0x080A2BA8;
#endif

typedef xfunction_t (*Scr_GetFunction_t)(const char** v_functionName, qboolean *v_developer);
#if COD_VERSION == COD2_1_0
static const Scr_GetFunction_t Scr_GetFunction = (Scr_GetFunction_t)0x08115824;
#elif COD_VERSION == COD2_1_2
static const Scr_GetFunction_t Scr_GetFunction = (Scr_GetFunction_t)0x08117B56;
#elif COD_VERSION == COD2_1_3
static const Scr_GetFunction_t Scr_GetFunction = (Scr_GetFunction_t)0x08117CB2;
#endif

typedef xmethod_t (*Scr_GetMethod_t)(const char** v_methodName, qboolean *v_developer);
#if COD_VERSION == COD2_1_0
static const Scr_GetMethod_t Scr_GetMethod = (Scr_GetMethod_t)0x0811595C;
#elif COD_VERSION == COD2_1_2
static const Scr_GetMethod_t Scr_GetMethod = (Scr_GetMethod_t)0x08117C8E;
#elif COD_VERSION == COD2_1_3
static const Scr_GetMethod_t Scr_GetMethod = (Scr_GetMethod_t)0x08117DEA;
#endif

typedef dvar_t * (*Dvar_FindVar_t)(const char *var_name);
#if COD_VERSION == COD2_1_0
static const Dvar_FindVar_t Dvar_FindVar = (Dvar_FindVar_t)0x080B0AE4;
#elif COD_VERSION == COD2_1_2
static const Dvar_FindVar_t Dvar_FindVar = (Dvar_FindVar_t)0x080B2D94;
#elif COD_VERSION == COD2_1_3
static const Dvar_FindVar_t Dvar_FindVar = (Dvar_FindVar_t)0x080B2ED8;
#endif

typedef byte (*Dvar_GetBool_t)(const char *dvarName);
#if COD_VERSION == COD2_1_0
static const Dvar_GetBool_t Dvar_GetBool = (Dvar_GetBool_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Dvar_GetBool_t Dvar_GetBool = (Dvar_GetBool_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Dvar_GetBool_t Dvar_GetBool = (Dvar_GetBool_t)0x080B2FAA;
#endif

typedef int (*Dvar_GetInt_t)(const char *dvarName);
#if COD_VERSION == COD2_1_0
static const Dvar_GetInt_t Dvar_GetInt = (Dvar_GetInt_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Dvar_GetInt_t Dvar_GetInt = (Dvar_GetInt_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Dvar_GetInt_t Dvar_GetInt = (Dvar_GetInt_t)0x080B2FFC;
#endif

typedef char * (*Dvar_GetString_t)(const char *dvarName);
#if COD_VERSION == COD2_1_0
static const Dvar_GetString_t Dvar_GetString = (Dvar_GetString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Dvar_GetString_t Dvar_GetString = (Dvar_GetString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Dvar_GetString_t Dvar_GetString = (Dvar_GetString_t)0x080B318A;
#endif

typedef void (*Dvar_SetBool_t)(dvar_t *dvar, byte value);
#if COD_VERSION == COD2_1_0
static const Dvar_SetBool_t Dvar_SetBool = (Dvar_SetBool_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Dvar_SetBool_t Dvar_SetBool = (Dvar_SetBool_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Dvar_SetBool_t Dvar_SetBool = (Dvar_SetBool_t)0x080B4980;
#endif

typedef void (*Dvar_SetInt_t)(dvar_t *dvar, int value);
#if COD_VERSION == COD2_1_0
static const Dvar_SetInt_t Dvar_SetInt = (Dvar_SetInt_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Dvar_SetInt_t Dvar_SetInt = (Dvar_SetInt_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Dvar_SetInt_t Dvar_SetInt = (Dvar_SetInt_t)0x080B49AA;
#endif

typedef void (*Dvar_SetString_t)(dvar_t *dvar, const char *value);
#if COD_VERSION == COD2_1_0
static const Dvar_SetString_t Dvar_SetString = (Dvar_SetString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Dvar_SetString_t Dvar_SetString = (Dvar_SetString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Dvar_SetString_t Dvar_SetString = (Dvar_SetString_t)0x080B4A80;
#endif

typedef void (*Dvar_SetStringByName_t)(const char *dvarName, const char *value);
#if COD_VERSION == COD2_1_0
static const Dvar_SetStringByName_t Dvar_SetStringByName = (Dvar_SetStringByName_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Dvar_SetStringByName_t Dvar_SetStringByName = (Dvar_SetStringByName_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Dvar_SetStringByName_t Dvar_SetStringByName = (Dvar_SetStringByName_t)0x080B4E86;
#endif

typedef dvar_t * (*Dvar_RegisterBool_t)(const char *var_name, qboolean var_value, unsigned short flags);
#if COD_VERSION == COD2_1_0
static const Dvar_RegisterBool_t Dvar_RegisterBool = (Dvar_RegisterBool_t)0x080B1B72;
#elif COD_VERSION == COD2_1_2
static const Dvar_RegisterBool_t Dvar_RegisterBool = (Dvar_RegisterBool_t)0x080B3E8E;
#elif COD_VERSION == COD2_1_3
static const Dvar_RegisterBool_t Dvar_RegisterBool = (Dvar_RegisterBool_t)0x080B3FD2;
#endif

typedef dvar_t * (*Dvar_RegisterString_t)(const char *var_name, const char *var_value, unsigned short flags);
#if COD_VERSION == COD2_1_0
static const Dvar_RegisterString_t Dvar_RegisterString = (Dvar_RegisterString_t)0x080B1DD2;
#elif COD_VERSION == COD2_1_2
static const Dvar_RegisterString_t Dvar_RegisterString = (Dvar_RegisterString_t)0x080B40EE;
#elif COD_VERSION == COD2_1_3
static const Dvar_RegisterString_t Dvar_RegisterString = (Dvar_RegisterString_t)0x080B4232;
#endif

typedef dvar_t * (*Dvar_RegisterInt_t)(const char * var_name, int var_value, int min_value, int max_value, unsigned short flags);
#if COD_VERSION == COD2_1_0
static const Dvar_RegisterInt_t Dvar_RegisterInt = (Dvar_RegisterInt_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Dvar_RegisterInt_t Dvar_RegisterInt = (Dvar_RegisterInt_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Dvar_RegisterInt_t Dvar_RegisterInt = (Dvar_RegisterInt_t)0x080B403A;
#endif

typedef dvar_t * (*Dvar_RegisterFloat_t)(const char* var_name, float var_value, float var_min, float var_max, unsigned short flags);
#if COD_VERSION == COD2_1_0
static const Dvar_RegisterFloat_t Dvar_RegisterFloat = (Dvar_RegisterFloat_t)0x080B1C2C;
#elif COD_VERSION == COD2_1_2
static const Dvar_RegisterFloat_t Dvar_RegisterFloat = (Dvar_RegisterFloat_t)0x080B3F48;
#elif COD_VERSION == COD2_1_3
static const Dvar_RegisterFloat_t Dvar_RegisterFloat = (Dvar_RegisterFloat_t)0x080B408C;
#endif

typedef void (*Dvar_ResetScriptInfo_t)(void);
#if COD_VERSION == COD2_1_0
static const Dvar_ResetScriptInfo_t Dvar_ResetScriptInfo = (Dvar_ResetScriptInfo_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Dvar_ResetScriptInfo_t Dvar_ResetScriptInfo = (Dvar_ResetScriptInfo_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Dvar_ResetScriptInfo_t Dvar_ResetScriptInfo = (Dvar_ResetScriptInfo_t)0x08093478;
#endif

typedef char * (*Dvar_InfoString_t)(unsigned short bit);
#if COD_VERSION == COD2_1_0
static const Dvar_InfoString_t Dvar_InfoString = (Dvar_InfoString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Dvar_InfoString_t Dvar_InfoString = (Dvar_InfoString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Dvar_InfoString_t Dvar_InfoString = (Dvar_InfoString_t)0x08064188;
#endif

typedef void (*SV_ConnectionlessPacket_t)(netadr_t from, msg_t *msg);
#if COD_VERSION == COD2_1_0
static const SV_ConnectionlessPacket_t SV_ConnectionlessPacket = (SV_ConnectionlessPacket_t)0x08093F1E;
#elif COD_VERSION == COD2_1_2
static const SV_ConnectionlessPacket_t SV_ConnectionlessPacket = (SV_ConnectionlessPacket_t)0x08095894;
#elif COD_VERSION == COD2_1_3
static const SV_ConnectionlessPacket_t SV_ConnectionlessPacket = (SV_ConnectionlessPacket_t)0x0809594E;
#endif

typedef void (*SV_ExecuteClientMessage_t)(client_t *cl, msg_t *msg);
#if COD_VERSION == COD2_1_0
static const SV_ExecuteClientMessage_t SV_ExecuteClientMessage = (SV_ExecuteClientMessage_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_ExecuteClientMessage_t SV_ExecuteClientMessage = (SV_ExecuteClientMessage_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_ExecuteClientMessage_t SV_ExecuteClientMessage = (SV_ExecuteClientMessage_t)0x0809114E;
#endif

typedef void (*SV_ResetSkeletonCache_t)(void);
#if COD_VERSION == COD2_1_0
static const SV_ResetSkeletonCache_t SV_ResetSkeletonCache = (SV_ResetSkeletonCache_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_ResetSkeletonCache_t SV_ResetSkeletonCache = (SV_ResetSkeletonCache_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_ResetSkeletonCache_t SV_ResetSkeletonCache = (SV_ResetSkeletonCache_t)0x08091EDC;
#endif

typedef void (*SV_VerifyIwds_f_t)(client_t *cl);
#if COD_VERSION == COD2_1_0
static const SV_VerifyIwds_f_t SV_VerifyIwds_f = (SV_VerifyIwds_f_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_VerifyIwds_f_t SV_VerifyIwds_f = (SV_VerifyIwds_f_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_VerifyIwds_f_t SV_VerifyIwds_f = (SV_VerifyIwds_f_t)0x08090534;
#endif

typedef void (*SV_UserMove_t)(client_t *cl, msg_t *msg, qboolean delta);
#if COD_VERSION == COD2_1_0
static const SV_UserMove_t SV_UserMove = (SV_UserMove_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_UserMove_t SV_UserMove = (SV_UserMove_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_UserMove_t SV_UserMove = (SV_UserMove_t)0x08090E34;
#endif

typedef int (*NET_CompareBaseAdr_t)(netadr_t a,netadr_t b);
#if COD_VERSION == COD2_1_0
static const NET_CompareBaseAdr_t NET_CompareBaseAdr = (NET_CompareBaseAdr_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const NET_CompareBaseAdr_t NET_CompareBaseAdr = (NET_CompareBaseAdr_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const NET_CompareBaseAdr_t NET_CompareBaseAdr = (NET_CompareBaseAdr_t)0x0806C424;
#endif

typedef qboolean (*NET_IsLocalAddress_t)(netadr_t from);
#if COD_VERSION == COD2_1_0
static const NET_IsLocalAddress_t NET_IsLocalAddress = (NET_IsLocalAddress_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const NET_IsLocalAddress_t NET_IsLocalAddress = (NET_IsLocalAddress_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const NET_IsLocalAddress_t NET_IsLocalAddress = (NET_IsLocalAddress_t)0x0806C57A;
#endif

typedef void (*NET_OutOfBandPrint_t)(netsrc_t net_socket, netadr_t adr, const char *format, ...);
#if COD_VERSION == COD2_1_0
static const NET_OutOfBandPrint_t NET_OutOfBandPrint = (NET_OutOfBandPrint_t)0x0806C40C;
#elif COD_VERSION == COD2_1_2
static const NET_OutOfBandPrint_t NET_OutOfBandPrint = (NET_OutOfBandPrint_t)0x0806C8D4;
#elif COD_VERSION == COD2_1_3
static const NET_OutOfBandPrint_t NET_OutOfBandPrint = (NET_OutOfBandPrint_t)0x0806C8CC;
#endif

typedef void (*NET_Sleep_t)(int msec);
#if COD_VERSION == COD2_1_0
static const NET_Sleep_t NET_Sleep = (NET_Sleep_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const NET_Sleep_t NET_Sleep = (NET_Sleep_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const NET_Sleep_t NET_Sleep = (NET_Sleep_t)0x080d5CC4;
#endif

typedef void (*SV_GameSendServerCommand_t)(int clientnum, svscmd_type type, const char *text);
#if COD_VERSION == COD2_1_0
static const SV_GameSendServerCommand_t SV_GameSendServerCommand = (SV_GameSendServerCommand_t)0x0808FE96;
#elif COD_VERSION == COD2_1_2
static const SV_GameSendServerCommand_t SV_GameSendServerCommand = (SV_GameSendServerCommand_t)0x080916A6;
#elif COD_VERSION == COD2_1_3
static const SV_GameSendServerCommand_t SV_GameSendServerCommand = (SV_GameSendServerCommand_t)0x080917AA;
#endif

typedef void (*SV_SaveSystemInfo_t)(void);
#if COD_VERSION == COD2_1_0
static const SV_SaveSystemInfo_t SV_SaveSystemInfo = (SV_SaveSystemInfo_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_SaveSystemInfo_t SV_SaveSystemInfo = (SV_SaveSystemInfo_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_SaveSystemInfo_t SV_SaveSystemInfo = (SV_SaveSystemInfo_t)0x08093486;
#endif

typedef void (*SV_SendClientSnapshot_t)(client_t *cl);
#if COD_VERSION == COD2_1_0
static const SV_SendClientSnapshot_t SV_SendClientSnapshot = (SV_SendClientSnapshot_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_SendClientSnapshot_t SV_SendClientSnapshot = (SV_SendClientSnapshot_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_SendClientSnapshot_t SV_SendClientSnapshot = (SV_SendClientSnapshot_t)0x0809ADEA;
#endif

typedef void (QDECL *SV_SendServerCommand_t)(client_t *cl, int type, const char *fmt, ...);
#if COD_VERSION == COD2_1_0
static const SV_SendServerCommand_t SV_SendServerCommand = (SV_SendServerCommand_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_SendServerCommand_t SV_SendServerCommand = (SV_SendServerCommand_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_SendServerCommand_t SV_SendServerCommand = (SV_SendServerCommand_t)0x08094A10;
#endif

typedef void (*SV_ShutdownGameProgs_t)(void);
#if COD_VERSION == COD2_1_0
static const SV_ShutdownGameProgs_t SV_ShutdownGameProgs = (SV_ShutdownGameProgs_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_ShutdownGameProgs_t SV_ShutdownGameProgs = (SV_ShutdownGameProgs_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_ShutdownGameProgs_t SV_ShutdownGameProgs = (SV_ShutdownGameProgs_t)0x080924B6;
#endif

typedef void (*SV_FreeClientScriptId_t)(client_t *client);
#if COD_VERSION == COD2_1_0
static const SV_FreeClientScriptId_t SV_FreeClientScriptId = (SV_FreeClientScriptId_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_FreeClientScriptId_t SV_FreeClientScriptId = (SV_FreeClientScriptId_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_FreeClientScriptId_t SV_FreeClientScriptId = (SV_FreeClientScriptId_t)0x08096558;
#endif

typedef void (*SV_FreeClient_t)(client_t *client);
#if COD_VERSION == COD2_1_0
static const SV_FreeClient_t SV_FreeClient = (SV_FreeClient_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_FreeClient_t SV_FreeClient = (SV_FreeClient_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_FreeClient_t SV_FreeClient = (SV_FreeClient_t)0x0808E1EA;
#endif

typedef void (*SV_DropClient_t)(client_t *drop, const char *reason);
#if COD_VERSION == COD2_1_0
static const SV_DropClient_t SV_DropClient = (SV_DropClient_t)0x0808DC8C;
#elif COD_VERSION == COD2_1_2
static const SV_DropClient_t SV_DropClient = (SV_DropClient_t)0x0808EF9A;
#elif COD_VERSION == COD2_1_3
static const SV_DropClient_t SV_DropClient = (SV_DropClient_t)0x0808F02E;
#endif

typedef void (*ClientUserinfoChanged_t)(int clientNum);
#if COD_VERSION == COD2_1_0
static const ClientUserinfoChanged_t ClientUserinfoChanged = (ClientUserinfoChanged_t)0x080F6506;
#elif COD_VERSION == COD2_1_2
static const ClientUserinfoChanged_t ClientUserinfoChanged = (ClientUserinfoChanged_t)0x080F8B1A;
#elif COD_VERSION == COD2_1_3
static const ClientUserinfoChanged_t ClientUserinfoChanged = (ClientUserinfoChanged_t)0x080F8C5E;
#endif

typedef void (*SV_GetUserinfo_t)(int index, char *buffer, int bufferSize);
#if COD_VERSION == COD2_1_0
static const SV_GetUserinfo_t SV_GetUserinfo = (SV_GetUserinfo_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_GetUserinfo_t SV_GetUserinfo = (SV_GetUserinfo_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_GetUserinfo_t SV_GetUserinfo = (SV_GetUserinfo_t)0x08092C04;
#endif

typedef void (*SV_ClearServer_t)(void);
#if COD_VERSION == COD2_1_0
static const SV_ClearServer_t SV_ClearServer = (SV_ClearServer_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_ClearServer_t SV_ClearServer = (SV_ClearServer_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_ClearServer_t SV_ClearServer = (SV_ClearServer_t)0x080932B6;
#endif

typedef qboolean (*SV_ClientCommand_t)(client_t *cl, msg_t *msg);
#if COD_VERSION == COD2_1_0
static const SV_ClientCommand_t SV_ClientCommand = (SV_ClientCommand_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_ClientCommand_t SV_ClientCommand = (SV_ClientCommand_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_ClientCommand_t SV_ClientCommand = (SV_ClientCommand_t)0x08090BAC;
#endif

typedef void (*SV_SendClientGameState_t)(client_t *cl);
#if COD_VERSION == COD2_1_0
static const SV_SendClientGameState_t SV_SendClientGameState = (SV_SendClientGameState_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_SendClientGameState_t SV_SendClientGameState = (SV_SendClientGameState_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_SendClientGameState_t SV_SendClientGameState = (SV_SendClientGameState_t)0x0808F302;
#endif

typedef void (*SV_UpdateLastTimeMasterServerCommunicated_t)(netadr_t from);
#if COD_VERSION == COD2_1_0
static const SV_UpdateLastTimeMasterServerCommunicated_t SV_UpdateLastTimeMasterServerCommunicated = (SV_UpdateLastTimeMasterServerCommunicated_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_UpdateLastTimeMasterServerCommunicated_t SV_UpdateLastTimeMasterServerCommunicated = (SV_UpdateLastTimeMasterServerCommunicated_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_UpdateLastTimeMasterServerCommunicated_t SV_UpdateLastTimeMasterServerCommunicated = (SV_UpdateLastTimeMasterServerCommunicated_t)0x0808CF40;
#endif

typedef void (*SV_UpdateServerCommandsToClient_t)(client_t *client, msg_t *msg);
#if COD_VERSION == COD2_1_0
static const SV_UpdateServerCommandsToClient_t SV_UpdateServerCommandsToClient = (SV_UpdateServerCommandsToClient_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_UpdateServerCommandsToClient_t SV_UpdateServerCommandsToClient = (SV_UpdateServerCommandsToClient_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_UpdateServerCommandsToClient_t SV_UpdateServerCommandsToClient = (SV_UpdateServerCommandsToClient_t)0x080988B2;
#endif

typedef int (*SV_ClientHasClientMuted_t)(int clientnum, int mutedclientnum);
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

typedef char * (*Info_ValueForKey_t)(const char *s, const char *key);
#if COD_VERSION == COD2_1_0
static const Info_ValueForKey_t Info_ValueForKey = (Info_ValueForKey_t)0x080B5B30;
#elif COD_VERSION == COD2_1_2
static const Info_ValueForKey_t Info_ValueForKey = (Info_ValueForKey_t)0x080B7FC4;
#elif COD_VERSION == COD2_1_3
static const Info_ValueForKey_t Info_ValueForKey = (Info_ValueForKey_t)0x080B8108;
#endif

typedef short (*Scr_ExecThread_t)(int callbackHook, unsigned int numArgs);
#if COD_VERSION == COD2_1_0
static const Scr_ExecThread_t Scr_ExecThread = (Scr_ExecThread_t)0x0808398E;
#elif COD_VERSION == COD2_1_2
static const Scr_ExecThread_t Scr_ExecThread = (Scr_ExecThread_t)0x08083F0A;
#elif COD_VERSION == COD2_1_3
static const Scr_ExecThread_t Scr_ExecThread = (Scr_ExecThread_t)0x08083FD6;
#endif

typedef short (*Scr_ExecEntThread_t)(gentity_t* ent, int callbackHook, unsigned int numArgs);
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

typedef void (*Scr_ParseGameTypeList_t)(void);
#if COD_VERSION == COD2_1_0
static const Scr_ParseGameTypeList_t Scr_ParseGameTypeList = (Scr_ParseGameTypeList_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_ParseGameTypeList_t Scr_ParseGameTypeList = (Scr_ParseGameTypeList_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_ParseGameTypeList_t Scr_ParseGameTypeList = (Scr_ParseGameTypeList_t)0x08117F70;
#endif

typedef void (*SVC_RemoteCommand_t)(netadr_t from, msg_t *msg);
#if COD_VERSION == COD2_1_0
static const SVC_RemoteCommand_t SVC_RemoteCommand = (SVC_RemoteCommand_t)0x080951B4;
#elif COD_VERSION == COD2_1_2
static const SVC_RemoteCommand_t SVC_RemoteCommand = (SVC_RemoteCommand_t)0x080970CC;
#elif COD_VERSION == COD2_1_3
static const SVC_RemoteCommand_t SVC_RemoteCommand = (SVC_RemoteCommand_t)0x08097188;
#endif

typedef int (*SV_GetGuid_t)(int num);
#if COD_VERSION == COD2_1_0
static const SV_GetGuid_t SV_GetGuid = (SV_GetGuid_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_GetGuid_t SV_GetGuid = (SV_GetGuid_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_GetGuid_t SV_GetGuid = (SV_GetGuid_t)0x08092412;
#endif

typedef void (*SV_GetChallenge_t)(netadr_t from);
#if COD_VERSION == COD2_1_0
static const SV_GetChallenge_t SV_GetChallenge = (SV_GetChallenge_t)0x0808BE54;
#elif COD_VERSION == COD2_1_2
static const SV_GetChallenge_t SV_GetChallenge = (SV_GetChallenge_t)0x0808D0C2;
#elif COD_VERSION == COD2_1_3
static const SV_GetChallenge_t SV_GetChallenge = (SV_GetChallenge_t)0x0808D18E;
#endif

typedef void (*SV_DirectConnect_t)(netadr_t from);
#if COD_VERSION == COD2_1_0
static const SV_DirectConnect_t SV_DirectConnect = (SV_DirectConnect_t)0x0808D0E6;
#elif COD_VERSION == COD2_1_2
static const SV_DirectConnect_t SV_DirectConnect = (SV_DirectConnect_t)0x0808E1EA;
#elif COD_VERSION == COD2_1_3
static const SV_DirectConnect_t SV_DirectConnect = (SV_DirectConnect_t)0x0808E2AA;
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

typedef const char * (*NET_AdrToString_t)(netadr_t a);
#if COD_VERSION == COD2_1_0
static const NET_AdrToString_t NET_AdrToString = (NET_AdrToString_t)0x0806AD14;
#elif COD_VERSION == COD2_1_2
static const NET_AdrToString_t NET_AdrToString = (NET_AdrToString_t)0x0806B1DC;
#elif COD_VERSION == COD2_1_3
static const NET_AdrToString_t NET_AdrToString = (NET_AdrToString_t)0x0806B1D4;
#endif

typedef qboolean (*NET_StringToAdr_t)(char *s, netadr_t *a);
#if COD_VERSION == COD2_1_0
static const NET_StringToAdr_t NET_StringToAdr = (NET_StringToAdr_t)0x0806C750;
#elif COD_VERSION == COD2_1_2
static const NET_StringToAdr_t NET_StringToAdr = (NET_StringToAdr_t)0x0806CCCC;
#elif COD_VERSION == COD2_1_3
static const NET_StringToAdr_t NET_StringToAdr = (NET_StringToAdr_t)0x0806CD98;
#endif

typedef qboolean (*NET_CompareAdr_t)(netadr_t a, netadr_t b);
#if COD_VERSION == COD2_1_0
static const NET_CompareAdr_t NET_CompareAdr = (NET_CompareAdr_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const NET_CompareAdr_t NET_CompareAdr = (NET_CompareAdr_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const NET_CompareAdr_t NET_CompareAdr = (NET_CompareAdr_t)0x0806C54C;
#endif

typedef void (*Scr_ObjectError_t)(const char *error);
#if COD_VERSION == COD2_1_0
static const Scr_ObjectError_t Scr_ObjectError = (Scr_ObjectError_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_ObjectError_t Scr_ObjectError = (Scr_ObjectError_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_ObjectError_t Scr_ObjectError = (Scr_ObjectError_t)0x08085478;
#endif

typedef void (*Scr_Error_t)(const char *string);
#if COD_VERSION == COD2_1_0
static const Scr_Error_t Scr_Error = (Scr_Error_t)0x08084DB4;
#elif COD_VERSION == COD2_1_2
static const Scr_Error_t Scr_Error = (Scr_Error_t)0x08085330;
#elif COD_VERSION == COD2_1_3
static const Scr_Error_t Scr_Error = (Scr_Error_t)0x080853FC;
#endif

typedef int (*G_FindConfigstringIndex_t)(const char *name, int start, int max, qboolean create, const char *fieldname);
#if COD_VERSION == COD2_1_0
static const G_FindConfigstringIndex_t G_FindConfigstringIndex = (G_FindConfigstringIndex_t)0x0811AE70;
#elif COD_VERSION == COD2_1_2
static const G_FindConfigstringIndex_t G_FindConfigstringIndex = (G_FindConfigstringIndex_t)0x0811D1A4;
#elif COD_VERSION == COD2_1_3
static const G_FindConfigstringIndex_t G_FindConfigstringIndex = (G_FindConfigstringIndex_t)0x0811D300;
#endif

typedef const char * (*SV_GetConfigstringConst_t)(int index);
#if COD_VERSION == COD2_1_0
static const SV_GetConfigstringConst_t SV_GetConfigstringConst = (SV_GetConfigstringConst_t)0x08091108;
#elif COD_VERSION == COD2_1_2
static const SV_GetConfigstringConst_t SV_GetConfigstringConst = (SV_GetConfigstringConst_t)0x08092918;
#elif COD_VERSION == COD2_1_3
static const SV_GetConfigstringConst_t SV_GetConfigstringConst = (SV_GetConfigstringConst_t)0x08092A1C;
#endif

typedef void (*SV_FreeConfigstrings_t)(void);
#if COD_VERSION == COD2_1_0
static const SV_FreeConfigstrings_t SV_FreeConfigstrings = (SV_FreeConfigstrings_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_FreeConfigstrings_t SV_FreeConfigstrings = (SV_FreeConfigstrings_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_FreeConfigstrings_t SV_FreeConfigstrings = (SV_FreeConfigstrings_t)0x080932B6;
#endif

typedef void (*SV_SetConfigstring_t)(int index, const char *val);
#if COD_VERSION == COD2_1_0
static const SV_SetConfigstring_t SV_SetConfigstring = (SV_SetConfigstring_t)0x08090E6C;
#elif COD_VERSION == COD2_1_2
static const SV_SetConfigstring_t SV_SetConfigstring = (SV_SetConfigstring_t)0x0809267C;
#elif COD_VERSION == COD2_1_3
static const SV_SetConfigstring_t SV_SetConfigstring = (SV_SetConfigstring_t)0x08092780;
#endif

typedef void (*SV_SetExpectedHunkUsage_t)(char *mapname);
#if COD_VERSION == COD2_1_0
static const SV_SetExpectedHunkUsage_t SV_SetExpectedHunkUsage = (SV_SetExpectedHunkUsage_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_SetExpectedHunkUsage_t SV_SetExpectedHunkUsage = (SV_SetExpectedHunkUsage_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_SetExpectedHunkUsage_t SV_SetExpectedHunkUsage = (SV_SetExpectedHunkUsage_t)0x080931A8;
#endif

typedef void (*SV_SetGametype_t)(void);
#if COD_VERSION == COD2_1_0
static const SV_SetGametype_t SV_SetGametype = (SV_SetGametype_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_SetGametype_t SV_SetGametype = (SV_SetGametype_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_SetGametype_t SV_SetGametype = (SV_SetGametype_t)0x080924FA;
#endif

typedef void * (*Z_MallocInternal_t)(int size);
#if COD_VERSION == COD2_1_0
static const Z_MallocInternal_t Z_MallocInternal = (Z_MallocInternal_t)0x080A92FA;
#elif COD_VERSION == COD2_1_2
static const Z_MallocInternal_t Z_MallocInternal = (Z_MallocInternal_t)0x080AB51A;
#elif COD_VERSION == COD2_1_3
static const Z_MallocInternal_t Z_MallocInternal = (Z_MallocInternal_t)0x080AB65E;
#endif

typedef void (*Z_FreeInternal_t)(void *ptr);
#if COD_VERSION == COD2_1_0
static const Z_FreeInternal_t Z_FreeInternal = (Z_FreeInternal_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Z_FreeInternal_t Z_FreeInternal = (Z_FreeInternal_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Z_FreeInternal_t Z_FreeInternal = (Z_FreeInternal_t)0x080AB5B8;
#endif

typedef int (*FS_Read_t)(void *buffer, int len, fileHandle_t f);
#if COD_VERSION == COD2_1_0
static const FS_Read_t FS_Read = (FS_Read_t)0x0809E328;
#elif COD_VERSION == COD2_1_2
static const FS_Read_t FS_Read = (FS_Read_t)0x080A03E6;
#elif COD_VERSION == COD2_1_3
static const FS_Read_t FS_Read = (FS_Read_t)0x080A052A;
#endif

typedef void (*MSG_Init_t)(msg_t *buf, byte *data, int length);
#if COD_VERSION == COD2_1_0
static const MSG_Init_t MSG_Init = (MSG_Init_t)0x08067718;
#elif COD_VERSION == COD2_1_2
static const MSG_Init_t MSG_Init = (MSG_Init_t)0x08067BE0;
#elif COD_VERSION == COD2_1_3
static const MSG_Init_t MSG_Init = (MSG_Init_t)0x08067BD8;
#endif

typedef void (*MSG_WriteBit0_t)(msg_t *msg);
#if COD_VERSION == COD2_1_0
static const MSG_WriteBit0_t MSG_WriteBit0 = (MSG_WriteBit0_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const MSG_WriteBit0_t MSG_WriteBit0 = (MSG_WriteBit0_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const MSG_WriteBit0_t MSG_WriteBit0 = (MSG_WriteBit0_t)0x08067d28;
#endif

typedef void (*MSG_WriteBit1_t)(msg_t *msg);
#if COD_VERSION == COD2_1_0
static const MSG_WriteBit1_t MSG_WriteBit1 = (MSG_WriteBit1_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const MSG_WriteBit1_t MSG_WriteBit1 = (MSG_WriteBit1_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const MSG_WriteBit1_t MSG_WriteBit1 = (MSG_WriteBit1_t)0x08067d86;
#endif

typedef void (*MSG_WriteBits_t)(msg_t *msg, int value, int bits);
#if COD_VERSION == COD2_1_0
static const MSG_WriteBits_t MSG_WriteBits = (MSG_WriteBits_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const MSG_WriteBits_t MSG_WriteBits = (MSG_WriteBits_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const MSG_WriteBits_t MSG_WriteBits = (MSG_WriteBits_t)0x08067c62;
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

typedef void (*MSG_WriteAngle16_t)(msg_t *msg, float f);
#if COD_VERSION == COD2_1_0
static const MSG_WriteAngle16_t MSG_WriteAngle16 = (MSG_WriteAngle16_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const MSG_WriteAngle16_t MSG_WriteAngle16 = (MSG_WriteAngle16_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const MSG_WriteAngle16_t MSG_WriteAngle16 = (MSG_WriteAngle16_t)0x08068366;
#endif

typedef void (*MSG_WriteBigString_t)(msg_t *msg, const char *s);
#if COD_VERSION == COD2_1_0
static const MSG_WriteBigString_t MSG_WriteBigString = (MSG_WriteBigString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const MSG_WriteBigString_t MSG_WriteBigString = (MSG_WriteBigString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const MSG_WriteBigString_t MSG_WriteBigString = (MSG_WriteBigString_t)0x08068256;
#endif

typedef void (*MSG_WriteString_t)(msg_t *msg, const char *s);
#if COD_VERSION == COD2_1_0
static const MSG_WriteString_t MSG_WriteString = (MSG_WriteString_t)0x08067CE4;
#elif COD_VERSION == COD2_1_2
static const MSG_WriteString_t MSG_WriteString = (MSG_WriteString_t)0x080681AC;
#elif COD_VERSION == COD2_1_3
static const MSG_WriteString_t MSG_WriteString = (MSG_WriteString_t)0x080681A4;
#endif

typedef void (*MSG_WriteData_t)(msg_t *msg, const void *data, int length);
#if COD_VERSION == COD2_1_0
static const MSG_WriteData_t MSG_WriteData = (MSG_WriteData_t)0x08067B84;
#elif COD_VERSION == COD2_1_2
static const MSG_WriteData_t MSG_WriteData = (MSG_WriteData_t)0x0806804C;
#elif COD_VERSION == COD2_1_3
static const MSG_WriteData_t MSG_WriteData = (MSG_WriteData_t)0x08068044;
#endif

typedef void (*MSG_WriteDeltaObjective_t)(msg_t *msg, objective_t *from, objective_t *to, int lc, int numStateFields, netField_t *objFields);
#if COD_VERSION == COD2_1_0
static const MSG_WriteDeltaObjective_t MSG_WriteDeltaObjective = (MSG_WriteDeltaObjective_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const MSG_WriteDeltaObjective_t MSG_WriteDeltaObjective = (MSG_WriteDeltaObjective_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const MSG_WriteDeltaObjective_t MSG_WriteDeltaObjective = (MSG_WriteDeltaObjective_t)0x08069602;
#endif

typedef void (*MSG_WriteDeltaHudElems_t)(msg_t *buf, hudelem_t *from, hudelem_t *to, int count);
#if COD_VERSION == COD2_1_0
static const MSG_WriteDeltaHudElems_t MSG_WriteDeltaHudElems = (MSG_WriteDeltaHudElems_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const MSG_WriteDeltaHudElems_t MSG_WriteDeltaHudElems = (MSG_WriteDeltaHudElems_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const MSG_WriteDeltaHudElems_t MSG_WriteDeltaHudElems = (MSG_WriteDeltaHudElems_t)0x08069f84;
#endif

typedef void (*MSG_BeginReading_t)(msg_t *buf);
#if COD_VERSION == COD2_1_0
static const MSG_BeginReading_t MSG_BeginReading = (MSG_BeginReading_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const MSG_BeginReading_t MSG_BeginReading = (MSG_BeginReading_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const MSG_BeginReading_t MSG_BeginReading = (MSG_BeginReading_t)0x08067C1C;
#endif

typedef int (*MSG_ReadBits_t)(msg_t *msg, int numBits);
#if COD_VERSION == COD2_1_0
static const MSG_ReadBits_t MSG_ReadBits = (MSG_ReadBits_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const MSG_ReadBits_t MSG_ReadBits = (MSG_ReadBits_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const MSG_ReadBits_t MSG_ReadBits = (MSG_ReadBits_t)0x08067E20;
#endif

typedef int (*MSG_ReadByte_t)(msg_t *msg);
#if COD_VERSION == COD2_1_0
static const MSG_ReadByte_t MSG_ReadByte = (MSG_ReadByte_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const MSG_ReadByte_t MSG_ReadByte = (MSG_ReadByte_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const MSG_ReadByte_t MSG_ReadByte = (MSG_ReadByte_t)0x080683A8;
#endif

typedef int (*MSG_ReadShort_t)(msg_t *msg);
#if COD_VERSION == COD2_1_0
static const MSG_ReadShort_t MSG_ReadShort = (MSG_ReadShort_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const MSG_ReadShort_t MSG_ReadShort = (MSG_ReadShort_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const MSG_ReadShort_t MSG_ReadShort = (MSG_ReadShort_t)0x080683F2;
#endif

typedef int (*MSG_ReadLong_t)(msg_t *msg);
#if COD_VERSION == COD2_1_0
static const MSG_ReadLong_t MSG_ReadLong = (MSG_ReadLong_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const MSG_ReadLong_t MSG_ReadLong = (MSG_ReadLong_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const MSG_ReadLong_t MSG_ReadLong = (MSG_ReadLong_t)0x08068450;
#endif

typedef char * (*MSG_ReadCommandString_t)(msg_t *msg);
#if COD_VERSION == COD2_1_0
static const MSG_ReadCommandString_t MSG_ReadCommandString = (MSG_ReadCommandString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const MSG_ReadCommandString_t MSG_ReadCommandString = (MSG_ReadCommandString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const MSG_ReadCommandString_t MSG_ReadCommandString = (MSG_ReadCommandString_t)0x08068522;
#endif

typedef char * (*MSG_ReadStringLine_t)(msg_t *msg);
#if COD_VERSION == COD2_1_0
static const MSG_ReadStringLine_t MSG_ReadStringLine = (MSG_ReadStringLine_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const MSG_ReadStringLine_t MSG_ReadStringLine = (MSG_ReadStringLine_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const MSG_ReadStringLine_t MSG_ReadStringLine = (MSG_ReadStringLine_t)0x08068606;
#endif

typedef void (*SV_ClientEnterWorld_t)(client_t *cl, usercmd_t *cmd);
#if COD_VERSION == COD2_1_0
static const SV_ClientEnterWorld_t SV_ClientEnterWorld = (SV_ClientEnterWorld_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_ClientEnterWorld_t SV_ClientEnterWorld = (SV_ClientEnterWorld_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_ClientEnterWorld_t SV_ClientEnterWorld = (SV_ClientEnterWorld_t)0x0808F628;
#endif

typedef void (*SV_ExecuteClientCommand_t)(client_t *cl, char *s, qboolean clientOK);
#if COD_VERSION == COD2_1_0
static const SV_ExecuteClientCommand_t SV_ExecuteClientCommand = (SV_ExecuteClientCommand_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_ExecuteClientCommand_t SV_ExecuteClientCommand = (SV_ExecuteClientCommand_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_ExecuteClientCommand_t SV_ExecuteClientCommand = (SV_ExecuteClientCommand_t)0x08090B14;
#endif

typedef void (*SV_SendMessageToClient_t)(msg_t *msg, client_t *cl);
#if COD_VERSION == COD2_1_0
static const SV_SendMessageToClient_t SV_SendMessageToClient = (SV_SendMessageToClient_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_SendMessageToClient_t SV_SendMessageToClient = (SV_SendMessageToClient_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_SendMessageToClient_t SV_SendMessageToClient = (SV_SendMessageToClient_t)0x0809ABA2;
#endif

typedef void (*SV_Netchan_AddOOBProfilePacket_t)(int iLength);
#if COD_VERSION == COD2_1_0
static const SV_Netchan_AddOOBProfilePacket_t SV_Netchan_AddOOBProfilePacket = (SV_Netchan_AddOOBProfilePacket_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_Netchan_AddOOBProfilePacket_t SV_Netchan_AddOOBProfilePacket = (SV_Netchan_AddOOBProfilePacket_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_Netchan_AddOOBProfilePacket_t SV_Netchan_AddOOBProfilePacket = (SV_Netchan_AddOOBProfilePacket_t)0x0809766E;
#endif

typedef void (*SV_Netchan_Decode_t)(client_t *client, byte *data, int remaining);
#if COD_VERSION == COD2_1_0
static const SV_Netchan_Decode_t SV_Netchan_Decode = (SV_Netchan_Decode_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_Netchan_Decode_t SV_Netchan_Decode = (SV_Netchan_Decode_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_Netchan_Decode_t SV_Netchan_Decode = (SV_Netchan_Decode_t)0x08097562;
#endif

typedef void (*SV_Netchan_TransmitNextFragment_t)(netchan_t *chan);
#if COD_VERSION == COD2_1_0
static const SV_Netchan_TransmitNextFragment_t SV_Netchan_TransmitNextFragment = (SV_Netchan_TransmitNextFragment_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_Netchan_TransmitNextFragment_t SV_Netchan_TransmitNextFragment = (SV_Netchan_TransmitNextFragment_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_Netchan_TransmitNextFragment_t SV_Netchan_TransmitNextFragment = (SV_Netchan_TransmitNextFragment_t)0x08097610;
#endif

typedef qboolean (*Netchan_Process_t)(netchan_t *chan, msg_t *msg);
#if COD_VERSION == COD2_1_0
static const Netchan_Process_t Netchan_Process = (Netchan_Process_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Netchan_Process_t Netchan_Process = (Netchan_Process_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Netchan_Process_t Netchan_Process = (Netchan_Process_t)0x0806BE8A;
#endif

typedef void (*BG_GetSpreadForWeapon_t)(const playerState_s *ps, int weaponIndex, float *minSpread, float *maxSpread);
#if COD_VERSION == COD2_1_0
static const BG_GetSpreadForWeapon_t BG_GetSpreadForWeapon = (BG_GetSpreadForWeapon_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const BG_GetSpreadForWeapon_t BG_GetSpreadForWeapon = (BG_GetSpreadForWeapon_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const BG_GetSpreadForWeapon_t BG_GetSpreadForWeapon = (BG_GetSpreadForWeapon_t)0x080EC8CC;
#endif

typedef WeaponDef_t * (*BG_WeaponDefs_t)(unsigned int weaponIndex);
#if COD_VERSION == COD2_1_0
static const BG_WeaponDefs_t BG_WeaponDefs = (BG_WeaponDefs_t)0x080E9270;
#elif COD_VERSION == COD2_1_2
static const BG_WeaponDefs_t BG_WeaponDefs = (BG_WeaponDefs_t)0x080EB860;
#elif COD_VERSION == COD2_1_3
static const BG_WeaponDefs_t BG_WeaponDefs = (BG_WeaponDefs_t)0x080EB9A4;
#endif

typedef WeaponDef_t * (*BG_GetWeaponDef_t)(unsigned int weaponIndex);
#if COD_VERSION == COD2_1_0
static const BG_GetWeaponDef_t BG_GetWeaponDef = (BG_GetWeaponDef_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const BG_GetWeaponDef_t BG_GetWeaponDef = (BG_GetWeaponDef_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const BG_GetWeaponDef_t BG_GetWeaponDef = (BG_GetWeaponDef_t)0x080EB9A4;
#endif

typedef int (*BG_WeaponIsClipOnly_t)(unsigned int weapon);
#if COD_VERSION == COD2_1_0
static const BG_WeaponIsClipOnly_t BG_WeaponIsClipOnly = (BG_WeaponIsClipOnly_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const BG_WeaponIsClipOnly_t BG_WeaponIsClipOnly = (BG_WeaponIsClipOnly_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const BG_WeaponIsClipOnly_t BG_WeaponIsClipOnly = (BG_WeaponIsClipOnly_t)0x080ED3E2;
#endif

typedef int (*BG_AmmoForWeapon_t)(unsigned int weapon);
#if COD_VERSION == COD2_1_0
static const BG_AmmoForWeapon_t BG_AmmoForWeapon = (BG_AmmoForWeapon_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const BG_AmmoForWeapon_t BG_AmmoForWeapon = (BG_AmmoForWeapon_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const BG_AmmoForWeapon_t BG_AmmoForWeapon = (BG_AmmoForWeapon_t)0x080ED3C8;
#endif

typedef int (*BG_ClipForWeapon_t)(unsigned int weapon);
#if COD_VERSION == COD2_1_0
static const BG_ClipForWeapon_t BG_ClipForWeapon = (BG_ClipForWeapon_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const BG_ClipForWeapon_t BG_ClipForWeapon = (BG_ClipForWeapon_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const BG_ClipForWeapon_t BG_ClipForWeapon = (BG_ClipForWeapon_t)0x080ED3AE;
#endif

typedef int (*BG_GetAmmoClipSize_t)(unsigned int weapon);
#if COD_VERSION == COD2_1_0
static const BG_GetAmmoClipSize_t BG_GetAmmoClipSize = (BG_GetAmmoClipSize_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const BG_GetAmmoClipSize_t BG_GetAmmoClipSize = (BG_GetAmmoClipSize_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const BG_GetAmmoClipSize_t BG_GetAmmoClipSize = (BG_GetAmmoClipSize_t)0x080EBA8A;
#endif

typedef int (*BG_GetAmmoTypeMax_t)(unsigned int weapon);
#if COD_VERSION == COD2_1_0
static const BG_GetAmmoTypeMax_t BG_GetAmmoTypeMax = (BG_GetAmmoTypeMax_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const BG_GetAmmoTypeMax_t BG_GetAmmoTypeMax = (BG_GetAmmoTypeMax_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const BG_GetAmmoTypeMax_t BG_GetAmmoTypeMax = (BG_GetAmmoTypeMax_t)0x080EBA6A;
#endif

typedef int (*BG_FindWeaponIndexForName_t)(const char *name);
#if COD_VERSION == COD2_1_0
static const BG_FindWeaponIndexForName_t BG_FindWeaponIndexForName = (BG_FindWeaponIndexForName_t)0x080E949C;
#elif COD_VERSION == COD2_1_2
static const BG_FindWeaponIndexForName_t BG_FindWeaponIndexForName = (BG_FindWeaponIndexForName_t)0x080EBA8C;
#elif COD_VERSION == COD2_1_3
static const BG_FindWeaponIndexForName_t BG_FindWeaponIndexForName = (BG_FindWeaponIndexForName_t)0x080EBBD0;
#endif

typedef int (*BG_GetNumWeapons_t)(void);
#if COD_VERSION == COD2_1_0
static const BG_GetNumWeapons_t BG_GetNumWeapons = (BG_GetNumWeapons_t)0x080E9322;
#elif COD_VERSION == COD2_1_2
static const BG_GetNumWeapons_t BG_GetNumWeapons = (BG_GetNumWeapons_t)0x080EB912;
#elif COD_VERSION == COD2_1_3
static const BG_GetNumWeapons_t BG_GetNumWeapons = (BG_GetNumWeapons_t)0x080EBA56;
#endif

typedef void (*Weapon_Melee_t)(gentity_t *inflictor, weaponParms *wp, float range, float width, float height);
#if COD_VERSION == COD2_1_0
static const Weapon_Melee_t Weapon_Melee = (Weapon_Melee_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Weapon_Melee_t Weapon_Melee = (Weapon_Melee_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Weapon_Melee_t Weapon_Melee = (Weapon_Melee_t)0x0811F844;
#endif

typedef gentity_t * (*weapon_grenadelauncher_fire_t)(gentity_t *player, int weaponIndex, weaponParms *wp);
#if COD_VERSION == COD2_1_0
static const weapon_grenadelauncher_fire_t weapon_grenadelauncher_fire = (weapon_grenadelauncher_fire_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const weapon_grenadelauncher_fire_t weapon_grenadelauncher_fire = (weapon_grenadelauncher_fire_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const weapon_grenadelauncher_fire_t weapon_grenadelauncher_fire = (weapon_grenadelauncher_fire_t)0x08120552;
#endif

typedef void (*Weapon_RocketLauncher_Fire_t)(gentity_t *player, float spread, weaponParms *wp);
#if COD_VERSION == COD2_1_0
static const Weapon_RocketLauncher_Fire_t Weapon_RocketLauncher_Fire = (Weapon_RocketLauncher_Fire_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Weapon_RocketLauncher_Fire_t Weapon_RocketLauncher_Fire = (Weapon_RocketLauncher_Fire_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Weapon_RocketLauncher_Fire_t Weapon_RocketLauncher_Fire = (Weapon_RocketLauncher_Fire_t)0x0812061A;
#endif

typedef void (*FireWeaponAntiLag_t)(gentity_t *player, int time);
#if COD_VERSION == COD2_1_0
static const FireWeaponAntiLag_t FireWeaponAntiLag = (FireWeaponAntiLag_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const FireWeaponAntiLag_t FireWeaponAntiLag = (FireWeaponAntiLag_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const FireWeaponAntiLag_t FireWeaponAntiLag = (FireWeaponAntiLag_t)0x08120870;
#endif

typedef qboolean (*BG_IsWeaponIndexValid_t)(int index); // Guessed function name
#if COD_VERSION == COD2_1_0
static const BG_IsWeaponIndexValid_t BG_IsWeaponIndexValid = (BG_IsWeaponIndexValid_t)0x080E9758;
#elif COD_VERSION == COD2_1_2
static const BG_IsWeaponIndexValid_t BG_IsWeaponIndexValid = (BG_IsWeaponIndexValid_t)0x080EBD48;
#elif COD_VERSION == COD2_1_3
static const BG_IsWeaponIndexValid_t BG_IsWeaponIndexValid = (BG_IsWeaponIndexValid_t)0x080EBE8C;
#endif

typedef qboolean (*COM_BitCheck_t)(unsigned int array[], int bitNum);
#if COD_VERSION == COD2_1_0
static const COM_BitCheck_t COM_BitCheck = (COM_BitCheck_t)0x080D9E84;
#elif COD_VERSION == COD2_1_2
static const COM_BitCheck_t COM_BitCheck = (COM_BitCheck_t)0x080DC464;
#elif COD_VERSION == COD2_1_3
static const COM_BitCheck_t COM_BitCheck = (COM_BitCheck_t)0x080DC5A8;
#endif

typedef void (*RuntimeError_Debug_t)(int channel, const char *pos, int error_index, const char *error_message);
#if COD_VERSION == COD2_1_0
static const RuntimeError_Debug_t RuntimeError_Debug = (RuntimeError_Debug_t)0x0807818C;
#elif COD_VERSION == COD2_1_2
static const RuntimeError_Debug_t RuntimeError_Debug = (RuntimeError_Debug_t)0x08078710;
#elif COD_VERSION == COD2_1_3
static const RuntimeError_Debug_t RuntimeError_Debug = (RuntimeError_Debug_t)0x080787DC;
#endif

typedef void (*RuntimeError_t)(const char *pos, int error_index, const char *error_message, const char *dialog_error_message);
#if COD_VERSION == COD2_1_0
static const RuntimeError_t RuntimeError = (RuntimeError_t)0x08078282;
#elif COD_VERSION == COD2_1_2
static const RuntimeError_t RuntimeError = (RuntimeError_t)0x08078806;
#elif COD_VERSION == COD2_1_3
static const RuntimeError_t RuntimeError = (RuntimeError_t)0x080788D2;
#endif

typedef void (*PM_AddEvent_t)(playerState_t *ps, entity_event_t newEvent);
#if COD_VERSION == COD2_1_0
static const PM_AddEvent_t PM_AddEvent = (PM_AddEvent_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const PM_AddEvent_t PM_AddEvent = (PM_AddEvent_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const PM_AddEvent_t PM_AddEvent = (PM_AddEvent_t)0x080E1452;
#endif

typedef StanceState (*PM_GetEffectiveStance_t)(struct playerState_s *ps);
#if COD_VERSION == COD2_1_0
static const PM_GetEffectiveStance_t PM_GetEffectiveStance = (PM_GetEffectiveStance_t)0x080DEE0E;
#elif COD_VERSION == COD2_1_2
static const PM_GetEffectiveStance_t PM_GetEffectiveStance = (PM_GetEffectiveStance_t)0x080E13EE;
#elif COD_VERSION == COD2_1_3
static const PM_GetEffectiveStance_t PM_GetEffectiveStance = (PM_GetEffectiveStance_t)0x080E1532;
#endif

typedef unsigned int (*PM_GroundSurfaceType_t)(pml_t *pml);
#if COD_VERSION == COD2_1_0
static const PM_GroundSurfaceType_t PM_GroundSurfaceType = (PM_GroundSurfaceType_t)0x080DFAFC;
#elif COD_VERSION == COD2_1_2
static const PM_GroundSurfaceType_t PM_GroundSurfaceType = (PM_GroundSurfaceType_t)0x080E20DC;
#elif COD_VERSION == COD2_1_3
static const PM_GroundSurfaceType_t PM_GroundSurfaceType = (PM_GroundSurfaceType_t)0x080E2220;
#endif

typedef int (*BG_AnimScriptEvent_t)(playerState_t *ps, scriptAnimEventTypes_t event, int isContinue, int force);
#if COD_VERSION == COD2_1_0
static const BG_AnimScriptEvent_t BG_AnimScriptEvent = (BG_AnimScriptEvent_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const BG_AnimScriptEvent_t BG_AnimScriptEvent = (BG_AnimScriptEvent_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const BG_AnimScriptEvent_t BG_AnimScriptEvent = (BG_AnimScriptEvent_t)0x080D96EE;
#endif

typedef int (*BG_AnimationIndexForString_t)(char *src);
#if COD_VERSION == COD2_1_0
static const BG_AnimationIndexForString_t BG_AnimationIndexForString = (BG_AnimationIndexForString_t)0x080D46AC;
#elif COD_VERSION == COD2_1_2
static const BG_AnimationIndexForString_t BG_AnimationIndexForString = (BG_AnimationIndexForString_t)0x080D6C8C;
#elif COD_VERSION == COD2_1_3
static const BG_AnimationIndexForString_t BG_AnimationIndexForString = (BG_AnimationIndexForString_t)0x080D6DD0;
#endif

typedef void (*Scr_ResetTimeout_t)(void);
#if COD_VERSION == COD2_1_0
static const Scr_ResetTimeout_t Scr_ResetTimeout = (Scr_ResetTimeout_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_ResetTimeout_t Scr_ResetTimeout = (Scr_ResetTimeout_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_ResetTimeout_t Scr_ResetTimeout = (Scr_ResetTimeout_t)0x08085628;
#endif

typedef void (*Scr_PlayerConnect_t)(gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const Scr_PlayerConnect_t Scr_PlayerConnect = (Scr_PlayerConnect_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_PlayerConnect_t Scr_PlayerConnect = (Scr_PlayerConnect_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_PlayerConnect_t Scr_PlayerConnect = (Scr_PlayerConnect_t)0x08118350;
#endif

typedef void (*Scr_PlayerKilled_t)(gentity_t *self, gentity_t *eInflictor, gentity_t *eAttacker, int iDamage, meansOfDeath_t meansOfDeath, int iWeapon, const float *vDir, hitLocation_t hitLoc, int timeOffset, int deathAnimDuration);
#if COD_VERSION == COD2_1_0
static const Scr_PlayerKilled_t Scr_PlayerKilled = (Scr_PlayerKilled_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_PlayerKilled_t Scr_PlayerKilled = (Scr_PlayerKilled_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_PlayerKilled_t Scr_PlayerKilled = (Scr_PlayerKilled_t)0x08118492;
#endif

typedef void (*LookAtKiller_t)(gentity_t *self, gentity_t *inflictor, gentity_t *attacker);
#if COD_VERSION == COD2_1_0
static const LookAtKiller_t LookAtKiller = (LookAtKiller_t)0x080FF17A;
#elif COD_VERSION == COD2_1_2
static const LookAtKiller_t LookAtKiller = (LookAtKiller_t)0x081014AE;
#elif COD_VERSION == COD2_1_3
static const LookAtKiller_t LookAtKiller = (LookAtKiller_t)0x0810160A;
#endif

typedef int (*Scr_IsSystemActive_t)();
#if COD_VERSION == COD2_1_0
static const Scr_IsSystemActive_t Scr_IsSystemActive = (Scr_IsSystemActive_t)0x08084030;
#elif COD_VERSION == COD2_1_2
static const Scr_IsSystemActive_t Scr_IsSystemActive = (Scr_IsSystemActive_t)0x080845AC;
#elif COD_VERSION == COD2_1_3
static const Scr_IsSystemActive_t Scr_IsSystemActive = (Scr_IsSystemActive_t)0x08084678;
#endif

typedef int (*Scr_GetInt_t)(unsigned int param);
#if COD_VERSION == COD2_1_0
static const Scr_GetInt_t Scr_GetInt = (Scr_GetInt_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_GetInt_t Scr_GetInt = (Scr_GetInt_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_GetInt_t Scr_GetInt = (Scr_GetInt_t)0x08084694;
#endif

typedef float (*Scr_GetFloat_t)(unsigned int param);
#if COD_VERSION == COD2_1_0
static const Scr_GetFloat_t Scr_GetFloat = (Scr_GetFloat_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_GetFloat_t Scr_GetFloat = (Scr_GetFloat_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_GetFloat_t Scr_GetFloat = (Scr_GetFloat_t)0x08084968;
#endif

typedef void (*Scr_GetVector_t)(unsigned int param, vec3_t *vec);
#if COD_VERSION == COD2_1_0
static const Scr_GetVector_t Scr_GetVector = (Scr_GetVector_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_GetVector_t Scr_GetVector = (Scr_GetVector_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_GetVector_t Scr_GetVector = (Scr_GetVector_t)0x08084D40;
#endif

typedef const char * (*Scr_GetString_t)(unsigned int param);
#if COD_VERSION == COD2_1_0
static const Scr_GetString_t Scr_GetString = (Scr_GetString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_GetString_t Scr_GetString = (Scr_GetString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_GetString_t Scr_GetString = (Scr_GetString_t)0x08084BB2;
#endif

typedef int (*Scr_GetType_t)(unsigned int param);
#if COD_VERSION == COD2_1_0
static const Scr_GetType_t Scr_GetType = (Scr_GetType_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_GetType_t Scr_GetType = (Scr_GetType_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_GetType_t Scr_GetType = (Scr_GetType_t)0x08084FF0;
#endif

typedef int (*Scr_GetPointerType_t)(unsigned int param);
#if COD_VERSION == COD2_1_0
static const Scr_GetPointerType_t Scr_GetPointerType = (Scr_GetPointerType_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_GetPointerType_t Scr_GetPointerType = (Scr_GetPointerType_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_GetPointerType_t Scr_GetPointerType = (Scr_GetPointerType_t)0x08085098;
#endif

typedef gentity_t * (*G_GetEntity_t)(unsigned int index); // Guessed function name
#if COD_VERSION == COD2_1_0
static const G_GetEntity_t G_GetEntity = (G_GetEntity_t)0x0810E03A;
#elif COD_VERSION == COD2_1_2
static const G_GetEntity_t G_GetEntity = (G_GetEntity_t)0x08110376;
#elif COD_VERSION == COD2_1_3
static const G_GetEntity_t G_GetEntity = (G_GetEntity_t)0x081104D2;
#endif

typedef int (*G_GetSavePersist_t)(void);
#if COD_VERSION == COD2_1_0
static const G_GetSavePersist_t G_GetSavePersist = (G_GetSavePersist_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_GetSavePersist_t G_GetSavePersist = (G_GetSavePersist_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_GetSavePersist_t G_GetSavePersist = (G_GetSavePersist_t)0x08108DEE;
#endif

typedef int (*G_GetClientScore_t)(int clientNum);
#if COD_VERSION == COD2_1_0
static const G_GetClientScore_t G_GetClientScore = (G_GetClientScore_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_GetClientScore_t G_GetClientScore = (G_GetClientScore_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_GetClientScore_t G_GetClientScore = (G_GetClientScore_t)0x08108E1A;
#endif

typedef gentity_t * (*Scr_GetEntity_t)(unsigned int index);
#if COD_VERSION == COD2_1_0
static const Scr_GetEntity_t Scr_GetEntity = (Scr_GetEntity_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_GetEntity_t Scr_GetEntity = (Scr_GetEntity_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_GetEntity_t Scr_GetEntity = (Scr_GetEntity_t)0x0811B16E;
#endif

typedef unsigned short (*Scr_GetConstString_t)(unsigned int param);
#if COD_VERSION == COD2_1_0
static const Scr_GetConstString_t Scr_GetConstString = (Scr_GetConstString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_GetConstString_t Scr_GetConstString = (Scr_GetConstString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_GetConstString_t Scr_GetConstString = (Scr_GetConstString_t)0x08084A08;
#endif

typedef unsigned int (*Scr_GetConstLowercaseString_t)(unsigned int param);
#if COD_VERSION == COD2_1_0
static const Scr_GetConstLowercaseString_t Scr_GetConstLowercaseString = (Scr_GetConstLowercaseString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_GetConstLowercaseString_t Scr_GetConstLowercaseString = (Scr_GetConstLowercaseString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_GetConstLowercaseString_t Scr_GetConstLowercaseString = (Scr_GetConstLowercaseString_t)0x08084A7C;
#endif

typedef void (*Scr_ConstructMessageString_t)(unsigned int firstParmIndex, unsigned int lastParmIndex, const char *errorContext, char *string, unsigned int stringLimit);
#if COD_VERSION == COD2_1_0
static const Scr_ConstructMessageString_t Scr_ConstructMessageString = (Scr_ConstructMessageString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_ConstructMessageString_t Scr_ConstructMessageString = (Scr_ConstructMessageString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_ConstructMessageString_t Scr_ConstructMessageString = (Scr_ConstructMessageString_t)0x08110720;
#endif

typedef void (*IncInParam_t)(void);
#if COD_VERSION == COD2_1_0
static const IncInParam_t IncInParam = (IncInParam_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const IncInParam_t IncInParam = (IncInParam_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const IncInParam_t IncInParam = (IncInParam_t)0x08083DF8;
#endif

typedef void (*Scr_AddBool_t)(qboolean value);
#if COD_VERSION == COD2_1_0
static const Scr_AddBool_t Scr_AddBool = (Scr_AddBool_t)0x08084AF8;
#elif COD_VERSION == COD2_1_2
static const Scr_AddBool_t Scr_AddBool = (Scr_AddBool_t)0x08085074;
#elif COD_VERSION == COD2_1_3
static const Scr_AddBool_t Scr_AddBool = (Scr_AddBool_t)0x08085140;
#endif

typedef void (*Scr_AddInt_t)(int value);
#if COD_VERSION == COD2_1_0
static const Scr_AddInt_t Scr_AddInt = (Scr_AddInt_t)0x08084B1C;
#elif COD_VERSION == COD2_1_2
static const Scr_AddInt_t Scr_AddInt = (Scr_AddInt_t)0x08085098;
#elif COD_VERSION == COD2_1_3
static const Scr_AddInt_t Scr_AddInt = (Scr_AddInt_t)0x08085164;
#endif

typedef void (*Scr_AddFloat_t)(float value);
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

typedef void (*Scr_AddConstString_t)(unsigned int stringValue);
#if COD_VERSION == COD2_1_0
static const Scr_AddConstString_t Scr_AddConstString = (Scr_AddConstString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_AddConstString_t Scr_AddConstString = (Scr_AddConstString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_AddConstString_t Scr_AddConstString = (Scr_AddConstString_t)0x080852D6;
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

typedef void (*Scr_AddEntity_t)(gentity_t* ent);
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

typedef void (*Scr_AddArrayStringIndexed_t)(unsigned int stringValue);
#if COD_VERSION == COD2_1_0
static const Scr_AddArrayStringIndexed_t Scr_AddArrayStringIndexed = (Scr_AddArrayStringIndexed_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_AddArrayStringIndexed_t Scr_AddArrayStringIndexed = (Scr_AddArrayStringIndexed_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_AddArrayStringIndexed_t Scr_AddArrayStringIndexed = (Scr_AddArrayStringIndexed_t)0x080853B6;
#endif

typedef void (*Scr_AddObject_t)(unsigned int object);
#if COD_VERSION == COD2_1_0
static const Scr_AddObject_t Scr_AddObject = (Scr_AddObject_t)0x08084BA2;
#elif COD_VERSION == COD2_1_2
static const Scr_AddObject_t Scr_AddObject = (Scr_AddObject_t)0x0808511E;
#elif COD_VERSION == COD2_1_3
static const Scr_AddObject_t Scr_AddObject = (Scr_AddObject_t)0x080851EA;
#endif

typedef void (*Scr_ParamError_t)(int paramNum, const char *error);
#if COD_VERSION == COD2_1_0
static const Scr_ParamError_t Scr_ParamError = (Scr_ParamError_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_ParamError_t Scr_ParamError = (Scr_ParamError_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_ParamError_t Scr_ParamError = (Scr_ParamError_t)0x0808545C;
#endif

typedef void (*Scr_PlayerDamage_t)(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int dFlags, meansOfDeath_t meansOfDeath, int iWeapon, const float *vPoint, const float *vDir, hitLocation_t hitLoc, int timeOffset);
#if COD_VERSION == COD2_1_0
static const Scr_PlayerDamage_t Scr_PlayerDamage = (Scr_PlayerDamage_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_PlayerDamage_t Scr_PlayerDamage = (Scr_PlayerDamage_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_PlayerDamage_t Scr_PlayerDamage = (Scr_PlayerDamage_t)0x081183B8;
#endif

typedef qboolean (*Scr_IsInOpcodeMemory_t)(const char *pos);
#if COD_VERSION == COD2_1_0
static const Scr_IsInOpcodeMemory_t Scr_IsInOpcodeMemory = (Scr_IsInOpcodeMemory_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_IsInOpcodeMemory_t Scr_IsInOpcodeMemory = (Scr_IsInOpcodeMemory_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_IsInOpcodeMemory_t Scr_IsInOpcodeMemory = (Scr_IsInOpcodeMemory_t)0x08075E14;
#endif

typedef unsigned int (*Scr_GetSourceBuffer_t)(const char *codePos);
#if COD_VERSION == COD2_1_0
static const Scr_GetSourceBuffer_t Scr_GetSourceBuffer = (Scr_GetSourceBuffer_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_GetSourceBuffer_t Scr_GetSourceBuffer = (Scr_GetSourceBuffer_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_GetSourceBuffer_t Scr_GetSourceBuffer = (Scr_GetSourceBuffer_t)0x0807839A;
#endif

typedef unsigned int (*Scr_GetPrevSourcePos_t)(const char *codePos, unsigned int index);
#if COD_VERSION == COD2_1_0
static const Scr_GetPrevSourcePos_t Scr_GetPrevSourcePos = (Scr_GetPrevSourcePos_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_GetPrevSourcePos_t Scr_GetPrevSourcePos = (Scr_GetPrevSourcePos_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_GetPrevSourcePos_t Scr_GetPrevSourcePos = (Scr_GetPrevSourcePos_t)0x08077B6A;
#endif

typedef void (*Scr_PrintSourcePos_t)(conChannel_t channel, const char *filename, const char *buf, unsigned int sourcePos);
#if COD_VERSION == COD2_1_0
static const Scr_PrintSourcePos_t Scr_PrintSourcePos = (Scr_PrintSourcePos_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_PrintSourcePos_t Scr_PrintSourcePos = (Scr_PrintSourcePos_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_PrintSourcePos_t Scr_PrintSourcePos = (Scr_PrintSourcePos_t)0x080781D6;
#endif

typedef int (*DirToByte_t)(vec3_t dir);
#if COD_VERSION == COD2_1_0
static const DirToByte_t DirToByte = (DirToByte_t)0x080A1C2A;
#elif COD_VERSION == COD2_1_2
static const DirToByte_t DirToByte = (DirToByte_t)0x080A3E4A;
#elif COD_VERSION == COD2_1_3
static const DirToByte_t DirToByte = (DirToByte_t)0x080A3F8E;
#endif

typedef void (*G_PrintEntities_t)(void);
#if COD_VERSION == COD2_1_0
static const G_PrintEntities_t G_PrintEntities = (G_PrintEntities_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_PrintEntities_t G_PrintEntities = (G_PrintEntities_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_PrintEntities_t G_PrintEntities = (G_PrintEntities_t)0x0811E8D4;
#endif

typedef const char * (*G_ModelName_t)(int index);
#if COD_VERSION == COD2_1_0
static const G_ModelName_t G_ModelName = (G_ModelName_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_ModelName_t G_ModelName = (G_ModelName_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_ModelName_t G_ModelName = (G_ModelName_t)0x0811D5B4;
#endif

typedef void (*G_SpawnString_t)(const char *key, const char *defaultString, const char **out);
#if COD_VERSION == COD2_1_0
static const G_SpawnString_t G_SpawnString = (G_SpawnString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_SpawnString_t G_SpawnString = (G_SpawnString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_SpawnString_t G_SpawnString = (G_SpawnString_t)0x0811A3DC;
#endif

typedef qboolean (*G_ParseSpawnVars_t)(SpawnVar *spawnVar);
#if COD_VERSION == COD2_1_0
static const G_ParseSpawnVars_t G_ParseSpawnVars = (G_ParseSpawnVars_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_ParseSpawnVars_t G_ParseSpawnVars = (G_ParseSpawnVars_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_ParseSpawnVars_t G_ParseSpawnVars = (G_ParseSpawnVars_t)0x080F3FD2;
#endif

typedef gitem_t * (*G_GetItemForClassname_t)(const char *classname);
#if COD_VERSION == COD2_1_0
static const G_GetItemForClassname_t G_GetItemForClassname = (G_GetItemForClassname_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_GetItemForClassname_t G_GetItemForClassname = (G_GetItemForClassname_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_GetItemForClassname_t G_GetItemForClassname = (G_GetItemForClassname_t)0x0811A9BA;
#endif

typedef void (*G_SetOrigin_t)(gentity_t *ent, const float *origin);
#if COD_VERSION == COD2_1_0
static const G_SetOrigin_t G_SetOrigin = (G_SetOrigin_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_SetOrigin_t G_SetOrigin = (G_SetOrigin_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_SetOrigin_t G_SetOrigin = (G_SetOrigin_t)0x0811F3C6;
#endif

typedef void (*G_SetAngle_t)(gentity_t *ent, const float *angle);
#if COD_VERSION == COD2_1_0
static const G_SetAngle_t G_SetAngle = (G_SetAngle_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_SetAngle_t G_SetAngle = (G_SetAngle_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_SetAngle_t G_SetAngle = (G_SetAngle_t)0x0811F426;
#endif

typedef void (*G_ParseEntityField_t)(const char *key, const char *value, gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const G_ParseEntityField_t G_ParseEntityField = (G_ParseEntityField_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_ParseEntityField_t G_ParseEntityField = (G_ParseEntityField_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_ParseEntityField_t G_ParseEntityField = (G_ParseEntityField_t)0x0811A610;
#endif

typedef void (*G_SetEntityPlacement_t)(gentity_t *ent); // Guessed function name
#if COD_VERSION == COD2_1_0
static const G_SetEntityPlacement_t G_SetEntityPlacement = (G_SetEntityPlacement_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_SetEntityPlacement_t G_SetEntityPlacement = (G_SetEntityPlacement_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_SetEntityPlacement_t G_SetEntityPlacement = (G_SetEntityPlacement_t)0x0811A7B0;
#endif

typedef void (*G_SetModel_t)(gentity_t *ent, const char *modelName);
#if COD_VERSION == COD2_1_0
static const G_SetModel_t G_SetModel = (G_SetModel_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_SetModel_t G_SetModel = (G_SetModel_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_SetModel_t G_SetModel = (G_SetModel_t)0x0811D87A;
#endif

typedef char (*G_EntLinkToWithOffset_t)(gentity_t *ent, gentity_t *parent, unsigned int tagId, vec3_t *originOffset, vec3_t *anglesOffset);
#if COD_VERSION == COD2_1_0
static const G_EntLinkToWithOffset_t G_EntLinkToWithOffset = (G_EntLinkToWithOffset_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_EntLinkToWithOffset_t G_EntLinkToWithOffset = (G_EntLinkToWithOffset_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_EntLinkToWithOffset_t G_EntLinkToWithOffset = (G_EntLinkToWithOffset_t)0x0811DD4A;
#endif

typedef void (*G_GeneralLink_t)(gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const G_GeneralLink_t G_GeneralLink = (G_GeneralLink_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_GeneralLink_t G_GeneralLink = (G_GeneralLink_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_GeneralLink_t G_GeneralLink = (G_GeneralLink_t)0x0811E316;
#endif

typedef void (*G_SpawnItem_t)(gentity_t *ent, gitem_t *item);
#if COD_VERSION == COD2_1_0
static const G_SpawnItem_t G_SpawnItem = (G_SpawnItem_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_SpawnItem_t G_SpawnItem = (G_SpawnItem_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_SpawnItem_t G_SpawnItem = (G_SpawnItem_t)0x08107342;
#endif

typedef void (*G_AddEvent_t)(gentity_t *ent, int event, int eventParm);
#if COD_VERSION == COD2_1_0
static const G_AddEvent_t G_AddEvent = (G_AddEvent_t)0x0811CDA2;
#elif COD_VERSION == COD2_1_2
static const G_AddEvent_t G_AddEvent = (G_AddEvent_t)0x0811F0D6;
#elif COD_VERSION == COD2_1_3
static const G_AddEvent_t G_AddEvent = (G_AddEvent_t)0x0811F232;
#endif

typedef void (*G_AddPredictableEvent_t)(gentity_t *ent, int event, int eventParm);
#if COD_VERSION == COD2_1_0
static const G_AddPredictableEvent_t G_AddPredictableEvent = (G_AddPredictableEvent_t)0x0811CD6C;
#elif COD_VERSION == COD2_1_2
static const G_AddPredictableEvent_t G_AddPredictableEvent = (G_AddPredictableEvent_t)0x0811F0A0;
#elif COD_VERSION == COD2_1_3
static const G_AddPredictableEvent_t G_AddPredictableEvent = (G_AddPredictableEvent_t)0x0811F1FC;
#endif

typedef void (*G_ClientStopUsingTurret_t)(gentity_t *turret);
#if COD_VERSION == COD2_1_0
static const G_ClientStopUsingTurret_t G_ClientStopUsingTurret = (G_ClientStopUsingTurret_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_ClientStopUsingTurret_t G_ClientStopUsingTurret = (G_ClientStopUsingTurret_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_ClientStopUsingTurret_t G_ClientStopUsingTurret = (G_ClientStopUsingTurret_t)0x0810B9A4;
#endif

typedef qboolean (*G_IsPlaying_t)(gentity_t *player);
#if COD_VERSION == COD2_1_0
static const G_IsPlaying_t G_IsPlaying = (G_IsPlaying_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_IsPlaying_t G_IsPlaying = (G_IsPlaying_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_IsPlaying_t G_IsPlaying = (G_IsPlaying_t)0x080F9F4E;
#endif

typedef qboolean (*G_IsTurretUsable_t)(gentity_t *useEnt, gentity_t *playerEnt);
#if COD_VERSION == COD2_1_0
static const G_IsTurretUsable_t G_IsTurretUsable = (G_IsTurretUsable_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_IsTurretUsable_t G_IsTurretUsable = (G_IsTurretUsable_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_IsTurretUsable_t G_IsTurretUsable = (G_IsTurretUsable_t)0x0810C358;
#endif

typedef void (*G_CalcMuzzlePoints_t)(gentity_t *ent, weaponParms *wp);
#if COD_VERSION == COD2_1_0
static const G_CalcMuzzlePoints_t G_CalcMuzzlePoints = (G_CalcMuzzlePoints_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_CalcMuzzlePoints_t G_CalcMuzzlePoints = (G_CalcMuzzlePoints_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_CalcMuzzlePoints_t G_CalcMuzzlePoints = (G_CalcMuzzlePoints_t)0x081207EC;
#endif

typedef void (*G_GetPlayerViewOrigin_t)(gentity_t *ent, float *origin);
#if COD_VERSION == COD2_1_0
static const G_GetPlayerViewOrigin_t G_GetPlayerViewOrigin = (G_GetPlayerViewOrigin_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_GetPlayerViewOrigin_t G_GetPlayerViewOrigin = (G_GetPlayerViewOrigin_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_GetPlayerViewOrigin_t G_GetPlayerViewOrigin = (G_GetPlayerViewOrigin_t)0x080F8916;
#endif

typedef void (*G_GetPlayerViewDirection_t)(gentity_t *ent, float *forward, float *right, float *up);
#if COD_VERSION == COD2_1_0
static const G_GetPlayerViewDirection_t G_GetPlayerViewDirection = (G_GetPlayerViewDirection_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_GetPlayerViewDirection_t G_GetPlayerViewDirection = (G_GetPlayerViewDirection_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_GetPlayerViewDirection_t G_GetPlayerViewDirection = (G_GetPlayerViewDirection_t)0x080F8AF8;
#endif

typedef qboolean (*G_DObjGetWorldTagPos_t)(gentity_t *ent, unsigned int tagName, float *pos);
#if COD_VERSION == COD2_1_0
static const G_DObjGetWorldTagPos_t G_DObjGetWorldTagPos = (G_DObjGetWorldTagPos_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_DObjGetWorldTagPos_t G_DObjGetWorldTagPos = (G_DObjGetWorldTagPos_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_DObjGetWorldTagPos_t G_DObjGetWorldTagPos = (G_DObjGetWorldTagPos_t)0x0811E5FA;
#endif

typedef void (*G_DObjCalcPose_t)(gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const G_DObjCalcPose_t G_DObjCalcPose = (G_DObjCalcPose_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_DObjCalcPose_t G_DObjCalcPose = (G_DObjCalcPose_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_DObjCalcPose_t G_DObjCalcPose = (G_DObjCalcPose_t)0x0811E3C0;
#endif

typedef void (*G_DObjUpdate_t)(gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const G_DObjUpdate_t G_DObjUpdate = (G_DObjUpdate_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_DObjUpdate_t G_DObjUpdate = (G_DObjUpdate_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_DObjUpdate_t G_DObjUpdate = (G_DObjUpdate_t)0x0811D69E;
#endif

typedef playerState_t * (*SV_GameClientNum_t)(int num);
#if COD_VERSION == COD2_1_0
static const SV_GameClientNum_t SV_GameClientNum = (SV_GameClientNum_t)0x0808FE02;
#elif COD_VERSION == COD2_1_2
static const SV_GameClientNum_t SV_GameClientNum = (SV_GameClientNum_t)0x08091612;
#elif COD_VERSION == COD2_1_3
static const SV_GameClientNum_t SV_GameClientNum = (SV_GameClientNum_t)0x08091716;
#endif

typedef gentity_t * (*SV_GentityNum_t)(int num);
#if COD_VERSION == COD2_1_0
static const SV_GentityNum_t SV_GentityNum = (SV_GentityNum_t)0x0808FDE4;
#elif COD_VERSION == COD2_1_2
static const SV_GentityNum_t SV_GentityNum = (SV_GentityNum_t)0x080915F4;
#elif COD_VERSION == COD2_1_3
static const SV_GentityNum_t SV_GentityNum = (SV_GentityNum_t)0x080916F8;
#endif

typedef gentity_t * (*SV_ClientThink_t)(client_t *cl, usercmd_t *);
#if COD_VERSION == COD2_1_0
static const SV_ClientThink_t SV_ClientThink = (SV_ClientThink_t)0x0808F488;
#elif COD_VERSION == COD2_1_2
static const SV_ClientThink_t SV_ClientThink = (SV_ClientThink_t)0x08090D18;
#elif COD_VERSION == COD2_1_3
static const SV_ClientThink_t SV_ClientThink = (SV_ClientThink_t)0x08090DAC;
#endif

typedef int (*SV_GetArchivedClientInfo_t)(int clientNum, int *pArchiveTime, playerState_t *ps, clientState_t *cs);
#if COD_VERSION == COD2_1_0
static const SV_GetArchivedClientInfo_t SV_GetArchivedClientInfo = (SV_GetArchivedClientInfo_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_GetArchivedClientInfo_t SV_GetArchivedClientInfo = (SV_GetArchivedClientInfo_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_GetArchivedClientInfo_t SV_GetArchivedClientInfo = (SV_GetArchivedClientInfo_t)0x0809a0b6;
#endif

typedef qboolean (*G_ClientCanSpectateTeam_t)(gclient_t *client, int team);
#if COD_VERSION == COD2_1_0
static const G_ClientCanSpectateTeam_t G_ClientCanSpectateTeam = (G_ClientCanSpectateTeam_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_ClientCanSpectateTeam_t G_ClientCanSpectateTeam = (G_ClientCanSpectateTeam_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_ClientCanSpectateTeam_t G_ClientCanSpectateTeam = (G_ClientCanSpectateTeam_t)0x080f6dd0;
#endif

typedef void (*SV_Heartbeat_f_t)(void);
#if COD_VERSION == COD2_1_0
static const SV_Heartbeat_f_t SV_Heartbeat_f = (SV_Heartbeat_f_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_Heartbeat_f_t SV_Heartbeat_f = (SV_Heartbeat_f_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_Heartbeat_f_t SV_Heartbeat_f = (SV_Heartbeat_f_t)0x0808CB98;
#endif

typedef void (*G_LogPrintf_t)(const char *fmt, ...);
#if COD_VERSION == COD2_1_0
static const G_LogPrintf_t G_LogPrintf = (G_LogPrintf_t)0x08107502;
#elif COD_VERSION == COD2_1_2
static const G_LogPrintf_t G_LogPrintf = (G_LogPrintf_t)0x08109836;
#elif COD_VERSION == COD2_1_3
static const G_LogPrintf_t G_LogPrintf = (G_LogPrintf_t)0x08109992;
#endif

typedef void (*I_CleanStr_t)(char *str);
#if COD_VERSION == COD2_1_0
static const I_CleanStr_t I_CleanStr = (I_CleanStr_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const I_CleanStr_t I_CleanStr = (I_CleanStr_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const I_CleanStr_t I_CleanStr = (I_CleanStr_t)0x080B7E54;
#endif

typedef void (*I_strncpyz_t)(char *dest, const char *src, int destsize);
#if COD_VERSION == COD2_1_0
static const I_strncpyz_t I_strncpyz = (I_strncpyz_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const I_strncpyz_t I_strncpyz = (I_strncpyz_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const I_strncpyz_t I_strncpyz = (I_strncpyz_t)0x080B7AB6;
#endif

typedef int (*I_stricmp_t)(const char *s1, const char *s2);
#if COD_VERSION == COD2_1_0
static const I_stricmp_t I_stricmp = (I_stricmp_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const I_stricmp_t I_stricmp = (I_stricmp_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const I_stricmp_t I_stricmp = (I_stricmp_t)0x080B7BF8;
#endif

typedef int (*I_strncmp_t)(const char *s1, const char *s2, int n);
#if COD_VERSION == COD2_1_0
static const I_strncmp_t I_strncmp = (I_strncmp_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const I_strncmp_t I_strncmp = (I_strncmp_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const I_strncmp_t I_strncmp = (I_strncmp_t)0x080B7B88;
#endif

typedef int (*I_strnicmp_t)(const char *s1, const char *s2, int n);
#if COD_VERSION == COD2_1_0
static const I_strnicmp_t I_strnicmp = (I_strnicmp_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const I_strnicmp_t I_strnicmp = (I_strnicmp_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const I_strnicmp_t I_strnicmp = (I_strnicmp_t)0x080B7AE2;
#endif

typedef char * (*I_strlwr_t)(char *s1);
#if COD_VERSION == COD2_1_0
static const I_strlwr_t I_strlwr = (I_strlwr_t)0x080B573C;
#elif COD_VERSION == COD2_1_2
static const I_strlwr_t I_strlwr = (I_strlwr_t)0x080B7BD0;
#elif COD_VERSION == COD2_1_3
static const I_strlwr_t I_strlwr = (I_strlwr_t)0x080B7D14;
#endif

typedef char * (*I_strupr_t)(char *s1);
#if COD_VERSION == COD2_1_0
static const I_strupr_t I_strupr = (I_strupr_t)0x080B577E;
#elif COD_VERSION == COD2_1_2
static const I_strupr_t I_strupr = (I_strupr_t)0x080B7C12;
#elif COD_VERSION == COD2_1_3
static const I_strupr_t I_strupr = (I_strupr_t)0x080B7D56;
#endif

typedef void (*I_strncat_t)(char *dest, int size, const char *src);
#if COD_VERSION == COD2_1_0
static const I_strncat_t I_strncat = (I_strncat_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const I_strncat_t I_strncat = (I_strncat_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const I_strncat_t I_strncat = (I_strncat_t)0x080B7D98;
#endif

typedef void (*G_FreeEntity_t)(gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const G_FreeEntity_t G_FreeEntity = (G_FreeEntity_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_FreeEntity_t G_FreeEntity = (G_FreeEntity_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_FreeEntity_t G_FreeEntity = (G_FreeEntity_t)0x0811EE50;
#endif

typedef void (*G_InitGentity_t)(gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const G_InitGentity_t G_InitGentity = (G_InitGentity_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_InitGentity_t G_InitGentity = (G_InitGentity_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_InitGentity_t G_InitGentity = (G_InitGentity_t)0x0811E85C;
#endif

typedef gentity_t * (*G_Spawn_t)(void);
#if COD_VERSION == COD2_1_0
static const G_Spawn_t G_Spawn = (G_Spawn_t)0x0811C546;
#elif COD_VERSION == COD2_1_2
static const G_Spawn_t G_Spawn = (G_Spawn_t)0x0811E87A;
#elif COD_VERSION == COD2_1_3
static const G_Spawn_t G_Spawn = (G_Spawn_t)0x0811E9D6;
#endif

typedef gentity_t * (*G_TempEntity_t)(vec3_t origin, int event);
#if COD_VERSION == COD2_1_0
static const G_TempEntity_t G_TempEntity = (G_TempEntity_t)0x0811CB34;
#elif COD_VERSION == COD2_1_2
static const G_TempEntity_t G_TempEntity = (G_TempEntity_t)0x0811EE68;
#elif COD_VERSION == COD2_1_3
static const G_TempEntity_t G_TempEntity = (G_TempEntity_t)0x0811EFC4;
#endif

typedef void (*SV_LinkEntity_t)(gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const SV_LinkEntity_t SV_LinkEntity = (SV_LinkEntity_t)0x0809A45E;
#elif COD_VERSION == COD2_1_2
static const SV_LinkEntity_t SV_LinkEntity = (SV_LinkEntity_t)0x0809C4F6;
#elif COD_VERSION == COD2_1_3
static const SV_LinkEntity_t SV_LinkEntity = (SV_LinkEntity_t)0x0809C63A;
#endif

typedef void (*SV_UnlinkEntity_t)(gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const SV_UnlinkEntity_t SV_UnlinkEntity = (SV_UnlinkEntity_t)0x0809A3BA;
#elif COD_VERSION == COD2_1_2
static const SV_UnlinkEntity_t SV_UnlinkEntity = (SV_UnlinkEntity_t)0x0809C452;
#elif COD_VERSION == COD2_1_3
static const SV_UnlinkEntity_t SV_UnlinkEntity = (SV_UnlinkEntity_t)0x0809C596;
#endif

typedef void (*SV_SetBrushModel_t)(gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const SV_SetBrushModel_t SV_SetBrushModel = (SV_SetBrushModel_t)0x0808FF44;
#elif COD_VERSION == COD2_1_2
static const SV_SetBrushModel_t SV_SetBrushModel = (SV_SetBrushModel_t)0x08091754;
#elif COD_VERSION == COD2_1_3
static const SV_SetBrushModel_t SV_SetBrushModel = (SV_SetBrushModel_t)0x08091858;
#endif

typedef void (*Scr_SetString_t)(unsigned short *strindexptr, unsigned const stringindex);
#if COD_VERSION == COD2_1_0
static const Scr_SetString_t Scr_SetString = (Scr_SetString_t)0x08079790;
#elif COD_VERSION == COD2_1_2
static const Scr_SetString_t Scr_SetString = (Scr_SetString_t)0x08079D14;
#elif COD_VERSION == COD2_1_3
static const Scr_SetString_t Scr_SetString = (Scr_SetString_t)0x08079DE0;
#endif

typedef float (*BG_GetBobCycle_t)(gclient_t *client);
#if COD_VERSION == COD2_1_0
static const BG_GetBobCycle_t BG_GetBobCycle = (BG_GetBobCycle_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const BG_GetBobCycle_t BG_GetBobCycle = (BG_GetBobCycle_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const BG_GetBobCycle_t BG_GetBobCycle = (BG_GetBobCycle_t)0x080F0BDE;
#endif

typedef float (*BG_GetSpeed_t)(const playerState_t *client, int time);
#if COD_VERSION == COD2_1_0
static const BG_GetSpeed_t BG_GetSpeed = (BG_GetSpeed_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const BG_GetSpeed_t BG_GetSpeed = (BG_GetSpeed_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const BG_GetSpeed_t BG_GetSpeed = (BG_GetSpeed_t)0x080E9528;
#endif

typedef float QDECL (*BG_GetVerticalBobFactor_t)(const playerState_t *ps, float cycle, float speed, float maxAmp);
#if COD_VERSION == COD2_1_0
static const BG_GetVerticalBobFactor_t BG_GetVerticalBobFactor = (BG_GetVerticalBobFactor_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const BG_GetVerticalBobFactor_t BG_GetVerticalBobFactor = (BG_GetVerticalBobFactor_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const BG_GetVerticalBobFactor_t BG_GetVerticalBobFactor = (BG_GetVerticalBobFactor_t)0x080F0C3A;
#endif

typedef float QDECL (*BG_GetHorizontalBobFactor_t)(const struct playerState_s *ps, float cycle, float speed, float maxAmp);
#if COD_VERSION == COD2_1_0
static const BG_GetHorizontalBobFactor_t BG_GetHorizontalBobFactor = (BG_GetHorizontalBobFactor_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const BG_GetHorizontalBobFactor_t BG_GetHorizontalBobFactor = (BG_GetHorizontalBobFactor_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const BG_GetHorizontalBobFactor_t BG_GetHorizontalBobFactor = (BG_GetHorizontalBobFactor_t)0x080F0CFA;
#endif

typedef gitem_t * (*BG_FindItemForWeapon_t)(unsigned int weaponIndex);
#if COD_VERSION == COD2_1_0
static const BG_FindItemForWeapon_t BG_FindItemForWeapon = (BG_FindItemForWeapon_t)0x080DCAE6;
#elif COD_VERSION == COD2_1_2
static const BG_FindItemForWeapon_t BG_FindItemForWeapon = (BG_FindItemForWeapon_t)0x080DF0C6;
#elif COD_VERSION == COD2_1_3
static const BG_FindItemForWeapon_t BG_FindItemForWeapon = (BG_FindItemForWeapon_t)0x080DF20A;
#endif

typedef int (*BG_CanItemBeGrabbed_t)(entityState_t *ent, playerState_t *ps, int touch);
#if COD_VERSION == COD2_1_0
static const BG_CanItemBeGrabbed_t BG_CanItemBeGrabbed = (BG_CanItemBeGrabbed_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const BG_CanItemBeGrabbed_t BG_CanItemBeGrabbed = (BG_CanItemBeGrabbed_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const BG_CanItemBeGrabbed_t BG_CanItemBeGrabbed = (BG_CanItemBeGrabbed_t)0x080DF39E;
#endif

typedef int (*BG_GetItemHintString_t)(gclient_t *client, gentity_t *ent); // Guessed function name
#if COD_VERSION == COD2_1_0
static const BG_GetItemHintString_t BG_GetItemHintString = (BG_GetItemHintString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const BG_GetItemHintString_t BG_GetItemHintString = (BG_GetItemHintString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const BG_GetItemHintString_t BG_GetItemHintString = (BG_GetItemHintString_t)0x08121AAA;
#endif

typedef XModel * (*SV_XModelGet_t)(const char *name);
#if COD_VERSION == COD2_1_0
static const SV_XModelGet_t SV_XModelGet = (SV_XModelGet_t)0x08090534;
#elif COD_VERSION == COD2_1_2
static const SV_XModelGet_t SV_XModelGet = (SV_XModelGet_t)0x08091D44;
#elif COD_VERSION == COD2_1_3
static const SV_XModelGet_t SV_XModelGet = (SV_XModelGet_t)0x08091E48;
#endif

typedef XModel * (*SV_XModelForIndex_t)(unsigned int modelIndex);
#if COD_VERSION == COD2_1_0
static const SV_XModelForIndex_t SV_XModelForIndex = (SV_XModelForIndex_t)0x0811B0F8;
#elif COD_VERSION == COD2_1_2
static const SV_XModelForIndex_t SV_XModelForIndex = (SV_XModelForIndex_t)0x0811D42C;
#elif COD_VERSION == COD2_1_3
static const SV_XModelForIndex_t SV_XModelForIndex = (SV_XModelForIndex_t)0x0811D588;
#endif

typedef qboolean (*SV_DObjExists_t)(gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const SV_DObjExists_t SV_DObjExists = (SV_DObjExists_t)0x08090A5A;
#elif COD_VERSION == COD2_1_2
static const SV_DObjExists_t SV_DObjExists = (SV_DObjExists_t)0x0809226A;
#elif COD_VERSION == COD2_1_3
static const SV_DObjExists_t SV_DObjExists = (SV_DObjExists_t)0x0809236E;
#endif

typedef int (*Com_Milliseconds_t)(void);
#if COD_VERSION == COD2_1_0
static const Com_Milliseconds_t Com_Milliseconds = (Com_Milliseconds_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Com_Milliseconds_t Com_Milliseconds = (Com_Milliseconds_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Com_Milliseconds_t Com_Milliseconds = (Com_Milliseconds_t)0x08061B7C;
#endif

typedef void (*Com_Error_t)(errorParm_t code, const char *format, ...);
#if COD_VERSION == COD2_1_0
static const Com_Error_t Com_Error = (Com_Error_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Com_Error_t Com_Error = (Com_Error_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Com_Error_t Com_Error = (Com_Error_t)0x08061124;
#endif

typedef DObj * (*Com_GetServerDObj_t)(int entNum);
#if COD_VERSION == COD2_1_0
static const Com_GetServerDObj_t Com_GetServerDObj = (Com_GetServerDObj_t)0x0806289C;
#elif COD_VERSION == COD2_1_2
static const Com_GetServerDObj_t Com_GetServerDObj = (Com_GetServerDObj_t)0x08062BCC;
#elif COD_VERSION == COD2_1_3
static const Com_GetServerDObj_t Com_GetServerDObj = (Com_GetServerDObj_t)0x08062BC4;
#endif

typedef char * (*Com_SurfaceTypeToName_t)(int surfaceType);
#if COD_VERSION == COD2_1_0
static const Com_SurfaceTypeToName_t Com_SurfaceTypeToName = (Com_SurfaceTypeToName_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Com_SurfaceTypeToName_t Com_SurfaceTypeToName = (Com_SurfaceTypeToName_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Com_SurfaceTypeToName_t Com_SurfaceTypeToName = (Com_SurfaceTypeToName_t)0x080B948A;
#endif

typedef void (*Com_Restart_t)(void);
#if COD_VERSION == COD2_1_0
static const Com_Restart_t Com_Restart = (Com_Restart_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Com_Restart_t Com_Restart = (Com_Restart_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Com_Restart_t Com_Restart = (Com_Restart_t)0x080628FA;
#endif

typedef void (*SV_DObjDumpInfo_t)(gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const SV_DObjDumpInfo_t SV_DObjDumpInfo = (SV_DObjDumpInfo_t)0x08090584;
#elif COD_VERSION == COD2_1_2
static const SV_DObjDumpInfo_t SV_DObjDumpInfo = (SV_DObjDumpInfo_t)0x08091D94;
#elif COD_VERSION == COD2_1_3
static const SV_DObjDumpInfo_t SV_DObjDumpInfo = (SV_DObjDumpInfo_t)0x08091E98;
#endif

typedef qboolean (*DObjSkelExists_t)(DObj *dobj, int skelTimeStamp);
#if COD_VERSION == COD2_1_0
static const DObjSkelExists_t DObjSkelExists = (DObjSkelExists_t)0x080B83A2;
#elif COD_VERSION == COD2_1_2
static const DObjSkelExists_t DObjSkelExists = (DObjSkelExists_t)0x080BA836;
#elif COD_VERSION == COD2_1_3
static const DObjSkelExists_t DObjSkelExists = (DObjSkelExists_t)0x080BA97A;
#endif

typedef int (*SV_DObjGetBoneIndex_t)(gentity_t *ent, uint boneIndex);
#if COD_VERSION == COD2_1_0
static const SV_DObjGetBoneIndex_t SV_DObjGetBoneIndex = (SV_DObjGetBoneIndex_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_DObjGetBoneIndex_t SV_DObjGetBoneIndex = (SV_DObjGetBoneIndex_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_DObjGetBoneIndex_t SV_DObjGetBoneIndex = (SV_DObjGetBoneIndex_t)0x080921AC;
#endif

typedef void (*BG_AddPredictableEventToPlayerstate_t)(int newEvent, unsigned int eventParm, playerState_s *ps);
#if COD_VERSION == COD2_1_0
static const BG_AddPredictableEventToPlayerstate_t BG_AddPredictableEventToPlayerstate = (BG_AddPredictableEventToPlayerstate_t)0x080DD554;
#elif COD_VERSION == COD2_1_2
static const BG_AddPredictableEventToPlayerstate_t BG_AddPredictableEventToPlayerstate = (BG_AddPredictableEventToPlayerstate_t)0x080DFB34;
#elif COD_VERSION == COD2_1_3
static const BG_AddPredictableEventToPlayerstate_t BG_AddPredictableEventToPlayerstate = (BG_AddPredictableEventToPlayerstate_t)0x080DFC78;
#endif

typedef void (*Scr_Notify_t)(gentity_t *ent, unsigned short constString, unsigned int numArgs);
#if COD_VERSION == COD2_1_0
static const Scr_Notify_t Scr_Notify = (Scr_Notify_t)0x08118E4E;
#elif COD_VERSION == COD2_1_2
static const Scr_Notify_t Scr_Notify = (Scr_Notify_t)0x0811B182;
#elif COD_VERSION == COD2_1_3
static const Scr_Notify_t Scr_Notify = (Scr_Notify_t)0x0811B2DE;
#endif

typedef unsigned short (*GScr_AllocString_t)(const char *s);
#if COD_VERSION == COD2_1_0
static const GScr_AllocString_t GScr_AllocString = (GScr_AllocString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const GScr_AllocString_t GScr_AllocString = (GScr_AllocString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const GScr_AllocString_t GScr_AllocString = (GScr_AllocString_t)0x081101B4;
#endif
   
typedef int (*Pickup_Ammo_t)(gentity_t *item, gentity_t *entity);
#if COD_VERSION == COD2_1_0
static const Pickup_Ammo_t Pickup_Ammo = (Pickup_Ammo_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Pickup_Ammo_t Pickup_Ammo = (Pickup_Ammo_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Pickup_Ammo_t Pickup_Ammo = (Pickup_Ammo_t)0x08104E64;
#endif

typedef int (*Pickup_Weapon_t)(gentity_t *item, gentity_t *entity, entity_event_t *event, int touch);
#if COD_VERSION == COD2_1_0
static const Pickup_Weapon_t Pickup_Weapon = (Pickup_Weapon_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Pickup_Weapon_t Pickup_Weapon = (Pickup_Weapon_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Pickup_Weapon_t Pickup_Weapon = (Pickup_Weapon_t)0x08105072;
#endif

typedef int (*Pickup_Health_t)(gentity_t *item, gentity_t *entity);
#if COD_VERSION == COD2_1_0
static const Pickup_Health_t Pickup_Health = (Pickup_Health_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Pickup_Health_t Pickup_Health = (Pickup_Health_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Pickup_Health_t Pickup_Health = (Pickup_Health_t)0x081059C0;
#endif

typedef void (*LargeLocalConstructor_t)(LargeLocal *buf, int size);
#if COD_VERSION == COD2_1_0
static const LargeLocalConstructor_t LargeLocalConstructor = (LargeLocalConstructor_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const LargeLocalConstructor_t LargeLocalConstructor = (LargeLocalConstructor_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const LargeLocalConstructor_t LargeLocalConstructor = (LargeLocalConstructor_t)0x080AC58E;
#endif

typedef byte * (*LargeLocalGetBuf_t)(LargeLocal *buf);
#if COD_VERSION == COD2_1_0
static const LargeLocalGetBuf_t LargeLocalGetBuf = (LargeLocalGetBuf_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const LargeLocalGetBuf_t LargeLocalGetBuf = (LargeLocalGetBuf_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const LargeLocalGetBuf_t LargeLocalGetBuf = (LargeLocalGetBuf_t)0x080AC5D8;
#endif

typedef void (*LargeLocalDestructor_t)(LargeLocal *buf);
#if COD_VERSION == COD2_1_0
static const LargeLocalDestructor_t LargeLocalDestructor = (LargeLocalDestructor_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const LargeLocalDestructor_t LargeLocalDestructor = (LargeLocalDestructor_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const LargeLocalDestructor_t LargeLocalDestructor = (LargeLocalDestructor_t)0x080AC5C2;
#endif

typedef int (*G_GetWeaponIndexForName_t)(const char *name);
#if COD_VERSION == COD2_1_0
static const G_GetWeaponIndexForName_t G_GetWeaponIndexForName = (G_GetWeaponIndexForName_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_GetWeaponIndexForName_t G_GetWeaponIndexForName = (G_GetWeaponIndexForName_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_GetWeaponIndexForName_t G_GetWeaponIndexForName = (G_GetWeaponIndexForName_t)0x08120DD0;
#endif

typedef meansOfDeath_t (*G_IndexForMeansOfDeath_t)(const char *meansOfDeath);
#if COD_VERSION == COD2_1_0
static const G_IndexForMeansOfDeath_t G_IndexForMeansOfDeath = (G_IndexForMeansOfDeath_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_IndexForMeansOfDeath_t G_IndexForMeansOfDeath = (G_IndexForMeansOfDeath_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_IndexForMeansOfDeath_t G_IndexForMeansOfDeath = (G_IndexForMeansOfDeath_t)0x081016FA;
#endif

typedef hitLocation_t (*G_GetHitLocationIndexFromString_t)(unsigned short constString);
#if COD_VERSION == COD2_1_0
static const G_GetHitLocationIndexFromString_t G_GetHitLocationIndexFromString = (G_GetHitLocationIndexFromString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_GetHitLocationIndexFromString_t G_GetHitLocationIndexFromString = (G_GetHitLocationIndexFromString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_GetHitLocationIndexFromString_t G_GetHitLocationIndexFromString = (G_GetHitLocationIndexFromString_t)0x08102886;
#endif

typedef int (*G_GetHintStringIndex_t)(int *index, const char *str);
#if COD_VERSION == COD2_1_0
static const G_GetHintStringIndex_t G_GetHintStringIndex = (G_GetHintStringIndex_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_GetHintStringIndex_t G_GetHintStringIndex = (G_GetHintStringIndex_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_GetHintStringIndex_t G_GetHintStringIndex = (G_GetHintStringIndex_t)0x08112432;
#endif

typedef clientState_t * (*G_GetClientState_t)(int num);
#if COD_VERSION == COD2_1_0
static const G_GetClientState_t G_GetClientState = (G_GetClientState_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_GetClientState_t G_GetClientState = (G_GetClientState_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_GetClientState_t G_GetClientState = (G_GetClientState_t)0x08108E72;
#endif

typedef qboolean (*GetFollowPlayerState_t)(int clientNum, playerState_t *ps);
#if COD_VERSION == COD2_1_0
static const GetFollowPlayerState_t GetFollowPlayerState = (GetFollowPlayerState_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const GetFollowPlayerState_t GetFollowPlayerState = (GetFollowPlayerState_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const GetFollowPlayerState_t GetFollowPlayerState = (GetFollowPlayerState_t)0x080F6DEA;
#endif

typedef svEntity_t * (*SV_SvEntityForGentity_t)(gentity_t *gEnt);
#if COD_VERSION == COD2_1_0
static const SV_SvEntityForGentity_t SV_SvEntityForGentity = (SV_SvEntityForGentity_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_SvEntityForGentity_t SV_SvEntityForGentity = (SV_SvEntityForGentity_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_SvEntityForGentity_t SV_SvEntityForGentity = (SV_SvEntityForGentity_t)0x08091734;
#endif

typedef void (*AddLeanToPosition_t)(float *position, float fViewYaw, float fLeanFrac, float fViewRoll, float fLeanDist);
#if COD_VERSION == COD2_1_0
static const AddLeanToPosition_t AddLeanToPosition = (AddLeanToPosition_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const AddLeanToPosition_t AddLeanToPosition = (AddLeanToPosition_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const AddLeanToPosition_t AddLeanToPosition = (AddLeanToPosition_t)0x080B8C98;
#endif

typedef void (*SV_AddEntitiesVisibleFromPoint_t)(float *point, int clientNum, snapshotEntityNumbers_t *eNums);
#if COD_VERSION == COD2_1_0
static const SV_AddEntitiesVisibleFromPoint_t SV_AddEntitiesVisibleFromPoint = (SV_AddEntitiesVisibleFromPoint_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_AddEntitiesVisibleFromPoint_t SV_AddEntitiesVisibleFromPoint = (SV_AddEntitiesVisibleFromPoint_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_AddEntitiesVisibleFromPoint_t SV_AddEntitiesVisibleFromPoint = (SV_AddEntitiesVisibleFromPoint_t)0x08098B98;
#endif

typedef void (*SV_AddCachedEntitiesVisibleFromPoint_t)(int from_num_entities, int from_first_entity, float *origin, int clientNum, snapshotEntityNumbers_t *eNums);
#if COD_VERSION == COD2_1_0
static const SV_AddCachedEntitiesVisibleFromPoint_t SV_AddCachedEntitiesVisibleFromPoint = (SV_AddCachedEntitiesVisibleFromPoint_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_AddCachedEntitiesVisibleFromPoint_t SV_AddCachedEntitiesVisibleFromPoint = (SV_AddCachedEntitiesVisibleFromPoint_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_AddCachedEntitiesVisibleFromPoint_t SV_AddCachedEntitiesVisibleFromPoint = (SV_AddCachedEntitiesVisibleFromPoint_t)0x08098E32;
#endif

typedef int (*G_GetClientArchiveTime_t)(int clnum);
#if COD_VERSION == COD2_1_0
static const G_GetClientArchiveTime_t G_GetClientArchiveTime = (G_GetClientArchiveTime_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_GetClientArchiveTime_t G_GetClientArchiveTime = (G_GetClientArchiveTime_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_GetClientArchiveTime_t G_GetClientArchiveTime = (G_GetClientArchiveTime_t)0x08108E36;
#endif

typedef void (*G_SetClientArchiveTime_t)(int clnum, int time);
#if COD_VERSION == COD2_1_0
static const G_SetClientArchiveTime_t G_SetClientArchiveTime = (G_SetClientArchiveTime_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_SetClientArchiveTime_t G_SetClientArchiveTime = (G_SetClientArchiveTime_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_SetClientArchiveTime_t G_SetClientArchiveTime = (G_SetClientArchiveTime_t)0x08108E52;
#endif

typedef cachedSnapshot_t * (*SV_GetCachedSnapshot_t)(int *pArchiveTime);
#if COD_VERSION == COD2_1_0
static const SV_GetCachedSnapshot_t SV_GetCachedSnapshot = (SV_GetCachedSnapshot_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_GetCachedSnapshot_t SV_GetCachedSnapshot = (SV_GetCachedSnapshot_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_GetCachedSnapshot_t SV_GetCachedSnapshot = (SV_GetCachedSnapshot_t)0x08099C04;
#endif

typedef XModel * (*CM_XModelPrecache_t)(const char *src);
#if COD_VERSION == COD2_1_0
static const CM_XModelPrecache_t CM_XModelPrecache = (CM_XModelPrecache_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const CM_XModelPrecache_t CM_XModelPrecache = (CM_XModelPrecache_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const CM_XModelPrecache_t CM_XModelPrecache = (CM_XModelPrecache_t)0x08058170;
#endif

typedef void (*CM_InitStaticModel_t)(cStaticModel_t *model, float *origin, float *angles, float (*scale) [3]);
#if COD_VERSION == COD2_1_0
static const CM_InitStaticModel_t CM_InitStaticModel = (CM_InitStaticModel_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const CM_InitStaticModel_t CM_InitStaticModel = (CM_InitStaticModel_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const CM_InitStaticModel_t CM_InitStaticModel = (CM_InitStaticModel_t)0x080583DC;
#endif

typedef void (*CM_LinkWorld_t)(void);
#if COD_VERSION == COD2_1_0
static const CM_LinkWorld_t CM_LinkWorld = (CM_LinkWorld_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const CM_LinkWorld_t CM_LinkWorld = (CM_LinkWorld_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const CM_LinkWorld_t CM_LinkWorld = (CM_LinkWorld_t)0x0805DC92;
#endif

typedef void (*CM_LoadMap_t)(const char *name, int *checksum);
#if COD_VERSION == COD2_1_0
static const CM_LoadMap_t CM_LoadMap = (CM_LoadMap_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const CM_LoadMap_t CM_LoadMap = (CM_LoadMap_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const CM_LoadMap_t CM_LoadMap = (CM_LoadMap_t)0x080521A2;
#endif

typedef qboolean (*CM_TraceBox_t)(traceExtents_t *extents, float *mins, float *maxs, float fraction);
#if COD_VERSION == COD2_1_0
static const CM_TraceBox_t CM_TraceBox = (CM_TraceBox_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const CM_TraceBox_t CM_TraceBox = (CM_TraceBox_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const CM_TraceBox_t CM_TraceBox = (CM_TraceBox_t)0x0805D8E0;
#endif

typedef qboolean (*CM_TransformedBoxTrace_t)(trace_t *results, const float *start, const float *end, const float *mins, const float *maxs, unsigned int model, int brushmask, const float *origin, const float *angles);
#if COD_VERSION == COD2_1_0
static const CM_TransformedBoxTrace_t CM_TransformedBoxTrace = (CM_TransformedBoxTrace_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const CM_TransformedBoxTrace_t CM_TransformedBoxTrace = (CM_TransformedBoxTrace_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const CM_TransformedBoxTrace_t CM_TransformedBoxTrace = (CM_TransformedBoxTrace_t)0x0805B8F6;
#endif

typedef clipHandle_t (*SV_ClipHandleForEntity_t)(gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const SV_ClipHandleForEntity_t SV_ClipHandleForEntity = (SV_ClipHandleForEntity_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_ClipHandleForEntity_t SV_ClipHandleForEntity = (SV_ClipHandleForEntity_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_ClipHandleForEntity_t SV_ClipHandleForEntity = (SV_ClipHandleForEntity_t)0x0809C544;
#endif

typedef void (*SV_ChangeMaxClients_t)(void);
#if COD_VERSION == COD2_1_0
static const SV_ChangeMaxClients_t SV_ChangeMaxClients = (SV_ChangeMaxClients_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_ChangeMaxClients_t SV_ChangeMaxClients = (SV_ChangeMaxClients_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_ChangeMaxClients_t SV_ChangeMaxClients = (SV_ChangeMaxClients_t)0x08092EEE;
#endif

typedef void (*SV_Startup_t)(void);
#if COD_VERSION == COD2_1_0
static const SV_Startup_t SV_Startup = (SV_Startup_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_Startup_t SV_Startup = (SV_Startup_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_Startup_t SV_Startup = (SV_Startup_t)0x08092DFA;
#endif

typedef void (*Scr_PlayFxError_t)(const char *error, int index); // Guessed function name
#if COD_VERSION == COD2_1_0
static const Scr_PlayFxError_t Scr_PlayFxError = (Scr_PlayFxError_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_PlayFxError_t Scr_PlayFxError = (Scr_PlayFxError_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_PlayFxError_t Scr_PlayFxError = (Scr_PlayFxError_t)0x08115034;
#endif

typedef const char * (*SEH_StringEd_GetString_t)(const char *pszReference);
#if COD_VERSION == COD2_1_0
static const SEH_StringEd_GetString_t SEH_StringEd_GetString = (SEH_StringEd_GetString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SEH_StringEd_GetString_t SEH_StringEd_GetString = (SEH_StringEd_GetString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SEH_StringEd_GetString_t SEH_StringEd_GetString = (SEH_StringEd_GetString_t)0x081384cc;
#endif

typedef void (*AxisToAngles_t)(float *axis, float *angles);
#if COD_VERSION == COD2_1_0
static const AxisToAngles_t AxisToAngles = (AxisToAngles_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const AxisToAngles_t AxisToAngles = (AxisToAngles_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const AxisToAngles_t AxisToAngles = (AxisToAngles_t)0x080A9558;
#endif

typedef void (*AnglesToAxis_t)(const vec3_t angles, vec3_t axis[3]);
#if COD_VERSION == COD2_1_0
static const AnglesToAxis_t AnglesToAxis = (AnglesToAxis_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const AnglesToAxis_t AnglesToAxis = (AnglesToAxis_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const AnglesToAxis_t AnglesToAxis = (AnglesToAxis_t)0x080A94AA;
#endif

typedef float (*vectoyaw_t)(const vec3_t vec);
#if COD_VERSION == COD2_1_0
static const vectoyaw_t vectoyaw = (vectoyaw_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const vectoyaw_t vectoyaw = (vectoyaw_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const vectoyaw_t vectoyaw = (vectoyaw_t)0x080A4D5A;
#endif

typedef void (*VecToAngles_t)(vec3_t value1, vec3_t angles);
#if COD_VERSION == COD2_1_0
static const VecToAngles_t VecToAngles = (VecToAngles_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const VecToAngles_t VecToAngles = (VecToAngles_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const VecToAngles_t VecToAngles = (VecToAngles_t)0x080A4FE2;
#endif

typedef double (*Vec3Distance_t)(float *v1, float *v2);
#if COD_VERSION == COD2_1_0
static const Vec3Distance_t Vec3Distance = (Vec3Distance_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Vec3Distance_t Vec3Distance = (Vec3Distance_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Vec3Distance_t Vec3Distance = (Vec3Distance_t)0x080A40EA;
#endif

typedef double (*Vec3DistanceSq_t)(float *v1, float *v2);
#if COD_VERSION == COD2_1_0
static const Vec3DistanceSq_t Vec3DistanceSq = (Vec3DistanceSq_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Vec3DistanceSq_t Vec3DistanceSq = (Vec3DistanceSq_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Vec3DistanceSq_t Vec3DistanceSq = (Vec3DistanceSq_t)0x080A4116;
#endif

typedef vec_t (*Vec3Normalize_t)(vec3_t v);
#if COD_VERSION == COD2_1_0
static const Vec3Normalize_t Vec3Normalize = (Vec3Normalize_t)0x080A20C0;
#elif COD_VERSION == COD2_1_2
static const Vec3Normalize_t Vec3Normalize = (Vec3Normalize_t)0x080A42E0;
#elif COD_VERSION == COD2_1_3
static const Vec3Normalize_t Vec3Normalize = (Vec3Normalize_t)0x080A4424;
#endif

typedef vec_t (*Vec3NormalizeTo_t)(vec3_t v, vec3_t out);
#if COD_VERSION == COD2_1_0
static const Vec3NormalizeTo_t Vec3NormalizeTo = (Vec3NormalizeTo_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Vec3NormalizeTo_t Vec3NormalizeTo = (Vec3NormalizeTo_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Vec3NormalizeTo_t Vec3NormalizeTo = (Vec3NormalizeTo_t)0x080A45FC;
#endif

typedef void (*Vec3Lerp_t)(const float *from, const float *to, float frac, float *result);
#if COD_VERSION == COD2_1_0
static const Vec3Lerp_t Vec3Lerp = (Vec3Lerp_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Vec3Lerp_t Vec3Lerp = (Vec3Lerp_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Vec3Lerp_t Vec3Lerp = (Vec3Lerp_t)0x080E129E;
#endif

typedef vec_t (*Vec2Normalize_t)(vec2_t v);
#if COD_VERSION == COD2_1_0
static const Vec2Normalize_t Vec2Normalize = (Vec2Normalize_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Vec2Normalize_t Vec2Normalize = (Vec2Normalize_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Vec2Normalize_t Vec2Normalize = (Vec2Normalize_t)0x080A44C2;
#endif

typedef void (*SP_worldspawn_t)(void);
#if COD_VERSION == COD2_1_0
static const SP_worldspawn_t SP_worldspawn = (SP_worldspawn_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SP_worldspawn_t SP_worldspawn = (SP_worldspawn_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SP_worldspawn_t SP_worldspawn = (SP_worldspawn_t)0x0811B56A;
#endif

typedef const char * (*SV_GetMapBaseName_t)(const char *mapname);
#if COD_VERSION == COD2_1_0
static const SV_GetMapBaseName_t SV_GetMapBaseName = (SV_GetMapBaseName_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_GetMapBaseName_t SV_GetMapBaseName = (SV_GetMapBaseName_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_GetMapBaseName_t SV_GetMapBaseName = (SV_GetMapBaseName_t)0x0808BD32;
#endif

typedef char * (*GetBspExtension_t)(void);
#if COD_VERSION == COD2_1_0
static const GetBspExtension_t GetBspExtension = (GetBspExtension_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const GetBspExtension_t GetBspExtension = (GetBspExtension_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const GetBspExtension_t GetBspExtension = (GetBspExtension_t)0x080A36FE;
#endif

typedef void (*AddRefToObject_t)(unsigned int id);
#if COD_VERSION == COD2_1_0
static const AddRefToObject_t AddRefToObject = (AddRefToObject_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const AddRefToObject_t AddRefToObject = (AddRefToObject_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const AddRefToObject_t AddRefToObject = (AddRefToObject_t)0x0807BE0A;
#endif

typedef void (*RemoveRefToObject_t)(unsigned int id);
#if COD_VERSION == COD2_1_0
static const RemoveRefToObject_t RemoveRefToObject = (RemoveRefToObject_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const RemoveRefToObject_t RemoveRefToObject = (RemoveRefToObject_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const RemoveRefToObject_t RemoveRefToObject = (RemoveRefToObject_t)0x0807BE2C;
#endif

typedef unsigned int (*FindVariable_t)(unsigned int parentId, unsigned int unsignedValue);
#if COD_VERSION == COD2_1_0
static const FindVariable_t FindVariable = (FindVariable_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const FindVariable_t FindVariable = (FindVariable_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const FindVariable_t FindVariable = (FindVariable_t)0x0807C184;
#endif

typedef unsigned int (*FindObject_t)(unsigned int id);
#if COD_VERSION == COD2_1_0
static const FindObject_t FindObject = (FindObject_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const FindObject_t FindObject = (FindObject_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const FindObject_t FindObject = (FindObject_t)0x0807D16E;
#endif

typedef unsigned int (*Scr_GetEntityId_t)(int entnum, unsigned int classnum);
#if COD_VERSION == COD2_1_0
static const Scr_GetEntityId_t Scr_GetEntityId = (Scr_GetEntityId_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Scr_GetEntityId_t Scr_GetEntityId = (Scr_GetEntityId_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Scr_GetEntityId_t Scr_GetEntityId = (Scr_GetEntityId_t)0x0807E9F8;
#endif

typedef void (*SetObjectiveIcon_t)(objective_t *obj, unsigned int index);
#if COD_VERSION == COD2_1_0
static const SetObjectiveIcon_t SetObjectiveIcon = (SetObjectiveIcon_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SetObjectiveIcon_t SetObjectiveIcon = (SetObjectiveIcon_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SetObjectiveIcon_t SetObjectiveIcon = (SetObjectiveIcon_t)0x081128CE;
#endif

typedef gentity_t * (*fire_grenade_t)(gentity_t *attacker, const float *start, const float *dir, int weaponIndex, int fuseTime);
#if COD_VERSION == COD2_1_0
static const fire_grenade_t fire_grenade = (fire_grenade_t)0x0810C1F6;
#elif COD_VERSION == COD2_1_2
static const fire_grenade_t fire_grenade = (fire_grenade_t)0x0810E532;
#elif COD_VERSION == COD2_1_3
static const fire_grenade_t fire_grenade = (fire_grenade_t)0x0810E68E;
#endif

typedef void (*G_AntiLagRewindClientPos_t)(int offset, antilagClientStore *antilagStore);
#if COD_VERSION == COD2_1_0
static const G_AntiLagRewindClientPos_t G_AntiLagRewindClientPos = (G_AntiLagRewindClientPos_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_AntiLagRewindClientPos_t G_AntiLagRewindClientPos = (G_AntiLagRewindClientPos_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_AntiLagRewindClientPos_t G_AntiLagRewindClientPos = (G_AntiLagRewindClientPos_t)0x0811FC76;
#endif

typedef void (*G_AntiLag_RestoreClientPos_t)(antilagClientStore *antilagStore);
#if COD_VERSION == COD2_1_0
static const G_AntiLag_RestoreClientPos_t G_AntiLag_RestoreClientPos = (G_AntiLag_RestoreClientPos_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_AntiLag_RestoreClientPos_t G_AntiLag_RestoreClientPos = (G_AntiLag_RestoreClientPos_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_AntiLag_RestoreClientPos_t G_AntiLag_RestoreClientPos = (G_AntiLag_RestoreClientPos_t)0x0811FDEC;
#endif

typedef int (*Bullet_CalcDamageRange_t)(const weaponParms *wp, float dist);
#if COD_VERSION == COD2_1_0
static const Bullet_CalcDamageRange_t Bullet_CalcDamageRange = (Bullet_CalcDamageRange_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Bullet_CalcDamageRange_t Bullet_CalcDamageRange = (Bullet_CalcDamageRange_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Bullet_CalcDamageRange_t Bullet_CalcDamageRange = (Bullet_CalcDamageRange_t)0x0811FB7C;
#endif

typedef void (*Bullet_Endpos_t)(float spread, float *end, const weaponParms *wp, float distance);
#if COD_VERSION == COD2_1_0
static const Bullet_Endpos_t Bullet_Endpos = (Bullet_Endpos_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Bullet_Endpos_t Bullet_Endpos = (Bullet_Endpos_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Bullet_Endpos_t Bullet_Endpos = (Bullet_Endpos_t)0x0811FAC8;
#endif

typedef void (*SV_AuthorizeIpPacket_t)(netadr_t from);
#if COD_VERSION == COD2_1_0
static const SV_AuthorizeIpPacket_t SV_AuthorizeIpPacket = (SV_AuthorizeIpPacket_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_AuthorizeIpPacket_t SV_AuthorizeIpPacket = (SV_AuthorizeIpPacket_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_AuthorizeIpPacket_t SV_AuthorizeIpPacket = (SV_AuthorizeIpPacket_t)0x0808DA2A;
#endif

typedef void (*SV_VoicePacket_t)(netadr_t from, msg_t *msg);
#if COD_VERSION == COD2_1_0
static const SV_VoicePacket_t SV_VoicePacket = (SV_VoicePacket_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_VoicePacket_t SV_VoicePacket = (SV_VoicePacket_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_VoicePacket_t SV_VoicePacket = (SV_VoicePacket_t)0x08094B56;
#endif

typedef void (*SV_QueueVoicePacket_t)(int talkerNum, int clientNum, VoicePacket_t *voicePacket);
#if COD_VERSION == COD2_1_0
static const SV_QueueVoicePacket_t SV_QueueVoicePacket = (SV_QueueVoicePacket_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_QueueVoicePacket_t SV_QueueVoicePacket = (SV_QueueVoicePacket_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_QueueVoicePacket_t SV_QueueVoicePacket = (SV_QueueVoicePacket_t)0x0809C21C;
#endif

typedef int (*Player_GetUseList_t)(gentity_t *player, useList_t *useList);
#if COD_VERSION == COD2_1_0
static const Player_GetUseList_t Player_GetUseList = (Player_GetUseList_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Player_GetUseList_t Player_GetUseList = (Player_GetUseList_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Player_GetUseList_t Player_GetUseList = (Player_GetUseList_t)0x08121494;
#endif

typedef void (*Player_SetTurretDropHintString_t)(gentity_t *player); // Guessed function name
#if COD_VERSION == COD2_1_0
static const Player_SetTurretDropHintString_t Player_SetTurretDropHintString = (Player_SetTurretDropHintString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Player_SetTurretDropHintString_t Player_SetTurretDropHintString = (Player_SetTurretDropHintString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Player_SetTurretDropHintString_t Player_SetTurretDropHintString = (Player_SetTurretDropHintString_t)0x08121B36;
#endif

typedef void (*Player_UseEntity_t)(gentity_t *playerEnt, gentity_t *useEnt);
#if COD_VERSION == COD2_1_0
static const Player_UseEntity_t Player_UseEntity = (Player_UseEntity_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Player_UseEntity_t Player_UseEntity = (Player_UseEntity_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Player_UseEntity_t Player_UseEntity = (Player_UseEntity_t)0x08121054;
#endif

typedef void (*DisablePbSv_t)(void);
#if COD_VERSION == COD2_1_0
static const DisablePbSv_t DisablePbSv = (DisablePbSv_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const DisablePbSv_t DisablePbSv = (DisablePbSv_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const DisablePbSv_t DisablePbSv = (DisablePbSv_t)0x0813C1D8;
#endif

typedef void (*EnablePbSv_t)(void);
#if COD_VERSION == COD2_1_0
static const EnablePbSv_t EnablePbSv = (EnablePbSv_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const EnablePbSv_t EnablePbSv = (EnablePbSv_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const EnablePbSv_t EnablePbSv = (EnablePbSv_t)0x0813C19C;
#endif

typedef void (*PbCaptureConsoleOutput_t)(const char *msg, int size);
#if COD_VERSION == COD2_1_0
static const PbCaptureConsoleOutput_t PbCaptureConsoleOutput = (PbCaptureConsoleOutput_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const PbCaptureConsoleOutput_t PbCaptureConsoleOutput = (PbCaptureConsoleOutput_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const PbCaptureConsoleOutput_t PbCaptureConsoleOutput = (PbCaptureConsoleOutput_t)0x0813C214;
#endif

typedef void (*PbPassConnectString_t)(const char *addr, byte *data);
#if COD_VERSION == COD2_1_0
static const PbPassConnectString_t PbPassConnectString = (PbPassConnectString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const PbPassConnectString_t PbPassConnectString = (PbPassConnectString_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const PbPassConnectString_t PbPassConnectString = (PbPassConnectString_t)0x0813C0EC;
#endif

typedef void (*PbServerForceProcess_t)(void);
#if COD_VERSION == COD2_1_0
static const PbServerForceProcess_t PbServerForceProcess = (PbServerForceProcess_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const PbServerForceProcess_t PbServerForceProcess = (PbServerForceProcess_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const PbServerForceProcess_t PbServerForceProcess = (PbServerForceProcess_t)0x0813C096;
#endif

typedef void (*PbSvAddEvent_t)(int event, int clientNum, int offset, byte *msg);
#if COD_VERSION == COD2_1_0
static const PbSvAddEvent_t PbSvAddEvent = (PbSvAddEvent_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const PbSvAddEvent_t PbSvAddEvent = (PbSvAddEvent_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const PbSvAddEvent_t PbSvAddEvent = (PbSvAddEvent_t)0x0813BF4C;
#endif

typedef void (*G_Damage_t)(gentity_t *self, gentity_t *inflictor, gentity_t *ent, const float *vDir, const float *vPoint, int value, int dFlags, meansOfDeath_t meansOfDeath, hitLocation_t hitLoc, int timeOffset);
#if COD_VERSION == COD2_1_0
static const G_Damage_t G_Damage = (G_Damage_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_Damage_t G_Damage = (G_Damage_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_Damage_t G_Damage = (G_Damage_t)0x08101C58;
#endif

typedef float (*G_GetWeaponHitLocationMultiplier_t)(int hitLoc, int weapon);
#if COD_VERSION == COD2_1_0
static const G_GetWeaponHitLocationMultiplier_t G_GetWeaponHitLocationMultiplier = (G_GetWeaponHitLocationMultiplier_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_GetWeaponHitLocationMultiplier_t G_GetWeaponHitLocationMultiplier = (G_GetWeaponHitLocationMultiplier_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_GetWeaponHitLocationMultiplier_t G_GetWeaponHitLocationMultiplier = (G_GetWeaponHitLocationMultiplier_t)0x08101AE2;
#endif

typedef void (*G_RunThink_t)(gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const G_RunThink_t G_RunThink = (G_RunThink_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_RunThink_t G_RunThink = (G_RunThink_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_RunThink_t G_RunThink = (G_RunThink_t)0x08109E60;
#endif

typedef void (*G_LocationalTrace_t)(trace_t *results, const float *start, const float *end, int passEntityNum, int contentmask, uint8_t *priorityMap);
#if COD_VERSION == COD2_1_0
static const G_LocationalTrace_t G_LocationalTrace = (G_LocationalTrace_t)0x08108134;
#elif COD_VERSION == COD2_1_2
static const G_LocationalTrace_t G_LocationalTrace = (G_LocationalTrace_t)0x0810A470;
#elif COD_VERSION == COD2_1_3
static const G_LocationalTrace_t G_LocationalTrace = (G_LocationalTrace_t)0x0810A5CC;
#endif

typedef int (*G_LocationalTracePassed_t)(const float *start, const float *end, int passEntityNum, int contentmask);
#if COD_VERSION == COD2_1_0
static const G_LocationalTracePassed_t G_LocationalTracePassed = (G_LocationalTracePassed_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_LocationalTracePassed_t G_LocationalTracePassed = (G_LocationalTracePassed_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_LocationalTracePassed_t G_LocationalTracePassed = (G_LocationalTracePassed_t)0x0810A622;
#endif

typedef void (*G_TraceCapsule_t)(trace_t *results, const float *start, const float *mins, const float *maxs, const float *end, int passEntityNum, int contentMask);
#if COD_VERSION == COD2_1_0
static const G_TraceCapsule_t G_TraceCapsule = (G_TraceCapsule_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_TraceCapsule_t G_TraceCapsule = (G_TraceCapsule_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_TraceCapsule_t G_TraceCapsule = (G_TraceCapsule_t)0x0810A528;
#endif

typedef void (*SV_Trace_t)(trace_t *results, const float *start, const float *mins, const float *maxs, const float *end, int passEntityNum, int contentMask, int locational, uint8_t *priorityMap, int staticmodels);
#if COD_VERSION == COD2_1_0
static const SV_Trace_t SV_Trace = (SV_Trace_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_Trace_t SV_Trace = (SV_Trace_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_Trace_t SV_Trace = (SV_Trace_t)0x0809d884;
#endif

typedef void (*G_SightTrace_t)(int *hitNum, const float *start, const float *end, int passEntityNum, int contentmask);
#if COD_VERSION == COD2_1_0
static const G_SightTrace_t G_SightTrace = (G_SightTrace_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_SightTrace_t G_SightTrace = (G_SightTrace_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_SightTrace_t G_SightTrace = (G_SightTrace_t)0x0810A672;
#endif

typedef float (*SV_FX_GetVisibility_t)(const float *start, const float *end);
#if COD_VERSION == COD2_1_0
static const SV_FX_GetVisibility_t SV_FX_GetVisibility = (SV_FX_GetVisibility_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_FX_GetVisibility_t SV_FX_GetVisibility = (SV_FX_GetVisibility_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_FX_GetVisibility_t SV_FX_GetVisibility = (SV_FX_GetVisibility_t)0x08090AFA;
#endif

typedef int (*SV_PointContents_t)(const vec3_t *p, int passEntityNum, int contentmask);
#if COD_VERSION == COD2_1_0
static const SV_PointContents_t SV_PointContents = (SV_PointContents_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_PointContents_t SV_PointContents = (SV_PointContents_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_PointContents_t SV_PointContents = (SV_PointContents_t)0x0809E368;
#endif

typedef void (*AngleVectors_t)(const vec3_t *angles, vec3_t *forward, vec3_t *right, vec3_t *up);
#if COD_VERSION == COD2_1_0
static const AngleVectors_t AngleVectors = (AngleVectors_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const AngleVectors_t AngleVectors = (AngleVectors_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const AngleVectors_t AngleVectors = (AngleVectors_t)0x080A5226;
#endif

typedef void (*MatrixTransposeTransformVector43_t)(const vec3_t in1, const float in2[4][3], vec3_t out);
#if COD_VERSION == COD2_1_0
static const MatrixTransposeTransformVector43_t MatrixTransposeTransformVector43 = (MatrixTransposeTransformVector43_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const MatrixTransposeTransformVector43_t MatrixTransposeTransformVector43 = (MatrixTransposeTransformVector43_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const MatrixTransposeTransformVector43_t MatrixTransposeTransformVector43 = (MatrixTransposeTransformVector43_t)0x080A74F8;
#endif

typedef void (*MatrixTransformVector_t)(const vec3_t in1, const vec3_t in2[3], vec3_t out);
#if COD_VERSION == COD2_1_0
static const MatrixTransformVector_t MatrixTransformVector = (MatrixTransformVector_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const MatrixTransformVector_t MatrixTransformVector = (MatrixTransformVector_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const MatrixTransformVector_t MatrixTransformVector = (MatrixTransformVector_t)0x080A71B6;
#endif

typedef void (*G_RunMissile_t)(gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const G_RunMissile_t G_RunMissile = (G_RunMissile_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_RunMissile_t G_RunMissile = (G_RunMissile_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_RunMissile_t G_RunMissile = (G_RunMissile_t)0x0810E0CE;
#endif

typedef void (*G_RunItem_t)(gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const G_RunItem_t G_RunItem = (G_RunItem_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_RunItem_t G_RunItem = (G_RunItem_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_RunItem_t G_RunItem = (G_RunItem_t)0x08107DE6;
#endif

typedef void (*G_RunCorpse_t)(gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const G_RunCorpse_t G_RunCorpse = (G_RunCorpse_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_RunCorpse_t G_RunCorpse = (G_RunCorpse_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_RunCorpse_t G_RunCorpse = (G_RunCorpse_t)0x08107DBC;
#endif

typedef void (*G_RunMover_t)(gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const G_RunMover_t G_RunMover = (G_RunMover_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_RunMover_t G_RunMover = (G_RunMover_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_RunMover_t G_RunMover = (G_RunMover_t)0x0810FDFE;
#endif

typedef void (*G_RunClient_t)(gentity_t *ent);
#if COD_VERSION == COD2_1_0
static const G_RunClient_t G_RunClient = (G_RunClient_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_RunClient_t G_RunClient = (G_RunClient_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_RunClient_t G_RunClient = (G_RunClient_t)0x080F66C8;
#endif

typedef void (*G_MissileLandAngles_t)(gentity_t *ent, trace_t *trace, float *angle, qboolean endPos);
#if COD_VERSION == COD2_1_0
static const G_MissileLandAngles_t G_MissileLandAngles = (G_MissileLandAngles_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_MissileLandAngles_t G_MissileLandAngles = (G_MissileLandAngles_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_MissileLandAngles_t G_MissileLandAngles = (G_MissileLandAngles_t)0x0810CF4C;
#endif

typedef void (*G_StartSolidTrace_t)(trace_t *results, float *start, float *end, int passentitynum, int contentmask); // Guessed function name
#if COD_VERSION == COD2_1_0
static const G_StartSolidTrace_t G_StartSolidTrace = (G_StartSolidTrace_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_StartSolidTrace_t G_StartSolidTrace = (G_StartSolidTrace_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_StartSolidTrace_t G_StartSolidTrace = (G_StartSolidTrace_t)0x0810DD2C;
#endif

typedef void (*G_StartSolidTraceNoContents_t)(trace_t *results, int passentitynum, gentity_t *ent, float *origin); // Guessed function name
#if COD_VERSION == COD2_1_0
static const G_StartSolidTraceNoContents_t G_StartSolidTraceNoContents = (G_StartSolidTraceNoContents_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_StartSolidTraceNoContents_t G_StartSolidTraceNoContents = (G_StartSolidTraceNoContents_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_StartSolidTraceNoContents_t G_StartSolidTraceNoContents = (G_StartSolidTraceNoContents_t)0x0810DDA6;
#endif

typedef void (*BG_EvaluateTrajectory_t)(const trajectory_t *tr, int atTime, float *result);
#if COD_VERSION == COD2_1_0
static const BG_EvaluateTrajectory_t BG_EvaluateTrajectory = (BG_EvaluateTrajectory_t)0x080DCEB0;
#elif COD_VERSION == COD2_1_2
static const BG_EvaluateTrajectory_t BG_EvaluateTrajectory = (BG_EvaluateTrajectory_t)0x080DF490;
#elif COD_VERSION == COD2_1_3
static const BG_EvaluateTrajectory_t BG_EvaluateTrajectory = (BG_EvaluateTrajectory_t)0x080DF5D4;
#endif

typedef void (*BG_EvaluateTrajectoryDelta_t)(const trajectory_t *tr, int atTime, float *result);
#if COD_VERSION == COD2_1_0
static const BG_EvaluateTrajectoryDelta_t BG_EvaluateTrajectoryDelta = (BG_EvaluateTrajectoryDelta_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const BG_EvaluateTrajectoryDelta_t BG_EvaluateTrajectoryDelta = (BG_EvaluateTrajectoryDelta_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const BG_EvaluateTrajectoryDelta_t BG_EvaluateTrajectoryDelta = (BG_EvaluateTrajectoryDelta_t)0x080DF948;
#endif

typedef double (*VectorLength_t)(float *vec);
#if COD_VERSION == COD2_1_0
static const VectorLength_t VectorLength = (VectorLength_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const VectorLength_t VectorLength = (VectorLength_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const VectorLength_t VectorLength = (VectorLength_t)0x081187F0;
#endif

typedef double (*VectorLength2_t)(float *vec);
#if COD_VERSION == COD2_1_0
static const VectorLength2_t VectorLength2 = (VectorLength2_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const VectorLength2_t VectorLength2 = (VectorLength2_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const VectorLength2_t VectorLength2 = (VectorLength2_t)0x080F7C08;
#endif

typedef float (*G_crandom_t)(void);
#if COD_VERSION == COD2_1_0
static const G_crandom_t G_crandom = (G_crandom_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_crandom_t G_crandom = (G_crandom_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_crandom_t G_crandom = (G_crandom_t)0x080A3792;
#endif

typedef float (*G_random_t)(void);
#if COD_VERSION == COD2_1_0
static const G_random_t G_random = (G_random_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_random_t G_random = (G_random_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_random_t G_random = (G_random_t)0x080A3774;
#endif

typedef void (*Cbuf_ExecuteText_t)(int exec_when, const char* text);
#if COD_VERSION == COD2_1_0
static const Cbuf_ExecuteText_t Cbuf_ExecuteText = (Cbuf_ExecuteText_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Cbuf_ExecuteText_t Cbuf_ExecuteText = (Cbuf_ExecuteText_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Cbuf_ExecuteText_t Cbuf_ExecuteText = (Cbuf_ExecuteText_t)0x0805FE3C;
#endif

typedef void (*AddPointToBounds_t)(float *origin, float *mins, float *maxs);
#if COD_VERSION == COD2_1_0
static const AddPointToBounds_t AddPointToBounds = (AddPointToBounds_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const AddPointToBounds_t AddPointToBounds = (AddPointToBounds_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const AddPointToBounds_t AddPointToBounds = (AddPointToBounds_t)0x080A896E;
#endif

typedef int (*CM_AreaEntities_t)(float *mins, float *maxs, int *entityList, int maxcount, int contentmask);
#if COD_VERSION == COD2_1_0
static const CM_AreaEntities_t CM_AreaEntities = (CM_AreaEntities_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const CM_AreaEntities_t CM_AreaEntities = (CM_AreaEntities_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const CM_AreaEntities_t CM_AreaEntities = (CM_AreaEntities_t)0x0805E986;
#endif

typedef int (*SV_SightTraceToEntity_t)(float *start, float *mins, float *maxs, float *end, int entityNum, int contentmask);
#if COD_VERSION == COD2_1_0
static const SV_SightTraceToEntity_t SV_SightTraceToEntity = (SV_SightTraceToEntity_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_SightTraceToEntity_t SV_SightTraceToEntity = (SV_SightTraceToEntity_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_SightTraceToEntity_t SV_SightTraceToEntity = (SV_SightTraceToEntity_t)0x0809E05E;
#endif

typedef void (*Activate_trigger_damage_t)(gentity_t *trigger, gentity_t *inflictor, int damage, meansOfDeath_t meansOfDeath);
#if COD_VERSION == COD2_1_0
static const Activate_trigger_damage_t Activate_trigger_damage = (Activate_trigger_damage_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Activate_trigger_damage_t Activate_trigger_damage = (Activate_trigger_damage_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Activate_trigger_damage_t Activate_trigger_damage = (Activate_trigger_damage_t)0x0811CCB6;
#endif

typedef void (*SnapAngles_t)(float *angles);
#if COD_VERSION == COD2_1_0
static const SnapAngles_t SnapAngles = (SnapAngles_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SnapAngles_t SnapAngles = (SnapAngles_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SnapAngles_t SnapAngles = (SnapAngles_t)0x0809C5C2;
#endif

typedef double (*RadiusFromBounds2_t)(float *mins, float *maxs);
#if COD_VERSION == COD2_1_0
static const RadiusFromBounds2_t RadiusFromBounds2 = (RadiusFromBounds2_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const RadiusFromBounds2_t RadiusFromBounds2 = (RadiusFromBounds2_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const RadiusFromBounds2_t RadiusFromBounds2 = (RadiusFromBounds2_t)0x080A8660;
#endif

typedef double (*RadiusFromBounds_t)(float *mins, float *maxs);
#if COD_VERSION == COD2_1_0
static const RadiusFromBounds_t RadiusFromBounds = (RadiusFromBounds_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const RadiusFromBounds_t RadiusFromBounds = (RadiusFromBounds_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const RadiusFromBounds_t RadiusFromBounds = (RadiusFromBounds_t)0x080A85D2;
#endif

typedef int (*CM_BoxLeafnums_t)(float *mins, float *maxs, int *list, int listsize, int *lastLeaf);
#if COD_VERSION == COD2_1_0
static const CM_BoxLeafnums_t CM_BoxLeafnums = (CM_BoxLeafnums_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const CM_BoxLeafnums_t CM_BoxLeafnums = (CM_BoxLeafnums_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const CM_BoxLeafnums_t CM_BoxLeafnums = (CM_BoxLeafnums_t)0x08058D6A;
#endif

typedef int (*CM_LeafCluster_t)(int leafNum);
#if COD_VERSION == COD2_1_0
static const CM_LeafCluster_t CM_LeafCluster = (CM_LeafCluster_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const CM_LeafCluster_t CM_LeafCluster = (CM_LeafCluster_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const CM_LeafCluster_t CM_LeafCluster = (CM_LeafCluster_t)0x080526F8;
#endif

typedef void (*CM_UnlinkEntity_t)(svEntity_t *ent);
#if COD_VERSION == COD2_1_0
static const CM_UnlinkEntity_t CM_UnlinkEntity = (CM_UnlinkEntity_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const CM_UnlinkEntity_t CM_UnlinkEntity = (CM_UnlinkEntity_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const CM_UnlinkEntity_t CM_UnlinkEntity = (CM_UnlinkEntity_t)0x0805DCA4;
#endif

typedef void (*CM_LinkEntity_t)(svEntity_t *ent, float *absmin, float *absmax, clipHandle_t clip);
#if COD_VERSION == COD2_1_0
static const CM_LinkEntity_t CM_LinkEntity = (CM_LinkEntity_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const CM_LinkEntity_t CM_LinkEntity = (CM_LinkEntity_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const CM_LinkEntity_t CM_LinkEntity = (CM_LinkEntity_t)0x0805E390;
#endif

typedef int (*DObjHasContents_t)(DObj *obj, int contentmask);
#if COD_VERSION == COD2_1_0
static const DObjHasContents_t DObjHasContents = (DObjHasContents_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const DObjHasContents_t DObjHasContents = (DObjHasContents_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const DObjHasContents_t DObjHasContents = (DObjHasContents_t)0x080BBCD0;
#endif

typedef void (*DObjGetBounds_t)(DObj *dobj, float *min, float *max);
#if COD_VERSION == COD2_1_0
static const DObjGetBounds_t DObjGetBounds = (DObjGetBounds_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const DObjGetBounds_t DObjGetBounds = (DObjGetBounds_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const DObjGetBounds_t DObjGetBounds = (DObjGetBounds_t)0x080BAA78;
#endif

typedef void (*DObjTraceline_t)(DObj *obj, float *start, float *end, unsigned char *priorityMap, DObjTrace_t *trace);
#if COD_VERSION == COD2_1_0
static const DObjTraceline_t DObjTraceline = (DObjTraceline_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const DObjTraceline_t DObjTraceline = (DObjTraceline_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const DObjTraceline_t DObjTraceline = (DObjTraceline_t)0x080BB27E;
#endif

typedef void (*DObjGeomTraceline_t)(DObj *obj, float *localStart, float *localEnd, int contentmask, DObjTrace_t *results);
#if COD_VERSION == COD2_1_0
static const DObjGeomTraceline_t DObjGeomTraceline = (DObjGeomTraceline_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const DObjGeomTraceline_t DObjGeomTraceline = (DObjGeomTraceline_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const DObjGeomTraceline_t DObjGeomTraceline = (DObjGeomTraceline_t)0x080BBB32;
#endif

typedef int (*SV_GEntityForSvEntity_t)(svEntity_t *svEnt);
#if COD_VERSION == COD2_1_0
static const SV_GEntityForSvEntity_t SV_GEntityForSvEntity = (SV_GEntityForSvEntity_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_GEntityForSvEntity_t SV_GEntityForSvEntity = (SV_GEntityForSvEntity_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_GEntityForSvEntity_t SV_GEntityForSvEntity = (SV_GEntityForSvEntity_t)0x0809177C;
#endif

typedef int (*SV_EntityContact_t)(const float *mins, const float *maxs, gentity_t *gEnt);
#if COD_VERSION == COD2_1_0
static const SV_EntityContact_t SV_EntityContact = (SV_EntityContact_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_EntityContact_t SV_EntityContact = (SV_EntityContact_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_EntityContact_t SV_EntityContact = (SV_EntityContact_t)0x08091AD6;
#endif

typedef void (*Touch_Multi_t)(gentity_t *trigger, gentity_t *player, int touch);
#if COD_VERSION == COD2_1_0
static const Touch_Multi_t Touch_Multi = (Touch_Multi_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Touch_Multi_t Touch_Multi = (Touch_Multi_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Touch_Multi_t Touch_Multi = (Touch_Multi_t)0x0811C66E;
#endif

typedef void (*hurt_use_t)(gentity_t *trigger, gentity_t *player);
#if COD_VERSION == COD2_1_0
static const hurt_use_t hurt_use = (hurt_use_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const hurt_use_t hurt_use = (hurt_use_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const hurt_use_t hurt_use = (hurt_use_t)0x0811CA92;
#endif

typedef void (*hurt_touch_t)(gentity_t *trigger, gentity_t *player, int touch);
#if COD_VERSION == COD2_1_0
static const hurt_touch_t hurt_touch = (hurt_touch_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const hurt_touch_t hurt_touch = (hurt_touch_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const hurt_touch_t hurt_touch = (hurt_touch_t)0x0811C98C;
#endif

typedef void (*Use_trigger_damage_t)(gentity_t *trigger, gentity_t *player);
#if COD_VERSION == COD2_1_0
static const Use_trigger_damage_t Use_trigger_damage = (Use_trigger_damage_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Use_trigger_damage_t Use_trigger_damage = (Use_trigger_damage_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Use_trigger_damage_t Use_trigger_damage = (Use_trigger_damage_t)0x0811CD62;
#endif

typedef void (*Pain_trigger_damage_t)(gentity_t *trigger, gentity_t *player, int damage, const float *point, meansOfDeath_t meansOfDeath, const float *dir, hitLocation_t hitLoc);
#if COD_VERSION == COD2_1_0
static const Pain_trigger_damage_t Pain_trigger_damage = (Pain_trigger_damage_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Pain_trigger_damage_t Pain_trigger_damage = (Pain_trigger_damage_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Pain_trigger_damage_t Pain_trigger_damage = (Pain_trigger_damage_t)0x0811CD92;
#endif

typedef void (*Die_trigger_damage_t)(gentity_t *trigger, gentity_t *inflictor, gentity_t *player, int damage, meansOfDeath_t meansOfDeath, int weapon, const float *dir, hitLocation_t hitLoc, int psTimeOffset);
#if COD_VERSION == COD2_1_0
static const Die_trigger_damage_t Die_trigger_damage = (Die_trigger_damage_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Die_trigger_damage_t Die_trigger_damage = (Die_trigger_damage_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Die_trigger_damage_t Die_trigger_damage = (Die_trigger_damage_t)0x0811CDD4;
#endif

typedef void (*Reached_ScriptMover_t)(gentity_t *trigger);
#if COD_VERSION == COD2_1_0
static const Reached_ScriptMover_t Reached_ScriptMover = (Reached_ScriptMover_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Reached_ScriptMover_t Reached_ScriptMover = (Reached_ScriptMover_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Reached_ScriptMover_t Reached_ScriptMover = (Reached_ScriptMover_t)0x08118A30;
#endif

typedef void (*G_ExplodeMissile_t)(gentity_t *missile);
#if COD_VERSION == COD2_1_0
static const G_ExplodeMissile_t G_ExplodeMissile = (G_ExplodeMissile_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_ExplodeMissile_t G_ExplodeMissile = (G_ExplodeMissile_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_ExplodeMissile_t G_ExplodeMissile = (G_ExplodeMissile_t)0x0810D970;
#endif

typedef void (*Touch_Item_Auto_t)(gentity_t *item, gentity_t *entity, int touch);
#if COD_VERSION == COD2_1_0
static const Touch_Item_Auto_t Touch_Item_Auto = (Touch_Item_Auto_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Touch_Item_Auto_t Touch_Item_Auto = (Touch_Item_Auto_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Touch_Item_Auto_t Touch_Item_Auto = (Touch_Item_Auto_t)0x08105C80;
#endif

typedef void (*player_die_t)(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, meansOfDeath_t meansOfDeath, int iWeapon, const float *vDir, hitLocation_t hitLoc, int psTimeOffset);
#if COD_VERSION == COD2_1_0
static const player_die_t player_die = (player_die_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const player_die_t player_die = (player_die_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const player_die_t player_die = (player_die_t)0x0810175A;
#endif

typedef void (*G_PlayerController_t)(gentity_t *trigger, gentity_t *player);
#if COD_VERSION == COD2_1_0
static const G_PlayerController_t G_PlayerController = (G_PlayerController_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const G_PlayerController_t G_PlayerController = (G_PlayerController_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const G_PlayerController_t G_PlayerController = (G_PlayerController_t)0x080F729A;
#endif

typedef void (*BodyEnd_t)(gentity_t *player);
#if COD_VERSION == COD2_1_0
static const BodyEnd_t BodyEnd = (BodyEnd_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const BodyEnd_t BodyEnd = (BodyEnd_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const BodyEnd_t BodyEnd = (BodyEnd_t)0x080FCC48;
#endif

typedef void (*Turret_FillWeaponParms_t)(gentity_t *ent, gentity_t *activator, weaponParms *wp);
#if COD_VERSION == COD2_1_0
static const Turret_FillWeaponParms_t Turret_FillWeaponParms = (Turret_FillWeaponParms_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Turret_FillWeaponParms_t Turret_FillWeaponParms = (Turret_FillWeaponParms_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Turret_FillWeaponParms_t Turret_FillWeaponParms = (Turret_FillWeaponParms_t)0x0810A880;
#endif

typedef void (*turret_think_init_t)(gentity_t *turret);
#if COD_VERSION == COD2_1_0
static const turret_think_init_t turret_think_init = (turret_think_init_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const turret_think_init_t turret_think_init = (turret_think_init_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const turret_think_init_t turret_think_init = (turret_think_init_t)0x0810BF00;
#endif

typedef void (*turret_use_t)(gentity_t *turret, gentity_t *player);
#if COD_VERSION == COD2_1_0
static const turret_use_t turret_use = (turret_use_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const turret_use_t turret_use = (turret_use_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const turret_use_t turret_use = (turret_use_t)0x0810C3DE;
#endif

typedef void (*turret_think_t)(gentity_t *turret);
#if COD_VERSION == COD2_1_0
static const turret_think_t turret_think = (turret_think_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const turret_think_t turret_think = (turret_think_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const turret_think_t turret_think = (turret_think_t)0x0810BE6E;
#endif

typedef void (*turret_controller_t)(gentity_t *player, gentity_t *turret);
#if COD_VERSION == COD2_1_0
static const turret_controller_t turret_controller = (turret_controller_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const turret_controller_t turret_controller = (turret_controller_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const turret_controller_t turret_controller = (turret_controller_t)0x0810C10E;
#endif

typedef void (*DroppedItemClearOwner_t)(gentity_t *item);
#if COD_VERSION == COD2_1_0
static const DroppedItemClearOwner_t DroppedItemClearOwner = (DroppedItemClearOwner_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const DroppedItemClearOwner_t DroppedItemClearOwner = (DroppedItemClearOwner_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const DroppedItemClearOwner_t DroppedItemClearOwner = (DroppedItemClearOwner_t)0x08106130;
#endif

typedef void (*FinishSpawningItem_t)(gentity_t *item);
#if COD_VERSION == COD2_1_0
static const FinishSpawningItem_t FinishSpawningItem = (FinishSpawningItem_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const FinishSpawningItem_t FinishSpawningItem = (FinishSpawningItem_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const FinishSpawningItem_t FinishSpawningItem = (FinishSpawningItem_t)0x08106A90;
#endif

typedef void (*use_trigger_use_t)(gentity_t *trigger, gentity_t *player);
#if COD_VERSION == COD2_1_0
static const use_trigger_use_t use_trigger_use = (use_trigger_use_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const use_trigger_use_t use_trigger_use = (use_trigger_use_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const use_trigger_use_t use_trigger_use = (use_trigger_use_t)0x0810FE4A;
#endif

typedef int (*get_bit_t)(byte *fin);
#if COD_VERSION == COD2_1_0
static const get_bit_t get_bit = (get_bit_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const get_bit_t get_bit = (get_bit_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const get_bit_t get_bit = (get_bit_t)0x0806627A;
#endif

typedef void (*CalculateRanks_t)(void);
#if COD_VERSION == COD2_1_0
static const CalculateRanks_t CalculateRanks = (CalculateRanks_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const CalculateRanks_t CalculateRanks = (CalculateRanks_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const CalculateRanks_t CalculateRanks = (CalculateRanks_t)0x081097E6;
#endif

typedef char * (*CopyStringInternal_t)(const char *in);
#if COD_VERSION == COD2_1_0
static const CopyStringInternal_t CopyStringInternal = (CopyStringInternal_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const CopyStringInternal_t CopyStringInternal = (CopyStringInternal_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const CopyStringInternal_t CopyStringInternal = (CopyStringInternal_t)0x080AB740;
#endif

typedef void (*SV_FlushRedirect_t)(char *outputbuf);
#if COD_VERSION == COD2_1_0
static const SV_FlushRedirect_t SV_FlushRedirect = (SV_FlushRedirect_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const SV_FlushRedirect_t SV_FlushRedirect = (SV_FlushRedirect_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const SV_FlushRedirect_t SV_FlushRedirect = (SV_FlushRedirect_t)0x08097050;
#endif

typedef void (*FX_CreateDefaultEffect_t)(void);
#if COD_VERSION == COD2_1_0
static const FX_CreateDefaultEffect_t FX_CreateDefaultEffect = (FX_CreateDefaultEffect_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const FX_CreateDefaultEffect_t FX_CreateDefaultEffect = (FX_CreateDefaultEffect_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const FX_CreateDefaultEffect_t FX_CreateDefaultEffect = (FX_CreateDefaultEffect_t)0x0812C34C;
#endif

typedef void (*FX_FreeSystem_t)(void);
#if COD_VERSION == COD2_1_0
static const FX_FreeSystem_t FX_FreeSystem = (FX_FreeSystem_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const FX_FreeSystem_t FX_FreeSystem = (FX_FreeSystem_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const FX_FreeSystem_t FX_FreeSystem = (FX_FreeSystem_t)0x081318E0;
#endif

typedef float (*FX_GetEffectLength_t)(EffectTemplate *effect);
#if COD_VERSION == COD2_1_0
static const FX_GetEffectLength_t FX_GetEffectLength = (FX_GetEffectLength_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const FX_GetEffectLength_t FX_GetEffectLength = (FX_GetEffectLength_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const FX_GetEffectLength_t FX_GetEffectLength = (FX_GetEffectLength_t)0x0813198C;
#endif

typedef void (*FX_InitSystem_t)(byte rendererExists);
#if COD_VERSION == COD2_1_0
static const FX_InitSystem_t FX_InitSystem = (FX_InitSystem_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const FX_InitSystem_t FX_InitSystem = (FX_InitSystem_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const FX_InitSystem_t FX_InitSystem = (FX_InitSystem_t)0x081318C6;
#endif

typedef EffectTemplate * (*FX_RegisterEffect_t)(const char *filepath);
#if COD_VERSION == COD2_1_0
static const FX_RegisterEffect_t FX_RegisterEffect = (FX_RegisterEffect_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const FX_RegisterEffect_t FX_RegisterEffect = (FX_RegisterEffect_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const FX_RegisterEffect_t FX_RegisterEffect = (FX_RegisterEffect_t)0x0812C38A;
#endif

typedef void * (*Hunk_AllocInternal_t)(int size);
#if COD_VERSION == COD2_1_0
static const Hunk_AllocInternal_t Hunk_AllocInternal = (Hunk_AllocInternal_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const Hunk_AllocInternal_t Hunk_AllocInternal = (Hunk_AllocInternal_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const Hunk_AllocInternal_t Hunk_AllocInternal = (Hunk_AllocInternal_t)0x080ABF78;
#endif

typedef const char * (*ClientConnect_t)(unsigned int clientNum, short scriptPersId);
#if COD_VERSION == COD2_1_0
static const ClientConnect_t ClientConnect = (ClientConnect_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const ClientConnect_t ClientConnect = (ClientConnect_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const ClientConnect_t ClientConnect = (ClientConnect_t)0x080F8E7A;
#endif

typedef qboolean (*OnSameTeam_t)(gentity_t *ent1, gentity_t *ent2);
#if COD_VERSION == COD2_1_0
static const OnSameTeam_t OnSameTeam = (OnSameTeam_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const OnSameTeam_t OnSameTeam = (OnSameTeam_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const OnSameTeam_t OnSameTeam = (OnSameTeam_t)0x0811C0D0;
#endif

typedef void (*TeleportPlayer_t)(gentity_t *player, float *origin, float *angles);
#if COD_VERSION == COD2_1_0
static const TeleportPlayer_t TeleportPlayer = (TeleportPlayer_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const TeleportPlayer_t TeleportPlayer = (TeleportPlayer_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const TeleportPlayer_t TeleportPlayer = (TeleportPlayer_t)0x0810A708;
#endif

typedef int (*unzGetGlobalInfo_t)(unzFile file, unz_global_info *pglobal_info);
#if COD_VERSION == COD2_1_0
static const unzGetGlobalInfo_t unzGetGlobalInfo = (unzGetGlobalInfo_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const unzGetGlobalInfo_t unzGetGlobalInfo = (unzGetGlobalInfo_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const unzGetGlobalInfo_t unzGetGlobalInfo = (unzGetGlobalInfo_t)0x080D22A1;
#endif

typedef int (*unzGoToFirstFile_t)(unzFile file);
#if COD_VERSION == COD2_1_0
static const unzGoToFirstFile_t unzGoToFirstFile = (unzGoToFirstFile_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const unzGoToFirstFile_t unzGoToFirstFile = (unzGoToFirstFile_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const unzGoToFirstFile_t unzGoToFirstFile = (unzGoToFirstFile_t)0x080D286D;
#endif

typedef int (*unzGetCurrentFileInfo_t)(unzFile file, unz_file_info *pfile_info, char *szFileName, unsigned long fileNameBufferSize, void *extraField, unsigned long extraFieldBufferSize, char *szComment, unsigned long commentBufferSize);
#if COD_VERSION == COD2_1_0
static const unzGetCurrentFileInfo_t unzGetCurrentFileInfo = (unzGetCurrentFileInfo_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const unzGetCurrentFileInfo_t unzGetCurrentFileInfo = (unzGetCurrentFileInfo_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const unzGetCurrentFileInfo_t unzGetCurrentFileInfo = (unzGetCurrentFileInfo_t)0x080D2821;
#endif

typedef int (*unzGoToNextFile_t)(unzFile file);
#if COD_VERSION == COD2_1_0
static const unzGoToNextFile_t unzGoToNextFile = (unzGoToNextFile_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const unzGoToNextFile_t unzGoToNextFile = (unzGoToNextFile_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const unzGoToNextFile_t unzGoToNextFile = (unzGoToNextFile_t)0x080D2915;
#endif

typedef int (*unzSetCurrentFileInfoPosition_t)(unzFile file, unsigned long pos);
#if COD_VERSION == COD2_1_0
static const unzGoToNextFile_t unzSetCurrentFileInfoPosition = (unzGoToNextFile_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const unzGoToNextFile_t unzSetCurrentFileInfoPosition = (unzGoToNextFile_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const unzGoToNextFile_t unzSetCurrentFileInfoPosition = (unzGoToNextFile_t)0x080D2A35;
#endif

typedef int (*unzOpenCurrentFile_t)(unzFile file);
#if COD_VERSION == COD2_1_0
static const unzOpenCurrentFile_t unzOpenCurrentFile = (unzOpenCurrentFile_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const unzOpenCurrentFile_t unzOpenCurrentFile = (unzOpenCurrentFile_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const unzOpenCurrentFile_t unzOpenCurrentFile = (unzOpenCurrentFile_t)0x080D2E9E;
#endif

typedef int (*unzReadCurrentFile_t)(unzFile file, void *buf, unsigned int len);
#if COD_VERSION == COD2_1_0
static const unzReadCurrentFile_t unzReadCurrentFile = (unzReadCurrentFile_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const unzReadCurrentFile_t unzReadCurrentFile = (unzReadCurrentFile_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const unzReadCurrentFile_t unzReadCurrentFile = (unzReadCurrentFile_t)0x080D3094;
#endif

typedef int (*unzCloseCurrentFile_t)(unzFile file);
#if COD_VERSION == COD2_1_0
static const unzCloseCurrentFile_t unzCloseCurrentFile = (unzCloseCurrentFile_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_2
static const unzCloseCurrentFile_t unzCloseCurrentFile = (unzCloseCurrentFile_t)0x0; // Not tested
#elif COD_VERSION == COD2_1_3
static const unzCloseCurrentFile_t unzCloseCurrentFile = (unzCloseCurrentFile_t)0x080D3506;
#endif

#endif