#include "gsc_bots.hpp"

#if COMPILE_BOTS == 1

void gsc_bots_set_walkdir(int id)
{
	char* dir;

	if ( ! stackGetParams("s", &dir))
	{
		stackError("gsc_bots_set_walkdir() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (ADDRESSTYPE(id) != NA_BOT)
	{
		stackError("gsc_bots_set_walkdir() player %i is not a bot", id);
		stackPushUndefined();
		return;
	}

	extern int bot_movement[64];

	if (strcmp(dir, "none") == 0)
		bot_movement[id] = (0);
	else if (strcmp(dir, "forward") == 0)
		bot_movement[id] = (127);
	else if (strcmp(dir, "back") == 0)
		bot_movement[id] = (129);
	else if (strcmp(dir, "right") == 0)
		bot_movement[id] = (127 << 8);
	else if (strcmp(dir, "left") == 0)
		bot_movement[id] = (129 << 8);
	else
	{
		stackError("gsc_bots_set_walkdir() invalid argument '%s'. Valid arguments are: 'none' 'forward' 'back' 'right' 'left'", dir);
		stackPushUndefined();
		return;
	}

	stackPushInt(1);
}

void gsc_bots_set_lean(int id)
{
	char* lean;

	if ( ! stackGetParams("s", &lean))
	{
		stackError("gsc_bots_set_lean() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (ADDRESSTYPE(id) != NA_BOT)
	{
		stackError("gsc_bots_set_lean() player %i is not a bot", id);
		stackPushUndefined();
		return;
	}

	extern int bot_lean[64];

	if (strcmp(lean, "none") == 0)
		bot_lean[id] = (0);
	else if (strcmp(lean, "left") == 0)
		bot_lean[id] = (64);
	else if (strcmp(lean, "right") == 0)
		bot_lean[id] = (128);
	else
	{
		stackError("gsc_bots_set_lean() invalid argument '%s'. Valid arguments are: 'right' 'left'", lean);
		stackPushUndefined();
		return;
	}

	stackPushInt(1);
}

void gsc_bots_set_stance(int id)
{
	char* stance;

	if ( ! stackGetParams("s", &stance))
	{
		stackError("gsc_bots_set_stance() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (ADDRESSTYPE(id) != NA_BOT)
	{
		stackError("gsc_bots_set_stance() player %i is not a bot", id);
		stackPushUndefined();
		return;
	}

	extern int bot_stance[64];

	if (strcmp(stance, "stand") == 0)
		bot_stance[id] = (0);
	else if (strcmp(stance, "crouch") == 0)
		bot_stance[id] = (512);
	else if (strcmp(stance, "prone") == 0)
		bot_stance[id] = (256);
	else if (strcmp(stance, "jump") == 0)
		bot_stance[id] = (1024);
	else
	{
		stackError("gsc_bots_set_stance() invalid argument '%s'. Valid arguments are: 'stand' 'crouch' 'prone' 'jump'", stance);
		stackPushUndefined();
		return;
	}

	stackPushInt(1);
}

void gsc_bots_thrownade(int id)
{
	int grenade;

	if ( ! stackGetParams("i", &grenade))
	{
		stackError("gsc_bots_thrownade() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (ADDRESSTYPE(id) != NA_BOT)
	{
		stackError("gsc_bots_thrownade() player %i is not a bot", id);
		stackPushUndefined();
		return;
	}

	extern int bot_grenade[64];

	if (!grenade)
		bot_grenade[id] = (0);
	else
		bot_grenade[id] = (65536);

	stackPushInt(1);
}

void gsc_bots_fireweapon(int id)
{
	int shoot;

	if ( ! stackGetParams("i", &shoot))
	{
		stackError("gsc_bots_fireweapon() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (ADDRESSTYPE(id) != NA_BOT)
	{
		stackError("gsc_bots_fireweapon() player %i is not a bot", id);
		stackPushUndefined();
		return;
	}

	extern int bot_shoot[64];

	if (!shoot)
		bot_shoot[id] = (0);
	else
		bot_shoot[id] = (1);

	stackPushInt(1);
}

void gsc_bots_meleeweapon(int id)
{
	int melee;

	if ( ! stackGetParams("i", &melee))
	{
		stackError("gsc_bots_meleeweapon() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (ADDRESSTYPE(id) != NA_BOT)
	{
		stackError("gsc_bots_meleeweapon() player %i is not a bot", id);
		stackPushUndefined();
		return;
	}

	extern int bot_melee[64];

	if (!melee)
		bot_melee[id] = (0);
	else
		bot_melee[id] = (32772);

	stackPushInt(1);
}

void gsc_bots_reloadweapon(int id)
{
	int reload;

	if ( ! stackGetParams("i", &reload))
	{
		stackError("gsc_bots_reloadweapon() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (ADDRESSTYPE(id) != NA_BOT)
	{
		stackError("gsc_bots_reloadweapon() player %i is not a bot", id);
		stackPushUndefined();
		return;
	}

	extern int bot_reload[64];

	if (!reload)
		bot_reload[id] = (0);
	else
		bot_reload[id] = (16);

	stackPushInt(1);
}

void gsc_bots_adsaim(int id)
{
	int ads;

	if ( ! stackGetParams("i", &ads))
	{
		stackError("gsc_bots_adsaim() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (ADDRESSTYPE(id) != NA_BOT)
	{
		stackError("gsc_bots_adsaim() player %i is not a bot", id);
		stackPushUndefined();
		return;
	}

	extern int bot_ads[64];

	if (!ads)
		bot_ads[id] = (0);
	else
		bot_ads[id] = (4096);

	stackPushInt(1);
}

void gsc_bots_switchtoweaponid(int id)
{
	int weaponid;

	if ( ! stackGetParams("i", &weaponid))
	{
		stackError("gsc_bots_switchtoweaponid() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (ADDRESSTYPE(id) != NA_BOT)
	{
		stackError("gsc_bots_switchtoweaponid() player %i is not a bot", id);
		stackPushUndefined();
		return;
	}

	extern int bot_weapon[64];

	bot_weapon[id] = (weaponid);
	stackPushInt(1);
}

#endif