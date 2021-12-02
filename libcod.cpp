#include "gsc.hpp"

/* ... ... ... */

cvar_t *sv_maxclients;
cvar_t *sv_allowDownload;
cvar_t *sv_pure;
cvar_t *developer;
cvar_t *rcon_password;
cvar_t *cl_allowDownload;
cvar_t *sv_timeout;
cvar_t *sv_zombietime;

#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
cvar_t *sv_wwwDownload;
cvar_t *cl_wwwDownload;
#endif

cvar_t *sv_cracked;
cvar_t *sv_noauthorize;
cvar_t *g_debugEvents;
cvar_t *g_playerCollision;
cvar_t *g_playerEject;
cvar_t *sv_allowRcon;
cvar_t *sv_limitLocalRcon;
cvar_t *fs_library;
cvar_t *sv_downloadMessage;
cvar_t *fs_callbacks;

cHook *hook_gametype_scripts;
cHook *hook_player_collision;
cHook *hook_player_eject;
cHook *hook_fire_grenade;
cHook *hook_play_movement;
cHook *hook_play_endframe;
cHook *hook_set_anim;
cHook *hook_init_opcode;
cHook *hook_add_opcode;
cHook *hook_print_codepos;
cHook *hook_touch_item;
cHook *hook_g_tempentity;
cHook *hook_msg_writedeltaplayerstate;

int codecallback_remotecommand = 0;
int codecallback_playercommand = 0;
int codecallback_userinfochanged = 0;
int codecallback_fire_grenade = 0;
int codecallback_vid_restart = 0;
int codecallback_client_spam = 0;
int codecallback_meleebutton = 0;
int codecallback_usebutton = 0;
int codecallback_attackbutton = 0;

/* ... ... ... */

void hook_sv_init(const char *format, ...)
{
	char s[COD2_MAX_STRINGLENGTH];
	va_list va;

	va_start(va, format);
	vsnprintf(s, sizeof(s), format, va);
	va_end(va);

	Com_Printf("%s", s);

	/* Do stuff after sv has been initialized here */
	
	// Register custom cvars
	sv_cracked = Cvar_RegisterBool("sv_cracked", qfalse, CVAR_ARCHIVE);
	sv_noauthorize = Cvar_RegisterBool("sv_noauthorize", qfalse, CVAR_ARCHIVE);
    g_debugEvents = Cvar_RegisterBool("g_debugEvents", qfalse, CVAR_ARCHIVE);
	g_playerCollision = Cvar_RegisterBool("g_playerCollision", qtrue, CVAR_ARCHIVE);
	g_playerEject = Cvar_RegisterBool("g_playerEject", qtrue, CVAR_ARCHIVE);
	sv_allowRcon = Cvar_RegisterBool("sv_allowRcon", qtrue, CVAR_ARCHIVE);
    sv_limitLocalRcon = Cvar_RegisterBool("sv_limitLocalRcon", qfalse, CVAR_ARCHIVE);
	fs_library = Cvar_RegisterString("fs_library", "", CVAR_ARCHIVE);
	sv_downloadMessage = Cvar_RegisterString("sv_downloadMessage", "", CVAR_ARCHIVE);
	fs_callbacks = Cvar_RegisterString("fs_callbacks", "", CVAR_ARCHIVE);

	// Force download on clients
	cl_allowDownload = Cvar_RegisterBool("cl_allowDownload", qtrue, CVAR_ARCHIVE | CVAR_SYSTEMINFO);

	#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
		cl_wwwDownload = Cvar_RegisterBool("cl_wwwDownload", qtrue, CVAR_ARCHIVE | CVAR_SYSTEMINFO);
	#endif

	sv_maxclients = Cvar_FindVar("sv_maxclients");
	sv_allowDownload = Cvar_FindVar("sv_allowDownload");
	sv_pure = Cvar_FindVar("sv_pure");
	developer = Cvar_FindVar("developer");
	rcon_password = Cvar_FindVar("rcon_password");
	sv_timeout = Cvar_FindVar("sv_timeout");
	sv_zombietime = Cvar_FindVar("sv_zombietime");

#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
	sv_wwwDownload = Cvar_FindVar("sv_wwwDownload");
#endif

}

void hook_bad_printf(const char *format, ...) {}

void hook_sv_spawnserver(const char *format, ...)
{
	char s[COD2_MAX_STRINGLENGTH];
	va_list va;

	va_start(va, format);
	vsnprintf(s, sizeof(s), format, va);
	va_end(va);

	Com_Printf("%s", s);
	
	/* Do stuff after sv has been spawned here */
}

int hook_codscript_gametype_scripts()
{
	char path_for_cb[512] = "maps/mp/gametypes/_callbacksetup";
	
	if (strlen(fs_callbacks->string))
		strncpy(path_for_cb, fs_callbacks->string, sizeof(path_for_cb));
	
	hook_gametype_scripts->unhook();
			
	codecallback_remotecommand = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_RemoteCommand", 0);
	codecallback_playercommand = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_PlayerCommand", 0);
	codecallback_userinfochanged = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_UserInfoChanged", 0);
	codecallback_fire_grenade = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_FireGrenade", 0);
	codecallback_vid_restart = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_VidRestart", 0);
	codecallback_client_spam = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_CLSpam", 0);
	codecallback_meleebutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_MeleeButton", 0);
 	codecallback_usebutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_UseButton", 0);
	codecallback_attackbutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_AttackButton", 0);

	int (*sig)();
	*(int *)&sig = hook_gametype_scripts->from;
	int ret = sig();
	hook_gametype_scripts->hook();

	return ret;
}

int player_collision(int a1)
{
	hook_player_collision->unhook();

	int (*sig)(int a1);
	*(int *)&sig = hook_player_collision->from;

	int ret;

	if (g_playerCollision->boolean)
		ret = sig(a1);
	else
		ret = 0;

	hook_player_collision->hook();

	return ret;
}

int player_eject(int a1)
{
	hook_player_eject->unhook();

	int (*sig)(int a1);
	*(int *)&sig = hook_player_eject->from;

	int ret;

	if (g_playerEject->boolean)
		ret = sig(a1);
	else
		ret = 0;

	hook_player_eject->hook();

	return ret;
}

gentity_t* fire_grenade(gentity_t *self, vec3_t start, vec3_t dir, int weapon, int time)
{
	hook_fire_grenade->unhook();

	gentity_t* (*sig)(gentity_t *self, vec3_t start, vec3_t dir, int weapon, int time);
	*(int *)&sig = hook_fire_grenade->from;

	gentity_t* grenade = sig(self, start, dir, weapon, time);

	hook_fire_grenade->hook();

	if (codecallback_fire_grenade)
	{
		WeaponDef_t *def = BG_WeaponDefs(weapon);
		stackPushString(def->szInternalName);
		stackPushEntity(grenade);
		short ret = Scr_ExecEntThread(self, codecallback_fire_grenade, 2);
		Scr_FreeThread(ret);
	}

	return grenade;
}

void hook_ClientCommand(int clientNum)
{

	if (!Scr_IsSystemActive())
		return;
			
	if ( ! codecallback_playercommand)
	{	
		ClientCommand(clientNum);
		return;
	}

	stackPushArray();
	int args = Cmd_Argc();
	for (int i = 0; i < args; i++)
	{
		char tmp[COD2_MAX_STRINGLENGTH];
		SV_Cmd_ArgvBuffer(i, tmp, sizeof(tmp));
		if(i == 1 && tmp[0] >= 20 && tmp[0] <= 22)
		{
			char *part = strtok(tmp + 1, " ");
			while(part != NULL)
			{
				stackPushString(part);
				stackPushArrayLast();
				part = strtok(NULL, " ");
			}
		}
		else
		{
			stackPushString(tmp);
			stackPushArrayLast();
		}
	}

	short ret = Scr_ExecEntThread(&g_entities[clientNum], codecallback_playercommand, 1);
	Scr_FreeThread(ret);
}

int hook_isLanAddress(netadr_t adr)
{
	if (sv_noauthorize->boolean)
		return 1;

	return Sys_IsLANAddress(adr);
}

const char* hook_AuthorizeState(int arg)
{
	const char *s = Cmd_Argv(arg);

	if (sv_cracked->boolean && strcmp(s, "deny") == 0)
		return "accept";

	return s;
}

void hook_ClientUserinfoChanged(int clientNum)
{
	if (!Scr_IsSystemActive())
		return;
		
	if ( ! codecallback_userinfochanged)
	{	
		ClientUserinfoChanged(clientNum);
		return;
	}
			
	stackPushInt(clientNum); // one parameter is required
	short ret = Scr_ExecEntThread(&g_entities[clientNum], codecallback_userinfochanged, 1);
	Scr_FreeThread(ret);
}

typedef void (*FUN_0808e1ea_t)(client_t *client);
FUN_0808e1ea_t FUN_0808e1ea = (FUN_0808e1ea_t)0x0808e1ea;

typedef int (*FUN_081384cc_t)(const char *str);
FUN_081384cc_t FUN_081384cc = (FUN_081384cc_t)0x081384cc;

void custom_SV_DropClient( client_t *drop, const char *reason ) {
	int i, pb_test;
	challenge_t *challenge;
	qboolean isBot = qfalse;
    char name[32];
    
    Com_DPrintf( "custom_SV_DropClient for %s\n", drop->name );

	if ( drop->state == CS_ZOMBIE ) {
		return;     // already dropped
	}
    
    drop->unksnapshotvar2 = 0; // *(drop+8)
    strcpy(name, drop->name);
    FUN_0808e1ea(drop);

	Com_DPrintf( "Going to CS_ZOMBIE for %s\n", name );
	drop->state = CS_ZOMBIE;        // become free in a few seconds

    if ( drop->netchan.remoteAddress.type == NA_BOT ) {
        isBot = qtrue;
    }

	if ( !isBot ) {
		// see if we already have a challenge for this ip
		challenge = &svs.challenges[0];

		for ( i = 0 ; i < MAX_CHALLENGES ; i++, challenge++ ) {
			if ( NET_CompareAdr( drop->netchan.remoteAddress, challenge->adr ) ) {
				challenge->connected = qfalse;
				break;
			}
		}
	}

    pb_test = FUN_081384cc(reason);
    if (!isBot && Q_stricmp(reason, "EXE_DISCONNECTED") != 0) { // do not show kick message at bots
        if (!pb_test) {
            SV_SendServerCommand(0, 0, "e \"\x15%s^7 %s%s\"", name, "", reason);
        } else {
            SV_SendServerCommand(0, 0, "e \"\x15%s^7 %s%s\"", name, "\x14", reason);
        }
    }
    Com_Printf("%i:%s %s\n", drop - svs.clients, name, reason);
    SV_SendServerCommand(0, 1, "J %d", drop - svs.clients);
    if (!pb_test) {
        SV_SendServerCommand(drop, 1, "w \"%s^7 %s\" PB", name, reason);
    } else {
        SV_SendServerCommand(drop, 1, "w \"%s\"", reason);
    }

	// if this was the last client on the server, send a heartbeat
	// to the master so it is known the server is empty
	// send a heartbeat now so the master will get up to date info
	// if there is already a slot for this ip, reuse it
	for ( i = 0 ; i < sv_maxclients->integer ; i++ ) {
		if ( svs.clients[i].state >= CS_CONNECTED ) {
			break;
		}
	}
	if ( i == sv_maxclients->integer ) {
		SV_Heartbeat();
	}
}

char const *eventnames[] = {
	"EV_NONE",
	"EV_FOOTSTEP_RUN_DEFAULT",
	"EV_FOOTSTEP_RUN_BARK",
	"EV_FOOTSTEP_RUN_BRICK",
	"EV_FOOTSTEP_RUN_CARPET",
	"EV_FOOTSTEP_RUN_CLOTH",
	"EV_FOOTSTEP_RUN_CONCRETE",
	"EV_FOOTSTEP_RUN_DIRT",
	"EV_FOOTSTEP_RUN_FLESH",
	"EV_FOOTSTEP_RUN_FOLIAGE",
	"EV_FOOTSTEP_RUN_GLASS",      // 10
	"EV_FOOTSTEP_RUN_GRASS",
	"EV_FOOTSTEP_RUN_GRAVEL",
	"EV_FOOTSTEP_RUN_ICE",
	"EV_FOOTSTEP_RUN_METAL",
	"EV_FOOTSTEP_RUN_MUD",
	"EV_FOOTSTEP_RUN_PAPER",
	"EV_FOOTSTEP_RUN_PLASTER",
	"EV_FOOTSTEP_RUN_ROCK",
	"EV_FOOTSTEP_RUN_SAND",
	"EV_FOOTSTEP_RUN_SNOW",       // 20
	"EV_FOOTSTEP_RUN_WATER",
	"EV_FOOTSTEP_RUN_WOOD",
	"EV_FOOTSTEP_RUN_ASPHALT",
	"EV_FOOTSTEP_WALK_DEFAULT",
	"EV_FOOTSTEP_WALK_BARK",
	"EV_FOOTSTEP_WALK_BRICK",
	"EV_FOOTSTEP_WALK_CARPET",
	"EV_FOOTSTEP_WALK_CLOTH",
	"EV_FOOTSTEP_WALK_CONCRETE",
	"EV_FOOTSTEP_WALK_DIRT",      // 30
	"EV_FOOTSTEP_WALK_FLESH",
	"EV_FOOTSTEP_WALK_FOLIAGE",
	"EV_FOOTSTEP_WALK_GLASS",
	"EV_FOOTSTEP_WALK_GRASS",
	"EV_FOOTSTEP_WALK_GRAVEL",
	"EV_FOOTSTEP_WALK_ICE",
	"EV_FOOTSTEP_WALK_METAL",
	"EV_FOOTSTEP_WALK_MUD",
	"EV_FOOTSTEP_WALK_PAPER",
	"EV_FOOTSTEP_WALK_PLASTER",   // 40
	"EV_FOOTSTEP_WALK_ROCK",
	"EV_FOOTSTEP_WALK_SAND",
	"EV_FOOTSTEP_WALK_SNOW",
	"EV_FOOTSTEP_WALK_WATER",
	"EV_FOOTSTEP_WALK_WOOD",
	"EV_FOOTSTEP_WALK_ASPHALT",
	"EV_FOOTSTEP_PRONE_DEFAULT",
	"EV_FOOTSTEP_PRONE_BARK",
	"EV_FOOTSTEP_PRONE_BRICK",
	"EV_FOOTSTEP_PRONE_CARPET",   // 50
	"EV_FOOTSTEP_PRONE_CLOTH",
	"EV_FOOTSTEP_PRONE_CONCRETE",
	"EV_FOOTSTEP_PRONE_DIRT",
	"EV_FOOTSTEP_PRONE_FLESH",
	"EV_FOOTSTEP_PRONE_FOLIAGE",
	"EV_FOOTSTEP_PRONE_GLASS",
	"EV_FOOTSTEP_PRONE_GRASS",
	"EV_FOOTSTEP_PRONE_GRAVEL",
	"EV_FOOTSTEP_PRONE_ICE",
	"EV_FOOTSTEP_PRONE_METAL",    // 60
	"EV_FOOTSTEP_PRONE_MUD",
	"EV_FOOTSTEP_PRONE_PAPER",
	"EV_FOOTSTEP_PRONE_PLASTER",
	"EV_FOOTSTEP_PRONE_ROCK",
	"EV_FOOTSTEP_PRONE_SAND",
	"EV_FOOTSTEP_PRONE_SNOW",
	"EV_FOOTSTEP_PRONE_WATER",
	"EV_FOOTSTEP_PRONE_WOOD",
	"EV_FOOTSTEP_PRONE_ASPHALT",
	"EV_JUMP_DEFAULT",            // 70
	"EV_JUMP_BARK",
	"EV_JUMP_BRICK",
	"EV_JUMP_CARPET",
	"EV_JUMP_CLOTH",
	"EV_JUMP_CONCRETE",
	"EV_JUMP_DIRT",
	"EV_JUMP_FLESH",
	"EV_JUMP_FOLIAGE",
	"EV_JUMP_GLASS",
	"EV_JUMP_GRASS",              // 80
	"EV_JUMP_GRAVEL",
	"EV_JUMP_ICE",
	"EV_JUMP_METAL",
	"EV_JUMP_MUD",
	"EV_JUMP_PAPER",
	"EV_JUMP_PLASTER",
	"EV_JUMP_ROCK",
	"EV_JUMP_SAND",
	"EV_JUMP_SNOW",
	"EV_JUMP_WATER",              // 90
	"EV_JUMP_WOOD",
	"EV_JUMP_ASPHALT",
	"EV_LANDING_DEFAULT",
	"EV_LANDING_BARK",
	"EV_LANDING_BRICK",
	"EV_LANDING_CARPET",
	"EV_LANDING_CLOTH",
	"EV_LANDING_CONCRETE",
	"EV_LANDING_DIRT",
	"EV_LANDING_FLESH",           // 100
	"EV_LANDING_FOLIAGE",
	"EV_LANDING_GLASS",
	"EV_LANDING_GRASS",
	"EV_LANDING_GRAVEL",
	"EV_LANDING_ICE",
	"EV_LANDING_METAL",
	"EV_LANDING_MUD",
	"EV_LANDING_PAPER",
	"EV_LANDING_PLASTER",
	"EV_LANDING_ROCK",            // 110
	"EV_LANDING_SAND",
	"EV_LANDING_SNOW",
	"EV_LANDING_WATER",
	"EV_LANDING_WOOD",
	"EV_LANDING_ASPHALT",
	"EV_LANDING_PAIN_DEFAULT",
	"EV_LANDING_PAIN_BARK",
	"EV_LANDING_PAIN_BRICK",
	"EV_LANDING_PAIN_CARPET",
	"EV_LANDING_PAIN_CLOTH",      // 120
	"EV_LANDING_PAIN_CONCRETE",
	"EV_LANDING_PAIN_DIRT",
	"EV_LANDING_PAIN_FLESH",
	"EV_LANDING_PAIN_FOLIAGE",
	"EV_LANDING_PAIN_GLASS",
	"EV_LANDING_PAIN_GRASS",
	"EV_LANDING_PAIN_GRAVEL",
	"EV_LANDING_PAIN_ICE",
	"EV_LANDING_PAIN_METAL",
	"EV_LANDING_PAIN_MUD",        // 130
	"EV_LANDING_PAIN_PAPER",
	"EV_LANDING_PAIN_PLASTER",
	"EV_LANDING_PAIN_ROCK",
	"EV_LANDING_PAIN_SAND",
	"EV_LANDING_PAIN_SNOW",
	"EV_LANDING_PAIN_WATER",
	"EV_LANDING_PAIN_WOOD",
	"EV_LANDING_PAIN_ASPHALT",
	"EV_FOLIAGE_SOUND",
	"EV_STANCE_FORCE_STAND",      // 140
	"EV_STANCE_FORCE_CROUCH",
	"EV_STANCE_FORCE_PRONE",
	"EV_STEP_VIEW",
	"EV_ITEM_PICKUP",
	"EV_AMMO_PICKUP",
	"EV_NOAMMO",
	"EV_EMPTYCLIP",
	"EV_EMPTY_OFFHAND",
	"EV_RESET_ADS",
	"EV_RELOAD",                  // 150
	"EV_RELOAD_FROM_EMPTY",
	"EV_RELOAD_START",
	"EV_RELOAD_END",
	"EV_RAISE_WEAPON",
	"EV_PUTAWAY_WEAPON",
	"EV_WEAPON_ALT",
	"EV_PULLBACK_WEAPON",
	"EV_FIRE_WEAPON",
	"EV_FIRE_WEAPONB",
	"EV_FIRE_WEAPON_LASTSHOT",    // 160
	"EV_RECHAMBER_WEAPON",
	"EV_EJECT_BRASS",
	"EV_MELEE_SWIPE",
	"EV_FIRE_MELEE",
	"EV_PREP_OFFHAND",
	"EV_USE_OFFHAND",
	"EV_SWITCH_OFFHAND",
	"EV_BINOCULAR_ENTER",
	"EV_BINOCULAR_EXIT",
	"EV_BINOCULAR_FIRE",          // 170
	"EV_BINOCULAR_RELEASE",
	"EV_BINOCULAR_DROP",
	"EV_MELEE_HIT",
	"EV_MELEE_MISS",
	"EV_FIRE_WEAPON_MG42",
	"EV_FIRE_QUADBARREL_1",
	"EV_FIRE_QUADBARREL_2",
	"EV_BULLET_TRACER",
	"EV_SOUND_ALIAS",
	"EV_SOUND_ALIAS_AS_MASTER",   // 180
	"EV_BULLET_HIT_SMALL",
	"EV_BULLET_HIT_LARGE",
	"EV_SHOTGUN_HIT",
	"EV_BULLET_HIT_AP",
	"EV_BULLET_HIT_CLIENT_SMALL",
	"EV_BULLET_HIT_CLIENT_LARGE",
	"EV_GRENADE_BOUNCE",
	"EV_GRENADE_EXPLODE",
	"EV_ROCKET_EXPLODE",
	"EV_ROCKET_EXPLODE_NOMARKS",  // 190
	"EV_CUSTOM_EXPLODE",
	"EV_CUSTOM_EXPLODE_NOMARKS",
	"EV_BULLET",
	"EV_PLAY_FX",
	"EV_PLAY_FX_ON_TAG",
	"EV_EARTHQUAKE",
	"EV_GRENADE_SUICIDE",
	"EV_OBITUARY"                 // 198
};

void custom_BG_AddPredictableEventToPlayerstate( int event, int eventParm, playerState_t *ps ) {
    if (event != EV_NONE) {
        if (g_debugEvents->boolean)
            Com_DPrintf("custom_BG_AddPredictableEventToPlayerstate() event %26s for client %2d\n", eventnames[event], ps->clientNum);
        
        /*
        // filter example:
        // hide footsteps on dirt for player 0 (the player can still hear himself)
        if (event == EV_FOOTSTEP_RUN_DIRT && ps->clientNum == 0) {
            Com_DPrintf("custom_BG_AddPredictableEventToPlayerstate() filter event EV_FOOTSTEP_RUN_DIRT for client 0\n", eventnames[event], eventParm, ps->clientNum);
            return;
        }
        */
        
        ps->events[ps->eventSequence & ( MAX_EVENTS - 1 )] = event & 0xff;
        ps->eventParms[ps->eventSequence & ( MAX_EVENTS - 1 )] = eventParm & 0xff;
        ps->eventSequence++;
    }
}

void custom_G_AddEvent (gentity_t * ent, int event, int eventParm) {
	if ( ent->client ) {
        if (g_debugEvents->boolean)
            Com_DPrintf("custom_G_AddEvent() event %26s for client %2d\n", eventnames[event], ent->client->ps.clientNum);
		ent->client->ps.events[ent->client->ps.eventSequence & ( MAX_EVENTS - 1 )] = event;
		ent->client->ps.eventParms[ent->client->ps.eventSequence & ( MAX_EVENTS - 1 )] = eventParm;
		ent->client->ps.eventSequence++;
	} else {
        if (g_debugEvents->boolean)
            Com_DPrintf("custom_G_AddEvent() event %26s for entity %2d\n", eventnames[event], ent->s.number);
		ent->s.events[ent->s.eventSequence & ( MAX_EVENTS - 1 )] = event;
		ent->s.eventParms[ent->s.eventSequence & ( MAX_EVENTS - 1 )] = eventParm;
		ent->s.eventSequence++;
	}
	ent->eventTime = level.time;
	ent->r.eventTime = level.time;
}

gentity_t* custom_G_TempEntity(vec3_t origin, int event)
{
	hook_g_tempentity->unhook();

	gentity_t* (*sig)(vec3_t origin, int event);
	*(int *)&sig = hook_g_tempentity->from;

    if (g_debugEvents->boolean)
        Com_DPrintf("custom_G_TempEntity() event %26s at (%f,%f,%f)\n", eventnames[event], origin[0], origin[1], origin[2]);
    
    /*
    // filter example:
    // use with caution: this can cause script runtime errors (or worse) on
    // some events, e.g., when using the obituary function, due to (then)
    // undefined parameters
    if (event == EV_PLAY_FX) {
        event = EV_NONE;
    }
    */
	gentity_t* tempEntity = sig(origin, event);
    
    hook_g_tempentity->hook();

	return tempEntity;
}

void custom_MSG_WriteDeltaPlayerstate2(msg_t *msg, playerState_t *from, playerState_t *to)
{
	hook_msg_writedeltaplayerstate->unhook();

	int (*sig)(msg_t *msg, playerState_t *from, playerState_t *to);
	*(int *)&sig = hook_msg_writedeltaplayerstate->from;
    
    Com_DPrintf("custom_MSG_WriteDeltaPlayerstate2() unknown = (%i,%i)\n", to+256, to+260);

	sig(msg, from , to);
    
    hook_msg_writedeltaplayerstate->hook();
}

void custom_MSG_WriteDeltaPlayerstate(msg_t *msg, playerState_t *from, playerState_t *to)
{
    playerState_t dummy;
    int i, j, lc;
    int *toF;
    int *fromF;
    netField_t *field;
    float fullFloat;
    int trunc;
    
    int local_271c;
    sbyte local_2714;
    int local_2710;
    int local_270c;
    
    uint bits;
    int clipbits;
    int ammobits [7];
    int statsbits;

    if ( !from ) {
        from = &dummy;
        memset( &dummy, 0, sizeof(dummy) );
    }
    
    if (g_debugEvents->boolean) {
        Com_DPrintf("custom_MSG_WriteDeltaPlayerstate() clientNum = %d\n", to->clientNum);
    }
    
	lc = 0;
	for ( i = 0, field = &playerStateFields; i < 105; i++, field++ ) {
		fromF = ( int * )( (byte *)from + field->offset );
		toF = ( int * )( (byte *)to + field->offset );
		if ( *fromF != *toF ) {
			lc = i + 1;
		}
	}
    
	MSG_WriteByte( msg, lc );
    
    i = 0;
    field = &playerStateFields;
    while (i < lc) {
		fromF = ( int * )( (byte *)from + field->offset );
		toF = ( int * )( (byte *)to + field->offset );
        
        if ( *fromF == *toF ) {
            MSG_WriteBit0(msg);
        } else {
            MSG_WriteBit1(msg);
            
            if (field->bits == 0) {
                fullFloat = *(float *)toF;
                trunc = (int)fullFloat;
                
                if ( trunc == fullFloat && trunc + FLOAT_INT_BIAS >= 0 && trunc + FLOAT_INT_BIAS < ( 1 << FLOAT_INT_BITS ) ) {
                    MSG_WriteBit0(msg);
                    MSG_WriteBits(msg, trunc + FLOAT_INT_BIAS, 5);
                    MSG_WriteByte(msg, (char)((trunc + 0x1000) >> 5));
                } else {
                    MSG_WriteBit1(msg);
                    MSG_WriteLong(msg, *toF);
                }
            } else {
                if (field->bits == -100) {
                    if (*toF == 0) {
                        MSG_WriteBit0(msg);
                    } else {
                        MSG_WriteBit1(msg);
                        MSG_WriteAngle16(msg, *toF);
                    }
                } else {
                    local_270c = *toF;
                    local_271c = field->bits;
                    if (local_271c < 0) {
                        local_271c = -local_271c;
                    }
                    local_2710 = local_271c;
                    bits = local_271c & 7;
                    if (bits != 0) {
                        MSG_WriteBits(msg, local_270c,bits);
                        local_2710 = local_271c - bits;
                        local_2714 = (sbyte)bits;
                        local_270c = local_270c >> local_2714;
                    }
                    while (local_2710 != 0) {
                        MSG_WriteByte(msg,(char)local_270c);
                        local_270c = local_270c >> 8;
                        local_2710 = local_2710 + -8;
                    }
                }
            }
        }
        i++;
        field++;
    }
    
    // Stats
    statsbits = 0;
    i = 0;
    while (i < 6) {
        if (to->stats[i] != from->stats[i]) {
            statsbits = statsbits | 1 << ((byte)i & 0x1f);
        }
        i++;
    }
    if (statsbits == 0) {
        MSG_WriteBit0(msg);
    } else {
        MSG_WriteBit1(msg);
        MSG_WriteBits(msg,statsbits,6);
        if ((statsbits & 1U) != 0) {
            MSG_WriteShort(msg,to->stats[0]);
        }
        if ((statsbits & 2U) != 0) {
            MSG_WriteShort(msg,to->stats[1]);
        }
        if ((statsbits & 4U) != 0) {
            MSG_WriteShort(msg,to->stats[2]);
        }
        if ((statsbits & 8U) != 0) {
            MSG_WriteBits(msg,to->stats[3],6);
        }
        if ((statsbits & 0x10U) != 0) {
            MSG_WriteShort(msg,to->stats[4]);
        }
        if ((statsbits & 0x20U) != 0) {
            MSG_WriteByte(msg,(char)to->stats[5]);
        }
    }
    
    // Ammo
    j = 0;
    while (j < 4) {
        ammobits[j] = 0;
        i = 0;
        while (i < 0x10) {
            if (to->ammo[j * 0x10 + i] != from->ammo[j * 0x10 + i]) {
                ammobits[j] = 1 << ((byte)i & 0x1f) | ammobits[j];
            }
            i++;
        }
        j++;
    }
    if ((((ammobits[0] == 0) && (ammobits[1] == 0)) && (ammobits[2] == 0)) && (ammobits[3] == 0)) {
        MSG_WriteBit0(msg);
    } else {
        MSG_WriteBit1(msg);
        j = 0;
        while (j < 4) {
            if (ammobits[j] == 0) {
                MSG_WriteBit0(msg);
            } else {
                MSG_WriteBit1(msg);
                MSG_WriteShort(msg,ammobits[j]);
                i = 0;
                while (i < 0x10) {
                    if ((ammobits[j] >> ((byte)i & 0x1f) & 1U) != 0) {
                        MSG_WriteShort(msg,to->ammo[j * 0x10 + i]);
                    }
                    i++;
                }
            }
            j++;
        }
    }
    
    // Clipammo
    j = 0;
    while (j < 4) {
        clipbits = 0;
        i = 0;
        while (i < 0x10) {
            if (to->ammoclip[j * 0x10 + i] != from->ammoclip[j * 0x10 + i]) {
                clipbits = clipbits | 1 << ((byte)i & 0x1f);
            }
            i++;
        }
        if (clipbits == 0) {
            MSG_WriteBit0(msg);
        } else {
            MSG_WriteBit1(msg);
            MSG_WriteShort(msg,clipbits);
            i = 0;
            while (i < 0x10) {
                if ((clipbits >> ((byte)i & 0x1f) & 1U) != 0) {
                    MSG_WriteShort(msg,to->ammoclip[j * 0x10 + i]);
                }
                i++;
            }
        }
        j++;
    }

    // Objectives
    if (!memcmp(from->objective, to->objective, sizeof(from->objective))) {
        MSG_WriteBit0(msg);
    } else {
        MSG_WriteBit1(msg);
        i = 0;
        while (i < 0x10) {
            MSG_WriteBits(msg, to->objective[i].state, 3);
            MSG_WriteDeltaObjective(msg, &from->objective[i], &to->objective[i], 0, 6, &objectiveFields);
            i++;
        }
    }
    
    // Huds
    if (!memcmp(&from->hud, &to->hud, sizeof(from->hud))) {
        MSG_WriteBit0(msg);
    } else {
        MSG_WriteBit1(msg);
        MSG_WriteDeltaHudElems(msg, from->hud.archival, to->hud.archival, 0x1f);
        MSG_WriteDeltaHudElems(msg, from->hud.current, to->hud.current, 0x1f);
    }
}

int gamestate_size[MAX_CLIENTS] = {0};
void custom_SV_SendClientGameState( client_t *client ) {
	int			start;
	entityState_t	*base, nullstate;
	msg_t		msg;
	byte		msgBuffer[MAX_MSGLEN];
    
    while(client->state != CS_FREE && client->netchan.unsentFragments){
		SV_Netchan_TransmitNextFragment(&client->netchan);
	}

 	Com_DPrintf("custom_SV_SendClientGameState() for %s\n", client->name);
	Com_DPrintf("Going from CS_CONNECTED to CS_PRIMED for %s\n", client->name);
	client->state = CS_PRIMED;
	client->pureAuthentic = 0;

	// when we receive the first packet from the client, we will
	// notice that it is from a different serverid and that the
	// gamestate message was not just sent, forcing a retransmit
	client->gamestateMessageNum = client->netchan.outgoingSequence;

	MSG_Init( &msg, msgBuffer, sizeof( msgBuffer ) );

	// NOTE, MRE: all server->client messages now acknowledge
	// let the client know which reliable clientCommands we have received
	MSG_WriteLong( &msg, client->lastClientCommand );

	// send any server commands waiting to be sent first.
	// we have to do this cause we send the client->reliableSequence
	// with a gamestate and it sets the clc.serverCommandSequence at
	// the client side
	SV_UpdateServerCommandsToClient( client, &msg );

	// send the gamestate
	MSG_WriteByte( &msg, svc_gamestate );
	MSG_WriteLong( &msg, client->reliableSequence );

	// write the configstrings
	for ( start = 0 ; start < MAX_CONFIGSTRINGS ; start++ ) {
		if (sv.configstrings[start][0]) {
			MSG_WriteByte( &msg, svc_configstring );
			MSG_WriteShort( &msg, start );
			MSG_WriteBigString( &msg, sv.configstrings[start] );
            //Com_DPrintf("Sending configstring %i to client %i: %s\n", start, client - svs.clients, sv.configstrings[start]);
		}
	}

	// write the baselines
	memset( &nullstate, 0, sizeof( nullstate ) ); // Com_Memset
    
	for ( start = 0 ; start < MAX_GENTITIES; start++ ) {
		base = &sv.svEntities[start].baseline.s;
		if ( !base->number ) {
			continue;
		}
		MSG_WriteByte( &msg, svc_baseline );
		MSG_WriteDeltaEntity( &msg, &nullstate, base, qtrue );
	}

	MSG_WriteByte( &msg, svc_EOF );
	MSG_WriteLong( &msg, client - svs.clients );
	MSG_WriteLong( &msg, sv.checksumFeed );
    MSG_WriteByte( &msg, svc_EOF );
    
    Com_DPrintf("Sending %i bytes in gamestate to client: %i\n", msg.cursize, client - svs.clients);
    gamestate_size[client - svs.clients] = int(msg.cursize);

	// deliver this to the client
	SV_SendMessageToClient( &msg, client );
}

void custom_SV_WriteDownloadToClient(client_t *cl, msg_t *msg)
{
	int curindex;
	int iwdFile;
	char errorMessage[COD2_MAX_STRINGLENGTH];

	if (cl->state == CS_ACTIVE) {
        //Com_DPrintf("clientDownload: Client already in game, name = '%s'\n", cl->downloadName);
		return;
    }
	if (!*cl->downloadName) {
        // Com_DPrintf("clientDownload: Nothing being downloaded\n");
		return;
    }
	if (strlen(cl->downloadName) < 4) {
        // Com_DPrintf("clientDownload: File length too short\n");
		return;
    }
	if (strcmp(&cl->downloadName[strlen(cl->downloadName) - 4], ".iwd") != 0) {
        // Com_DPrintf("clientDownload: Not a iwd file\n");
		return;
    }

#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
	if ( cl->wwwDlAck ) {
        // Com_DPrintf("clientDownload: wwwDl acknowleged\n");
		return;
    }
#endif

	if (strlen(sv_downloadMessage->string))
	{
		Com_sprintf(errorMessage, sizeof(errorMessage), sv_downloadMessage->string);

        MSG_WriteByte( msg, svc_download );
		MSG_WriteShort( msg, 0 ); // client is expecting block zero
		MSG_WriteLong( msg, -1 ); // illegal file size
		MSG_WriteString( msg, errorMessage );

		*cl->downloadName = 0;
		return; // Download message instead of download
	}

#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
	if (sv_wwwDownload->boolean && cl->wwwDownload)
	{
		if (!cl->wwwDl_failed)
		{
			SV_WWWRedirectClient(cl, msg);
			return; // wwwDl redirect
		}
	}
#endif

	// Hardcode client variables to make max download speed for everyone
	cl->state = CS_CONNECTED;
	cl->rate = 25000;
	cl->snapshotMsec = 50;

	if (!cl->download)
	{
		// We open the file here

		Com_Printf("clientDownload: %d : begining \"%s\"\n", cl - svs.clients, cl->downloadName);

		iwdFile = FS_iwIwd(cl->downloadName, "main");

		if ( !sv_allowDownload->integer || iwdFile || ( cl->downloadSize = FS_SV_FOpenFileRead( cl->downloadName, &cl->download ) ) <= 0 )
		{
			// cannot auto-download file
			if (iwdFile)
			{
				Com_Printf("clientDownload: %d : \"%s\" cannot download iwd files\n", cl - svs.clients, cl->downloadName);
				Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_CANTAUTODLGAMEIWD\x15%s", cl->downloadName);
			}
			else if ( !sv_allowDownload->boolean )
			{
				Com_Printf("clientDownload: %d : \"%s\" download disabled\n", cl - svs.clients, cl->downloadName);

				if (sv_pure->boolean)
					Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_AUTODL_SERVERDISABLED_PURE\x15%s", cl->downloadName);
				else
					Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_AUTODL_SERVERDISABLED\x15%s", cl->downloadName);
			}
			else
			{
				Com_Printf("clientDownload: %d : \"%s\" file not found on server\n", cl - svs.clients, cl->downloadName);
				Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_AUTODL_FILENOTONSERVER\x15%s", cl->downloadName);
			}

			MSG_WriteByte( msg, svc_download );
			MSG_WriteShort( msg, 0 ); // client is expecting block zero
			MSG_WriteLong( msg, -1 ); // illegal file size
			MSG_WriteString( msg, errorMessage );

			*cl->downloadName = 0;
			return;
		}

		// Init
		cl->downloadCurrentBlock = cl->downloadClientBlock = cl->downloadXmitBlock = 0;
		cl->downloadCount = 0;
		cl->downloadEOF = qfalse;
	}

	// Perform any reads that we need to
	while (cl->downloadCurrentBlock - cl->downloadClientBlock < MAX_DOWNLOAD_WINDOW && cl->downloadSize != cl->downloadCount)
	{
		curindex = (cl->downloadCurrentBlock % MAX_DOWNLOAD_WINDOW);

		if (!cl->downloadBlocks[curindex])
			cl->downloadBlocks[curindex] = (unsigned char *)Z_MallocInternal(MAX_DOWNLOAD_BLKSIZE);

		cl->downloadBlockSize[curindex] = FS_Read( cl->downloadBlocks[curindex], MAX_DOWNLOAD_BLKSIZE, cl->download );

		if (cl->downloadBlockSize[curindex] < 0)
		{
			// EOF right now
			cl->downloadCount = cl->downloadSize;
			break;
		}

		cl->downloadCount += cl->downloadBlockSize[curindex];

		// Load in next block
		cl->downloadCurrentBlock++;
	}

	// Check to see if we have eof condition and add the EOF block
	if (cl->downloadCount == cl->downloadSize && !cl->downloadEOF && cl->downloadCurrentBlock - cl->downloadClientBlock < MAX_DOWNLOAD_WINDOW)
	{
		cl->downloadBlockSize[cl->downloadCurrentBlock % MAX_DOWNLOAD_WINDOW] = 0;
		cl->downloadCurrentBlock++;
		cl->downloadEOF = qtrue;  // We have added the EOF block
	}

	// Write out the next section of the file, if we have already reached our window,
	// automatically start retransmitting

	if (cl->downloadClientBlock == cl->downloadCurrentBlock)
		return; // Nothing to transmit

	if (cl->downloadXmitBlock == cl->downloadCurrentBlock)
	{
		// We have transmitted the complete window, should we start resending?

		//FIXME:  This uses a hardcoded one second timeout for lost blocks
		//the timeout should be based on client rate somehow
		if (svs.time - cl->downloadSendTime > 1000)
			cl->downloadXmitBlock = cl->downloadClientBlock;
		else
			return;
	}

	// Send current block
	curindex = (cl->downloadXmitBlock % MAX_DOWNLOAD_WINDOW);

	MSG_WriteByte( msg, svc_download );
	MSG_WriteShort( msg, cl->downloadXmitBlock );

	// block zero is special, contains file size
	if ( cl->downloadXmitBlock == 0 )
		MSG_WriteLong( msg, cl->downloadSize );

	MSG_WriteShort( msg, cl->downloadBlockSize[curindex] );

	// Write the block
	if ( cl->downloadBlockSize[curindex] )
		MSG_WriteData( msg, cl->downloadBlocks[curindex], cl->downloadBlockSize[curindex] );

	Com_DPrintf("clientDownload: %d : writing block %d\n", cl - svs.clients, cl->downloadXmitBlock);

	// Move on to the next block
	// It will get sent with next snap shot.  The rate will keep us in line.
	cl->downloadXmitBlock++;

	cl->downloadSendTime = svs.time;
}

// Segfault fix
int hook_BG_IsWeaponValid(int a1, int a2)
{
	if ( !(unsigned char)sub_80E9758(a2) )
		return 0;

	if ( !(unsigned char)sub_80D9E84(a1 + 1348, a2) )
		return 0;

	int weapon = (int)BG_WeaponDefs(a2);

	if ( !weapon )
		return 0;

	if ( !*(long *)(weapon + 132) && *(char *)(a1 + 1365) != a2 && *(char *)(a1 + 1366) != a2 && *(long *)(weapon + 876) != a2 )
		return 0;

	return 1;
}

char *custom_va(const char *format, ...)
{
	struct va_info_t *info;
	int index;
	va_list va;

	info = (va_info_t*)Sys_GetValue(1);
	index = info->index;
	info->index = (info->index + 1) % MAX_VASTRINGS;

	va_start(va, format);
	vsnprintf(info->va_string[index], sizeof(info->va_string[0]), format, va);
	va_end(va);

	info->va_string[index][1023] = 0;

	return info->va_string[index];
}

void hook_SV_VerifyIwds_f(client_t *cl)
{
	if (sv_pure->boolean)
		cl->pureAuthentic = 1;
}

void hook_SV_ResetPureClient_f(client_t *cl)
{
	cl->pureAuthentic = 0;

	if (codecallback_vid_restart && Scr_IsSystemActive())
	{	
		if (cl->gentity == NULL)
			return;
		
		stackPushInt(cl - svs.clients);
		short ret = Scr_ExecEntThread(cl->gentity, codecallback_vid_restart, 1);
		Scr_FreeThread(ret);
	}
}

// Adds bot pings and removes spam on 1.2 and 1.3
void custom_SV_CalcPings( void )
{
	int i, j;
	client_t *cl;
	int total, count;
	int delta;

	for ( i = 0 ; i < sv_maxclients->integer ; i++ )
	{
		cl = &svs.clients[i];

		if ( cl->state != CS_ACTIVE )
		{
			cl->ping = -1;
			continue;
		}

		if ( !cl->gentity )
		{
			cl->ping = -1;
			continue;
		}

		if ( cl->netchan.remoteAddress.type == NA_BOT )
		{
			cl->ping = 0;
			cl->lastPacketTime = svs.time;
			continue;
		}

		total = 0;
		count = 0;

		for ( j = 0 ; j < PACKET_BACKUP ; j++ )
		{
			if ( cl->frames[j].messageAcked == 0xFFFFFFFF )
				continue;

			delta = cl->frames[j].messageAcked - cl->frames[j].messageSent;
			count++;
			total += delta;
		}

		if ( !count )
			cl->ping = 999;
		else
		{
			cl->ping = total / count;

			if ( cl->ping > 999 )
				cl->ping = 999;
		}
	}
}

void custom_SV_CheckTimeouts( void )
{
	int	i;
	client_t *cl;
	int	droppoint;
	int	zombiepoint;

	droppoint = svs.time - 1000 * sv_timeout->integer;
	zombiepoint = svs.time - 1000 * sv_zombietime->integer;

	for ( i = 0, cl = svs.clients; i < sv_maxclients->integer; i++, cl++ )
	{
		// message times may be wrong across a changelevel
		if (cl->lastPacketTime > svs.time)
			cl->lastPacketTime = svs.time;

		if (cl->state == CS_ZOMBIE && cl->lastPacketTime < zombiepoint)
		{
			cl->state = CS_FREE; // can now be reused
			continue;
		}

		if (cl->state >= CS_CONNECTED && cl->lastPacketTime < droppoint)
		{
			// wait several frames so a debugger session doesn't
			// cause a timeout
			if ( ++cl->timeoutCount > 5 )
			{
				SV_DropClient(cl, "EXE_TIMEDOUT");
				cl->state = CS_FREE; // don't bother with zombie state
			}
		}
		else
			cl->timeoutCount = 0;
	}
}

#if COMPILE_BOTS == 1
int bot_buttons[MAX_CLIENTS] = {0};
int bot_weapon[MAX_CLIENTS] = {0};
char bot_forwardmove[MAX_CLIENTS] = {0};
char bot_rightmove[MAX_CLIENTS] = {0};
void custom_SV_BotUserMove(client_t *client)
{
	int num;
	usercmd_t ucmd = {0};

	if (client->gentity == NULL)
		return;

	num = client - svs.clients;
	ucmd.serverTime = svs.time;

	playerState_t *ps = SV_GameClientNum(num);
	gentity_t *ent = SV_GentityNum(num);

	if (bot_weapon[num])
		ucmd.weapon = (byte)(bot_weapon[num] & 0xFF);
	else
		ucmd.weapon = (byte)(ps->weapon & 0xFF);

	if (ent->client == NULL)
		return;

	if (ent->client->sess.archiveTime == 0)
	{
		ucmd.buttons = bot_buttons[num];

		ucmd.forwardmove = bot_forwardmove[num];
		ucmd.rightmove = bot_rightmove[num];

		VectorCopy(ent->client->sess.cmd.angles, ucmd.angles);
	}

	client->deltaMessage = client->netchan.outgoingSequence - 1;
	SV_ClientThink(client, &ucmd);
}
#endif

void hook_scriptError(int a1, int a2, int a3, void *a4)
{
	if (developer->integer == 2)
		runtimeError(0, a1, a2, a3);
	else
		scriptError(a1, a2, a3, a4);
}

int player_sw_pickup[MAX_CLIENTS] = {1};
int touch_item(gentity_t * item, gentity_t * entity, int touch)
{
	if (!player_sw_pickup[entity->client->ps.clientNum])
		return 0;
	
	hook_touch_item->unhook();
	
	int (*sig)(gentity_t *, gentity_t *, int);
	*(int *)&sig = hook_touch_item->from;

	int ret = sig(item, entity, touch);
	
	hook_touch_item->hook();
	
	return ret;
}

#if COMPILE_PLAYER == 1

void hook_gamestate_info(const char *format, ...)
{
	char s[COD2_MAX_STRINGLENGTH];
	va_list va;

	va_start(va, format);
	vsnprintf(s, sizeof(s), format, va);
	va_end(va);

	Com_DPrintf("%s", s);
		
	char *tok;
	int gamestate = 0;
	int clientnum = 0;
	tok = strtok(s, " ");

	for (int i = 0; tok != NULL; i++)
	{
		if (i == 1)
			gamestate = atoi(tok);
		if (i == 7)
			clientnum = atoi(tok);
		tok = strtok(NULL, " ");
	}

	gamestate_size[clientnum] = gamestate;
}

int clientfps[MAX_CLIENTS] = {0};
int tempfps[MAX_CLIENTS] = {0};
int fpstime[MAX_CLIENTS] = {0};
int previousbuttons[MAX_CLIENTS] = {0};
int play_movement(client_t *cl, usercmd_t *ucmd)
{
	hook_play_movement->unhook();

	int (*sig)(client_t *cl, usercmd_t *ucmd);
	*(int *)&sig = hook_play_movement->from;

	int ret = sig(cl, ucmd);

	hook_play_movement->hook();

	int clientnum = cl - svs.clients;

	tempfps[clientnum]++;

	if (svs.time - fpstime[clientnum] >= 1000)
	{
		clientfps[clientnum] = tempfps[clientnum];
		fpstime[clientnum] = svs.time;
		tempfps[clientnum] = 0;
	}
	
	if (ucmd->buttons & KEY_MASK_MELEE && !(previousbuttons[clientnum] & KEY_MASK_MELEE))
	{
		if(codecallback_meleebutton)
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_meleebutton, 0);
			Scr_FreeThread(ret);
		}
	}

	if (ucmd->buttons & KEY_MASK_USE && !(previousbuttons[clientnum] & KEY_MASK_USE))
	{
		if(codecallback_usebutton)
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_usebutton, 0);
			Scr_FreeThread(ret);
		}
	}

	if (ucmd->buttons & KEY_MASK_FIRE && !(previousbuttons[clientnum] & KEY_MASK_FIRE))
	{
		if(codecallback_attackbutton)
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_attackbutton, 0);
			Scr_FreeThread(ret);
		}
	}

	previousbuttons[clientnum] = ucmd->buttons;
	return ret;
}

int player_g_speed[MAX_CLIENTS] = {0};
int player_g_gravity[MAX_CLIENTS] = {0};
int play_endframe(gentity_t *ent)
{
	hook_play_endframe->unhook();

	int (*sig)(gentity_t *ent);
	*(int *)&sig = hook_play_endframe->from;

	int ret = sig(ent);

	hook_play_endframe->hook();

	if (ent->client->sess.state == STATE_PLAYING)
	{
		int num = ent - g_entities;

		if (player_g_speed[num] > 0)
			ent->client->ps.speed = player_g_speed[num];

		if (player_g_gravity[num] > 0)
			ent->client->ps.gravity = player_g_gravity[num];

	}

	return ret;
}

int custom_animation[MAX_CLIENTS] = {0};
int set_anim(playerState_t *ps, int animNum, animBodyPart_t bodyPart, int forceDuration, qboolean setTimer, qboolean isContinue, qboolean force)
{
	hook_set_anim->unhook();

	int (*sig)(playerState_t *ps, int animNum, animBodyPart_t bodyPart, int forceDuration, qboolean setTimer, qboolean isContinue, qboolean force);
	*(int *)&sig = hook_set_anim->from;

	int ret;

	if (!custom_animation[ps->clientNum])
		ret = sig(ps, animNum, bodyPart, forceDuration, setTimer, isContinue, force);
	else
		ret = sig(ps, custom_animation[ps->clientNum], bodyPart, forceDuration, qtrue, isContinue, qtrue);

	hook_set_anim->hook();

	return ret;
}
#endif

#if COMPILE_RATELIMITER == 1
// ioquake3 rate limit connectionless requests
// https://github.com/ioquake/ioq3/blob/master/code/server/sv_main.c

// This is deliberately quite large to make it more of an effort to DoS
#define MAX_BUCKETS	16384
#define MAX_HASHES 1024

static leakyBucket_t buckets[ MAX_BUCKETS ];
static leakyBucket_t* bucketHashes[ MAX_HASHES ];
leakyBucket_t outboundLeakyBucket;

static long SVC_HashForAddress( netadr_t address )
{
	unsigned char *ip = address.ip;
	int	i;
	long hash = 0;

	for ( i = 0; i < 4; i++ )
	{
		hash += (long)( ip[ i ] ) * ( i + 119 );
	}

	hash = ( hash ^ ( hash >> 10 ) ^ ( hash >> 20 ) );
	hash &= ( MAX_HASHES - 1 );

	return hash;
}

static leakyBucket_t *SVC_BucketForAddress( netadr_t address, int burst, int period )
{
	leakyBucket_t *bucket = NULL;
	int	i;
	long hash = SVC_HashForAddress( address );
	uint64_t now = Sys_Milliseconds();

	for ( bucket = bucketHashes[ hash ]; bucket; bucket = bucket->next )
	{
		if ( memcmp( bucket->adr, address.ip, 4 ) == 0 )
		{
			return bucket;
		}
	}

	for ( i = 0; i < MAX_BUCKETS; i++ )
	{
		int interval;

		bucket = &buckets[ i ];
		interval = now - bucket->lastTime;

		// Reclaim expired buckets
		if ( bucket->lastTime > 0 && ( interval > ( burst * period ) ||
		                               interval < 0 ) )
		{
			if ( bucket->prev != NULL )
			{
				bucket->prev->next = bucket->next;
			}
			else
			{
				bucketHashes[ bucket->hash ] = bucket->next;
			}

			if ( bucket->next != NULL )
			{
				bucket->next->prev = bucket->prev;
			}

			memset( bucket, 0, sizeof( leakyBucket_t ) );
		}

		if ( bucket->type == 0 )
		{
			bucket->type = address.type;
			memcpy( bucket->adr, address.ip, 4 );

			bucket->lastTime = now;
			bucket->burst = 0;
			bucket->hash = hash;

			// Add to the head of the relevant hash chain
			bucket->next = bucketHashes[ hash ];
			if ( bucketHashes[ hash ] != NULL )
			{
				bucketHashes[ hash ]->prev = bucket;
			}

			bucket->prev = NULL;
			bucketHashes[ hash ] = bucket;

			return bucket;
		}
	}

	// Couldn't allocate a bucket for this address
	return NULL;
}

bool SVC_RateLimit( leakyBucket_t *bucket, int burst, int period )
{
	if ( bucket != NULL )
	{
		uint64_t now = Sys_Milliseconds();
		int interval = now - bucket->lastTime;
		int expired = interval / period;
		int expiredRemainder = interval % period;

		if ( expired > bucket->burst || interval < 0 )
		{
			bucket->burst = 0;
			bucket->lastTime = now;
		}
		else
		{
			bucket->burst -= expired;
			bucket->lastTime = now - expiredRemainder;
		}

		if ( bucket->burst < burst )
		{
			bucket->burst++;

			return false;
		}
	}

	return true;
}

bool SVC_RateLimitAddress( netadr_t from, int burst, int period )
{
	leakyBucket_t *bucket = SVC_BucketForAddress( from, burst, period );

	return SVC_RateLimit( bucket, burst, period );
}

bool SVC_callback(const char * str, const char * ip)
{	
	if( codecallback_client_spam && Scr_IsSystemActive())
	{
		stackPushString(ip);
		stackPushString(str);
		short ret = Scr_ExecThread(codecallback_client_spam, 2);
		Scr_FreeThread(ret);
		
		return true;
	}
	
	return false;
}

bool SVC_ApplyRconLimit( netadr_t from, bool badRconPassword )
{
    // Prevent using rcon as an amplifier and make dictionary attacks impractical
    if ( SVC_RateLimitAddress( from, 10, 1000 ) )
    {
        if (!SVC_callback("RCON:ADDRESS", NET_AdrToString(from)))
            Com_DPrintf( "SVC_RemoteCommand: rate limit from %s exceeded, dropping request\n", NET_AdrToString( from ) );
        return true;
    }

    if ( badRconPassword )
    {
        static leakyBucket_t bucket;

        // Make DoS via rcon impractical
        if ( SVC_RateLimit( &bucket, 10, 1000 ) )
        {
            if (!SVC_callback("RCON:GLOBAL", NET_AdrToString(from)))
                Com_DPrintf( "SVC_RemoteCommand: rate limit exceeded, dropping request\n" );
            return true;
        }
    }
    
    return false;
}

void hook_SVC_RemoteCommand(netadr_t from, msg_t *msg)
{
	if (!sv_allowRcon->boolean)
		return;
    
    bool badRconPassword = !strlen( rcon_password->string ) || strcmp(Cmd_Argv(1), rcon_password->string) != 0;
	
    if (!sv_limitLocalRcon->boolean)
    {
        unsigned char *ip = from.ip;
    
        if (!(ip[0] == 10 ||                                    // 10.0.0.0 – 10.255.255.255
             (ip[0] == 172 && (ip[1] >= 16 && ip[1] <= 31)) ||  // 172.16.0.0 – 172.31.255.255
             (ip[0] == 192 && ip[1] == 168)))                   // 192.168.0.0 – 192.168.255.255
        {
            if ( SVC_ApplyRconLimit( from, badRconPassword ) )
                return;
        }
    }
    else
    {
        if ( SVC_ApplyRconLimit( from, badRconPassword ) )
            return;
    }
    
	if (
        codecallback_remotecommand && 
        !badRconPassword && 
        Scr_IsSystemActive() && 
        strcmp(Cmd_Argv(2), "map") != 0 && 
        strcmp(Cmd_Argv(2), "devmap") != 0 && 
        strcmp(Cmd_Argv(2), "map_restart") != 0 && 
        strcmp(Cmd_Argv(2), "fast_restart") != 0
        )
	{
        msg->data[(int)msg->cursize] = '\0';
        
		stackPushInt((int)msg);
		stackPushString((char *)msg->data);
		stackPushString(NET_AdrToString(from));
	
		short ret = Scr_ExecThread(codecallback_remotecommand, 3);
		Scr_FreeThread(ret);
		
		return;
	}

	#if COD_VERSION == COD2_1_0
	int lasttime_offset = 0x0848B674;
	#elif COD_VERSION == COD2_1_2
	int lasttime_offset = 0x0849EB74;
	#elif COD_VERSION == COD2_1_3
	int lasttime_offset = 0x0849FBF4;
	#endif

	*(int *)lasttime_offset = 0;

	SVC_RemoteCommand(from, msg);
}

void hook_SV_GetChallenge(netadr_t from)
{
	// Prevent using getchallenge as an amplifier
	if ( SVC_RateLimitAddress( from, 10, 1000 ) )
	{
		if (!SVC_callback("CHALLENGE:ADDRESS", NET_AdrToString(from)))
			Com_DPrintf("SV_GetChallenge: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
		return;
	}

	// Allow getchallenge to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit( &outboundLeakyBucket, 10, 100 ) )
	{
		if (!SVC_callback("CHALLENGE:GLOBAL", NET_AdrToString(from)))
			Com_DPrintf("SV_GetChallenge: rate limit exceeded, dropping request\n");
		return;
	}

	SV_GetChallenge(from);
}

void hook_SVC_Info(netadr_t from)
{
	// Prevent using getinfo as an amplifier
	if ( SVC_RateLimitAddress( from, 10, 1000 ) )
	{
		if (!SVC_callback("INFO:ADDRESS", NET_AdrToString(from)))
			Com_DPrintf("SVC_Info: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
		return;
	}

	// Allow getinfo to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit( &outboundLeakyBucket, 10, 100 ) )
	{
		if (!SVC_callback("INFO:GLOBAL", NET_AdrToString(from)))
			Com_DPrintf("SVC_Info: rate limit exceeded, dropping request\n");
		return;
	}

	SVC_Info(from);
}

void hook_SVC_Status(netadr_t from)
{
	// Prevent using getstatus as an amplifier
	if ( SVC_RateLimitAddress( from, 10, 1000 ) )
	{
		if (!SVC_callback("STATUS:ADDRESS", NET_AdrToString(from)))
			Com_DPrintf("SVC_Status: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
		return;
	}

	// Allow getstatus to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit( &outboundLeakyBucket, 10, 100 ) )
	{
		if (!SVC_callback("STATUS:GLOBAL", NET_AdrToString(from)))
			Com_DPrintf("SVC_Status: rate limit exceeded, dropping request\n");
		return;
	}

	SVC_Status(from);
}
#endif

void manymaps_prepare(const char *mapname, int read)
{
	char library_path[512], map_check[512];
	
	cvar_t *fs_homepath = Cvar_FindVar("fs_homepath");
	cvar_t *fs_game = Cvar_FindVar("fs_game");
	cvar_t *map = Cvar_FindVar("mapname");

	if (strlen(fs_library->string))
		strncpy(library_path, fs_library->string, sizeof(library_path));
	else
		snprintf(library_path, sizeof(library_path), "%s/%s/Library", fs_homepath->string, fs_game->string);

	snprintf(map_check, sizeof(map_check), "%s/%s.iwd", library_path, mapname);

#if COD_VERSION == COD2_1_0
	const char *stock_maps[] = { "mp_breakout", "mp_brecourt", "mp_burgundy", "mp_carentan", "mp_dawnville", "mp_decoy", "mp_downtown", "mp_farmhouse", "mp_leningrad", "mp_matmata", "mp_railyard", "mp_toujane", "mp_trainstation" };
#else
	const char *stock_maps[] = { "mp_breakout", "mp_brecourt", "mp_burgundy", "mp_carentan", "mp_dawnville", "mp_decoy", "mp_downtown", "mp_farmhouse", "mp_leningrad", "mp_matmata", "mp_railyard", "mp_toujane", "mp_trainstation", "mp_rhine", "mp_harbor" };
#endif

	bool map_found = false, from_stock_map = false;

	for (int i = 0; i < int( sizeof(stock_maps) / sizeof(stock_maps[0]) ); i++)
	{
		if (strcmp(map->string, stock_maps[i]) == 0)
		{
			from_stock_map = true;
			break;
		}
	}

	for (int i = 0; i < int( sizeof(stock_maps) / sizeof(stock_maps[0]) ); i++)
	{
		if (strcmp(mapname, stock_maps[i]) == 0)
		{
			map_found = true;

			if (from_stock_map) // When changing from stock map to stock map do not trigger manymap
				return;
			else
				break;
		}
	}

	int map_exists = access(map_check, F_OK) != -1;

	if (!map_exists && !map_found)
		return;

	DIR *dir;
	struct dirent *dir_ent;

	dir = opendir(library_path);

	if (!dir)
		return;

	while ((dir_ent = readdir(dir)) != NULL)
	{
		if (strcmp(dir_ent->d_name, ".") == 0 || strcmp(dir_ent->d_name, "..") == 0)
			continue;

		char fileDelete[512];
		snprintf(fileDelete, sizeof(fileDelete), "%s/%s/%s", fs_homepath->string, fs_game->string, dir_ent->d_name);

		if (access(fileDelete, F_OK) != -1)
		{
			int unlink_success = unlink(fileDelete) == 0;
			printf("manymaps> REMOVED OLD LINK: %s result of unlink: %s\n", fileDelete, unlink_success?"success":"failed");
		}
	}

	closedir(dir);

	if (map_exists)
	{
		char src[512], dst[512];

		snprintf(src, sizeof(src), "%s/%s.iwd", library_path, mapname);
		snprintf(dst, sizeof(dst), "%s/%s/%s.iwd", fs_homepath->string, fs_game->string, mapname);

		if (access(src, F_OK) != -1)
		{
			int link_success = symlink(src, dst) == 0;
			printf("manymaps> NEW LINK: src=%s dst=%s result of link: %s\n", src, dst, link_success?"success":"failed");

			if (link_success && read == -1) // FS_LoadDir is needed when empty.iwd is missing (then .d3dbsp isn't referenced anywhere)
				FS_LoadDir(fs_homepath->string, fs_game->string);
		}
	}
}

int hook_findMap(const char *qpath, void **buffer)
{
	int read = FS_ReadFile(qpath, buffer);
	manymaps_prepare(Cmd_Argv(1), read);

	if (read != -1)
		return read;
	else
		return FS_ReadFile(qpath, buffer);
}

void custom_Scr_InitOpcodeLookup()
{
	hook_init_opcode->unhook();
	
	void (*GE_Scr_InitOpcodeLookup)();
	*(int *)&GE_Scr_InitOpcodeLookup = hook_init_opcode->from;
	
	scrVarPub_t * vars = &scrVarPub;
	
	vars->developer = 1;
	GE_Scr_InitOpcodeLookup();
	
	if(!developer->integer)
		vars->developer = 0;
	
	hook_init_opcode->hook();
}

void custom_AddOpcodePos(int a1, int a2)
{
	hook_add_opcode->unhook();
	
	void (*GE_AddOpcodePos)(int, int);
	*(int *)&GE_AddOpcodePos = hook_add_opcode->from;
	
	scrVarPub_t * vars = &scrVarPub;
	
	vars->developer = 1;
	GE_AddOpcodePos(a1, a2);
	
	if(!developer->integer)
		vars->developer = 0;
	
	hook_add_opcode->hook();
}

void custom_Scr_PrintPrevCodePos(int a1, char *a2, int a3)
{
	hook_print_codepos->unhook();
	
	void (*GE_Scr_PrintPrevCodePos)(int, char *, int);
	*(int *)&GE_Scr_PrintPrevCodePos = hook_print_codepos->from;
	
	scrVarPub_t * vars = &scrVarPub;
	
	vars->developer = 1;
	GE_Scr_PrintPrevCodePos(a1, a2, a3);
	
	if(!developer->integer)
		vars->developer = 0;
	
	hook_print_codepos->hook();
}

class cCallOfDuty2Pro
{
public:
	cCallOfDuty2Pro()
	{
		// dont inherit lib of parent
		unsetenv("LD_PRELOAD");
		
		// otherwise the printf()'s are printed at crash/end on older os/compiler versions
		setbuf(stdout, NULL);

#if COD_VERSION == COD2_1_0
		printf("> [LIBCOD] Compiled for: CoD2 1.0\n");
#elif COD_VERSION == COD2_1_2
		printf("> [LIBCOD] Compiled for: CoD2 1.2\n");
#elif COD_VERSION == COD2_1_3
		printf("> [LIBCOD] Compiled for: CoD2 1.3\n");
#endif

		printf("> [LIBCOD] Compiled %s %s using GCC %s\n", __DATE__, __TIME__, __VERSION__);

		// allow to write in executable memory
		mprotect((void *)0x08048000, 0x135000, PROT_READ | PROT_WRITE | PROT_EXEC);

#if COD_VERSION == COD2_1_0
		cracking_hook_call(0x08061FE7, (int)hook_sv_init);
		cracking_hook_call(0x08091D0C, (int)hook_sv_spawnserver);
		cracking_hook_call(0x0808F281, (int)hook_ClientCommand);
		cracking_hook_call(0x0808C8C0, (int)hook_AuthorizeState);
		cracking_hook_call(0x0808BFCA, (int)hook_isLanAddress);
		cracking_hook_call(0x0808AD00, (int)hook_findMap);
		cracking_hook_call(0x0808F134, (int)hook_ClientUserinfoChanged);
		cracking_hook_call(0x0807059F, (int)Scr_GetCustomFunction);
		cracking_hook_call(0x080707C3, (int)Scr_GetCustomMethod);
        cracking_hook_call(0x080E9524, (int)hook_findWeaponIndex);

#if COMPILE_PLAYER == 1
		cracking_hook_call(0x0808E18F, (int)hook_gamestate_info);
#endif

		cracking_hook_call(0x08081CFE, (int)hook_scriptError);

		hook_gametype_scripts = new cHook(0x0810DDEE, (int)hook_codscript_gametype_scripts);
		hook_gametype_scripts->hook();
        
		hook_init_opcode = new cHook(0x08076B9C, (int)custom_Scr_InitOpcodeLookup);
		hook_init_opcode->hook();
		hook_add_opcode = new cHook(0x08076D92, (int)custom_AddOpcodePos);
		hook_add_opcode->hook();
		hook_print_codepos = new cHook(0x08077DBA, (int)custom_Scr_PrintPrevCodePos);
		hook_print_codepos->hook();
		
		hook_player_collision = new cHook(0x080F2F2E, (int)player_collision);
		hook_player_collision->hook();
		hook_player_eject = new cHook(0x080F474A, (int)player_eject);
		hook_player_eject->hook();
		hook_fire_grenade = new cHook(0x0810C1F6, (int)fire_grenade);
		hook_fire_grenade->hook();
		hook_touch_item = new cHook(0x081037F0, int(touch_item));
		hook_touch_item->hook();

#if COMPILE_PLAYER == 1
		hook_play_movement = new cHook(0x0808F488, (int)play_movement);
		hook_play_movement->hook();
		hook_play_endframe = new cHook(0x080F4DBE, (int)play_endframe);
		hook_play_endframe->hook();
		hook_set_anim = new cHook(0x080D69B2, (int)set_anim);
		hook_set_anim->hook();
#endif

		cracking_hook_function(0x080E97F0, (int)hook_BG_IsWeaponValid);
		cracking_hook_function(0x0808E544, (int)custom_SV_WriteDownloadToClient);
		cracking_hook_function(0x080B59CE, (int)custom_va);
		cracking_hook_function(0x0808EC66, (int)hook_SV_VerifyIwds_f);
		cracking_hook_function(0x0808EEEC, (int)hook_SV_ResetPureClient_f);
		cracking_hook_function(0x0809443E, (int)custom_SV_CalcPings);
		cracking_hook_function(0x080945AC, (int)custom_SV_CheckTimeouts);

#if COMPILE_BOTS == 1
		cracking_hook_function(0x0809479A, (int)custom_SV_BotUserMove);
#endif

#if COMPILE_RATELIMITER == 1
		cracking_hook_call(0x08094081, (int)hook_SVC_Info);
		cracking_hook_call(0x0809403E, (int)hook_SVC_Status);
		cracking_hook_call(0x080940C4, (int)hook_SV_GetChallenge);
		cracking_hook_call(0x08094191, (int)hook_SVC_RemoteCommand);
#endif

#elif COD_VERSION == COD2_1_2
		cracking_hook_call(0x08062301, (int)hook_sv_init);
		cracking_hook_call(0x08093572, (int)hook_sv_spawnserver);
		cracking_hook_call(0x08090B0C, (int)hook_ClientCommand);
		cracking_hook_call(0x0808DA52, (int)hook_AuthorizeState);
		cracking_hook_call(0x0808D22E, (int)hook_isLanAddress);
		cracking_hook_call(0x0808BCFC, (int)hook_findMap);
		cracking_hook_call(0x080909BE, (int)hook_ClientUserinfoChanged);
		cracking_hook_call(0x08070B1B, (int)Scr_GetCustomFunction);
		cracking_hook_call(0x08070D3F, (int)Scr_GetCustomMethod);
		cracking_hook_call(0x0808227A, (int)hook_scriptError);
		cracking_hook_call(0x0808FCBE, (int)hook_bad_printf);
        cracking_hook_call(0x080EBB14, (int)hook_findWeaponIndex);

#if COMPILE_PLAYER == 1
		cracking_hook_call(0x0808F533, (int)hook_gamestate_info);
#endif

		hook_gametype_scripts = new cHook(0x0811012A, (int)hook_codscript_gametype_scripts);
		hook_gametype_scripts->hook();
        
        hook_init_opcode = new cHook(0x08077110, (int)custom_Scr_InitOpcodeLookup);
        hook_init_opcode->hook();
        hook_add_opcode = new cHook(0x08077306, (int)custom_AddOpcodePos);
        hook_add_opcode->hook();
        hook_print_codepos = new cHook(0x0807832E, (int)custom_Scr_PrintPrevCodePos);
        hook_print_codepos->hook();
		
		hook_player_collision = new cHook(0x080F553E, (int)player_collision);
		hook_player_collision->hook();
		hook_player_eject = new cHook(0x080F6D5A, (int)player_eject);
		hook_player_eject->hook();
		hook_fire_grenade = new cHook(0x0810E532, (int)fire_grenade);
		hook_fire_grenade->hook();
		hook_touch_item = new cHook(0x08105B24, int(touch_item));
		hook_touch_item->hook();

#if COMPILE_PLAYER == 1
		hook_play_movement = new cHook(0x08090D18, (int)play_movement);
		hook_play_movement->hook();
		hook_play_endframe = new cHook(0x080F73D2, (int)play_endframe);
		hook_play_endframe->hook();
		hook_set_anim = new cHook(0x080D8F92, (int)set_anim);
		hook_set_anim->hook();
#endif

		cracking_hook_function(0x080EBDE0, (int)hook_BG_IsWeaponValid);
		cracking_hook_function(0x0808FD2E, (int)custom_SV_WriteDownloadToClient);
		cracking_hook_function(0x080B7E62, (int)custom_va);
		cracking_hook_function(0x080904A0, (int)hook_SV_VerifyIwds_f);
		cracking_hook_function(0x08090726, (int)hook_SV_ResetPureClient_f);
		cracking_hook_function(0x0809630E, (int)custom_SV_CalcPings);
		cracking_hook_function(0x080964C4, (int)custom_SV_CheckTimeouts);

#if COMPILE_BOTS == 1
		cracking_hook_function(0x080966B2, (int)custom_SV_BotUserMove);
#endif

#if COMPILE_RATELIMITER == 1
		cracking_hook_call(0x08095B8E, (int)hook_SVC_Info);
		cracking_hook_call(0x08095ADA, (int)hook_SVC_Status);
		cracking_hook_call(0x08095BF8, (int)hook_SV_GetChallenge);
		cracking_hook_call(0x08095D63, (int)hook_SVC_RemoteCommand);
#endif

#elif COD_VERSION == COD2_1_3
		cracking_hook_call(0x080622F9, (int)hook_sv_init);
		cracking_hook_call(0x0809362A, (int)hook_sv_spawnserver);
		cracking_hook_call(0x08090BA0, (int)hook_ClientCommand);
		cracking_hook_call(0x0808DB12, (int)hook_AuthorizeState);
		cracking_hook_call(0x0808D2FA, (int)hook_isLanAddress);
		cracking_hook_call(0x0808BDC8, (int)hook_findMap);
		cracking_hook_call(0x08090A52, (int)hook_ClientUserinfoChanged);
		cracking_hook_call(0x08070BE7, (int)Scr_GetCustomFunction);
		cracking_hook_call(0x08070E0B, (int)Scr_GetCustomMethod);
		cracking_hook_call(0x08082346, (int)hook_scriptError);
		cracking_hook_call(0x0808FD52, (int)hook_bad_printf);
        cracking_hook_call(0x080EBC58, (int)hook_findWeaponIndex);
        
#if COMPILE_PLAYER == 1
		cracking_hook_call(0x0808F5C7, (int)hook_gamestate_info);
#endif

		hook_gametype_scripts = new cHook(0x08110286, (int)hook_codscript_gametype_scripts);
		hook_gametype_scripts->hook();

		hook_init_opcode = new cHook(0x080771DC, (int)custom_Scr_InitOpcodeLookup);
		hook_init_opcode->hook();
		hook_add_opcode = new cHook(0x080773D2, (int)custom_AddOpcodePos);
		hook_add_opcode->hook();
		hook_print_codepos = new cHook(0x080783FA, (int)custom_Scr_PrintPrevCodePos);
		hook_print_codepos->hook();
		
		hook_player_collision = new cHook(0x080F5682, (int)player_collision);
		hook_player_collision->hook();
		hook_player_eject = new cHook(0x080F6E9E, (int)player_eject);
		hook_player_eject->hook();
		hook_fire_grenade = new cHook(0x0810E68E, (int)fire_grenade);
		hook_fire_grenade->hook();
		hook_touch_item = new cHook(0x08105C80, int(touch_item));
		hook_touch_item->hook();
		hook_g_tempentity = new cHook(0x0811EFC4, (int)custom_G_TempEntity);
		hook_g_tempentity->hook();
		//hook_msg_writedeltaplayerstate = new cHook(0x0806A200, (int)custom_MSG_WriteDeltaPlayerstate2);
		//hook_msg_writedeltaplayerstate->hook();

#if COMPILE_PLAYER == 1
		hook_play_movement = new cHook(0x08090DAC, (int)play_movement);
		hook_play_movement->hook();
		hook_play_endframe = new cHook(0x080F7516, (int)play_endframe);
		hook_play_endframe->hook();
		hook_set_anim = new cHook(0x080D90D6, (int)set_anim);
		hook_set_anim->hook();
#endif

        cracking_hook_function(0x0811F232, (int)custom_G_AddEvent);
		cracking_hook_function(0x080EBF24, (int)hook_BG_IsWeaponValid);
        cracking_hook_function(0x080DFC78, (int)custom_BG_AddPredictableEventToPlayerstate);
        cracking_hook_function(0x0806A200, (int)custom_MSG_WriteDeltaPlayerstate);
        cracking_hook_function(0x0808F302, (int)custom_SV_SendClientGameState);
        cracking_hook_function(0x0808F02E, (int)custom_SV_DropClient);
		cracking_hook_function(0x0808FDC2, (int)custom_SV_WriteDownloadToClient);
		cracking_hook_function(0x080B7FA6, (int)custom_va);
		cracking_hook_function(0x08090534, (int)hook_SV_VerifyIwds_f);
		cracking_hook_function(0x080907BA, (int)hook_SV_ResetPureClient_f);
		cracking_hook_function(0x080963C8, (int)custom_SV_CalcPings);
		cracking_hook_function(0x0809657E, (int)custom_SV_CheckTimeouts);

#if COMPILE_BOTS == 1
		cracking_hook_function(0x0809676C, (int)custom_SV_BotUserMove);
#endif

#if COMPILE_RATELIMITER == 1
		cracking_hook_call(0x08095C48, (int)hook_SVC_Info);
		cracking_hook_call(0x08095B94, (int)hook_SVC_Status);
		cracking_hook_call(0x08095CB2, (int)hook_SV_GetChallenge);
		cracking_hook_call(0x08095E1D, (int)hook_SVC_RemoteCommand);
#endif

#endif
        gsc_weapons_init();
		printf("> [PLUGIN LOADED]\n");
	}

	~cCallOfDuty2Pro()
	{
        gsc_weapons_free();
		printf("> [PLUGIN UNLOADED]\n");
	}
};

cCallOfDuty2Pro *pro;

// lol, single again: because it got loaded two times now
// both now: able to load with wrapper AND directly
// IMPORTANT: file needs "lib" infront of name, otherwise it wont be loaded
// will be called when LD_PRELOAD is referencing this .so

void __attribute__ ((constructor)) lib_load(void)
{
	pro = new cCallOfDuty2Pro;
}

void __attribute__ ((destructor)) lib_unload(void)
{
	delete pro;
}