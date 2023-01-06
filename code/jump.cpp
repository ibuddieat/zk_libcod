#include "jump.hpp"

#if COMPILE_JUMP == 1

cvar_t *jump_height;
cvar_t *jump_stepSize;
cvar_t *jump_slowdownEnable;
cvar_t *jump_ladderPushVel;
cvar_t *jump_spreadAdd;

#define JUMP_LAND_SLOWDOWN_TIME 1800

bool player_jump_height_enabled[MAX_CLIENTS] = {0};
float player_jump_height[MAX_CLIENTS] = {0};
bool player_jump_slowdownenable_enabled[MAX_CLIENTS] = {0};
bool player_jump_slowdownenable[MAX_CLIENTS] = {0};

int playerstateToClientNum(playerState_t* ps)
{
	return (int)(((byte *)ps - (byte *)sv.gameClients) / sv.gameClientSize);
}

float getJumpHeight(playerState_t* ps)
{
	int clientid = playerstateToClientNum(ps);
	if(player_jump_height_enabled[clientid])
		return player_jump_height[clientid];
	return jump_height->floatval;
}

bool getJumpSlowdownEnable(playerState_t* ps)
{
	int clientid = playerstateToClientNum(ps);
	if(player_jump_slowdownenable_enabled[clientid])
		return player_jump_slowdownenable[clientid];
	return jump_slowdownEnable->boolean;
}

float Jump_CalcHeight(playerState_t* ps )
{
	float val;
	float newdiv;
	float jumpHeight = getJumpHeight(ps);//jump_height->floatval;

	val = jumpHeight;
	val = (val + val) * ps->gravity;

	if(ps->pm_flags & PMF_JUMPING && ps->pm_time <= JUMP_LAND_SLOWDOWN_TIME )
	{
		if(getJumpSlowdownEnable(ps))//jump_slowdownEnable->boolean)
		{

			if(ps->pm_time > 1699)
			{

				newdiv = 2.5;

			}
			else
			{
				newdiv = ps->pm_time * (float)1.5 * (float)0.00058823527 + (float)1.0;
			}

		}
		else
		{
			newdiv = 1.0;
		}
		val = val / newdiv;
	}

	return val;
}

double Jump_GetLandFactor(playerState_s *ps)
{
	if ( !getJumpSlowdownEnable(ps))//jump_slowdownEnable->boolean )
	{
		return 1.0;
	}
	if ( ps->pm_time < 1700 )
	{
		return (double)ps->pm_time * 1.5 * 0.00058823527 + 1.0;
	}
	return 2.5;
}

void Jump_AddSurfaceEvent(playerState_s *ps, pml_t *pml)
{
	int surfType;

	if ( ps->pm_flags & PMF_LADDER )
	{
		BG_AddPredictableEventToPlayerstate(76, 0x15u, ps);
	}
	else
	{
		surfType = PM_GroundSurfaceType(pml);
		if ( surfType )
		{
			BG_AddPredictableEventToPlayerstate(76, surfType, ps);
		}
	}
}

void Jump_PushOffLadder(playerState_s *ps, pml_t *pml)
{
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
	ps->velocity[0] = jump_ladderPushVel->floatval * pushOffDir[0];
	ps->velocity[1] = jump_ladderPushVel->floatval * pushOffDir[1];
	ps->pm_flags &= 0xFFFFFFDF;
}

void Jump_Start(pmove_t *pm, pml_t *pml, float height)
{
	float factor;
	float velocitySqrd;
	playerState_s *ps;

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
	ps->groundEntityNum = 1023;
	ps->jumpTime = pm->cmd.serverTime;
	ps->jumpOriginZ = ps->origin[2];
	ps->velocity[2] = sqrtf(velocitySqrd);
	ps->pm_flags &= 0xFFFFFE7F;
	ps->pm_flags |= PMF_JUMPING;
	ps->pm_time = 0;
	ps->aimSpreadScale = ps->aimSpreadScale + jump_spreadAdd->floatval;
	if ( ps->aimSpreadScale > 255.0 )
	{
		ps->aimSpreadScale = 255.0;
	}
}

void Jump_ClearState(playerState_s *ps)
{
	ps->pm_flags &= ~PMF_JUMPING;
	ps->jumpOriginZ = 0.0;
}

double Jump_ReduceFriction(playerState_s *ps)
{
	float control;

	if ( ps->pm_time > JUMP_LAND_SLOWDOWN_TIME )
	{
		Jump_ClearState(ps);
		control = 1.0;
	}
	else
	{
		control = Jump_GetLandFactor(ps);
	}
	return control;
}

void Jump_ClampVelocity(playerState_t* ps, vec3_t vec)
{
	float comp;
	float newZVelocity;

	if(ps->origin[2] - vec[2] > 0)
	{

		float jumpHeight = getJumpHeight(ps);//jump_height->floatval;

		comp = ps->jumpOriginZ + jumpHeight - ps->origin[2];

		if(comp >= (float)0.1)
		{

			newZVelocity = sqrtf( (comp + comp) * (float)ps->gravity);

			if(ps->velocity[2] > newZVelocity )
			{
				ps->velocity[2] = newZVelocity;
			}

		}
		else
		{
			ps->velocity[2] = 0;
			return;
		}

	}
}

qboolean Jump_IsPlayerAboveMax(playerState_t* ps)
{
	float jumpHeight = getJumpHeight(ps);//jump_height->floatval;

	if(ps->origin[2] >= ps->jumpOriginZ + jumpHeight )
		return qtrue;
	else
		return qfalse;

}

qboolean Jump_GetStepHeight(playerState_t* ps, const vec3_t vec1, float* val2)
{
	float jumpHeight = getJumpHeight(ps);//jump_height->floatval;

	if(vec1[2] > ps->jumpOriginZ + jumpHeight)
		return qfalse;

	*val2 = jump_stepSize->floatval;

	if(vec1[2] + jump_stepSize->floatval > ps->jumpOriginZ + jumpHeight)
		*val2 = ps->jumpOriginZ + jumpHeight - vec1[2];

	return qtrue;

}

qboolean Jump_Check(pmove_t *pm, pml_t *pml)
{
	playerState_s *ps;

	ps = pm->ps;

	if ( ps->pm_flags & 0x1000 )
	{
		return qfalse;
	}
	if ( pm->cmd.serverTime - ps->jumpTime < 500 )
	{
		return qfalse;
	}
	if ( ps->pm_flags & 0x400 )
	{
		return qfalse;
	}
	if ( ps->pm_flags & 4 )
	{
		return qfalse;
	}
	if ( ps->pm_type > 5 )
	{
		return qfalse;
	}
	if ( PM_GetEffectiveStance(ps) && ps->groundEntityNum != 1023 )
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
	Jump_Start(pm, pml, getJumpHeight(ps));//jump_height->floatval);
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

void Jump_ApplySlowdown(playerState_s *ps)
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
	if ( getJumpSlowdownEnable(ps))//jump_slowdownEnable->boolean )
	{
		VectorScale(ps->velocity, scale, ps->velocity);
	}
}

void Jump_ActivateSlowdown(playerState_s *ps)
{
	if ( !ps->pm_time )
	{
		ps->pm_flags |= PMF_JUMPING;
		ps->pm_time = JUMP_LAND_SLOWDOWN_TIME;
	}
}

void Jump_RegisterDvars()
{
	jump_height = Cvar_RegisterFloat("jump_height", 39.0, 0.0, 128.0, CVAR_CHEAT | CVAR_SYSTEMINFO);
	jump_stepSize = Cvar_RegisterFloat("jump_stepSize", 18.0, 0.0, 64.0, CVAR_CHEAT | CVAR_SYSTEMINFO);
	jump_slowdownEnable = Cvar_RegisterBool("jump_slowdownEnable", qtrue, CVAR_CHEAT | CVAR_SYSTEMINFO);
	jump_ladderPushVel = Cvar_RegisterFloat("jump_ladderPushVel", 128.0, 0.0, 1024.0, CVAR_CHEAT | CVAR_SYSTEMINFO);
	jump_spreadAdd = Cvar_RegisterFloat("jump_spreadAdd", 64.0, 0.0, 512.0, CVAR_CHEAT | CVAR_SYSTEMINFO);
}

#endif
