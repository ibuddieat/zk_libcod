#ifndef _GSC_BOTS_HPP_
#define _GSC_BOTS_HPP_

#include "gsc.hpp"

void gsc_bots_setwalkvalues(scr_entref_t id);
void gsc_bots_setwalkdir(scr_entref_t id);
void gsc_bots_setlean(scr_entref_t id);
void gsc_bots_setbotstance(scr_entref_t id);
void gsc_bots_throwgrenade(scr_entref_t id);
void gsc_bots_fireweapon(scr_entref_t id);
void gsc_bots_meleeweapon(scr_entref_t id);
void gsc_bots_reloadweapon(scr_entref_t id);
void gsc_bots_setaim(scr_entref_t id);
void gsc_bots_switchtoweaponid(scr_entref_t id);
void gsc_bots_setnexttestclientname();
void gsc_bots_resettestclientnaming();

#endif