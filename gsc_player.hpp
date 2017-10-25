#ifndef _GSC_PLAYER_HPP_
#define _GSC_PLAYER_HPP_

#ifdef __cplusplus
extern "C" {
#endif

/* gsc functions */
#include "gsc.hpp"

void gsc_player_lookatkiller(int id);

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
void gsc_player_stance_set(int id);

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
