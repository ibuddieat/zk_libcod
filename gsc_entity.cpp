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

void gsc_entity_movegravity(scr_entref_t id)
{
    vec3_t dir;
    float time;

    if ( ! stackGetParams("vf", &dir, &time))
    {
        stackError("gsc_entity_movegravity() one or more arguments is undefined or has a wrong type");
        stackPushUndefined();
        return;
    }

    gentity_t *entity = &g_entities[id];

    entity->s.pos.trType = TR_GRAVITY;
    entity->s.pos.trTime = level.time;
    entity->s.pos.trDuration = time * 1000.0;

    VectorCopy(entity->r.currentOrigin, entity->s.pos.trBase);
    VectorCopy(dir, entity->s.pos.trDelta);

    BG_EvaluateTrajectory(&entity->s.pos, level.time, entity->r.currentOrigin);
    SV_LinkEntity(entity);

    stackPushBool(qtrue);
} 

#endif