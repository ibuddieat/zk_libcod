#ifndef _DVAR_HPP_
#define _DVAR_HPP_

#include "gsc.hpp"

void Dvar_Protect_f(void);

void custom_Com_DvarDump(conChannel_t channel);
void custom_Com_InitDvars(void);
void custom_Dvar_AddCommands(void);
qboolean custom_Dvar_Command(void);
void custom_Dvar_List_f(void);
void custom_Dvar_Reset_f(void);
void custom_Dvar_SetCommand(const char *dvarName, const char *string);
void custom_Dvar_SetFromDvar_f(void);
void custom_Dvar_SetFromStringFromSource(dvar_t *dvar, const char *string, DvarSetSource source);
void custom_Dvar_SetS_f(void);
void custom_Dvar_SetU_f(void);
void custom_Dvar_Toggle_f(void);
void custom_Dvar_TogglePrint_f(void);
void custom_Dvar_WriteDefaults(fileHandle_t f);
void custom_Dvar_WriteVariables(fileHandle_t f);
qboolean custom_FS_RegisterDvars(void);
void custom_G_ProcessIPBans(void);
void custom_GScr_GetDvar(void);
void custom_GScr_MakeDvarServerInfo(void);
void custom_GScr_SetDvar(void);

void hook_Com_Printf_in_Com_Init_Try_Block_Function(const char *format, ...);

#endif