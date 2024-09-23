#include "gsc_bots.hpp"

#if COMPILE_BOTS == 1

extern customPlayerState_t customPlayerState[MAX_CLIENTS];

void gsc_bots_setwalkvalues(scr_entref_t ref)
{
	int id = ref.entnum;
    int fwcount;
    int rgcount;

    if ( !stackGetParams("ii", &fwcount, &rgcount) )
    {
        stackError("gsc_bots_setwalkvalues() arguments are undefined or have a wrong type");
        stackPushUndefined();
        return;
    }

    customPlayerState[id].botForwardMove = fwcount;
    customPlayerState[id].botRightMove = rgcount;

	stackPushBool(qtrue);
}

void gsc_bots_setwalkdir(scr_entref_t ref)
{
	int id = ref.entnum;
	char *dir;

	if ( !stackGetParams("s", &dir) )
	{
		stackError("gsc_bots_setwalkdir() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( id >= MAX_CLIENTS )
	{
		stackError("gsc_bots_setwalkdir() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	if ( client->netchan.remoteAddress.type != NA_BOT )
	{
		stackError("gsc_bots_setwalkdir() player %i is not a bot", id);
		stackPushUndefined();
		return;
	}

	if ( strcmp(dir, "none") == 0 )
	{
		customPlayerState[id].botForwardMove = KEY_MASK_NONE;
		customPlayerState[id].botRightMove = KEY_MASK_NONE;
	}
	else if ( strcmp(dir, "forward") == 0 )
		customPlayerState[id].botForwardMove = KEY_MASK_FORWARD;
	else if ( strcmp(dir, "back") == 0 )
		customPlayerState[id].botForwardMove = KEY_MASK_BACK;
	else if ( strcmp(dir, "right") == 0 )
		customPlayerState[id].botRightMove = KEY_MASK_MOVERIGHT;
	else if ( strcmp(dir, "left") == 0 )
		customPlayerState[id].botRightMove = KEY_MASK_MOVELEFT;
	else
	{
		stackError("gsc_bots_setwalkdir() invalid argument '%s'. Valid arguments are: 'none' 'forward' 'back' 'right' 'left'", dir);
		stackPushUndefined();
		return;
	}

	stackPushBool(qtrue);
}

void gsc_bots_setlean(scr_entref_t ref)
{
	int id = ref.entnum;
	char *lean;

	if ( !stackGetParams("s", &lean) )
	{
		stackError("gsc_bots_setlean() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( id >= MAX_CLIENTS )
	{
		stackError("gsc_bots_setlean() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	if ( client->netchan.remoteAddress.type != NA_BOT )
	{
		stackError("gsc_bots_setlean() player %i is not a bot", id);
		stackPushUndefined();
		return;
	}

	if ( strcmp(lean, "none") == 0 )
		customPlayerState[id].botButtons &= ~(KEY_MASK_LEANLEFT | KEY_MASK_LEANRIGHT);
	else if ( strcmp(lean, "left") == 0 )
		customPlayerState[id].botButtons |= KEY_MASK_LEANLEFT;
	else if ( strcmp(lean, "right") == 0 )
		customPlayerState[id].botButtons |= KEY_MASK_LEANRIGHT;
	else
	{
		stackError("gsc_bots_setlean() invalid argument '%s'. Valid arguments are: 'right' 'left'", lean);
		stackPushUndefined();
		return;
	}

	stackPushBool(qtrue);
}

void gsc_bots_setbotstance(scr_entref_t ref)
{
	int id = ref.entnum;
	char *stance;

	if ( !stackGetParams("s", &stance) )
	{
		stackError("gsc_bots_setbotstance() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( id >= MAX_CLIENTS )
	{
		stackError("gsc_bots_setbotstance() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	if ( client->netchan.remoteAddress.type != NA_BOT )
	{
		stackError("gsc_bots_setbotstance() player %i is not a bot", id);
		stackPushUndefined();
		return;
	}

	if ( strcmp(stance, "stand") == 0 )
		customPlayerState[id].botButtons &= ~(KEY_MASK_CROUCH | KEY_MASK_PRONE | KEY_MASK_JUMP);
	else if ( strcmp(stance, "crouch") == 0 )
		customPlayerState[id].botButtons |= KEY_MASK_CROUCH;
	else if ( strcmp(stance, "prone") == 0 )
		customPlayerState[id].botButtons |= KEY_MASK_PRONE;
	else if ( strcmp(stance, "jump") == 0 )
		customPlayerState[id].botButtons |= KEY_MASK_JUMP;
	else
	{
		stackError("gsc_bots_setbotstance() invalid argument '%s'. Valid arguments are: 'stand' 'crouch' 'prone' 'jump'", stance);
		stackPushUndefined();
		return;
	}

	stackPushBool(qtrue);
}

void gsc_bots_throwgrenade(scr_entref_t ref)
{
	int id = ref.entnum;
	int grenade;

	if ( !stackGetParams("i", &grenade) )
	{
		stackError("gsc_bots_throwgrenade() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( id >= MAX_CLIENTS )
	{
		stackError("gsc_bots_throwgrenade() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	if ( client->netchan.remoteAddress.type != NA_BOT )
	{
		stackError("gsc_bots_throwgrenade() player %i is not a bot", id);
		stackPushUndefined();
		return;
	}

	if ( !grenade )
		customPlayerState[id].botButtons &= ~KEY_MASK_FRAG;
	else
		customPlayerState[id].botButtons |= KEY_MASK_FRAG;

	stackPushBool(qtrue);
}

void gsc_bots_throwsmokegrenade(scr_entref_t ref)
{
	int id = ref.entnum;
	int grenade;

	if ( !stackGetParams("i", &grenade) )
	{
		stackError("gsc_bots_throwsmokegrenade() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( id >= MAX_CLIENTS )
	{
		stackError("gsc_bots_throwsmokegrenade() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	if ( client->netchan.remoteAddress.type != NA_BOT )
	{
		stackError("gsc_bots_throwsmokegrenade() player %i is not a bot", id);
		stackPushUndefined();
		return;
	}

	if ( !grenade )
		customPlayerState[id].botButtons &= ~KEY_MASK_SMOKE;
	else
		customPlayerState[id].botButtons |= KEY_MASK_SMOKE;

	stackPushBool(qtrue);
}

void gsc_bots_fireweapon(scr_entref_t ref)
{
	int id = ref.entnum;
	int shoot;

	if ( !stackGetParams("i", &shoot) )
	{
		stackError("gsc_bots_fireweapon() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( id >= MAX_CLIENTS )
	{
		stackError("gsc_bots_fireweapon() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	if ( client->netchan.remoteAddress.type != NA_BOT )
	{
		stackError("gsc_bots_fireweapon() player %i is not a bot", id);
		stackPushUndefined();
		return;
	}

	if ( !shoot )
		customPlayerState[id].botButtons &= ~KEY_MASK_FIRE;
	else
		customPlayerState[id].botButtons |= KEY_MASK_FIRE;

	stackPushBool(qtrue);
}

void gsc_bots_meleeweapon(scr_entref_t ref)
{
	int id = ref.entnum;
	int melee;

	if ( !stackGetParams("i", &melee) )
	{
		stackError("gsc_bots_meleeweapon() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( id >= MAX_CLIENTS )
	{
		stackError("gsc_bots_meleeweapon() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	if ( client->netchan.remoteAddress.type != NA_BOT )
	{
		stackError("gsc_bots_meleeweapon() player %i is not a bot", id);
		stackPushUndefined();
		return;
	}

	if ( !melee )
		customPlayerState[id].botButtons &= ~KEY_MASK_MELEE;
	else
		customPlayerState[id].botButtons |= KEY_MASK_MELEE;

	stackPushBool(qtrue);
}

void gsc_bots_reloadweapon(scr_entref_t ref)
{
	int id = ref.entnum;
	int reload;

	if ( !stackGetParams("i", &reload) )
	{
		stackError("gsc_bots_reloadweapon() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( id >= MAX_CLIENTS )
	{
		stackError("gsc_bots_reloadweapon() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	if ( client->netchan.remoteAddress.type != NA_BOT )
	{
		stackError("gsc_bots_reloadweapon() player %i is not a bot", id);
		stackPushUndefined();
		return;
	}

	if ( !reload )
		customPlayerState[id].botButtons &= ~KEY_MASK_RELOAD;
	else
		customPlayerState[id].botButtons |= KEY_MASK_RELOAD;

	stackPushBool(qtrue);
}

void gsc_bots_setaim(scr_entref_t ref)
{
	int id = ref.entnum;
	int ads;

	if ( !stackGetParams("i", &ads) )
	{
		stackError("gsc_bots_setaim() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( id >= MAX_CLIENTS )
	{
		stackError("gsc_bots_setaim() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	if ( client->netchan.remoteAddress.type != NA_BOT )
	{
		stackError("gsc_bots_setaim() player %i is not a bot", id);
		stackPushUndefined();
		return;
	}

	if ( !ads )
		customPlayerState[id].botButtons &= ~KEY_MASK_ADS_MODE;
	else
		customPlayerState[id].botButtons |= KEY_MASK_ADS_MODE;

	stackPushBool(qtrue);
}

void gsc_bots_switchtoweaponid(scr_entref_t ref)
{
	int id = ref.entnum;
	int weaponid;

	if ( !stackGetParams("i", &weaponid) )
	{
		stackError("gsc_bots_switchtoweaponid() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( id >= MAX_CLIENTS )
	{
		stackError("gsc_bots_switchtoweaponid() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	if ( client->netchan.remoteAddress.type != NA_BOT )
	{
		stackError("gsc_bots_switchtoweaponid() player %i is not a bot", id);
		stackPushUndefined();
		return;
	}

	customPlayerState[id].botWeapon = weaponid;

	stackPushBool(qtrue);
}

void gsc_bots_setnexttestclientname()
{
	char *str;

	if ( !stackGetParams("s", &str) )
	{
		stackError("gsc_bots_setnexttestclientname() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

    if ( !strlen(str) || strlen(str) > 31 )
    {
		stackError("gsc_bots_setnexttestclientname() requires a string of length 1-31 characters");
		stackPushUndefined();
		return;
    }
	
    // original str size = 158 chars + null + 25 more free nulls after in data segment
    // removed the "head" and "color" values so we have enough space for a full 31 character long bot name and do not risk running into an overflow when the bot id gets long
    // the bot id that is incremented in SV_AddTestClient is dumped into the "model" value
    snprintf(&testclient_connect_string, 184, "connect \"\\cg_predictItems\\1\\cl_punkbuster\\0\\cl_anonymous\\0\\model\\%%d\\snaps\\20\\rate\\5000\\name\\%s\\protocol\\%%d\"", str);

	stackPushBool(qtrue);
}

void gsc_bots_resettestclientnaming()
{
    snprintf(&testclient_connect_string, 184, "connect \"\\cg_predictItems\\1\\cl_punkbuster\\0\\cl_anonymous\\0\\color\\4\\head\\default\\model\\multi\\snaps\\20\\rate\\5000\\name\\bot%%d\\protocol\\%%d\"");

	stackPushBool(qtrue);
}

#endif