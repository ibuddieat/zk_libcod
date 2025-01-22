#include "jump.hpp"

#if COMPILE_JUMP == 1

// Stock dvars
extern dvar_t *jump_height;
extern dvar_t *jump_stepSize;
extern dvar_t *jump_slowdownEnable;
extern dvar_t *jump_ladderPushVel;
extern dvar_t *jump_spreadAdd;

// Custom dvars
extern dvar_t *g_resetSlide;
extern dvar_t *jump_carryMoverVelocity;

#define JUMP_LAND_SLOWDOWN_TIME 1800

extern customEntityState_t customEntityState[MAX_GENTITIES];
extern customPlayerState_t customPlayerState[MAX_CLIENTS];

int playerstateToClientNum(playerState_t* ps)
{
	return (int)(((byte *)ps - (byte *)sv.gameClients) / sv.gameClientSize);
}

float getJumpHeight(playerState_t* ps)
{
	int id = playerstateToClientNum(ps);

	if ( customPlayerState[id].overrideJumpHeight )
		return customPlayerState[id].jumpHeight;
	return jump_height->current.decimal;
}

bool getJumpSlowdownEnable(playerState_t* ps)
{
	int id = playerstateToClientNum(ps);

	if ( customPlayerState[id].overrideJumpSlowdown )
		return customPlayerState[id].jumpSlowdown;
	return jump_slowdownEnable->current.boolean;
}

double Jump_GetLandFactor(playerState_t *ps)
{
	if ( !getJumpSlowdownEnable(ps) ) // Original: jump_slowdownEnable->current.boolean
	{
		return 1.0;
	}
	if ( ps->pm_time < 1700 )
	{
		return (double)ps->pm_time * 1.5 * 0.00058823527 + 1.0;
	}
	return 2.5;
}

void Jump_PushOffLadder(playerState_s *ps, pml_t *pml)
{
	// Function follows stock logic, but it is necessary to be reimplemented
	// here because we redefined the jump_ladderPushVel dvar

	vec3_t flatForward;
	vec3_t pushOffDir;
	float dot;

	ps->velocity[2] = ps->velocity[2] * 0.75;
	flatForward[0] = pml->forward[0];
	flatForward[1] = pml->forward[1];
	flatForward[2] = 0.0;
	Vec3Normalize(flatForward);
	dot = DotProduct(ps->vLadderVec, pml->forward);
	if ( dot >= 0.0 )
	{
		VectorCopy(flatForward, pushOffDir);
	}
	else
	{
		dot = DotProduct(ps->vLadderVec, flatForward);
		VectorMA(flatForward, -2.0 * dot, ps->vLadderVec, pushOffDir);
		Vec3Normalize(pushOffDir);
	}
	ps->velocity[0] = jump_ladderPushVel->current.decimal * pushOffDir[0];
	ps->velocity[1] = jump_ladderPushVel->current.decimal * pushOffDir[1];
	ps->pm_flags &= ~PMF_LADDER;
}

void Jump_Start(pmove_t *pm, pml_t *pml, float height)
{
	float factor;
	float velocitySqrd;
	playerState_t *ps;
	int groundEntityNum = pm->ps->groundEntityNum;

	ps = pm->ps;
	velocitySqrd = (float)(height * 2.0) * (float)ps->gravity;
	if ( ps->pm_flags & PMF_JUMPING )
	{
		if ( ps->pm_time <= JUMP_LAND_SLOWDOWN_TIME )
		{
			factor = Jump_GetLandFactor(ps);
			velocitySqrd = velocitySqrd / factor;
		}
	}
	pml->groundPlane = 0;
	pml->almostGroundPlane = 0;
	pml->walking = 0;
	ps->groundEntityNum = ENTITY_NONE;
	ps->jumpTime = pm->cmd.serverTime;
	ps->jumpOriginZ = ps->origin[2];
	if ( g_resetSlide->current.boolean ) // New
	{
		ps->pm_flags &= 0xFFFFFE7F;
	}
	ps->pm_flags |= PMF_JUMPING;
	ps->pm_time = 0;
	ps->velocity[2] = sqrtf(velocitySqrd);
	ps->aimSpreadScale = ps->aimSpreadScale + jump_spreadAdd->current.decimal;
	if ( ps->aimSpreadScale > 255.0 )
	{
		ps->aimSpreadScale = 255.0;
	}

	/* New code start: jump_carryMoverVelocity dvar */
	if ( jump_carryMoverVelocity->current.boolean &&
		groundEntityNum != ENTITY_WORLD &&
		groundEntityNum != ENTITY_NONE )
	{
		gentity_t *ent = &g_entities[groundEntityNum];

		if ( ent->s.eType == ET_SCRIPTMOVER )
		{
			VectorAdd(ps->velocity, customEntityState[ent->s.number].velocity, ps->velocity);
		}
	}
	/* New code end */
}

double Jump_ReduceFriction(playerState_t *ps)
{
	float control;

	/* New code start: Jump tweaks */
	if ( ps->pm_time > JUMP_LAND_SLOWDOWN_TIME )
	{
		Jump_ClearState(ps);
		control = 1.0;
	}
	/* New code end */
	else
	{
		control = Jump_GetLandFactor(ps); // New: Uses per-player jump slowdown
	}
	return control;
}

void Jump_ClampVelocity(playerState_t* ps, vec3_t vec)
{
	float comp;
	float newZVelocity;

	if ( ps->origin[2] - vec[2] > 0.0 )
	{
		float jumpHeight = getJumpHeight(ps); // Original: jump_height->current.decimal;
		comp = ps->jumpOriginZ + jumpHeight - ps->origin[2];
		if ( comp >= (float)0.1 )
		{
			newZVelocity = sqrtf((comp + comp) * (float)ps->gravity);
			if ( ps->velocity[2] > newZVelocity )
			{
				ps->velocity[2] = newZVelocity;
			}
		}
		else
		{
			ps->velocity[2] = 0.0;
		}
	}
}

qboolean Jump_IsPlayerAboveMax(playerState_t* ps)
{
	float jumpHeight = getJumpHeight(ps); // Original: jump_height->current.decimal;

	if ( ps->origin[2] >= ps->jumpOriginZ + jumpHeight )
		return qtrue;
	else
		return qfalse;
}

qboolean Jump_GetStepHeight(playerState_t* ps, const vec3_t vec1, float* val2)
{
	float jumpHeight = getJumpHeight(ps); // Original: jump_height->current.decimal;

	if ( vec1[2] > ps->jumpOriginZ + jumpHeight )
		return qfalse;

	*val2 = jump_stepSize->current.decimal;
	if ( vec1[2] + jump_stepSize->current.decimal > ps->jumpOriginZ + jumpHeight )
		*val2 = ps->jumpOriginZ + jumpHeight - vec1[2];

	return qtrue;
}

qboolean Jump_Check(pmove_t *pm, pml_t *pml)
{
	playerState_t *ps = pm->ps;

	if ( pm->cmd.serverTime - ps->jumpTime < 500 )
	{
		return qfalse;
	}
	if ( ps->pm_flags & 0x1000 )
	{
		return qfalse;
	}
	if ( g_resetSlide->current.boolean ) // New
	{
		if ( ps->pm_flags & 0x400 )
		{
			return qfalse;
		}
	}
	if ( ps->pm_flags & 4 )
	{
		return qfalse;
	}
	if ( ps->pm_type > 5 )
	{
		return qfalse;
	}
	if ( PM_GetEffectiveStance(ps) && ps->groundEntityNum != ENTITY_NONE ) // New: Added ground entity check
	{
		return qfalse;
	}
	if ( !(pm->cmd.buttons & 0x400) )
	{
		return qfalse;
	}
	if ( pm->oldcmd.buttons & 0x400 )
	{
		pm->cmd.buttons &= 0xFFFFFBFF;
		return qfalse;
	}
	Jump_Start(pm, pml, getJumpHeight(ps)); // Original: jump_height->current.decimal
	Jump_AddSurfaceEvent(ps, pml);
	if ( ps->pm_flags & PMF_LADDER )
	{
		Jump_PushOffLadder(ps, pml);
	}
	if ( pm->cmd.forwardmove < 0 )
	{
		BG_AnimScriptEvent(ps, ANIM_ET_JUMPBK, 0, 1);
	}
	else
	{
		BG_AnimScriptEvent(ps, ANIM_ET_JUMP, 0, 1);
	}
	return qtrue;
}

void Jump_ApplySlowdown(playerState_t *ps)
{
	float scale;

	scale = 1.0;
	if ( ps->pm_time <= JUMP_LAND_SLOWDOWN_TIME )
	{
		if ( !ps->pm_time )
		{
			if ( (float)(ps->jumpOriginZ + 18.0) <= ps->origin[2] )
			{
				ps->pm_time = 1200;
				scale = 0.5;
			}
			else
			{
				ps->pm_time = JUMP_LAND_SLOWDOWN_TIME;
				scale = 0.64999998;
			}
		}
	}
	else
	{
		Jump_ClearState(ps);
		scale = 0.64999998;
	}
	if ( getJumpSlowdownEnable(ps) ) // Original: jump_slowdownEnable->current.boolean
	{
		VectorScale(ps->velocity, scale, ps->velocity);
	}
}

void Jump_RegisterDvars()
{
	// Stock dvars
	// New: Flags. Original flags: DVAR_CHANGEABLE_RESET | DVAR_CODINFO | DVAR_CHEAT
	jump_height = Dvar_RegisterFloat("jump_height", 39.0, 0.0, 128.0, DVAR_CHEAT | DVAR_SYSTEMINFO);
	jump_stepSize = Dvar_RegisterFloat("jump_stepSize", 18.0, 0.0, 64.0, DVAR_CHEAT | DVAR_SYSTEMINFO);
	jump_slowdownEnable = Dvar_RegisterBool("jump_slowdownEnable", qtrue, DVAR_CHEAT | DVAR_SYSTEMINFO);
	jump_ladderPushVel = Dvar_RegisterFloat("jump_ladderPushVel", 128.0, 0.0, 1024.0, DVAR_CHEAT | DVAR_SYSTEMINFO);
	jump_spreadAdd = Dvar_RegisterFloat("jump_spreadAdd", 64.0, 0.0, 512.0, DVAR_CHEAT | DVAR_SYSTEMINFO);

	// Custom dvars
	jump_carryMoverVelocity = Dvar_RegisterBool("jump_carryMoverVelocity", qfalse, DVAR_CHEAT | DVAR_SYSTEMINFO);
}

#endif
