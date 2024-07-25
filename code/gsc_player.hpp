#ifndef _GSC_PLAYER_HPP_
#define _GSC_PLAYER_HPP_

#include "gsc.hpp"
#include "libcod.hpp"

void gsc_player_getprotocol(scr_entref_t ref);
void gsc_player_getprotocolstring(scr_entref_t ref);
void gsc_player_enablebulletdrop(scr_entref_t ref);
void gsc_player_disablebulletdrop(scr_entref_t ref);
void gsc_player_setbulletdrag(scr_entref_t ref);
void gsc_player_setbulletvelocity(scr_entref_t ref);
void gsc_player_setbulletmodel(scr_entref_t ref);
void gsc_player_overridecontents(scr_entref_t ref);
void gsc_player_useentity(scr_entref_t ref);
void gsc_player_useturret(scr_entref_t ref);
void gsc_player_canuseturret(scr_entref_t ref);
void gsc_player_isreloading(scr_entref_t ref);
void gsc_player_isthrowinggrenade(scr_entref_t ref);
void gsc_player_isusingbinoculars(scr_entref_t ref);
void gsc_player_getcurrentweaponammo(scr_entref_t ref);
void gsc_player_getcurrentweaponclipammo(scr_entref_t ref);
void gsc_player_setcurrentweaponammo(scr_entref_t ref);
void gsc_player_setcurrentweaponclipammo(scr_entref_t ref);
void gsc_player_forceshot(scr_entref_t ref);
void gsc_player_setvelocity(scr_entref_t ref);
void gsc_player_addvelocity(scr_entref_t ref);
void gsc_player_getvelocity(scr_entref_t ref);
void gsc_player_button_ads(scr_entref_t ref);
void gsc_player_button_left(scr_entref_t ref);
void gsc_player_button_right(scr_entref_t ref);
void gsc_player_button_forward(scr_entref_t ref);
void gsc_player_button_back(scr_entref_t ref);
void gsc_player_button_leanleft(scr_entref_t ref);
void gsc_player_button_leanright(scr_entref_t ref);
void gsc_player_button_jump(scr_entref_t ref);
void gsc_player_button_reload(scr_entref_t ref);
void gsc_player_button_frag(scr_entref_t ref);
void gsc_player_button_smoke(scr_entref_t ref);
void gsc_player_getstance(scr_entref_t ref);
void gsc_player_setstance(scr_entref_t ref);
void gsc_player_getspectatorclient(scr_entref_t ref);
void gsc_player_getuserinfo(scr_entref_t ref);
void gsc_player_setuserinfo(scr_entref_t ref);
void gsc_player_getip(scr_entref_t ref);
void gsc_player_getping(scr_entref_t ref);
void gsc_player_setping(scr_entref_t ref);
void gsc_player_processclientuserinfochange(scr_entref_t ref);
void gsc_player_processclientcommand(scr_entref_t ref);
void gsc_player_getlastconnecttime(scr_entref_t ref);
void gsc_player_getlastmsg(scr_entref_t ref);
void gsc_player_getclientconnectstate(scr_entref_t ref);
void gsc_player_addresstype(scr_entref_t ref);
void gsc_player_renameclient(scr_entref_t ref);
void gsc_player_connectionlesspackettoclient(scr_entref_t ref);
void gsc_player_connectionlesspackettoserver(scr_entref_t ref);
void gsc_player_resetnextreliabletime(scr_entref_t ref);
void gsc_player_ismantling(scr_entref_t ref);
void gsc_player_isonladder(scr_entref_t ref);
void gsc_player_isusingturret(scr_entref_t ref);
void gsc_player_getjumpslowdowntimer(scr_entref_t ref);
void gsc_player_clearjumpstate(scr_entref_t ref);
void gsc_player_setjumpslowdownenable(scr_entref_t ref);
void gsc_player_setjumpheight(scr_entref_t ref);
void gsc_player_setspeed(scr_entref_t ref);
void gsc_player_setgravity(scr_entref_t ref);
void gsc_player_getspeed(scr_entref_t ref);
void gsc_player_getgravity(scr_entref_t ref);
void gsc_player_setbulletmask(scr_entref_t ref);
void gsc_player_setfirethroughwalls(scr_entref_t ref);
void gsc_player_setfirerangescale(scr_entref_t ref);
void gsc_player_setweaponfiremeleedelay(scr_entref_t ref);
void gsc_player_setmeleeheightscale(scr_entref_t ref);
void gsc_player_setmeleerangescale(scr_entref_t ref);
void gsc_player_setmeleewidthscale(scr_entref_t ref);
void gsc_player_setweaponspreadscale(scr_entref_t ref);
void gsc_player_setturretspreadscale(scr_entref_t ref);
void gsc_player_getweaponindexoffhand(scr_entref_t ref);
void gsc_player_getcurrentoffhandslotammo(scr_entref_t ref);
void gsc_player_setanimation(scr_entref_t ref);
void gsc_player_playscriptanimation(scr_entref_t ref);
void gsc_player_getcooktime(scr_entref_t ref);
void gsc_player_setguid(scr_entref_t ref);
void gsc_player_clienthasclientmuted(scr_entref_t ref);
void gsc_player_muteclient(scr_entref_t ref);
void gsc_player_unmuteclient(scr_entref_t ref);
void gsc_player_getlastgamestatesize(scr_entref_t ref);
void gsc_player_getfps(scr_entref_t ref);
void gsc_player_lookatkiller(scr_entref_t ref);
void gsc_player_isbot(scr_entref_t ref);
void gsc_player_disableitempickup(scr_entref_t ref);
void gsc_player_enableitempickup(scr_entref_t ref);
void gsc_player_noclip(scr_entref_t ref);
void gsc_player_getinactivitytime(scr_entref_t ref);
void gsc_player_disableearthquakes(scr_entref_t ref);
void gsc_player_enableearthquakes(scr_entref_t ref);
void gsc_player_earthquakeforplayer(scr_entref_t ref);
void gsc_player_playfxforplayer(scr_entref_t ref);
void gsc_player_playfxontagforplayer(scr_entref_t ref);
void gsc_player_getclienthudelemcount(scr_entref_t ref);
void gsc_player_disablesilent(scr_entref_t ref);
void gsc_player_enablesilent(scr_entref_t ref);
void gsc_player_getgroundentity(scr_entref_t ref);
void gsc_player_getentertime(scr_entref_t ref);
void gsc_player_stopuseturret(scr_entref_t ref);
void gsc_player_getplayerstateflags(scr_entref_t ref);
void gsc_player_objective_player_add(scr_entref_t ref);
void gsc_player_objective_player_delete(scr_entref_t ref);
void gsc_player_objective_player_icon(scr_entref_t ref);
void gsc_player_objective_player_position(scr_entref_t ref);
void gsc_player_objective_player_state(scr_entref_t ref);
void gsc_player_getvieworigin(scr_entref_t ref);
void gsc_player_getservercommandqueuesize(scr_entref_t ref);
void gsc_player_setconfigstring(scr_entref_t ref);
void gsc_player_getcollisionteam(scr_entref_t ref);
void gsc_player_setcollisionteam(scr_entref_t ref);
void gsc_player_disablebulletimpacts(scr_entref_t ref);
void gsc_player_enablebulletimpacts(scr_entref_t ref);
void gsc_player_disabletalkericon(scr_entref_t ref);
void gsc_player_enabletalkericon(scr_entref_t ref);
void gsc_player_processsuicide(scr_entref_t ref);
void gsc_player_sethiddenfromscroreboard(scr_entref_t ref);
void gsc_player_gethiddenfromscroreboard(scr_entref_t ref);
#if COMPILE_CUSTOM_VOICE == 1
void gsc_player_playsoundfile(scr_entref_t ref);
void gsc_player_stopsoundfile(scr_entref_t ref);
void gsc_player_getremainingsoundfileduration(scr_entref_t ref);
void gsc_player_isplayingsoundfile(scr_entref_t ref);
#endif

#endif