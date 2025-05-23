#ifndef _GSC_EXTRA_HPP_
#define _GSC_EXTRA_HPP_

/* gsc functions */
#include "gsc.hpp"

__attribute__((weak)) void extra_Scr_PlayerConnect_After(gentity_t *ent); // Called after the CodeCallback_PlayerConnect callback is executed
__attribute__((weak)) void extra_GScr_LoadGameTypeScript_After(); // Called after normal gametype scripts are loaded (not executed), can be used as an init function
__attribute__((weak)) void extra_fire_grenade_After(gentity_t *attacker, gentity_t *grenade); // Called after the grenade entity is spawned and before the fire_grenade callback is executed
__attribute__((weak)) void extra_SV_DropClient_Before(client_t *drop, const char *reason); // Called before SV_DropClient is called
__attribute__((weak)) void extra_ClientBegin_After(client_t *client); // Called after ClientBegin is called
__attribute__((weak)) void extra_SV_ClientThink_Before(client_t *client, usercmd_t *ucmd); // Called before SV_ClientThink is called
__attribute__((weak)) void extra_SV_ClientThink_After(client_t *client, usercmd_t *ucmd); // Called after SV_ClientThink is called
__attribute__((weak)) void extra_G_RunFrame_Before(int levelTime); // Called before G_RunFrame is called
__attribute__((weak)) void extra_G_RunFrame_After(int levelTime); // Called after G_RunFrame is called
__attribute__((weak)) bool extra_G_ClientCanSpectateClient(gclient_t *spectator, client_t *player); // Checks if spectator is allowed to spectate player. Return true if this is allowed, false if not
__attribute__((weak)) xfunction_t Scr_GetExtraFunction(const char **fname, int *fdev); // Therein define your custom functions, return xfunction_t if found, NULL if not
__attribute__((weak)) xmethod_t Scr_GetExtraMethod(const char **fname, qboolean *fdev); // Therein define your custom methods, return xmethod_t if found, NULL if not

#endif