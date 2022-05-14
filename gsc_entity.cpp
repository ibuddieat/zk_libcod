#include "gsc_entity.hpp"

#if COMPILE_ENTITY == 1

void gsc_entity_setalive(scr_entref_t id)
{
	int isAlive;

	if ( ! stackGetParams("i", &isAlive))
	{
		stackError("gsc_entity_setalive() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	gentity_t *entity = &g_entities[id];

	entity->takedamage = isAlive;
	stackPushBool(qtrue);
}

void gsc_entity_setbounds(scr_entref_t id)
{
	float width, height;

	if ( ! stackGetParams("ff", &width, &height))
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

void gsc_entity_gettagangles(scr_entref_t id)
{
	gentity_t *parent = &g_entities[id];

    if ( (parent->flags & 0x1000U) == 0 )
    {
        // try to enableLinkTo if it's disabled
        if ( parent->s.eType || parent->physicsObject )
        {
            stackError("gsc_entity_gettagangles() entity (classname: \'%s\') does not currently support enableLinkTo", SL_ConvertToString(parent->classname));
            stackPushUndefined();
            return;
        }
        parent->flags = parent->flags | 0x1000;
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

    // create an entity that we can link to
    gentity_t *tempEnt = G_Spawn();
    SV_LinkEntity(tempEnt);

    // link entities with zero offsets
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

    // apply movement as if a frame would have passed
    vec3_t angles;
    G_GeneralLink(tempEnt);
    VectorCopy(tempEnt->r.currentAngles, angles);
    G_FreeEntity(tempEnt);
    
    stackPushVector(angles);
}

void gsc_entity_gettagorigin(scr_entref_t id)
{
	gentity_t *parent = &g_entities[id];

    if ( (parent->flags & 0x1000U) == 0 )
    {
        // try to enableLinkTo if it's disabled
        if ( parent->s.eType || parent->physicsObject )
        {
            stackError("gsc_entity_gettagorigin() entity (classname: \'%s\') does not currently support enableLinkTo", SL_ConvertToString(parent->classname));
            stackPushUndefined();
            return;
        }
        parent->flags = parent->flags | 0x1000;
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

    // create an entity that we can link to
    gentity_t *tempEnt = G_Spawn();
    SV_LinkEntity(tempEnt);

    // link entities with zero offsets
    if ( !G_EntLinkToWithOffset(tempEnt, parent, tagId, &originOffset, &anglesOffset) )
    {
        G_FreeEntity(tempEnt);
        if ( !SV_DObjExists(parent) )
        {
            if ( !parent->model )
            {
                stackError("gsc_entity_gettagorigin() failed to link entity since parent has no model");
                stackPushUndefined();
                return;
            }
            stackError("gsc_entity_gettagorigin() failed to link entity since parent model \'%s\' is invalid", G_ModelName(parent->model));
            stackPushUndefined();
            return;
        }
        if ( tagId && SV_DObjGetBoneIndex(parent, tagId) < 0 )
        {
            SV_DObjDumpInfo(parent);
            stackError("gsc_entity_gettagorigin() failed to link entity since tag \'%s\' does not exist in parent model \'%s\'", tagName, G_ModelName(parent->model));
            stackPushUndefined();
            return;
        }
        stackError("gsc_entity_gettagorigin() failed to link entity due to link cycle");
        stackPushUndefined();
        return;
    }

    // apply movement as if a frame would have passed
    vec3_t origin;
    G_GeneralLink(tempEnt);
    VectorCopy(tempEnt->r.currentOrigin, origin);
    G_FreeEntity(tempEnt);
    
    stackPushVector(origin);
}

void gsc_entity_hastag(scr_entref_t id)
{
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

void gsc_player_islinkedto(scr_entref_t id)
{
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

#endif