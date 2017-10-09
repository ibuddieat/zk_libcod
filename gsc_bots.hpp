#ifndef _GSC_BOTS_HPP_
#define _GSC_BOTS_HPP_

#ifdef __cplusplus
extern "C" {
#endif

/* default stuff */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* gsc functions */
#include "gsc.hpp"

void gsc_bots_set_walkdir(int id);
void gsc_bots_set_lean(int id);
void gsc_bots_set_stance(int id);
void gsc_bots_thrownade(int id);
void gsc_bots_fireweapon(int id);
void gsc_bots_meleeweapon(int id);
void gsc_bots_reloadweapon(int id);
void gsc_bots_adsaim(int id);
void gsc_bots_switchtoweaponid(int id);

#ifdef __cplusplus
}
#endif

#endif