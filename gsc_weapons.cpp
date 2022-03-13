#include "gsc_weapons.hpp"

#if COMPILE_WEAPONS == 1

qboolean isValidWeaponId(int id)
{
	int weps = BG_GetNumWeapons();

	if (id >= weps || id < 0 || weps == 0)
		return qfalse;

	return qtrue;
}

void gsc_weapons_getweaponmaxammo()
{
	int id;
	char *name;

	if (stackGetParams("s", &name)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("i", &id)) 
	{
		stackError("gsc_weapons_getweaponmaxammo() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_getweaponmaxammo() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}

	WeaponDef_t *weapon = BG_WeaponDefs(id);
	stackPushInt(weapon->iMaxAmmo);
}

void gsc_weapons_getweaponclipsize()
{
	int id;
	char *name;

	if (stackGetParams("s", &name)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("i", &id))
	{
		stackError("gsc_weapons_getweaponclipsize() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_getweaponclipsize() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}

	WeaponDef_t *weapon = BG_WeaponDefs(id);
	stackPushInt(weapon->iClipSize);
}

void gsc_weapons_getweapondamage()
{
	int id;
	char *name;

	if (stackGetParams("s", &name)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("i", &id))
	{
		stackError("gsc_weapons_getweapondamage() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_getweapondamage() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}

	WeaponDef_t *weapon = BG_WeaponDefs(id);
	stackPushInt(weapon->damage);
}

void gsc_weapons_getweaponmeleedamage()
{
	int id;
	char *name;

	if (stackGetParams("s", &name)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("i", &id))
	{
		stackError("gsc_weapons_getweaponmeleedamage() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_getweaponmeleedamage() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}

	WeaponDef_t *weapon = BG_WeaponDefs(id);
	stackPushInt(weapon->iMeleeDamage);
}

void gsc_weapons_getweaponfiretime()
{
	int id;
	char *name;

	if (stackGetParams("s", &name)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("i", &id))
	{
		stackError("gsc_weapons_getweaponfiretime() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_getweaponfiretime() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}

	WeaponDef_t *weapon = BG_WeaponDefs(id);
	stackPushInt(weapon->iFireTime);
}

void gsc_weapons_getweaponmeleetime()
{
	int id;
	char *name;

	if (stackGetParams("s", &name)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("i", &id))
	{
		stackError("gsc_weapons_getweaponmeleetime() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_getweaponmeleetime() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}

	WeaponDef_t *weapon = BG_WeaponDefs(id);
	stackPushInt(weapon->iMeleeTime);
}

void gsc_weapons_getweaponreloadtime()
{
	int id;
	char *name;

	if (stackGetParams("s", &name)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("i", &id))
	{
		stackError("gsc_weapons_getweaponreloadtime() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_getweaponreloadtime() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}

	WeaponDef_t *weapon = BG_WeaponDefs(id);
	stackPushInt(weapon->iReloadTime);
}

void gsc_weapons_getweaponreloademptytime()
{
	int id;
	char *name;

	if (stackGetParams("s", &name)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("i", &id))
	{
		stackError("gsc_weapons_getweaponreloademptytime() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_getweaponreloademptytime() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}

	WeaponDef_t *weapon = BG_WeaponDefs(id);
	stackPushInt(weapon->iReloadEmptyTime);
}

void gsc_weapons_getweaponcookable()
{
	int id;
	char *name;

	if (stackGetParams("s", &name)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("i", &id))
	{
		stackError("gsc_weapons_getweaponcookable() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_getweaponcookable() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}

	WeaponDef_t *weapon = BG_WeaponDefs(id);
	stackPushInt(weapon->bCookOffHold);
}

void gsc_weapons_getweaponfusetime()
{
	int id;
	char *name;

	if (stackGetParams("s", &name)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("i", &id))
	{
		stackError("gsc_weapons_getweaponfusetime() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_getweaponfusetime() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}

	WeaponDef_t *weapon = BG_WeaponDefs(id);
	stackPushInt(weapon->fuseTime);
}

void gsc_weapons_setweapondamage()
{
	int id;
	char *name;
	int damage;

	if (stackGetParams("si", &name, &damage)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("ii", &id, &damage))
	{
		stackError("gsc_weapons_setweapondamage() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_setweapondamage() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}

	WeaponDef_t *weapon = BG_WeaponDefs(id);
	weapon->damage = damage;

	stackPushBool(qtrue);
}

void gsc_weapons_setweaponmaxammo()
{
	int id;
	char *name;
	int ammo;

	if (stackGetParams("si", &name, &ammo)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("ii", &id, &ammo))
	{
		stackError("gsc_weapons_setweaponmaxammo() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_setweaponmaxammo() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}

	WeaponDef_t *weapon = BG_WeaponDefs(id);
	weapon->iMaxAmmo = ammo;

	stackPushBool(qtrue);
}

void gsc_weapons_setweaponclipsize()
{
	int id;
	char *name;
	int clipSize;

	if (stackGetParams("si", &name, &clipSize)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("ii", &id, &clipSize))
	{
		stackError("gsc_weapons_setweaponclipsize() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_setweaponclipsize() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}

	WeaponDef_t *weapon = BG_WeaponDefs(id);
	weapon->iClipSize = clipSize;

	stackPushBool(qtrue);
}

void gsc_weapons_setweaponmeleedamage()
{
	int id;
	char *name;
	int damage;

	if (stackGetParams("si", &name, &damage)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("ii", &id, &damage))
	{
		stackError("gsc_weapons_setweaponmeleedamage() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_setweaponmeleedamage() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}

	WeaponDef_t *weapon = BG_WeaponDefs(id);
	weapon->iMeleeDamage = damage;

	stackPushBool(qtrue);
}

void gsc_weapons_setweaponfiretime()
{
	int id;
	char *name;
	int time;

	if (stackGetParams("si", &name, &time)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("ii", &id, &time))
	{
		stackError("gsc_weapons_setweaponfiretime() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_setweaponfiretime() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}

	WeaponDef_t *weapon = BG_WeaponDefs(id);
	weapon->iFireTime = time;

	stackPushBool(qtrue);
}

void gsc_weapons_setweaponmeleetime()
{
	int id;
	char *name;
	int time;

	if (stackGetParams("si", &name, &time)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("ii", &id, &time))
	{
		stackError("gsc_weapons_setweaponmeleetime() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_setweaponmeleetime() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}

	WeaponDef_t *weapon = BG_WeaponDefs(id);
	weapon->iMeleeTime = time;

	stackPushBool(qtrue);
}

void gsc_weapons_setweaponreloadtime()
{
	int id;
	char *name;
	int time;

	if (stackGetParams("si", &name, &time)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("ii", &id, &time))
	{
		stackError("gsc_weapons_setweaponreloadtime() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_setweaponreloadtime() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}

	WeaponDef_t *weapon = BG_WeaponDefs(id);
	weapon->iReloadTime = time;

	stackPushBool(qtrue);
}

void gsc_weapons_setweaponreloademptytime()
{
	int id;
	char *name;
	int time;

	if (stackGetParams("si", &name, &time)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("ii", &id, &time))
	{
		stackError("gsc_weapons_setweaponreloademptytime() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_setweaponreloademptytime() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}

	WeaponDef_t *weapon = BG_WeaponDefs(id);
	weapon->iReloadEmptyTime = time;

	stackPushBool(qtrue);
}

void gsc_weapons_setweaponcookable()
{
	int id;
	char *name;
	int cookable;

	if (stackGetParams("si", &name, &cookable)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("ii", &id, &cookable))
	{
		stackError("gsc_weapons_setweaponcookable() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_setweaponcookable() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}

	WeaponDef_t *weapon = BG_WeaponDefs(id);
	weapon->bCookOffHold = cookable;

	stackPushBool(qtrue);
}

void gsc_weapons_setweaponfusetime()
{
	int id;
	char *name;
	int time;

	if (stackGetParams("si", &name, &time)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("ii", &id, &time))
	{
		stackError("gsc_weapons_setweaponfusetime() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_setweaponfusetime() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}

	WeaponDef_t *weapon = BG_WeaponDefs(id);
	weapon->fuseTime = time;

	stackPushBool(qtrue);
}

const char *hitlocs[] = { "none", "helmet", "head", "neck", "torso_upper", "torso_lower", "right_arm_upper", "right_arm_lower", "right_hand", "left_arm_upper", "left_arm_lower", "left_hand", "right_leg_upper", "right_leg_lower", "right_foot", "left_leg_upper", "left_leg_lower", "left_foot", "gun" };
int getHitLocOffset(const char *hitloc)
{
	int offset = 0;

	for (int i = 0; i < int( sizeof(hitlocs) / sizeof(hitlocs[0]) ); i++)
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
	char *name;
	char *hitloc;

	if (stackGetParams("ss", &name, &hitloc)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("is", &id, &hitloc))
	{
		stackError("gsc_weapons_getweaponhitlocmultiplier() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_getweaponhitlocmultiplier() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}

	WeaponDef_t *weapon = BG_WeaponDefs(id);
	int offset = getHitLocOffset(hitloc);

	stackPushFloat(weapon->locationDamageMultipliers[offset]);
}

void gsc_weapons_setweaponhitlocmultiplier()
{
	int id;
	char *name;
	float multiplier;
	char* hitloc;

	if (stackGetParams("ssf", &name, &hitloc, &multiplier)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("isf", &id, &hitloc, &multiplier))
	{
		stackError("gsc_weapons_setweaponhitlocmultiplier() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_setweaponhitlocmultiplier() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}

	WeaponDef_t *weapon = BG_WeaponDefs(id);
	int offset = getHitLocOffset(hitloc);

	weapon->locationDamageMultipliers[offset] = multiplier;
	stackPushBool(qtrue);
}

void gsc_weapons_getmovespeedscale()
{
	int id;
	char *name;

	if (stackGetParams("s", &name)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("i", &id))
	{
		stackError("gsc_weapons_getmovespeedscale() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_getmovespeedscale() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}

	WeaponDef_t *weapon = BG_WeaponDefs(id);

	stackPushFloat(weapon->moveSpeedScale);
}

void gsc_weapons_setmovespeedscale()
{
	int id;
    char *name;
	float scale;
    
    if (stackGetParams("sf", &name, &scale)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("if", &id, &scale))
	{
		stackError("gsc_weapons_setmovespeedscale() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_setmovespeedscale() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}

	WeaponDef_t *weapon = BG_WeaponDefs(id);

	weapon->moveSpeedScale = scale;
	stackPushBool(qtrue);
}

void gsc_weapons_getloadedweapons()
{
	int numweapons = BG_GetNumWeapons();

	stackPushArray();

	for (int i = 0; i < numweapons; i++)
	{
		WeaponDef_t *weapon = BG_WeaponDefs(i);
		stackPushString(weapon->szInternalName);
		stackPushArrayLast();
	}
}

void gsc_weapons_getweapondisplayname()
{
	int id;
	char *name;

	if (stackGetParams("s", &name)) 
	{
		id = BG_FindWeaponIndexForName(name);
	}
	else if ( ! stackGetParams("i", &id))
	{
		stackError("gsc_weapons_getweapondisplayname() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!isValidWeaponId(id))
	{
		stackError("gsc_weapons_getweapondisplayname() weapon index is out of bounds");
		stackPushUndefined();
		return;
	}
	
	WeaponDef_t *weapon = BG_WeaponDefs(id);
	stackPushString(weapon->szDisplayName);
	
	VariableValue *var;
	int param = 0;

	var = &scrVmPub.top[-param];
	var->type = STACK_LOCALIZED_STRING;
}

#define MAX_WEAPON_IGNORE_SIZE 20
#define MAX_WEAPON_NAME_SIZE 32
char* defaultweapon_mp = (char*)malloc(MAX_WEAPON_NAME_SIZE);
char ignoredWeapons[MAX_WEAPON_IGNORE_SIZE][MAX_WEAPON_NAME_SIZE];
int ignoredWeaponCount = 0;

void gsc_weapons_init() {
	strcpy(defaultweapon_mp, "defaultweapon_mp");
	defaultweapon_mp[strlen(defaultweapon_mp)] = '\0';
}

void gsc_weapons_free() {
	free(defaultweapon_mp);
}

bool isOnIgnoreList(char* weapon) {
	if(ignoredWeaponCount == 0)
		return false;
	
	for(int i=0;i<ignoredWeaponCount;i++) {
		if(strcmp(ignoredWeapons[i], weapon) == 0)
			return true;
	}
	
	return false;
}

int hook_findWeaponIndex(char* weapon) {
	typedef int (*findIndexWeapon_t)(char* weapon);
	#if COD_VERSION == COD2_1_0
		findIndexWeapon_t findIndexWeapon = (findIndexWeapon_t)0x080E949C;
	#elif COD_VERSION == COD2_1_2
		findIndexWeapon_t findIndexWeapon = (findIndexWeapon_t)0x080EBA8C;
	#elif COD_VERSION == COD2_1_3
		findIndexWeapon_t findIndexWeapon = (findIndexWeapon_t)0x080EBBD0;
	#else
		#warning findIndexWeapon_t findIndexWeapon = NULL;
		findIndexWeapon_t findIndexWeapon = (findIndexWeapon_t)NULL;
	#endif
	
	if(isOnIgnoreList(weapon))
		return findIndexWeapon(defaultweapon_mp);
	else
		return findIndexWeapon(weapon);
}

void gsc_weapons_resetignoredweapons() {
	ignoredWeaponCount = 0;
}

void gsc_weapons_ignoreweapon() {
	char* weapon;
	if ( ! stackGetParams("s", &weapon)) {
		printf("scriptengine> wrongs args for: ignoreWeapon(weapon)\n");
		stackPushUndefined();
		return;
	}
	
	if(strlen(weapon) > MAX_WEAPON_NAME_SIZE - 1) {
		printf("scriptengine> weapon name is too long: ignoreWeapon(weapon)\n");
		stackPushUndefined();
		return;
	}
	
	if(ignoredWeaponCount >= MAX_WEAPON_IGNORE_SIZE) {
		printf("scriptengine> Exceeded MAX_WEAPON_IGNORE_SIZE %d\n", MAX_WEAPON_IGNORE_SIZE);
		stackPushUndefined();
		return;
	}
	
	strcpy(ignoredWeapons[ignoredWeaponCount], weapon);
	ignoredWeapons[ignoredWeaponCount][strlen(weapon)] = '\0';
	ignoredWeaponCount++;
	stackPushInt(1);
}

void gsc_weapons_setdefaultweapon() {
	char* weapon;
	if ( ! stackGetParams("s", &weapon)) {
		printf("scriptengine> wrongs args for: setDefaultWeapon(weapon)\n");
		stackPushUndefined();
		return;
	}
	
	if(strlen(weapon) > MAX_WEAPON_NAME_SIZE - 1) {
		printf("scriptengine> weapon name is too long: setDefaultWeapon(weapon)\n");
		stackPushUndefined();
		return;
	}
	
	if(strcmp(defaultweapon_mp, weapon) == 0) {
		stackPushInt(2);
		return;
	}
	
	strcpy(defaultweapon_mp, weapon);
	defaultweapon_mp[strlen(weapon)] = '\0';
	#if COD_VERSION == COD2_1_0
		memcpy((void*)0x0811E929, &defaultweapon_mp, 4); // default
		memcpy((void*)0x080E8AAD, &defaultweapon_mp, 4); // not found
		//memcpy((void*)0x080F014D, &defaultweapon_mp, 4); // not found backup
		memcpy((void*)0x080E928A, &defaultweapon_mp, 4); // unknown
	#elif COD_VERSION == COD2_1_2
		memcpy((void*)0x08120C5A, &defaultweapon_mp, 4); // default
		memcpy((void*)0x080EB09D, &defaultweapon_mp, 4); // not found
		//memcpy((void*)0x080F273D, &defaultweapon_mp, 4); // not found backup
		memcpy((void*)0x080EB87A, &defaultweapon_mp, 4); // unknown
	#elif COD_VERSION == COD2_1_3
		memcpy((void*)0x08120DB9, &defaultweapon_mp, 4); // default
		memcpy((void*)0x080EB1E1, &defaultweapon_mp, 4); // not found
		//memcpy((void*)0x080F2881, &defaultweapon_mp, 4); // not found backup
		memcpy((void*)0x080EB9BE, &defaultweapon_mp, 4); // unknown
	#endif
	stackPushInt(1);
}

void gsc_weapons_getweaponitemammo(scr_entref_t id)
{
    gentity_t *entity = &g_entities[id];
    gitem_t *bg_item = &bg_itemlist;
    
    if ( entity->s.index < 1 || 0x82 < entity->s.index )
    {
        stackError("gsc_weapons_getweaponitemammo() must be called on a weapon entity");
        stackPushUndefined();
        return;
    }
    
    bg_item += entity->s.index;
    if ( bg_item->giType == IT_WEAPON )
    {
        stackPushInt(bg_item->quantity);
    } else {
        stackError("gsc_weapons_getweaponitemammo() must be called on a weapon entity");
        stackPushUndefined();
        return;
    }
}

void gsc_weapons_setweaponitemammo(scr_entref_t id)
{
    int quantity;
    gentity_t *entity = &g_entities[id];
    gitem_t *bg_item = &bg_itemlist;

    if ( ! stackGetParams("i", &quantity))
    {
        stackError("gsc_weapons_setweaponitemammo() argument is undefined or has a wrong type");
        stackPushUndefined();
        return;
    }

    if ( entity->s.index < 1 || 0x82 < entity->s.index )
    {
        stackError("gsc_weapons_setweaponitemammo() must be called on a weapon entity");
        stackPushUndefined();
        return;
    }
    
    bg_item += entity->s.index;
    if ( bg_item->giType == IT_WEAPON )
    {
        bg_item->quantity = quantity;
        stackPushBool(qtrue);
    } else {
        stackError("gsc_weapons_setweaponitemammo() must be called on a weapon entity");
        stackPushUndefined();
        return;
    }
}

#endif
