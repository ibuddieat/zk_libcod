#include "gsc_entity.hpp"

#if COMPILE_ENTITY == 1

extern customEntityState_t customEntityState[MAX_GENTITIES];
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
	float length, width, height;

	if ( !stackGetParams("fff", &length, &width, &height) )
	{
		stackError("gsc_entity_setbounds() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	gentity_t *entity = &g_entities[id];

	vec3_t mins = {-length, -width, -height};
	vec3_t maxs = {length, width, height};

	VectorCopy(mins, entity->r.mins);
	VectorCopy(maxs, entity->r.maxs);

	stackPushBool(qtrue);
}

void gsc_entity_hidefromplayer(scr_entref_t ref)
{
	gentity_t *player = Scr_GetEntity(0);
	gentity_t *object = &g_entities[ref.entnum];

	if ( player->s.number < MAX_CLIENTS )
	{
		object->r.clientMask[player->s.number >> 5] |= (1 << (player->s.number & 0x1F));
		if ( object->r.clientMask[0] == -1 && object->r.clientMask[1] == -1 )
		{
			object->flags |= FL_INVISIBLE; // Object fully hidden from everyone
		}
		else
		{
			object->flags &= ~FL_INVISIBLE; // Object visible to some
		}
	}
	else
	{
		stackError("gsc_entity_hidefromplayer() not called on a player");
		stackPushUndefined();
		return;
	}
	stackPushBool(qtrue);
}

void gsc_entity_notsolidforplayer(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *entity = &g_entities[id];

	if ( !entity->r.bmodel )
	{
		stackError("gsc_entity_notsolidforplayer() entity %i does not have brush models", id);
		stackPushUndefined();
		return;
	}

	gentity_t *player = Scr_GetEntity(0);
	int id2 = player->s.number;
	if ( id2 >= MAX_CLIENTS )
		Scr_ParamError(0, va("entity %i is not a player", id2));

	customEntityState[id].clientMask[id2 >> 5] |= 1 << (id2 & 0x1F);
	customEntityState[id].notSolidBrushModel = qtrue;

	stackPushBool(qtrue);
}

void gsc_entity_solidforplayer(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *entity = &g_entities[id];

	if ( !entity->r.bmodel )
	{
		stackError("gsc_entity_notsolidforplayer() entity %i does not have brush models", id);
		stackPushUndefined();
		return;
	}

	gentity_t *player = Scr_GetEntity(0);
	int id2 = player->s.number;
	if ( id2 >= MAX_CLIENTS )
		Scr_ParamError(0, va("entity %i is not a player", id2));

	customEntityState[id].clientMask[id2 >> 5] &= ~(1 << (id2 & 0x1F));
	if ( !customEntityState[id].clientMask[0] && !customEntityState[id].clientMask[1] )
		customEntityState[id].notSolidBrushModel = qfalse;

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
	gentity_t *ent = &g_entities[id];
	float tagMat[4][3];
	vec3_t angles;
	unsigned int tagId;
	const char *tagName;

	tagId = Scr_GetConstLowercaseString(0);
	tagName = SL_ConvertToString(tagId);
	if ( !*tagName )
		tagId = 0; // Defaults to tag_origin

	if ( !G_DObjGetWorldTagMatrix(ent, tagId, tagMat) )
	{
		stackError("gsc_entity_gettagangles() could not find tag '%s' on model '%s'", tagName, G_ModelName(ent->model));
		stackPushUndefined();
		return;
	}

	AxisToAngles((float *)tagMat, angles);
	stackPushVector(angles);
}

void gsc_entity_gettagorigin(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *ent = &g_entities[id];
	vec3_t origin;
	unsigned int tagId;
	const char *tagName;

	tagId = Scr_GetConstLowercaseString(0);
	tagName = SL_ConvertToString(tagId);
	if ( !*tagName )
		tagId = 0; // Defaults to tag_origin

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
		if ( ent->r.ownerNum == ENTITYNUM_NONE )
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
			ent->s.eFlags |= EF_BOUNCE;

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
			ent->s.eFlags &= ~EF_BOUNCE;

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
		ent->clipmask = MASK_SHOT | CONTENTS_PLAYERCLIP | CONTENTS_CANSHOTCLIP;
		ent->physicsObject = 1;
		memset(&ent->s.pos, 0, sizeof(trajectory_t));
		if ( customEntityState[id].angledGravity )
		{
			ent->s.apos.trType = TR_LINEAR;
			ent->s.apos.trTime = level.time;
			VecToAngles(ent->r.currentAngles, ent->s.apos.trBase);
		}

		stackPushBool(qtrue);
	}
	else
	{
		stackError("gsc_entity_enablegravity() entity is not a script_model");
		stackPushUndefined();
	}
}

void Scr_DisableGravity(gentity_t *ent)
{
	int id = ent - g_entities;

	customEntityState[id].gravityType = GRAVITY_NONE;
	customEntityState[id].collideModels = qfalse;
	ent->s.eFlags &= ~EF_BOUNCE;
	ent->clipmask = CONTENTS_NONE;
	ent->physicsObject = 0;
	ent->s.groundEntityNum = ENTITYNUM_NONE;
	G_SetOrigin(ent, ent->r.currentOrigin);
	G_SetAngle(ent, ent->r.currentAngles);
}

void gsc_entity_disablegravity(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *ent = &g_entities[id];

	if ( ent->classname == scr_const.script_model )
	{
		if ( customEntityState[id].gravityType )
		{
			Scr_DisableGravity(ent);

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

			Scr_GetVector(0, velocity);
			ent->s.pos.trType = TR_GRAVITY;
			ent->s.pos.trTime = level.time;
			VectorCopy(ent->r.currentOrigin, ent->s.pos.trBase);
			VectorAdd(customEntityState[id].velocity, velocity, ent->s.pos.trDelta);
			if ( customEntityState[id].angledGravity )
			{
				ent->s.apos.trType = TR_LINEAR;
				ent->s.apos.trTime = level.time;
				VecToAngles(ent->r.currentAngles, ent->s.apos.trBase);
			}
			if ( !IsNullVector(velocity) )
				ent->s.groundEntityNum = ENTITYNUM_NONE;

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

			Scr_GetVector(0, velocity);
			ent->s.pos.trType = TR_GRAVITY;
			ent->s.pos.trTime = level.time;
			VectorCopy(ent->r.currentOrigin, ent->s.pos.trBase);
			VectorCopy(velocity, ent->s.pos.trDelta);
			if ( customEntityState[id].angledGravity )
			{
				ent->s.apos.trType = TR_LINEAR;
				ent->s.apos.trTime = level.time;
				VecToAngles(ent->r.currentAngles, ent->s.apos.trBase);
			}
			if ( !IsNullVector(velocity) )
				ent->s.groundEntityNum = ENTITYNUM_NONE;

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
			stackPushVector(customEntityState[id].velocity); // ent->s.pos.trDelta returns only the added velocity
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

	stackPushInt(ent->r.contents);
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

void gsc_entity_getitemquantity(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *entity = &g_entities[id];
	gitem_t *bg_item = &bg_itemlist;

	if ( entity->s.item < 1 || bg_numItems <= entity->s.item )
	{
		stackError("gsc_entity_getitemquantity() must be called on an ammo or health entity");
		stackPushUndefined();
		return;
	}

	bg_item += entity->s.item;
	if ( bg_item->giType == IT_AMMO || bg_item->giType == IT_HEALTH )
	{
		stackPushInt(bg_item->quantity);
	}
	else
	{
		stackError("gsc_entity_getitemquantity() must be called on an ammo or health entity");
		stackPushUndefined();
		return;
	}
}

void gsc_entity_setitemquantity(scr_entref_t ref)
{
	int id = ref.entnum;
	int quantity;
	gentity_t *entity = &g_entities[id];
	gitem_t *bg_item = &bg_itemlist;

	if ( !stackGetParams("i", &quantity) )
	{
		stackError("gsc_entity_setitemquantity() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( entity->s.item < 1 || bg_numItems <= entity->s.item )
	{
		stackError("gsc_entity_setitemquantity() must be called on an ammo or health entity");
		stackPushUndefined();
		return;
	}

	bg_item += entity->s.item;
	if ( bg_item->giType == IT_AMMO || bg_item->giType == IT_HEALTH )
	{
		bg_item->quantity = quantity;
		stackPushBool(qtrue);
	}
	else
	{
		stackError("gsc_entity_setitemquantity() must be called on an ammo or health entity");
		stackPushUndefined();
		return;
	}
}

#endif