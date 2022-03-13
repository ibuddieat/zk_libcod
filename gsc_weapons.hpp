#ifndef _GSC_WEAPONS_HPP_
#define _GSC_WEAPONS_HPP_

#include "gsc.hpp"

void gsc_weapons_init();
void gsc_weapons_free();
int hook_findWeaponIndex(char* weapon);
void gsc_weapons_getweaponmaxammo();
void gsc_weapons_setweaponmaxammo();
void gsc_weapons_getweaponclipsize();
void gsc_weapons_setweaponclipsize();
void gsc_weapons_getweapondamage();
void gsc_weapons_setweapondamage();
void gsc_weapons_getweaponmeleedamage();
void gsc_weapons_setweaponmeleedamage();
void gsc_weapons_getweaponfiretime();
void gsc_weapons_setweaponfiretime();
void gsc_weapons_getweaponmeleetime();
void gsc_weapons_setweaponmeleetime();
void gsc_weapons_getweaponreloadtime();
void gsc_weapons_setweaponreloadtime();
void gsc_weapons_getweaponreloademptytime();
void gsc_weapons_setweaponreloademptytime();
void gsc_weapons_getweaponcookable();
void gsc_weapons_setweaponcookable();
void gsc_weapons_getweaponfusetime();
void gsc_weapons_setweaponfusetime();
void gsc_weapons_getweaponhitlocmultiplier();
void gsc_weapons_setweaponhitlocmultiplier();
void gsc_weapons_getmovespeedscale();
void gsc_weapons_setmovespeedscale();
void gsc_weapons_getloadedweapons();
void gsc_weapons_getweapondisplayname();
void gsc_weapons_resetignoredweapons();
void gsc_weapons_ignoreweapon();
void gsc_weapons_setdefaultweapon();
void gsc_weapons_getweaponitemammo(scr_entref_t id);
void gsc_weapons_setweaponitemammo(scr_entref_t id);

#endif