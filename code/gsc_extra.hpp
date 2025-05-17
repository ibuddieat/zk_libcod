#ifndef _GSC_EXTRA_HPP_
#define _GSC_EXTRA_HPP_

/* gsc functions */
#include "gsc.hpp"

__attribute__((weak)) void extra_Scr_PlayerConnect(gentity_t *ent);   //called right after Scr_PlayerConnect
__attribute__((weak)) void extra_GScr_LoadGameTypeScript(); //called after normal gametypescripts are loaded, can be used as an init function
__attribute__((weak)) void extra_fire_grenade(gentity_t *attacker, gentity_t *grenade); //called right before the fire_grenade callback is generated
__attribute__((weak)) void extra_SV_DropClient(client_t *drop, const char *reason); //called right before SV_DropClient is called
__attribute__((weak)) void extra_ClientBegin(client_t *client); //called right after ClientBegin is called
__attribute__((weak)) void extra_SV_ClientThinkBefore(client_t *client, usercmd_t *ucmd); //called right before SV_ClientThink is called
__attribute__((weak)) void extra_SV_ClientThinkAfter(client_t *client, usercmd_t *ucmd); //called right after SV_ClientThink is called
__attribute__((weak)) void extra_G_RunFrameBefore(int levelTime); //called right before G_RunFrame is called
__attribute__((weak)) void extra_G_RunFrameAfter(int levelTime); //called right after G_RunFrame is called
__attribute__((weak)) bool extra_G_ClientCanSpectateClient(gclient_t *spectator, client_t *player); //check if spectator is allowed to spectate player. Return true if this is allowed, false if not
__attribute__((weak)) xfunction_t Scr_GetExtraFunction(const char **fname, int *fdev); //iterate through your custom functions, return xfunction_t if found, NULL if not found
__attribute__((weak)) xmethod_t Scr_GetExtraMethod(const char **fname, qboolean *fdev); //iterate through your custom methods, return xmethod_t if found, NULL if not found

#endif