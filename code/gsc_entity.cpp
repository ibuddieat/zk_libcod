#include "gsc_entity.hpp"

#if COMPILE_ENTITY == 1

extern customPlayerState_t customPlayerState[MAX_CLIENTS];

void gsc_entity_setalive(scr_entref_t ref)
{
	int id = ref.entnum;
	int isAlive;

	if ( !stackGetParams("i", &isAlive) )
	{
		stackError("gsc_entity_setalive() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	gentity_t *entity = &g_entities[id];

	entity->takedamage = isAlive;
	stackPushBool(qtrue);
}

void gsc_entity_setbounds(scr_entref_t ref)
{
	int id = ref.entnum;
	float width, height;

	if ( !stackGetParams("ff", &width, &height) )
	{
		stackError("gsc_entity_setbounds() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	gentity_t *entity = &g_entities[id];

	vec3_t mins = {-height, -width, -width};
	vec3_t maxs = {width, width, height};

	VectorCopy(mins, entity->r.mins);
	VectorCopy(maxs, entity->r.maxs);

	stackPushBool(qtrue);
}

void gsc_entity_getvmax(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *entity = &g_entities[id];

	stackPushVector(entity->r.maxs);
}

void gsc_entity_getvmin(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *entity = &g_entities[id];

	stackPushVector(entity->r.mins);
}

void gsc_entity_gettagangles(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *parent = &g_entities[id];

	if ( ( parent->flags & FL_LINKTO_ENABLED ) == 0 )
	{
		// Try to enableLinkTo if it's disabled
		if ( parent->s.eType || parent->physicsObject )
		{
			stackError("gsc_entity_gettagangles() entity (classname: \'%s\') does not currently support enableLinkTo", SL_ConvertToString(parent->classname));
			stackPushUndefined();
			return;
		}
		parent->flags = parent->flags | FL_LINKTO_ENABLED;
	}

	unsigned int tagId;
	char *tagName;
	vec3_t originOffset = {0, 0, 0};
	vec3_t anglesOffset = {0, 0, 0};

	tagId = Scr_GetConstLowercaseString(0);
	tagName = SL_ConvertToString(tagId);
	if ( !*tagName )
	{
		tagId = 0;
	}

	// Create an entity that we can link to
	gentity_t *tempEnt = G_Spawn();
	SV_LinkEntity(tempEnt);

	// Link entities with zero offsets
	if ( !G_EntLinkToWithOffset(tempEnt, parent, tagId, &originOffset, &anglesOffset) )
	{
		G_FreeEntity(tempEnt);
		if ( !SV_DObjExists(parent) )
		{
			if ( !parent->model )
			{
				stackError("gsc_entity_gettagangles() failed to link entity since parent has no model");
				stackPushUndefined();
				return;
			}
			stackError("gsc_entity_gettagangles() failed to link entity since parent model \'%s\' is invalid", G_ModelName(parent->model));
			stackPushUndefined();
			return;
		}
		if ( tagId && SV_DObjGetBoneIndex(parent, tagId) < 0 )
		{
			SV_DObjDumpInfo(parent);
			stackError("gsc_entity_gettagangles() failed to link entity since tag \'%s\' does not exist in parent model \'%s\'", tagName, G_ModelName(parent->model));
			stackPushUndefined();
			return;
		}
		stackError("gsc_entity_gettagangles() failed to link entity due to link cycle");
		stackPushUndefined();
		return;
	}

	// Apply movement as if a frame would have passed
	vec3_t angles;
	G_GeneralLink(tempEnt);
	VectorCopy(tempEnt->r.currentAngles, angles);
	G_FreeEntity(tempEnt);
	
	stackPushVector(angles);
}

void gsc_entity_gettagorigin(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *ent = &g_entities[id];
	vec3_t origin;
	unsigned int tagId;
	char *tagName;

	tagId = Scr_GetConstLowercaseString(0);
	tagName = SL_ConvertToString(tagId);
	if ( !*tagName )
		tagId = 0; // Defaults to origin

	if ( !G_DObjGetWorldTagPos(ent, tagId, origin) )
	{
		stackError("gsc_entity_gettagorigin() could not find tag \'%s\' on model \'%s\'", tagName, G_ModelName(ent->model));
		stackPushUndefined();
		return;
	}

	stackPushVector(origin);
}

void gsc_entity_hastag(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *ent = &g_entities[id];
	unsigned int tagId;
	char *tagName;

	tagId = Scr_GetConstLowercaseString(0);
	tagName = SL_ConvertToString(tagId);
	if ( !*tagName )
	{
		tagId = 0;
	}

	if ( !SV_DObjExists(ent) )
	{
		if ( !ent->model )
		{
			stackError("gsc_entity_hastag() entity has no model");
			stackPushInt(0);
			return;
		}
		stackError("gsc_entity_hastag() entity model \'%s\' is invalid", G_ModelName(ent->model));
		stackPushInt(0);
		return;
	}
	if ( tagId && SV_DObjGetBoneIndex(ent, tagId) < 0 )
	{
		SV_DObjDumpInfo(ent);
		stackError("gsc_entity_hastag() tag \'%s\' does not exist in model \'%s\'", tagName, G_ModelName(ent->model));
		stackPushInt(0);
		return;
	}

	stackPushInt(1);
}

void gsc_entity_islinkedto(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *ent = &g_entities[id];

	if ( ent->tagInfo )
	{
		stackPushEntity(&g_entities[ent->tagInfo->parent->s.number]);
	}
	else
	{
		stackPushUndefined();
	}
}

void gsc_entity_isturret(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *ent = &g_entities[id];

	if ( ent->pTurretInfo )
		stackPushBool(qtrue);
	else
		stackPushBool(qfalse);
}

void gsc_entity_getturretowner(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *ent = &g_entities[id];

	if ( ent->pTurretInfo )
	{
		if ( ent->r.ownerNum == ENTITY_NONE )
		{
			stackPushUndefined();
		}
		else
		{
			stackPushEntity(&g_entities[ent->r.ownerNum]);
		}
	}
	else
	{
		stackError("gsc_entity_getturretowner() entity is not a turret");
		stackPushUndefined();
	}
}

extern customEntityState_t customEntityState[MAX_GENTITIES];

void gsc_entity_enablebounce(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *ent = &g_entities[id];

	if ( ent->classname == scr_const.script_model )
	{
		if ( customEntityState[id].gravityType )
		{
			float parallelBounce = 0.5; // default from frag_grenade_german_mp
			float perpendicularBounce = 0.25; // default from frag_grenade_german_mp

			if ( Scr_GetNumParam() > 0 )
			{
				parallelBounce = Scr_GetFloat(0);
				if ( Scr_GetNumParam() > 1 )
					perpendicularBounce = Scr_GetFloat(1);
			}

			customEntityState[id].gravityType = GRAVITY_BOUNCE;
			customEntityState[id].parallelBounce = parallelBounce;
			customEntityState[id].perpendicularBounce = perpendicularBounce;
			(ent->s).eFlags |= EF_BOUNCE;

			stackPushBool(qtrue);
		}
		else
		{
			stackPushBool(qfalse);
		}
	}
	else
	{
		stackError("gsc_entity_enablebounce() entity is not a script_model");
		stackPushUndefined();
	}
}

void gsc_entity_disablebounce(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *ent = &g_entities[id];

	if ( ent->classname == scr_const.script_model )
	{
		if ( customEntityState[id].gravityType )
		{
			customEntityState[id].gravityType = GRAVITY_NO_BOUNCE;
			(ent->s).eFlags &= ~EF_BOUNCE;

			stackPushBool(qtrue);
		}
		else
		{
			stackPushBool(qfalse);
		}
	}
	else
	{
		stackError("gsc_entity_disablebounce() entity is not a script_model");
		stackPushUndefined();
	}
}

void gsc_entity_enablegravity(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *ent = &g_entities[id];

	if ( ent->classname == scr_const.script_model )
	{
		int collideModels = qtrue;
		qboolean angledGravity = qtrue;

		if ( Scr_GetNumParam() > 0 && Scr_GetInt(0) == 0 )
		{
			collideModels = qfalse;
		}

		if ( Scr_GetNumParam() > 1 && Scr_GetInt(1) == 0 )
		{
			angledGravity = qfalse;
		}

		customEntityState[id].gravityType = GRAVITY_NO_BOUNCE;
		customEntityState[id].collideModels = collideModels;
		customEntityState[id].angledGravity = angledGravity;
		customEntityState[id].maxVelocity = 8192.0;
		ent->clipmask = 0x2812891;
		ent->physicsObject = 1;
		if ( customEntityState[id].angledGravity )
		{
			(ent->s).apos.trType = TR_LINEAR;
			(ent->s).apos.trTime = level.time;
			VecToAngles((ent->r).currentAngles, (ent->s).apos.trBase);
		}

		stackPushBool(qtrue);
	}
	else
	{
		stackError("gsc_entity_enablegravity() entity is not a script_model");
		stackPushUndefined();
	}
}

void gsc_entity_disablegravity(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *ent = &g_entities[id];

	if ( ent->classname == scr_const.script_model )
	{
		if ( customEntityState[id].gravityType )
		{
			customEntityState[id].gravityType = GRAVITY_NONE;
			customEntityState[id].collideModels = qfalse;
			(ent->s).eFlags &= ~EF_BOUNCE;
			ent->clipmask = 0;
			ent->physicsObject = 0;
			(ent->s).groundEntityNum = ENTITY_NONE;
			G_SetOrigin(ent, ent->r.currentOrigin);
			G_SetAngle(ent, ent->r.currentAngles);

			stackPushBool(qtrue);
		}
		else
		{
			stackPushBool(qfalse);
		}
	}
	else
	{
		stackError("gsc_entity_disablegravity() entity is not a script_model");
		stackPushUndefined();
	}
}

void gsc_entity_isgravityenabled(scr_entref_t ref)
{
	int id = ref.entnum;

	if ( customEntityState[id].gravityType )
		stackPushBool(qtrue);
	else
		stackPushBool(qfalse);
}

void gsc_entity_addentityvelocity(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *ent = &g_entities[id];

	if ( ent->classname == scr_const.script_model )
	{
		if ( customEntityState[id].gravityType )
		{
			vec3_t velocity;

			Scr_GetVector(0, &velocity);
			(ent->s).pos.trType = TR_GRAVITY;
			(ent->s).pos.trTime = level.time;
			VectorCopy((ent->r).currentOrigin, (ent->s).pos.trBase);
			VectorAdd(customEntityState[id].velocity, velocity, (ent->s).pos.trDelta);
			if ( customEntityState[id].angledGravity )
			{
				(ent->s).apos.trType = TR_LINEAR;
				(ent->s).apos.trTime = level.time;
				VecToAngles((ent->r).currentAngles, (ent->s).apos.trBase);
			}
			if( !IsNullVector(velocity) )
				(ent->s).groundEntityNum = ENTITY_NONE;

			stackPushBool(qtrue);
		}
		else
		{
			stackPushBool(qfalse);
		}
	}
	else
	{
		stackError("gsc_entity_addentityvelocity() entity is not a script_model");
		stackPushUndefined();
	}
}

void gsc_entity_setentityvelocity(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *ent = &g_entities[id];

	if ( ent->classname == scr_const.script_model )
	{
		if ( customEntityState[id].gravityType )
		{
			vec3_t velocity;

			Scr_GetVector(0, &velocity);
			(ent->s).pos.trType = TR_GRAVITY;
			(ent->s).pos.trTime = level.time;
			VectorCopy((ent->r).currentOrigin, (ent->s).pos.trBase);
			VectorCopy(velocity, (ent->s).pos.trDelta);
			if ( customEntityState[id].angledGravity )
			{
				(ent->s).apos.trType = TR_LINEAR;
				(ent->s).apos.trTime = level.time;
				VecToAngles((ent->r).currentAngles, (ent->s).apos.trBase);
			}
			if( !IsNullVector(velocity) )
				(ent->s).groundEntityNum = ENTITY_NONE;

			stackPushBool(qtrue);
		}
		else
		{
			stackPushBool(qfalse);
		}
	}
	else
	{
		stackError("gsc_entity_setentityvelocity() entity is not a script_model");
		stackPushUndefined();
	}
}

void gsc_entity_getentityvelocity(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *ent = &g_entities[id];

	if ( ent->classname == scr_const.script_model )
	{
		if ( customEntityState[id].gravityType )
		{
			stackPushVector(customEntityState[id].velocity); // (ent->s).pos.trDelta returns only the added velocity
		}
		else
		{
			stackPushUndefined();
		}
	}
	else
	{
		stackError("gsc_entity_getentityvelocity() entity is not a script_model");
		stackPushUndefined();
	}
}

void gsc_entity_setmaxentityvelocity(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *ent = &g_entities[id];

	if ( ent->classname == scr_const.script_model )
	{
		if ( customEntityState[id].gravityType )
		{
			customEntityState[id].maxVelocity = Scr_GetFloat(0);
			if ( customEntityState[id].maxVelocity < 0.0 )
				customEntityState[id].maxVelocity = 0.0;

			stackPushBool(qtrue);
		}
		else
		{
			stackPushBool(qfalse);
		}
	}
	else
	{
		stackError("gsc_entity_setmaxentityvelocity() entity is not a script_model");
		stackPushUndefined();
	}
}

void gsc_entity_getmaxentityvelocity(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *ent = &g_entities[id];

	if ( ent->classname == scr_const.script_model )
	{
		if ( customEntityState[id].gravityType )
		{
			stackPushFloat(customEntityState[id].maxVelocity);
		}
		else
		{
			stackPushUndefined();
		}
	}
	else
	{
		stackError("gsc_entity_getmaxentityvelocity() entity is not a script_model");
		stackPushUndefined();
	}
}

void gsc_entity_setclipmask(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *ent = &g_entities[id];

	ent->clipmask = Scr_GetInt(0);
	stackPushBool(qtrue);
}

void gsc_entity_getclipmask(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *ent = &g_entities[id];

	stackPushInt(ent->clipmask);
}

void gsc_entity_getcontents(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *ent = &g_entities[id];

	if ( ent->client && customPlayerState[id].overrideContents )
	{
		stackPushInt(customPlayerState[id].contents);
		return;
	}

	stackPushInt((ent->r).contents);
}

void gsc_entity_setlight(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *ent = &g_entities[id];

	if ( ent->classname == scr_const.script_model )
	{
		int r, g, b, i;

		if ( !stackGetParams("iiii", &r, &g, &b, &i) )
		{
			stackError("gsc_entity_setlight() argument is undefined or has a wrong type");
			stackPushUndefined();
			return;
		}

		if ( r < 0 )
		{
			r = 0;
		}
		else if ( r > 255 )
		{
			r = 255;
		}
		if ( g < 0 )
		{
			g = 0;
		}
		else if ( g > 255 )
		{
			g = 255;
		}
		if ( b < 0 )
		{
			b = 0;
		}
		else if ( b > 255 )
		{
			b = 255;
		}
		if ( i < 0 )
		{
			i = 0;
		}
		else if ( i > 255 )
		{
			i = 255;
		}
		ent->s.constantLight = r | ( g << 8 ) | ( b << 16 ) | ( i << 24 );

		stackPushBool(qtrue);
	}
	else
	{
		stackError("gsc_entity_setlight() entity is not a script_model");
		stackPushUndefined();
	}
}

#endif