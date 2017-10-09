#include "gsc_weapons.hpp"

#if COMPILE_WEAPONS == 1

bool isValidWeaponId(int id)
{
	int weps = BG_GetNumWeapons();
	if(id >= weps || id < 0 || weps == 0)
		return false;

	return true;
}

void gsc_weapons_getweaponoffsetint(char* funcname, int offset)
{
	int id;
	if ( ! stackGetParams("i", &id))
	{
		stackError("wrongs args for: %s(id)", funcname);
		stackPushUndefined();
		return;
	}

	if(!isValidWeaponId(id))
	{
		stackError("index is out of bounds: %s(id)", funcname);
		stackPushUndefined();
		return;
	}

	int value = *(int*)(BG_WeaponDefs(id) + offset);
	stackPushInt(value);
}

void gsc_weapons_setweaponoffsetint(char* funcname, int offset)
{
	int id;
	int value;
	if ( ! stackGetParams("ii", &id, &value))
	{
		stackError("scriptengine> wrongs args for: %s(id, value)", funcname);
		stackPushUndefined();
		return;
	}

	if(!isValidWeaponId(id))
	{
		stackError("index is out of bounds: %s(id, value)", funcname);
		stackPushUndefined();
		return;
	}

	int* index = (int*)(BG_WeaponDefs(id) + offset);
	*index = value;
	stackPushInt(1);
}

void gsc_weapons_getweaponmaxammo()
{
	gsc_weapons_getweaponoffsetint("getweaponmaxammo", 468);
}

void gsc_weapons_getweaponclipsize()
{
	gsc_weapons_getweaponoffsetint("getweaponclipsize", 472);
}

void gsc_weapons_getweapondamage()
{
	gsc_weapons_getweaponoffsetint("getweapondamage", 492);
}

void gsc_weapons_setweapondamage()
{
	gsc_weapons_setweaponoffsetint("setweapondamage", 492);
}

void gsc_weapons_getweaponmeleedamage()
{
	gsc_weapons_getweaponoffsetint("getweapondamagemelee", 500);
}

void gsc_weapons_setweaponmeleedamage()
{
	gsc_weapons_setweaponoffsetint("setweapondamagemelee", 500);
}

void gsc_weapons_getweaponfiretime()
{
	gsc_weapons_getweaponoffsetint("getweaponfiretime", 516);
}

void gsc_weapons_setweaponfiretime()
{
	gsc_weapons_setweaponoffsetint("setweaponfiretime", 516); // see 80EF58A
}

void gsc_weapons_getweaponmeleetime()
{
	gsc_weapons_getweaponoffsetint("getweaponmeleetime", 532);
}

void gsc_weapons_setweaponmeleetime()
{
	gsc_weapons_setweaponoffsetint("setweaponmeleetime", 532);
}

void gsc_weapons_getweaponreloadtime()
{
	gsc_weapons_getweaponoffsetint("getweaponreloadtime", 536);
}

void gsc_weapons_setweaponreloadtime()
{
	gsc_weapons_setweaponoffsetint("setweaponreloadtime", 536);
}

void gsc_weapons_getweaponreloademptytime()
{
	gsc_weapons_getweaponoffsetint("getweaponreloademptytime", 540);
}

void gsc_weapons_setweaponreloademptytime()
{
	gsc_weapons_setweaponoffsetint("setweaponreloademptytime", 540);
}

char* hitlocs[] = { "none", "helmet", "head", "neck", "torso_upper", "torso_lower", "right_arm_upper", "right_arm_lower", "right_hand", "left_arm_upper", "left_arm_lower", "left_hand", "right_leg_upper", "right_leg_lower", "right_foot", "left_leg_upper", "left_leg_lower", "left_foot", "gun" };
int getHitLocOffset(char* hitloc)
{
	int offset = 0; // none
	for (int i = 0; i < int( sizeof(hitlocs) / sizeof(hitlocs[0]) ); i++) // prevent out of bound
	{
		if (strcmp(hitlocs[i], hitloc) == 0)
		{
			offset = i;
			break;
		}
	}
	return offset;
}

void gsc_weapons_getweaponhitlocmultiplier()
{
	int id;
	char* hitloc;
	if ( ! stackGetParams("is", &id, &hitloc))
	{
		stackError("gsc_weapons_getweaponhitlocmultiplier() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if(!isValidWeaponId(id))
	{
		stackError("index is out of bounds: getweaponhitlocmultiplier(id, hitloc)");
		stackPushUndefined();
		return;
	}

	int offset = getHitLocOffset(hitloc);
	float multiplier = *(float*)(BG_WeaponDefs(id) + 4 * offset + 1456);
	stackPushFloat(multiplier);
}

void gsc_weapons_setweaponhitlocmultiplier()
{
	int id;
	float multiplier;
	char* hitloc;
	if ( ! stackGetParams("isf", &id, &hitloc, &multiplier))
	{
		stackError("gsc_weapons_setweaponhitlocmultiplier() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if(!isValidWeaponId(id))
	{
		stackError("index out of bounds: getweaponhitlocmultiplier(id, hitloc, multiplier)");
		stackPushUndefined();
		return;
	}

	int offset = getHitLocOffset(hitloc);
	float* multiPointer = (float*)(BG_WeaponDefs(id) + 4 * offset + 1456);
	*multiPointer = multiplier;
	stackPushFloat(1);
}

void gsc_weapons_getloadedweapons()
{
	stackPushArray();
	int weps = BG_GetNumWeapons();
	if(weps == 0)
	{
		stackError("gsc_weapons_getloadedweapons() an error has occured");
		stackPushUndefined();
		return;
	}

	for(int i=0; i<weps; i++)
	{
		int w = BG_WeaponDefs(i);
		stackPushString(*(char**)w);
		stackPushArrayLast();
	}

	// the offset are written in hex after each name (e.g fireTime at 8187084 with 0x204 (516))
	// 0 = weapon_mp
	// 4 = display name
	// 468 = max ammo
	// 472 = clip size
	// 476 = shot count
	// 492 = damage
	// 500 = melee damage
	// 612 = moveSpeedScale // see 80E1C58 (cod2 1.3) call 80E268A
	// 1456 - 1528 = locNone till locGun
	// [id][weapon_mp][worldmodel][viewmodel]: displayname
}

#endif