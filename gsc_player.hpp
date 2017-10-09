#ifndef _GSC_PLAYER_HPP_
#define _GSC_PLAYER_HPP_

#ifdef __cplusplus
extern "C" {
#endif

/* default stuff */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* gsc functions */
#include "gsc.hpp"

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

void gsc_player_velocity_set(int id);
void gsc_player_velocity_add(int id);
void gsc_player_velocity_get(int id);

void gsc_player_button_ads(int id);
void gsc_player_button_left(int id);
void gsc_player_button_right(int id);
void gsc_player_button_forward(int id);
void gsc_player_button_back(int id);
void gsc_player_button_leanleft(int id);
void gsc_player_button_leanright(int id);
void gsc_player_button_jump(int id);
void gsc_player_button_reload(int id);
void gsc_player_button_frag(int id);
void gsc_player_button_smoke(int id);

void gsc_player_stance_get(int id);

void gsc_player_spectatorclient_get(int id);
void gsc_get_userinfo(int id);
void gsc_set_userinfo(int id);
void gsc_player_getip(int id);
void gsc_player_getping(int id);
void gsc_player_clientuserinfochanged(int id);

void gsc_player_ClientCommand(int id);

void gsc_player_getLastConnectTime(int id);
void gsc_player_getLastMSG(int id);
void gsc_player_getclientstate(int id);

void gsc_player_addresstype(int id);
void gsc_player_renameclient(int id);
void gsc_player_outofbandprint(int id);
void gsc_player_connectionlesspacket(int id);
void gsc_player_resetNextReliableTime(int id);
void gsc_player_ismantling(int id);
void gsc_player_isonladder(int id);
void gsc_player_getjumpslowdowntimer(int id);
long double hook_player_setmovespeed(int client, int a2);
void hook_player_g_speed(int client);
void gsc_player_setmovespeedscale(int id);
void gsc_player_setg_speed(int id);
void gsc_player_setg_gravity(int id);
void gsc_player_setweaponfiremeleedelay(int id);
int hook_pickup_item(int weapon, int player, int message);
void gsc_player_disable_item_pickup(int id);
void gsc_player_enable_item_pickup(int id);
void gsc_player_set_anim(int id);
void gsc_player_getcooktime(int id);
void gsc_player_setguid(int id);
void gsc_player_clienthasclientmuted(int id);
void gsc_player_getlastgamestatesize(int id);
void gsc_player_getfps(int id);

// entity functions
void gsc_entity_setalive(int id);
void gsc_entity_setbounds(int id);

// player functions without entity
void gsc_kick_slot();

#ifdef __cplusplus
}
#endif

#endif
