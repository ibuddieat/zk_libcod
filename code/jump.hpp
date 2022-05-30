#ifndef _GSC_JUMP_HPP_
#define _GSC_JUMP_HPP_

/* gsc functions */
#include "gsc.hpp"

void Jump_ClearState(playerState_s *ps);
double Jump_ReduceFriction(playerState_s *ps);
void Jump_ClampVelocity(playerState_t* ps, vec3_t vec);
qboolean Jump_IsPlayerAboveMax(playerState_t* ps);
qboolean Jump_GetStepHeight(playerState_t* ps, const vec3_t vec1, float* val2);
qboolean Jump_Check(pmove_t *pm, pml_t *pml);
void Jump_ApplySlowdown(playerState_s *ps);
void Jump_ActivateSlowdown(playerState_s *ps);
void Jump_RegisterDvars();

#endif
