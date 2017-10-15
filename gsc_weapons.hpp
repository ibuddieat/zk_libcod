#ifndef _GSC_WEAPONS_HPP_
#define _GSC_WEAPONS_HPP_

#ifdef __cplusplus
extern "C" {
#endif

/* default stuff */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* gsc functions */
#include "gsc.hpp"

void gsc_weapons_getweaponmaxammo();
void gsc_weapons_getweaponclipsize();
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
void gsc_weapons_getweaponhitlocmultiplier();
void gsc_weapons_setweaponhitlocmultiplier();
void gsc_weapons_getloadedweapons();

#ifdef __cplusplus
}
#endif

#endif
