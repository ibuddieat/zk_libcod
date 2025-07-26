#include "gsc_level.hpp"

#if COMPILE_LEVEL == 1

void gsc_level_getmovers()
{
	gentity_t *ent = g_entities;
	int i;

	stackPushArray();
	for ( i = 0; i < level.num_entities; i++, ent++ )
	{
		if ( ent->s.eType == ET_SCRIPTMOVER )
		{
			stackPushEntity(ent);
			stackPushArrayLast();
		}
	}
}

void gsc_level_getnumberofstaticmodels()
{
	stackPushInt(cm.numStaticModels);
}

void gsc_level_getstaticmodelname()
{
	int index;

	if ( !stackGetParams("i", &index) )
	{
		stackError("gsc_level_getstaticmodelname() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( index < 0 || index >= (int)cm.numStaticModels )
	{
		stackError("gsc_level_getstaticmodelname() index is out of range");
		stackPushUndefined();
		return;
	}

	stackPushString(cm.staticModelList[index].xmodel->name);
}

void gsc_level_getstaticmodelorigin()
{
	int index;

	if ( !stackGetParams("i", &index) )
	{
		stackError("gsc_level_getstaticmodelorigin() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( index < 0 || index >= (int)cm.numStaticModels )
	{
		stackError("gsc_level_getstaticmodelorigin() index is out of range");
		stackPushUndefined();
		return;
	}

	stackPushVector(cm.staticModelList[index].origin);
}

void gsc_level_getentitycount()
{
	int inUseOnly;
	gentity_t *ent;
	int i;
	int entities = 0;

	if ( !stackGetParams("i", &inUseOnly) )
	{
		inUseOnly = 0;
	}

	if ( inUseOnly )
	{
		ent = g_entities;
		for ( i = 0; i < level.num_entities; i++, ent++ )
		{
			if ( ent->r.inuse != 0 )
			{
				entities++;
			}
		}
		stackPushInt(entities);
	}
	else
	{
		stackPushInt(level.num_entities);
	}
}

void gsc_utils_getsavepersist()
{
	stackPushBool(level.savepersist);
}

void gsc_utils_setsavepersist()
{
	int save;

	if ( !stackGetParams("i",  &save) )
	{
		stackError("gsc_utils_setsavepersist() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	level.savepersist = save;
	stackPushBool(qtrue);
}

#endif