#ifndef _GSC_ENTITY_HPP_
#define _GSC_ENTITY_HPP_

#include "gsc.hpp"
#include "utils.hpp"

void gsc_entity_addentityvelocity(scr_entref_t ref);
void gsc_entity_disablebounce(scr_entref_t ref);
void gsc_entity_disablegravity(scr_entref_t ref);
void gsc_entity_enablebounce(scr_entref_t ref);
void gsc_entity_enablegravity(scr_entref_t ref);
void gsc_entity_getclipmask(scr_entref_t ref);
void gsc_entity_getcontents(scr_entref_t ref);
void gsc_entity_getentityvelocity(scr_entref_t ref);
void gsc_entity_getmaxentityvelocity(scr_entref_t ref);
void gsc_entity_gettagangles(scr_entref_t ref);
void gsc_entity_gettagorigin(scr_entref_t ref);
void gsc_entity_getturretowner(scr_entref_t ref);
void gsc_entity_getvmax(scr_entref_t ref);
void gsc_entity_getvmin(scr_entref_t ref);
void gsc_entity_hastag(scr_entref_t ref);
void gsc_entity_isgravityenabled(scr_entref_t ref);
void gsc_entity_islinkedto(scr_entref_t ref);
void gsc_entity_isturret(scr_entref_t ref);
void gsc_entity_setalive(scr_entref_t ref);
void gsc_entity_setbounds(scr_entref_t ref);
void gsc_entity_setclipmask(scr_entref_t ref);
void gsc_entity_setentityvelocity(scr_entref_t ref);
void gsc_entity_setmaxentityvelocity(scr_entref_t ref);
void gsc_entity_setlight(scr_entref_t ref);
void gsc_entity_notsolidforplayer(scr_entref_t ref);
void gsc_entity_solidforplayer(scr_entref_t ref);
void gsc_entity_hidefromplayer(scr_entref_t ref);

#endif