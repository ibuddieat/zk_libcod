#include "gsc.hpp"

// Stock cvars
cvar_t *cl_allowDownload;
cvar_t *developer;
cvar_t *rcon_password;
cvar_t *sv_allowDownload;
cvar_t *sv_fps;
cvar_t *sv_maxclients;
cvar_t *sv_padPackets;
cvar_t *sv_pure;
cvar_t *sv_timeout;
cvar_t *sv_zombietime;
#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
	cvar_t *cl_wwwDownload;
	cvar_t *sv_wwwBaseURL;
	cvar_t *sv_wwwDlDisconnected;
	cvar_t *sv_wwwDownload;
#endif

// Custom cvars
#if COMPILE_UTILS == 1
	cvar_t *con_coloredPrints;
#endif
cvar_t *fs_callbacks;
cvar_t *fs_library;
cvar_t *g_debugEvents;
cvar_t *g_debugStaticModels;
cvar_t *g_logPickup;
cvar_t *g_playerCollision;
cvar_t *g_playerEject;
cvar_t *g_spawnMapTurrets;
cvar_t *g_spawnMapWeapons;
cvar_t *sv_allowRcon;
cvar_t *sv_botKickMessages;
cvar_t *sv_cracked;
cvar_t *sv_disconnectMessages;
cvar_t *sv_downloadMessage;
cvar_t *sv_kickMessages;
cvar_t *sv_limitLocalRcon;
cvar_t *sv_logHeartbeats;
cvar_t *sv_logRcon;
cvar_t *sv_noauthorize;
cvar_t *sv_timeoutMessages;
#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
	cvar_t *sv_wwwDlDisconnectedMessages;
#endif

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
cHook *hook_touch_item_auto;
cHook *hook_developer_prints;
cHook *hook_console_print;
cHook *hook_g_tempentity;
cHook *hook_sv_masterheartbeat;
cHook *hook_g_runframe;
cHook *hook_scr_loadgametype;
cHook *hook_vm_notify;

int codecallback_client_spam = 0;
int codecallback_dprintf = 0;
int codecallback_error = 0;
int codecallback_fire_grenade = 0;
int codecallback_map_turrets_load = 0;
int codecallback_map_weapons_load = 0;
int codecallback_notify = 0;
int codecallback_pickup = 0;
int codecallback_playercommand = 0;
int codecallback_remotecommand = 0;
int codecallback_userinfochanged = 0;
int codecallback_vid_restart = 0;

int codecallback_adsbutton = 0;
int codecallback_attackbutton = 0;
int codecallback_crouchbutton = 0;
int codecallback_fragbutton = 0;
int codecallback_holdbreathbutton = 0;
int codecallback_jumpbutton = 0;
int codecallback_leanleftbutton = 0;
int codecallback_leanrightbutton = 0;
int codecallback_meleebreathbutton = 0;
int codecallback_meleebutton = 0;
int codecallback_pronebutton = 0;
int codecallback_reloadbutton = 0;
int codecallback_smokebutton = 0;
int codecallback_usebutton = 0;

qboolean logRcon = qtrue;
qboolean logHeartbeat = qtrue;

scr_error_t scr_errors[MAX_ERROR_BUFFER];
int scr_errors_index = 0;

scr_notify_t scr_notify[MAX_NOTIFY_BUFFER];
int scr_notify_index = 0;

int player_no_pickup[MAX_CLIENTS] = {0};
int player_no_earthquakes[MAX_CLIENTS] = {0};
int player_silent[MAX_CLIENTS] = {0};
int player_g_speed[MAX_CLIENTS] = {0};
int player_g_gravity[MAX_CLIENTS] = {0};
int custom_animation[MAX_CLIENTS] = {0};
int clientfps[MAX_CLIENTS] = {0};
int clientframes[MAX_CLIENTS] = {0};
uint64_t clientframetime[MAX_CLIENTS] = {0};
int previousbuttons[MAX_CLIENTS] = {0};
#if COMPILE_BOTS == 1
	int bot_buttons[MAX_CLIENTS] = {0};
	int bot_weapon[MAX_CLIENTS] = {0};
	char bot_forwardmove[MAX_CLIENTS] = {0};
	char bot_rightmove[MAX_CLIENTS] = {0};
#endif
int gamestate_size[MAX_CLIENTS] = {0};

void hook_sv_init(const char *format, ...)
{
	char s[COD2_MAX_STRINGLENGTH];
	va_list va;

	va_start(va, format);
	vsnprintf(s, sizeof(s), format, va);
	va_end(va);

	Com_Printf("%s", s);

	/* Do stuff after sv has been initialized here */
	
	// Get references to stock cvars
	cl_allowDownload = Cvar_RegisterBool("cl_allowDownload", qtrue, CVAR_ARCHIVE | CVAR_SYSTEMINFO); // Force-enable download for clients
	developer = Cvar_FindVar("developer");
	rcon_password = Cvar_FindVar("rcon_password");
	sv_allowDownload = Cvar_FindVar("sv_allowDownload");
	sv_fps = Cvar_FindVar("sv_fps");
	sv_maxclients = Cvar_FindVar("sv_maxclients");
	sv_padPackets = Cvar_FindVar("sv_padPackets");
	sv_pure = Cvar_FindVar("sv_pure");
	sv_timeout = Cvar_FindVar("sv_timeout");
	sv_zombietime = Cvar_FindVar("sv_zombietime");
	#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
		cl_wwwDownload = Cvar_RegisterBool("cl_wwwDownload", qtrue, CVAR_ARCHIVE | CVAR_SYSTEMINFO); // Force-enable wwwDownload for clients
		sv_wwwBaseURL = Cvar_FindVar("sv_wwwBaseURL");
		sv_wwwDlDisconnected = Cvar_FindVar("sv_wwwDlDisconnected");
		sv_wwwDownload = Cvar_FindVar("sv_wwwDownload");
	#endif

	// Register custom cvars
	#if COMPILE_UTILS == 1
		con_coloredPrints = Cvar_RegisterBool("con_coloredPrints", qfalse, CVAR_ARCHIVE);
	#endif
	fs_callbacks = Cvar_RegisterString("fs_callbacks", "", CVAR_ARCHIVE);
	fs_library = Cvar_RegisterString("fs_library", "", CVAR_ARCHIVE);
	g_debugEvents = Cvar_RegisterBool("g_debugEvents", qfalse, CVAR_ARCHIVE);
	g_debugStaticModels = Cvar_RegisterBool("g_debugStaticModels", qfalse, CVAR_ARCHIVE);
	g_logPickup = Cvar_RegisterBool("g_logPickup", qtrue, CVAR_ARCHIVE);
	g_playerCollision = Cvar_RegisterBool("g_playerCollision", qtrue, CVAR_ARCHIVE);
	g_playerEject = Cvar_RegisterBool("g_playerEject", qtrue, CVAR_ARCHIVE);
	g_spawnMapTurrets = Cvar_RegisterBool("g_spawnMapTurrets", qtrue, CVAR_ARCHIVE);
	g_spawnMapWeapons = Cvar_RegisterBool("g_spawnMapWeapons", qtrue, CVAR_ARCHIVE);
	sv_allowRcon = Cvar_RegisterBool("sv_allowRcon", qtrue, CVAR_ARCHIVE);
	sv_botKickMessages = Cvar_RegisterBool("sv_botKickMessages", qtrue, CVAR_ARCHIVE);
	sv_cracked = Cvar_RegisterBool("sv_cracked", qfalse, CVAR_ARCHIVE);
	sv_disconnectMessages = Cvar_RegisterBool("sv_disconnectMessages", qtrue, CVAR_ARCHIVE);
	sv_downloadMessage = Cvar_RegisterString("sv_downloadMessage", "", CVAR_ARCHIVE);
	sv_kickMessages = Cvar_RegisterBool("sv_kickMessages", qtrue, CVAR_ARCHIVE);
	sv_limitLocalRcon = Cvar_RegisterBool("sv_limitLocalRcon", qtrue, CVAR_ARCHIVE);
	sv_logHeartbeats = Cvar_RegisterBool("sv_logHeartbeats", qtrue, CVAR_ARCHIVE);
	sv_logRcon = Cvar_RegisterBool("sv_logRcon", qtrue, CVAR_ARCHIVE);
	sv_noauthorize = Cvar_RegisterBool("sv_noauthorize", qfalse, CVAR_ARCHIVE);
	sv_timeoutMessages = Cvar_RegisterBool("sv_timeoutMessages", qtrue, CVAR_ARCHIVE);
	#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
		sv_wwwDlDisconnectedMessages = Cvar_RegisterInt("sv_wwwDlDisconnectedMessages", 1, 0, 2, CVAR_ARCHIVE);
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

	hook_developer_prints->hook();
}

void custom_sv_masterheartbeat(const char *hbname)
{
	hook_sv_masterheartbeat->unhook();

	void (*sig)(const char *hbname);
	*(int *)&sig = hook_sv_masterheartbeat->from;

	#if COD_VERSION == COD2_1_0
		int sending_heartbeat_string_offset = 0x0; // Not tested
	#elif COD_VERSION == COD2_1_2
		int sending_heartbeat_string_offset = 0x0; // Not tested
	#elif COD_VERSION == COD2_1_3
		int sending_heartbeat_string_offset = 0x0814BBC0;
	#endif

	if ( logHeartbeat && !sv_logHeartbeats->boolean )
	{
		byte disable = 0;
		memcpy((void *)sending_heartbeat_string_offset, &disable, 1);
		logHeartbeat = qfalse;
		Com_DPrintf("Disabled heartbeat logging\n");
	}
	else if ( !logHeartbeat && sv_logHeartbeats->boolean )
	{
		byte enable = 0x53; // "S"
		memcpy((void *)sending_heartbeat_string_offset, &enable, 1);
		logHeartbeat = qtrue;
		Com_DPrintf("Enabled heartbeat logging\n");
	}

	sig(hbname);
	
	hook_sv_masterheartbeat->hook();
}

int hook_codscript_gametype_scripts()
{
	char path_for_cb[512] = "maps/mp/gametypes/_callbacksetup";
	
	if ( strlen(fs_callbacks->string) )
		strncpy(path_for_cb, fs_callbacks->string, sizeof(path_for_cb));
	
	hook_gametype_scripts->unhook();
	
	codecallback_client_spam = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_CLSpam", 0);
	codecallback_dprintf = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_DPrintf", 0);
	codecallback_error = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_Error", 0);
	codecallback_fire_grenade = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_FireGrenade", 0);
	codecallback_map_turrets_load = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_MapTurretsLoad", 0);
	codecallback_map_weapons_load = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_MapWeaponsLoad", 0);
	codecallback_notify = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_Notify", 0);
	codecallback_pickup = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_Pickup", 0);
	codecallback_playercommand = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_PlayerCommand", 0);
	codecallback_remotecommand = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_RemoteCommand", 0);
	codecallback_userinfochanged = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_UserInfoChanged", 0);
	codecallback_vid_restart = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_VidRestart", 0);
	
	codecallback_adsbutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_AdsButton", 0);
	codecallback_attackbutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_AttackButton", 0);
	codecallback_crouchbutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_CrouchButton", 0);
	codecallback_fragbutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_FragButton", 0);
	codecallback_holdbreathbutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_HoldBreathButton", 0);
	codecallback_jumpbutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_JumpButton", 0);
 	codecallback_leanleftbutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_LeanLeftButton", 0);
 	codecallback_leanrightbutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_LeanRightButton", 0);
	codecallback_meleebreathbutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_MeleeBreathButton", 0);
	codecallback_meleebutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_MeleeButton", 0);
	codecallback_pronebutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_ProneButton", 0);
	codecallback_reloadbutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_ReloadButton", 0);
	codecallback_smokebutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_SmokeButton", 0);
 	codecallback_usebutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_UseButton", 0);

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

	if ( g_playerCollision->boolean )
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

	if ( g_playerEject->boolean )
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

	if ( codecallback_fire_grenade && Scr_IsSystemActive() )
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

	if ( !Scr_IsSystemActive() )
		return;
			
	if ( !codecallback_playercommand )
	{	
		ClientCommand(clientNum);
		return;
	}

	stackPushArray();
	int args = Cmd_Argc();
	for ( int i = 0; i < args; i++ )
	{
		char tmp[COD2_MAX_STRINGLENGTH];
		SV_Cmd_ArgvBuffer(i, tmp, sizeof(tmp));
		if( i == 1 && tmp[0] >= 20 && tmp[0] <= 22 )
		{
			char *part = strtok(tmp + 1, " ");
			while( part != NULL )
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
	if ( sv_noauthorize->boolean )
		return 1;

	return Sys_IsLANAddress(adr);
}

const char* hook_AuthorizeState(int arg)
{
	const char *s = Cmd_Argv(arg);

	if ( sv_cracked->boolean && strcmp(s, "deny") == 0 )
		return "accept";

	return s;
}

void hook_ClientUserinfoChanged(int clientNum)
{
	if ( !Scr_IsSystemActive() )
		return;

	if ( !codecallback_userinfochanged )
	{	
		ClientUserinfoChanged(clientNum);
		return;
	}

	short ret = Scr_ExecEntThread(&g_entities[clientNum], codecallback_userinfochanged, 0);
	Scr_FreeThread(ret);
}

void custom_SV_DropClient(client_t *drop, const char *reason)
{
	int i, pb_test;
	challenge_t *challenge;
	qboolean isBot = qfalse;
	qboolean showIngameMessage = qtrue;
	char name[0x40];
	
	Com_DPrintf("SV_DropClient for %s\n", drop->name);

	if ( drop->state == CS_ZOMBIE )
		return;	// Already dropped
	
	drop->delayDropMsg = 0;
	I_strncpyz(name, drop->name, sizeof(name));
	SV_FreeClient(drop);

	Com_DPrintf("Going to CS_ZOMBIE for %s\n", name);
	drop->state = CS_ZOMBIE; // Become free in a few seconds

	if ( drop->netchan.remoteAddress.type == NA_BOT )
		isBot = qtrue;

	if ( !isBot )
	{
		// See if we already have a challenge for this ip
		challenge = &svs.challenges[0];

		for ( i = 0 ; i < MAX_CHALLENGES ; i++, challenge++ )
		{
			if ( NET_CompareAdr( drop->netchan.remoteAddress, challenge->adr ) )
			{
				challenge->connected = qfalse;
				break;
			}
		}
	}

	pb_test = FUN_081384cc(reason);

	if ( isBot && !sv_botKickMessages->boolean && I_stricmp(reason, "EXE_DISCONNECTED") == 0 )
		showIngameMessage = qfalse;

	if ( !sv_kickMessages->boolean && I_stricmp(reason, "EXE_PLAYERKICKED") == 0 )
		showIngameMessage = qfalse; // This overrides enabled sv_botKickMessages

	if ( !sv_timeoutMessages->boolean && I_stricmp(reason, "EXE_TIMEDOUT") == 0 )
		showIngameMessage = qfalse;

	if ( !sv_disconnectMessages->boolean && I_stricmp(reason, "EXE_DISCONNECTED") == 0 )
		showIngameMessage = qfalse;

	if ( sv_wwwDlDisconnectedMessages->integer != 1 && I_stricmp(reason, "PC_PATCH_1_1_DOWNLOADDISCONNECTED") == 0 )
		showIngameMessage = qfalse;

	if ( showIngameMessage )
	{
		if ( !pb_test )
			SV_SendServerCommand(0, 0, "e \"\x15%s^7 %s%s\"", name, "", reason);
		else
			SV_SendServerCommand(0, 0, "e \"\x15%s^7 %s%s\"", name, "\x14", reason);
	}

	Com_Printf("%i:%s %s\n", drop - svs.clients, name, reason);

	SV_SendServerCommand(0, 1, "J %d", drop - svs.clients);
	if ( !pb_test )
		SV_SendServerCommand(drop, 1, "w \"%s^7 %s\" PB", name, reason);
	else
		SV_SendServerCommand(drop, 1, "w \"%s\"", reason);

	/* If this was the last client on the server, send a heartbeat
	   to the master so it is known the server is empty
	   send a heartbeat now so the master will get up to date info
	   if there is already a slot for this ip, reuse it */
	for ( i = 0 ; i < sv_maxclients->integer ; i++ )
	{
		if ( svs.clients[i].state >= CS_CONNECTED )
			break;
	}
	if ( i == sv_maxclients->integer )
		SV_Heartbeat();
}

void custom_Touch_Item(gentity_t *item, gentity_t *entity, int touch)
{
	gclient_t * client;
	entity_event_t event;
	gitem_t *bg_item;
	char name[0x40];
	int respawn;
	itemType_t type;
	
	if ( !item->active )
		return;
	
	item->active = 0;
	client = entity->client;
	
	if ( !client || entity->healthPoints <= 0 || level.clientIsSpawning )
		return;

	event = EV_ITEM_PICKUP;
	bg_item = &bg_itemlist;
	bg_item += item->params.item[0].index;
	
	if ( !BG_CanItemBeGrabbed(&item->s, &entity->client->ps, touch) )
	{
		if ( (!touch && (item->s).clientNum != (entity->s).number) && bg_item->giType == IT_WEAPON )
		{
			if ( !(((entity->client->ps).weapons[bg_item->giAmmoIndex >> 5] >> (bg_item->giAmmoIndex & 0x1F)) & 1) )
			{
				if ( (BG_WeaponDefs(bg_item->giAmmoIndex)->impactType - 1) < 2)
					SV_GameSendServerCommand(entity - g_entities, 0, custom_va("%c \"GAME_CANT_GET_PRIMARY_WEAP_MESSAGE\"", 0x66));
			}
			else
			{
				SV_GameSendServerCommand(entity - g_entities, 0, custom_va("%c \"GAME_PICKUP_CANTCARRYMOREAMMO\x14%s\"", 0x66, BG_WeaponDefs(bg_item->giAmmoIndex)->szDisplayName));
			}
		}
	}
	else
	{
		I_strncpyz(name, (entity->client->sess).manualModeName, 0x40);
		I_CleanStr(name);
		
		if ( g_logPickup->boolean )
		{
			if ( bg_item->giType == IT_WEAPON )
				G_LogPrintf("Weapon;%d;%d;%s;%s\n", SV_GetGuid((entity->s).number), (entity->s).number, name, BG_WeaponDefs(bg_item->giAmmoIndex)->szInternalName);
			else
				G_LogPrintf("Item;%d;%d;%s;%s\n", SV_GetGuid((entity->s).number), (entity->s).number, name, bg_item->classname);
		}
		
		respawn = qtrue;
		type = bg_item->giType;
		if ( type == IT_AMMO )
		{
			if ( codecallback_pickup && Scr_IsSystemActive() )
			{
				stackPushString("ammo");
				short ret = Scr_ExecEntThread(entity, codecallback_pickup, 1);
				Scr_FreeThread(ret);
			}
			else
			{
				respawn = Pickup_Ammo(item, entity);
			}
		}
		else if ( type < 3 )
		{
			if ( type != IT_WEAPON )
				return;

			if ( codecallback_pickup && Scr_IsSystemActive() )
			{
				stackPushVector(item->r.currentAngles);
				stackPushVector(item->r.currentOrigin);
				stackPushInt(bg_item->quantity);
				stackPushString(bg_item->display_name);
				stackPushString(BG_WeaponDefs(bg_item->giAmmoIndex)->szInternalName);
				stackPushString("weapon");
				short ret = Scr_ExecEntThread(entity, codecallback_pickup, 6);
				Scr_FreeThread(ret);
			}
			else
			{
				respawn = Pickup_Weapon(item, entity, &event, touch);
			}
		}
		else
		{
			if ( type != IT_HEALTH )
				return;
			
			if ( codecallback_pickup && Scr_IsSystemActive() )
			{
				stackPushString("health");
				short ret = Scr_ExecEntThread(entity, codecallback_pickup, 1);
				Scr_FreeThread(ret);
			}
			else
			{
				respawn = Pickup_Health(item, entity);
			}
		}
		
		if ( !respawn )
			return;
		
		if ( (entity->client->sess).predictItemPickup == 0 )
			G_AddEvent(entity, event, (item->s).index);
		else
			G_AddPredictableEvent(entity, event, (item->s).index);
		G_FreeEntity(item);
	}
}

void custom_BG_AddPredictableEventToPlayerstate(int event, int eventParm, playerState_t *ps)
{
	if ( event != EV_NONE )
	{
		if ( g_debugEvents->boolean )
			Com_DPrintf("BG_AddPredictableEventToPlayerstate() event %26s for client %2d\n", *(&entity_event_names + event), ps->clientNum);
		
		/* New code start: silent */
		if ( ( ( event >= EV_FOOTSTEP_RUN_DEFAULT && event <= EV_FOLIAGE_SOUND ) || event == EV_NOAMMO ) && player_silent[ps->clientNum] )
			return;
		/* New code end */
		
		ps->events[ps->eventSequence & ( MAX_EVENTS - 1 )] = event & 0xff;
		ps->eventParms[ps->eventSequence & ( MAX_EVENTS - 1 )] = eventParm & 0xff;
		ps->eventSequence++;
	}
}

void custom_G_AddEvent(gentity_t * ent, int event, int eventParm)
{
	if ( ent->client )
	{
		if ( g_debugEvents->boolean )
			Com_DPrintf("G_AddEvent() event %26s for client %2d\n", *(&entity_event_names + event), ent->client->ps.clientNum);
		ent->client->ps.events[ent->client->ps.eventSequence & ( MAX_EVENTS - 1 )] = event;
		ent->client->ps.eventParms[ent->client->ps.eventSequence & ( MAX_EVENTS - 1 )] = eventParm;
		ent->client->ps.eventSequence++;
	}
	else
	{
		if ( g_debugEvents->boolean )
			Com_DPrintf("G_AddEvent() event %26s for entity %2d\n", *(&entity_event_names + event), ent->s.number);
		ent->s.events[ent->s.eventSequence & ( MAX_EVENTS - 1 )] = event;
		ent->s.eventParms[ent->s.eventSequence & ( MAX_EVENTS - 1 )] = eventParm;
		ent->s.eventSequence++;
	}
	ent->eventTime = level.time;
	ent->r.eventTime = level.time;
}

gentity_t* custom_G_TempEntity(vec3_t * origin, int event)
{
	hook_g_tempentity->unhook();

	gentity_t* (*sig)(vec3_t * origin, int event);
	*(int *)&sig = hook_g_tempentity->from;

	if ( g_debugEvents->boolean )
		Com_DPrintf("G_TempEntity() event %26s at (%f,%f,%f)\n", *(&entity_event_names + event), &origin[0], &origin[1], &origin[2]);

	/* Filter example:
	   use with caution: this can cause script runtime errors (or worse) on
	   some events, e.g., when using the obituary function, due to (then)
	   undefined parameters

	if (event == EV_PLAY_FX)
		event = EV_NONE;
	*/
	gentity_t* tempEntity = sig(origin, event);

	hook_g_tempentity->hook();

	return tempEntity;
}

void custom_MSG_WriteDeltaField(msg_t *buf, byte *from, byte *to, netField_t *field)
{
	int trunc;
	float fullFloat;
	int *fromF, *toF;
	int32_t absbits;
	uint8_t abs3bits;
	int32_t absto;
	
	fromF = ( int * )( (byte *)from + field->offset );
	toF = ( int * )( (byte *)to + field->offset );
	
	if ( *fromF == *toF )
	{
		MSG_WriteBit0(buf);
	}
	else
	{
		MSG_WriteBit1(buf);
		if ( field->bits == 0 )
		{
			fullFloat = *(float *)toF;
			trunc = (int)fullFloat;

			if ( fullFloat == 0.0 )
			{
				MSG_WriteBit0(buf);
			}
			else
			{
				MSG_WriteBit1(buf);
				if ( trunc == fullFloat && trunc + FLOAT_INT_BIAS >= 0 && trunc + FLOAT_INT_BIAS < ( 1 << FLOAT_INT_BITS ) )
				{
					MSG_WriteBit0(buf);
					MSG_WriteBits(buf, trunc + FLOAT_INT_BIAS, 5);
					MSG_WriteByte(buf, (char)((trunc + 0x1000) >> 5));
				}
				else
				{
					MSG_WriteBit1(buf);
					MSG_WriteLong(buf, *toF);
				}
			}
		}
		else if ( field->bits == -99 )
		{
			
			fullFloat = *(float *)toF;
			trunc = (int)fullFloat;
			
			if ( fullFloat == 0.0 )
			{
				MSG_WriteBit0(buf);
			}
			else
			{
				MSG_WriteBit1(buf);
				if ( ( (float)trunc == fullFloat && ( -1 < trunc + 0x200 )) && ( trunc + 0x200 < 0x400 ) )
				{
					MSG_WriteBit0(buf);
					MSG_WriteBits(buf, trunc + 0x200, 2);
					MSG_WriteByte(buf, (trunc + 0x200) >> 2);
				}
				else
				{
					MSG_WriteBit1(buf);
					MSG_WriteLong(buf, *toF);
				}
			}
		}
		else if ( field->bits == -100 )
		{
			if ( *toF == 0 )
			{
				MSG_WriteBit0(buf);
			}
			else
			{
				MSG_WriteBit1(buf);
				MSG_WriteAngle16(buf, *(float *)toF);
			}
		}
		else if ( *toF == 0 )
		{
			MSG_WriteBit0(buf);
		}
		else
		{
			MSG_WriteBit1(buf);
			absto = *toF;
			absbits = field->bits;
			if ( absbits < 0 )
				absbits *= -1;
			abs3bits = absbits & 7;
			if ( abs3bits )
			{
				MSG_WriteBits(buf, absto, abs3bits);
				absbits -= abs3bits;
				absto >>= abs3bits;
			}
			while( absbits )
			{
				MSG_WriteByte(buf, absto);
				absto >>= 8;
				absbits -= 8;
			}
		}
	}
}

void custom_MSG_WriteDeltaStruct(msg_t *msg, entityState_t *from, entityState_t *to, int force, int numFields, int indexBits, netField_t *stateFieldsPointer, int bChangeBit, int clientNum, int spectatorClientNum)
{
	int *toF;
	int *fromF;
	netField_t *field;
	int lc;
	int i;
	int attackerEntityNum = 0; // For playFxOnTagForPlayer
	int team; // For obituary
	vec3_t origin; 	// For obituary
	int max_distance; // For obituary

	if ( !to )
	{
		if ( bChangeBit )
			MSG_WriteBit1(msg);
		MSG_WriteBits(msg, from->number, indexBits);
		MSG_WriteBit1(msg);
	}
	else
	{
		lc = 0;
		field = stateFieldsPointer;

		for ( i = 0; i < numFields; i++, field++ )
		{
			fromF = ( int * )( (byte *)from + field->offset );
			toF = ( int * )( (byte *)to + field->offset );
			
			/* New code start: playFxOnTagForPlayer */
			if ( i == 49 )
				attackerEntityNum = *toF;
			/* New code end */
			
			/* New code start: obituary */
			if ( i == 17 ) // scale
				team = *toF;
			else if ( i == 58 ) // dmgFlags
				max_distance = *toF;
			else if ( i == 2 ) // pos.trBase[0]
				origin[0] = *(vec_t *)toF;
			else if ( i == 1 ) // pos.trBase[1]
				origin[1] = *(vec_t *)toF;
			else if ( i == 8 ) // pos.trBase[2]
				origin[2] = *(vec_t *)toF;
			/* New code end */
			
			if ( *fromF != *toF )
				lc = i + 1;
		}
		
		if ( !lc )
		{
			if ( force )
			{
				if ( bChangeBit )
					MSG_WriteBit1(msg);
				MSG_WriteBits(msg, to->number, indexBits);
				MSG_WriteBit0(msg);
				MSG_WriteBit0(msg);
			}
		}
		else
		{
			
			if ( bChangeBit )
				MSG_WriteBit1(msg);
			MSG_WriteBits(msg, to->number, indexBits);
			MSG_WriteBit0(msg);
			MSG_WriteBit1(msg);
			MSG_WriteByte(msg, lc);
			
			field = stateFieldsPointer;
			for ( i = 0; i < lc; i++, field++ )
			{
				/* New code start: playFxOnTagForPlayer */
				if ( numFields == 0x3b && ( i == 21 || i == 22 || i == 23 || i == 25 ) ) // entityStateFields: events[0], ..., events[3]
				{
					toF = ( int * )( (byte *)to + field->offset );
					if ( *toF == EV_PLAY_FX_ON_TAG && clientNum != attackerEntityNum - 1 && spectatorClientNum != attackerEntityNum - 1 )
						*toF = EV_NONE;
				}
				/* New code end */
				
				/* New code start: obituary */
				if ( numFields == 0x3b && i == 12 ) // entityStateFields: eType
				{
					toF = ( int * )( (byte *)to + field->offset );
					if ( ( *toF - 10 ) == EV_OBITUARY )
					{
						client_t *client;
						if ( spectatorClientNum != -1 )
							client = &svs.clients[spectatorClientNum];
						else
							client = &svs.clients[clientNum];

						int player_team = client->gentity->client->sess.team;
						if ( team )
						{
							if ( team != player_team && ( ( player_team == 1 && team == 6 ) || ( player_team == 2 && team == 5 ) || ( player_team == 3 && team == 4 ) ) )
								*toF = EV_NONE;
						}
						
						if ( max_distance > 0 )
						{
							double distance = Vec3DistanceSq(&client->gentity->r.currentOrigin, &origin);
							if ( (int)distance > ( max_distance * max_distance ) )
								*toF = EV_NONE;
						}
					}
				}
				/* New code end */
				
				custom_MSG_WriteDeltaField(msg, (byte *)from, (byte *)to, field);
			}
		}
	}
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
	int32_t absbits;
	uint8_t abs3bits;
	int32_t absto;
	int clipbits;
	int ammobits[7];
	int statsbits;

	if ( !from )
	{
		from = &dummy;
		memset(&dummy, 0, sizeof(dummy));
	}
	
	lc = 0;
	for ( i = 0, field = &playerStateFields; i < 0x69; i++, field++ )
	{
		fromF = ( int * )( (byte *)from + field->offset );
		toF = ( int * )( (byte *)to + field->offset );
		
		if ( *fromF != *toF )
			lc = i + 1;
	}
	
	MSG_WriteByte( msg, lc );
	
	field = &playerStateFields;
	for ( i = 0; i < lc; i++, field++ )
	{
		fromF = ( int * )( (byte *)from + field->offset );
		toF = ( int * )( (byte *)to + field->offset );
		
		if ( *fromF == *toF )
		{
			MSG_WriteBit0(msg);
		}
		else
		{
			MSG_WriteBit1(msg);
			
			if ( !field->bits )
			{
				fullFloat = *(float *)toF;
				trunc = (int)fullFloat;
				
				if ( trunc == fullFloat && trunc + FLOAT_INT_BIAS >= 0 && trunc + FLOAT_INT_BIAS < ( 1 << FLOAT_INT_BITS ) )
				{
					MSG_WriteBit0(msg);
					MSG_WriteBits(msg, trunc + FLOAT_INT_BIAS, 5);
					MSG_WriteByte(msg, (char)((trunc + 0x1000) >> 5));
				}
				else
				{
					MSG_WriteBit1(msg);
					MSG_WriteLong(msg, *toF);
				}
			}
			else
			{
				if ( field->bits == -100 )
				{
					if ( *toF == 0 )
					{
						MSG_WriteBit0(msg);
					}
					else
					{
						MSG_WriteBit1(msg);
						MSG_WriteAngle16(msg, *(float *)toF);
					}
				}
				else
				{
					absto = *toF;
					absbits = field->bits;
					if ( absbits < 0 )
						absbits *= -1;
					abs3bits = absbits & 7;
					if ( abs3bits )
					{
						MSG_WriteBits(msg, absto, abs3bits);
						absbits -= abs3bits;
						absto >>= abs3bits;
					}
					while( absbits )
					{
						MSG_WriteByte(msg, absto);
						absto >>= 8;
						absbits -= 8;
					}
				}
			}
		}
	}
	
	statsbits = 0;
	i = 0;
	while ( i < 6 )
	{
		if ( to->stats[i] != from->stats[i] )
			statsbits = statsbits | 1 << ((byte)i & 0x1f);
		i++;
	}
	if ( !statsbits )
	{
		MSG_WriteBit0(msg);
	}
	else
	{
		MSG_WriteBit1(msg);
		MSG_WriteBits(msg, statsbits, 6);
		if ( statsbits & 1U )
			MSG_WriteShort(msg, to->stats[0]);
		if ( statsbits & 2U )
			MSG_WriteShort(msg, to->stats[1]);
		if ( statsbits & 4U )
			MSG_WriteShort(msg, to->stats[2]);
		if ( statsbits & 8U )
			MSG_WriteBits(msg, to->stats[3],6);
		if ( statsbits & 0x10U )
			MSG_WriteShort(msg, to->stats[4]);
		if ( statsbits & 0x20U )
			MSG_WriteByte(msg, (char)to->stats[5]);
	}
	
	j = 0;
	while ( j < 4 )
	{
		ammobits[j] = 0;
		i = 0;
		while ( i < 0x10 )
		{
			if ( to->ammo[j * 0x10 + i] != from->ammo[j * 0x10 + i] )
				ammobits[j] = 1 << ((byte)i & 0x1f) | ammobits[j];
			i++;
		}
		j++;
	}
	if ( !ammobits[0] && !ammobits[1] && !ammobits[2] && !ammobits[3] )
	{
		MSG_WriteBit0(msg);
	}
	else
	{
		MSG_WriteBit1(msg);
		j = 0;
		while ( j < 4 )
		{
			if ( ammobits[j] == 0 )
			{
				MSG_WriteBit0(msg);
			}
			else
			{
				MSG_WriteBit1(msg);
				MSG_WriteShort(msg,ammobits[j]);
				i = 0;
				while ( i < 0x10 )
				{
					if ( (ammobits[j] >> ((byte)i & 0x1f) & 1U) != 0 )
						MSG_WriteShort(msg, to->ammo[j * 0x10 + i]);
					i++;
				}
			}
			j++;
		}
	}
	
	j = 0;
	while ( j < 4 )
	{
		clipbits = 0;
		i = 0;
		while ( i < 0x10 )
		{
			if ( to->ammoclip[j * 0x10 + i] != from->ammoclip[j * 0x10 + i] )
				clipbits = clipbits | 1 << ((byte)i & 0x1f);
			i++;
		}
		if ( !clipbits )
		{
			MSG_WriteBit0(msg);
		}
		else
		{
			MSG_WriteBit1(msg);
			MSG_WriteShort(msg,clipbits);
			i = 0;
			while ( i < 0x10 )
			{
				if ( clipbits >> ((byte)i & 0x1f) & 1U )
					MSG_WriteShort(msg, to->ammoclip[j * 0x10 + i]);
				i++;
			}
		}
		j++;
	}

	if ( !memcmp(from->objective, to->objective, sizeof(from->objective)) )
	{
		MSG_WriteBit0(msg);
	}
	else
	{
		MSG_WriteBit1(msg);
		i = 0;
		while ( i < 0x10 )
		{
			MSG_WriteBits(msg, to->objective[i].state, 3);
			MSG_WriteDeltaObjective(msg, &from->objective[i], &to->objective[i], 0, 6, &objectiveFields);
			i++;
		}
	}
	
	if ( !memcmp(&from->hud, &to->hud, sizeof(from->hud)) )
	{
		MSG_WriteBit0(msg);
	}
	else
	{
		MSG_WriteBit1(msg);
		MSG_WriteDeltaHudElems(msg, from->hud.archival, to->hud.archival, 0x1f);
		MSG_WriteDeltaHudElems(msg, from->hud.current, to->hud.current, 0x1f);
	}
}

void custom_MSG_WriteDeltaClient(msg_t *msg, clientState_t *from, clientState_t *to, qboolean force)
{
	clientState_t nullstate;

	if ( !from )
	{
		from = &nullstate;
		memset(&nullstate, 0, sizeof(nullstate));
	}

	custom_MSG_WriteDeltaStruct(msg, (entityState_t *)from, (entityState_t *)to, force, 0x16, 6, &clientStateFields, 1, -1, -1);
}

void custom_MSG_WriteDeltaArchivedEntity(msg_t *msg, archivedEntity_s *from, archivedEntity_s *to, int force)
{
	custom_MSG_WriteDeltaStruct(msg, (entityState_t *)from, (entityState_t *)to, force, 0x44, 10, &archivedEntityFields, 0, -1, -1);
}

void custom_MSG_WriteDeltaEntity(msg_t *msg, entityState_t *from, entityState_t *to, qboolean force, int clientNum)
{
	qboolean disable = qfalse;
	client_t *client = &svs.clients[clientNum];
	int spectatorClientNum = -1;
	
	if ( client->gentity && client->gentity->client )
		spectatorClientNum = client->gentity->client->spectatorClient;

	if ( to )
	{
		if ( (to->eType - 10) == EV_EARTHQUAKE )
		{
			if ( spectatorClientNum != -1 )
			{
				if ( !client->gentity->client->sess.archiveTime )
				{
					// Client spectates someone and is not in killcam
					if ( player_no_earthquakes[spectatorClientNum] )
						disable = qtrue;
				}
			}
			else
			{
				// Client plays normally
				if ( player_no_earthquakes[clientNum] )
					disable = qtrue;
			}
		}

		if ( (to->eType - 10) == EV_PLAY_FX || (to->eType - 10) == EV_EARTHQUAKE )
		{
			if ( to->otherEntityNum )
			{
				// Effect/Earthquake is restricted to a player
				if ( spectatorClientNum != -1 )
				{
					// Client spectates someone (includes killcam)
					if ( ! (to->otherEntityNum == (spectatorClientNum + 1)) )
						disable = qtrue;
				}
				else
				{
					// Client plays normally
					if ( ! (to->otherEntityNum == (clientNum + 1)) )
						disable = qtrue;
				}
			}
		}
	}

	if ( disable )
		to->eType = EV_NONE;

	custom_MSG_WriteDeltaStruct(msg, from, to, force, 0x3b, 10, &entityStateFields, 0, clientNum, spectatorClientNum);
}

void custom_SV_EmitPacketEntities(int client, int from_num_entities, int from_first_entity, int num_entities, int first_entity, msg_t *msg)
{
	int newnum;
	int oldnum;
	int newindex;
	int oldindex;
	entityState_t *newent;
	entityState_t *oldent;

	newent = NULL;
	oldent = NULL;
	newindex = 0;
	oldindex = 0;
	while ( newindex < num_entities || oldindex < from_num_entities )
	{
		if ( newindex < num_entities )
		{
			newent = &svs.snapshotEntities[(newindex + first_entity) % svs.numSnapshotEntities];
			newnum = newent->number;
		}
		else
		{
			newnum = 9999;
		}
		
		if ( oldindex < from_num_entities )
		{
			oldent = &svs.snapshotEntities[(oldindex + from_first_entity) % svs.numSnapshotEntities];
			oldnum = oldent->number;
		}
		else
		{
			oldnum = 9999;
		}
		
		if ( newnum == oldnum )
		{
			custom_MSG_WriteDeltaEntity(msg, oldent, newent, 0, client);
			oldindex++;
			newindex++;
		}
		else if ( newnum < oldnum )
		{
			custom_MSG_WriteDeltaEntity(msg, &sv.svEntities[newnum].baseline.s, newent, 1, client);
			newindex++;
		}
		else if ( oldnum < newnum )
		{
			custom_MSG_WriteDeltaEntity(msg, oldent, NULL, 1, client);
			oldindex++;
		}
	}
	MSG_WriteBits(msg, 0x3ff, 10);
}

void custom_SV_EmitPacketClients(int client, int from_num_clients, int from_first_client, int num_clients, int first_client, msg_t *msg)
{
	int newnum;
	int oldnum;
	int newindex;
	int oldindex;
	clientState_t *newcs;
	clientState_t *oldcs;

	newcs = NULL;
	oldcs = NULL;
	newindex = 0;
	oldindex = 0;
	while ( newindex < num_clients || oldindex < from_num_clients )
	{
		if ( newindex < num_clients )
		{
			newcs = &svs.snapshotClients[(newindex + first_client) % svs.numSnapshotClients];
			newnum = newcs->clientIndex;
		}
		else
		{
			newnum = 9999;
		}
		
		if ( oldindex < from_num_clients )
		{
			oldcs = &svs.snapshotClients[(oldindex + from_first_client) % svs.numSnapshotClients];
			oldnum = oldcs->clientIndex;
		}
		else
		{
			oldnum = 9999;
		}
		
		if ( newnum == oldnum )
		{
			custom_MSG_WriteDeltaClient(msg, oldcs, newcs, 0);
			oldindex++;
			newindex++;
		}
		else if ( newnum < oldnum )
		{
			custom_MSG_WriteDeltaClient(msg, NULL, newcs, 1);
			newindex++;
		}
		else if ( oldnum < newnum )
		{
			custom_MSG_WriteDeltaClient(msg, oldcs, NULL, 1);
			oldindex++;
		}
	}
	MSG_WriteBit0(msg);
}

void custom_SV_WriteSnapshotToClient(client_t *client, msg_t *msg)
{
	int from_first_client;
	int from_num_clients;
	int from_first_entity;
	int from_num_entities;
	int snapFlags;
	int i;
	int lastframe;
	clientSnapshot_t *oldframe;
	playerState_t *to;
	int frame_index;
	
	frame_index = client->netchan.outgoingSequence & PACKET_MASK;
	to = (playerState_t *)(client->frames + frame_index);

	if ( client->deltaMessage < 1 || client->state != CS_ACTIVE )
	{
		oldframe = NULL;
		lastframe = 0;
	}
	else if ( (client->netchan).outgoingSequence - client->deltaMessage < ( PACKET_BACKUP - 3 ) )
	{
		oldframe = &client->frames[ client->deltaMessage & PACKET_MASK ];
		lastframe = client->netchan.outgoingSequence - client->deltaMessage;
		if ( oldframe->first_entity < svs.nextSnapshotEntities - svs.numSnapshotEntities )
		{
			Com_DPrintf("%s: Delta request from out of date entities.\n", client->name);
			oldframe = NULL;
			lastframe = 0;
		}
	}
	else
	{
		Com_DPrintf("%s: (writing snapshot) Snapshot delta request from out of date packet.\n", client->name);
		oldframe = NULL;
		lastframe = 0;
	}
	
	MSG_WriteByte(msg, 6);
	MSG_WriteLong(msg, svs.time);
	MSG_WriteByte(msg, lastframe);
	snapFlags = svs.snapFlagServerBit;
	if ( client->rateDelayed != 0 )
		snapFlags = svs.snapFlagServerBit | 1;

	if ( client->state == CS_ACTIVE )
		client->delayed = 1;
	else if ( client->state != CS_ZOMBIE )
		client->delayed = 0;

	if ( client->delayed == 0 )
		snapFlags = snapFlags | 2;

	MSG_WriteByte(msg, snapFlags);
	if ( oldframe == NULL )
	{
		custom_MSG_WriteDeltaPlayerstate(msg, NULL, to);
		from_num_entities = 0;
		from_first_entity = 0;
		from_num_clients = 0;
		from_first_client = 0;
	}
	else
	{
		custom_MSG_WriteDeltaPlayerstate(msg, &oldframe->ps, to);
		from_num_entities = oldframe->num_entities;
		from_first_entity = oldframe->first_entity;
		from_num_clients = oldframe->num_clients;
		from_first_client = oldframe->first_client;
	}
	
	custom_SV_EmitPacketEntities(client - svs.clients, from_num_entities, from_first_entity, client->frames[frame_index].num_entities, client->frames[frame_index].first_entity, msg);
	custom_SV_EmitPacketClients(client - svs.clients, from_num_clients, from_first_client, client->frames[frame_index].num_clients, client->frames[frame_index].first_client, msg);
	
	for ( i = 0; i < sv_padPackets->integer; i++ )
		MSG_WriteByte(msg, 0);
}

void custom_SV_SendClientGameState(client_t *client)
{
	int				start;
	entityState_t	*base, nullstate;
	msg_t			msg;
	byte			*data;
	LargeLocal		buf;
	
	LargeLocalConstructor(&buf, MAX_MSGLEN);
	data = (byte *)LargeLocalGetBuf(&buf);
	while ( client->state != CS_FREE && client->netchan.unsentFragments )
		SV_Netchan_TransmitNextFragment(&client->netchan);
	
 	Com_DPrintf("SV_SendClientGameState() for %s\n", client->name);
	Com_DPrintf("Going from CS_CONNECTED to CS_PRIMED for %s\n", client->name);
	
	client->state = CS_PRIMED;
	client->pureAuthentic = 0;
	client->gamestateMessageNum = client->netchan.outgoingSequence;
	
	/* New code start */
	player_no_pickup[client - svs.clients] = 0;
	player_no_earthquakes[client - svs.clients] = 0;
	player_silent[client - svs.clients] = 0;
	player_g_speed[client - svs.clients] = 0;
	player_g_gravity[client - svs.clients] = 0;
	custom_animation[client - svs.clients] = 0;
	
	clientfps[client - svs.clients] = 0;
	clientframes[client - svs.clients] = 0;
	clientframetime[client - svs.clients] = 0;
	previousbuttons[client - svs.clients] = 0;
	
	#if COMPILE_BOTS == 1
		bot_buttons[client - svs.clients] = 0;
		bot_weapon[client - svs.clients] = 0;
		bot_forwardmove[client - svs.clients] = 0;
		bot_rightmove[client - svs.clients] = 0;
	#endif
	/* New code end */
	
	MSG_Init(&msg, data, MAX_MSGLEN);
	MSG_WriteLong(&msg, client->lastClientCommand);
	SV_UpdateServerCommandsToClient(client, &msg);
	MSG_WriteByte(&msg, svc_gamestate);
	MSG_WriteLong(&msg, client->reliableSequence);
	for ( start = 0; start < MAX_CONFIGSTRINGS; start++ )
	{
		if ( sv.configstrings[start][0] )
		{
			MSG_WriteByte(&msg, svc_configstring);
			MSG_WriteShort(&msg, start);
			MSG_WriteBigString(&msg, sv.configstrings[start]);
		}
	}
	memset(&nullstate, 0, sizeof(nullstate));
	for ( start = 0; start < MAX_GENTITIES; start++ )
	{
		base = &sv.svEntities[start].baseline.s;
		if ( !base->number )
			continue;
		MSG_WriteByte(&msg, svc_baseline);
		custom_MSG_WriteDeltaEntity(&msg, &nullstate, base, qtrue, client - svs.clients);
	}
	MSG_WriteByte(&msg, svc_EOF);
	MSG_WriteLong(&msg, client - svs.clients);
	MSG_WriteLong(&msg, sv.checksumFeed);
	MSG_WriteByte(&msg, svc_EOF);
	
	Com_DPrintf("Sending %i bytes in gamestate to client: %i\n", msg.cursize, client - svs.clients);
	
	gamestate_size[client - svs.clients] = int(msg.cursize); // New code
	
	SV_SendMessageToClient(&msg, client);
	LargeLocalDestructor(&buf);
}

int custom_SV_WWWRedirectClient(client_t *cl, msg_t *msg)
{
	long size;
	fileHandle_t fp;

	size = FS_SV_FOpenFileRead(cl->downloadName, &fp);
	if ( !size )
	{
		Com_Printf("ERROR: Client \'%s\'^7: couldn\'t extract file size for %s\n", cl->name, cl->downloadName);
	}
	else
	{
		FS_FCloseFile(fp);
		I_strncpyz(cl->wwwDownloadURL, custom_va("%s/%s", sv_wwwBaseURL->string, cl->downloadName), MAX_OSPATH);
		Com_Printf("Redirecting client \'%s\'^7 to %s\n", cl->name, cl->wwwDownloadURL);
		cl->wwwDownloadStarted = 1;
		MSG_WriteByte(msg, 5);
		MSG_WriteShort(msg, -1);
		MSG_WriteString(msg, cl->wwwDownloadURL);
		MSG_WriteLong(msg, size);
		MSG_WriteLong(msg, sv_wwwDlDisconnected->boolean != 0);

		/* New code start */
		if ( sv_wwwDlDisconnectedMessages->integer == 2 )
			SV_SendServerCommand(0, 0, "f \"%s^7 downloads %s\"", cl->name, cl->downloadName);
		/* New code end */

		cl->downloadName[0] = '\0';
	}
	return size != 0;
}

void custom_SV_WriteDownloadToClient(client_t *cl, msg_t *msg)
{
	int curindex;
	int iwdFile;
	char errorMessage[COD2_MAX_STRINGLENGTH];

	if ( cl->state == CS_ACTIVE )
		return;

	if ( !*cl->downloadName )
		return;

	if ( strlen(cl->downloadName) < 4 )
		return;

	if ( strcmp(&cl->downloadName[strlen(cl->downloadName) - 4], ".iwd") != 0 )
		return;

	#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
		if ( cl->wwwDlAck )
			return;
	#endif

	if ( strlen(sv_downloadMessage->string) )
	{
		Com_sprintf(errorMessage, sizeof(errorMessage), sv_downloadMessage->string);

		MSG_WriteByte( msg, svc_download );
		MSG_WriteShort( msg, 0 ); // Client is expecting block zero
		MSG_WriteLong( msg, -1 ); // Illegal file size
		MSG_WriteString( msg, errorMessage );

		*cl->downloadName = 0;
		return; // Download message instead of download
	}

	#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
		if ( sv_wwwDownload->boolean && cl->wwwDownload )
		{
			if ( !cl->wwwDl_failed )
			{
				custom_SV_WWWRedirectClient(cl, msg);
				return; // wwwDl redirect
			}
		}
	#endif

	// Hardcode client variables to make max download speed for everyone
	cl->state = CS_CONNECTED;
	cl->rate = 25000;
	cl->snapshotMsec = 50;

	if ( !cl->download )
	{
		// We open the file here
		Com_Printf("clientDownload: %d : begining \"%s\"\n", cl - svs.clients, cl->downloadName);

		iwdFile = FS_iwIwd(cl->downloadName, "main");

		if ( !sv_allowDownload->integer || iwdFile || ( cl->downloadSize = FS_SV_FOpenFileRead( cl->downloadName, &cl->download ) ) <= 0 )
		{
			// Cannot auto-download file
			if (iwdFile)
			{
				Com_Printf("clientDownload: %d : \"%s\" cannot download iwd files\n", cl - svs.clients, cl->downloadName);
				Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_CANTAUTODLGAMEIWD\x15%s", cl->downloadName);
			}
			else if ( !sv_allowDownload->boolean )
			{
				Com_Printf("clientDownload: %d : \"%s\" download disabled\n", cl - svs.clients, cl->downloadName);

				if ( sv_pure->boolean )
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
			MSG_WriteShort( msg, 0 ); // Client is expecting block zero
			MSG_WriteLong( msg, -1 ); // Illegal file size
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
	while ( cl->downloadCurrentBlock - cl->downloadClientBlock < MAX_DOWNLOAD_WINDOW && cl->downloadSize != cl->downloadCount )
	{
		curindex = (cl->downloadCurrentBlock % MAX_DOWNLOAD_WINDOW);

		if ( !cl->downloadBlocks[curindex] )
			cl->downloadBlocks[curindex] = (unsigned char *)Z_MallocInternal(MAX_DOWNLOAD_BLKSIZE);

		cl->downloadBlockSize[curindex] = FS_Read( cl->downloadBlocks[curindex], MAX_DOWNLOAD_BLKSIZE, cl->download );

		if ( cl->downloadBlockSize[curindex] < 0 )
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
	if ( cl->downloadCount == cl->downloadSize && !cl->downloadEOF && cl->downloadCurrentBlock - cl->downloadClientBlock < MAX_DOWNLOAD_WINDOW )
	{
		cl->downloadBlockSize[cl->downloadCurrentBlock % MAX_DOWNLOAD_WINDOW] = 0;
		cl->downloadCurrentBlock++;
		cl->downloadEOF = qtrue;  // We have added the EOF block
	}

	// Write out the next section of the file, if we have already reached our window,
	// automatically start retransmitting

	if ( cl->downloadClientBlock == cl->downloadCurrentBlock )
		return; // Nothing to transmit

	if ( cl->downloadXmitBlock == cl->downloadCurrentBlock )
	{
		// We have transmitted the complete window, should we start resending?

		// FIXME: This uses a hardcoded one second timeout for lost blocks
		// The timeout should be based on client rate somehow
		if ( svs.time - cl->downloadSendTime > 1000 )
			cl->downloadXmitBlock = cl->downloadClientBlock;
		else
			return;
	}

	// Send current block
	curindex = (cl->downloadXmitBlock % MAX_DOWNLOAD_WINDOW);

	MSG_WriteByte( msg, svc_download );
	MSG_WriteShort( msg, cl->downloadXmitBlock );

	// Block zero is special, contains file size
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
	if ( sv_pure->boolean )
		cl->pureAuthentic = 1;
}

void hook_SV_ResetPureClient_f(client_t *cl)
{
	cl->pureAuthentic = 0;

	if ( codecallback_vid_restart && Scr_IsSystemActive() )
	{	
		if ( cl->gentity == NULL )
			return;

		short ret = Scr_ExecEntThread(cl->gentity, codecallback_vid_restart, 0);
		Scr_FreeThread(ret);
	}
}

// Adds bot pings and removes spam on 1.2 and 1.3
void custom_SV_CalcPings(void)
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
		// Message times may be wrong across a changelevel
		if ( cl->lastPacketTime > svs.time )
			cl->lastPacketTime = svs.time;

		if ( cl->state == CS_ZOMBIE && cl->lastPacketTime < zombiepoint )
		{
			cl->state = CS_FREE; // Can now be reused
			continue;
		}

		if ( cl->state >= CS_CONNECTED && cl->lastPacketTime < droppoint )
		{
			// Wait several frames so a debugger session doesn't
			// cause a timeout
			if ( ++cl->timeoutCount > 5 )
			{
				SV_DropClient(cl, "EXE_TIMEDOUT");
				cl->state = CS_FREE; // Don't bother with zombie state
			}
		}
		else
		{
			cl->timeoutCount = 0;
		}
	}
}

#if COMPILE_BOTS == 1
void custom_SV_BotUserMove(client_t *client)
{
	int num;
	usercmd_t ucmd = {0};

	if ( client->gentity == NULL )
		return;

	num = client - svs.clients;
	ucmd.serverTime = svs.time;

	playerState_t *ps = SV_GameClientNum(num);
	gentity_t *ent = SV_GentityNum(num);

	if ( bot_weapon[num] )
		ucmd.weapon = (byte)(bot_weapon[num] & 0xFF);
	else
		ucmd.weapon = (byte)(ps->weapon & 0xFF);

	if ( ent->client == NULL )
		return;

	if ( ent->client->sess.archiveTime == 0 )
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
	if ( developer->integer == 2 )
		runtimeError(0, a1, a2, a3);
	else
		scriptError(a1, a2, a3, a4);
}

int touch_item_auto(gentity_t * item, gentity_t * entity, int touch)
{
	if ( player_no_pickup[entity->client->ps.clientNum] )
		return 0;
	
	hook_touch_item_auto->unhook();
	
	int (*sig)(gentity_t *, gentity_t *, int);
	*(int *)&sig = hook_touch_item_auto->from;

	int ret = sig(item, entity, touch);
	
	hook_touch_item_auto->hook();
	
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

	for ( int i = 0; tok != NULL; i++ )
	{
		if ( i == 1 )
			gamestate = atoi(tok);
		if ( i == 7 )
			clientnum = atoi(tok);
		tok = strtok(NULL, " ");
	}

	gamestate_size[clientnum] = gamestate;
}

int play_movement(client_t *cl, usercmd_t *ucmd)
{
	hook_play_movement->unhook();

	int (*sig)(client_t *cl, usercmd_t *ucmd);
	*(int *)&sig = hook_play_movement->from;

	int ret = sig(cl, ucmd);

	hook_play_movement->hook();

	int clientnum = cl - svs.clients;

	clientframes[clientnum]++;

	if ( Sys_MilliSeconds64() - clientframetime[clientnum] >= 1000 )
	{
		if (clientframes[clientnum] > 1000)
			clientframes[clientnum] = 1000;

		clientfps[clientnum] = clientframes[clientnum];
		clientframetime[clientnum] = Sys_MilliSeconds64();
		clientframes[clientnum] = 0;
	}
	
	if ( ucmd->buttons & KEY_MASK_FIRE && !(previousbuttons[clientnum] & KEY_MASK_FIRE) )
	{
		if ( codecallback_attackbutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_attackbutton, 0);
			Scr_FreeThread(ret);
		}
	}
	
	if ( ucmd->buttons & KEY_MASK_MELEE && !(previousbuttons[clientnum] & KEY_MASK_MELEE) )
	{
		if ( codecallback_meleebutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_meleebutton, 0);
			Scr_FreeThread(ret);
		}
	}

	if ( ucmd->buttons & KEY_MASK_USE && !(previousbuttons[clientnum] & KEY_MASK_USE) )
	{
		if ( codecallback_usebutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_usebutton, 0);
			Scr_FreeThread(ret);
		}
	}

	if ( ucmd->buttons & KEY_MASK_RELOAD && !(previousbuttons[clientnum] & KEY_MASK_RELOAD) )
	{
		if( codecallback_reloadbutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_reloadbutton, 0);
			Scr_FreeThread(ret);
		}
	}
	
	if ( ucmd->buttons & KEY_MASK_LEANLEFT && !(previousbuttons[clientnum] & KEY_MASK_LEANLEFT) )
	{
		if ( codecallback_leanleftbutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_leanleftbutton, 0);
			Scr_FreeThread(ret);
		}
	}
	
	if ( ucmd->buttons & KEY_MASK_LEANRIGHT && !(previousbuttons[clientnum] & KEY_MASK_LEANRIGHT) )
	{
		if ( codecallback_leanrightbutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_leanrightbutton, 0);
			Scr_FreeThread(ret);
		}
	}
	
	if ( ucmd->buttons & KEY_MASK_PRONE && !(previousbuttons[clientnum] & KEY_MASK_PRONE) )
	{
		if ( codecallback_pronebutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_pronebutton, 0);
			Scr_FreeThread(ret);
		}
	}
	
	if ( ucmd->buttons & KEY_MASK_CROUCH && !(previousbuttons[clientnum] & KEY_MASK_CROUCH) )
	{
		if ( codecallback_crouchbutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_crouchbutton, 0);
			Scr_FreeThread(ret);
		}
	}
	
	if ( ucmd->buttons & KEY_MASK_JUMP && !(previousbuttons[clientnum] & KEY_MASK_JUMP) )
	{
		if ( codecallback_jumpbutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_jumpbutton, 0);
			Scr_FreeThread(ret);
		}
	}
	
	if ( ucmd->buttons & KEY_MASK_ADS_MODE && !(previousbuttons[clientnum] & KEY_MASK_ADS_MODE) )
	{
		if ( codecallback_adsbutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_adsbutton, 0);
			Scr_FreeThread(ret);
		}
	}
	
	if ( ucmd->buttons & KEY_MASK_MELEE_BREATH && !(previousbuttons[clientnum] & KEY_MASK_MELEE_BREATH) )
	{
		if ( codecallback_meleebreathbutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_meleebreathbutton, 0);
			Scr_FreeThread(ret);
		}
	}
	
	if ( ucmd->buttons & KEY_MASK_HOLDBREATH && !(previousbuttons[clientnum] & KEY_MASK_HOLDBREATH) )
	{
		if ( codecallback_holdbreathbutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_holdbreathbutton, 0);
			Scr_FreeThread(ret);
		}
	}
	
	if ( ucmd->buttons & KEY_MASK_FRAG && !(previousbuttons[clientnum] & KEY_MASK_FRAG) )
	{
		if ( codecallback_fragbutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_fragbutton, 0);
			Scr_FreeThread(ret);
		}
	}
	
	if ( ucmd->buttons & KEY_MASK_SMOKE && !(previousbuttons[clientnum] & KEY_MASK_SMOKE) )
	{
		if ( codecallback_smokebutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_smokebutton, 0);
			Scr_FreeThread(ret);
		}
	}

	previousbuttons[clientnum] = ucmd->buttons;
	return ret;
}

int play_endframe(gentity_t *ent)
{
	hook_play_endframe->unhook();

	int (*sig)(gentity_t *ent);
	*(int *)&sig = hook_play_endframe->from;

	int ret = sig(ent);

	hook_play_endframe->hook();

	if ( ent->client->sess.state == STATE_PLAYING )
	{
		int num = ent - g_entities;

		if ( player_g_speed[num] > 0 )
			ent->client->ps.speed = player_g_speed[num];

		if ( player_g_gravity[num] > 0 )
			ent->client->ps.gravity = player_g_gravity[num];

	}

	return ret;
}

int set_anim(playerState_t *ps, int animNum, animBodyPart_t bodyPart, int forceDuration, qboolean setTimer, qboolean isContinue, qboolean force)
{
	hook_set_anim->unhook();

	int (*sig)(playerState_t *ps, int animNum, animBodyPart_t bodyPart, int forceDuration, qboolean setTimer, qboolean isContinue, qboolean force);
	*(int *)&sig = hook_set_anim->from;

	int ret;

	if ( !custom_animation[ps->clientNum] )
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
	int now = Sys_MilliSeconds();

	for ( bucket = bucketHashes[ hash ]; bucket; bucket = bucket->next )
	{
		if ( memcmp( bucket->adr, address.ip, 4 ) == 0 )
			return bucket;
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
				bucket->prev->next = bucket->next;
			else
				bucketHashes[ bucket->hash ] = bucket->next;

			if ( bucket->next != NULL )
				bucket->next->prev = bucket->prev;

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
				bucketHashes[ hash ]->prev = bucket;

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
		int now = Sys_MilliSeconds();
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
	if ( codecallback_client_spam && Scr_IsSystemActive() )
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
		if ( !SVC_callback("RCON:ADDRESS", NET_AdrToString(from)) )
			Com_DPrintf("SVC_RemoteCommand: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
		return true;
	}

	if ( badRconPassword )
	{
		static leakyBucket_t bucket;

		// Make DoS via rcon impractical
		if ( SVC_RateLimit( &bucket, 10, 1000 ) )
		{
			if ( !SVC_callback("RCON:GLOBAL", NET_AdrToString(from)) )
				Com_DPrintf("SVC_RemoteCommand: rate limit exceeded, dropping request\n");
			return true;
		}
	}
	
	return false;
}

void hook_SVC_RemoteCommand(netadr_t from, msg_t *msg)
{
	if ( !sv_allowRcon->boolean )
		return;
	
	bool badRconPassword = !strlen( rcon_password->string ) || strcmp(Cmd_Argv(1), rcon_password->string) != 0;
	
	if ( !sv_limitLocalRcon->boolean )
	{
		unsigned char *ip = from.ip;
	
		if ( !(ip[0] == 10 ||									// 10.0.0.0 – 10.255.255.255
			  (ip[0] == 172 && (ip[1] >= 16 && ip[1] <= 31)) || // 172.16.0.0 – 172.31.255.255
			  (ip[0] == 192 && ip[1] == 168)) )				    // 192.168.0.0 – 192.168.255.255
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
	
	#if COD_VERSION == COD2_1_0
		int rcon_from_string_offset = 0x0; // Not tested
	#elif COD_VERSION == COD2_1_2
		int rcon_from_string_offset = 0x0; // Not tested
	#elif COD_VERSION == COD2_1_3
		int rcon_from_string_offset = 0x0814bC61;
	#endif
	
	if ( logRcon && !sv_logRcon->boolean )
	{
		byte disable = 0;
		memcpy((void *)rcon_from_string_offset, &disable, 1);
		logRcon = qfalse;
		Com_DPrintf("Disabled rcon messages logging\n");
	}
	else if ( !logRcon && sv_logRcon->boolean )
	{
		byte enable = 0x52; // "R"
		memcpy((void *)rcon_from_string_offset, &enable, 1);
		logRcon = qtrue;
		Com_DPrintf("Enabled rcon messages logging\n");
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
		stackPushInt((int)msg);
		stackPushArray();
		int args = Cmd_Argc();
		for (int i = 2; i < args; i++)
		{
			char tmp[COD2_MAX_STRINGLENGTH];
			SV_Cmd_ArgvBuffer(i, tmp, sizeof(tmp));
			stackPushString(tmp);
			stackPushArrayLast();
		}
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
		if ( !SVC_callback("CHALLENGE:ADDRESS", NET_AdrToString(from)) )
			Com_DPrintf("SV_GetChallenge: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
		return;
	}

	// Allow getchallenge to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit( &outboundLeakyBucket, 10, 100 ) )
	{
		if ( !SVC_callback("CHALLENGE:GLOBAL", NET_AdrToString(from)) )
			Com_DPrintf("SV_GetChallenge: rate limit exceeded, dropping request\n");
		return;
	}

	SV_GetChallenge(from);
}

void hook_SV_DirectConnect(netadr_t from)
{
	// Prevent using connect as an amplifier
	if ( SVC_RateLimitAddress( from, 10, 1000 ) )
	{
		Com_DPrintf( "SV_DirectConnect: rate limit from %s exceeded, dropping request\n", NET_AdrToString( from ) );
		return;
	}

	// Allow connect to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit( &outboundLeakyBucket, 10, 100 ) )
	{
		Com_DPrintf( "SV_DirectConnect: rate limit exceeded, dropping request\n" );
		return;
	}

	SV_DirectConnect(from);
}

void hook_SVC_Info(netadr_t from)
{
	// Prevent using getinfo as an amplifier
	if ( SVC_RateLimitAddress( from, 10, 1000 ) )
	{
		if ( !SVC_callback("INFO:ADDRESS", NET_AdrToString(from)) )
			Com_DPrintf("SVC_Info: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
		return;
	}

	// Allow getinfo to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit( &outboundLeakyBucket, 10, 100 ) )
	{
		if ( !SVC_callback("INFO:GLOBAL", NET_AdrToString(from)) )
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
		if ( !SVC_callback("STATUS:ADDRESS", NET_AdrToString(from)) )
			Com_DPrintf("SVC_Status: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
		return;
	}

	// Allow getstatus to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit( &outboundLeakyBucket, 10, 100 ) )
	{
		if ( !SVC_callback("STATUS:GLOBAL", NET_AdrToString(from)) )
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

	if ( strlen(fs_library->string) )
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

	for ( int i = 0; i < int( sizeof(stock_maps) / sizeof(stock_maps[0]) ); i++ )
	{
		if ( strcmp(map->string, stock_maps[i]) == 0 )
		{
			from_stock_map = true;
			break;
		}
	}

	for ( int i = 0; i < int( sizeof(stock_maps) / sizeof(stock_maps[0]) ); i++ )
	{
		if ( strcmp(mapname, stock_maps[i]) == 0 )
		{
			map_found = true;

			if ( from_stock_map ) // When changing from stock map to stock map do not trigger manymap
				return;
			else
				break;
		}
	}

	int map_exists = access(map_check, F_OK) != -1;

	if ( !map_exists && !map_found )
		return;

	DIR *dir;
	struct dirent *dir_ent;

	dir = opendir(library_path);

	if ( !dir )
		return;

	while ( (dir_ent = readdir(dir)) != NULL )
	{
		if ( strcmp(dir_ent->d_name, ".") == 0 || strcmp(dir_ent->d_name, "..") == 0 )
			continue;

		char fileDelete[512];
		snprintf(fileDelete, sizeof(fileDelete), "%s/%s/%s", fs_homepath->string, fs_game->string, dir_ent->d_name);

		if ( access(fileDelete, F_OK) != -1 )
		{
			int unlink_success = unlink(fileDelete) == 0;
			printf("manymaps> REMOVED OLD LINK: %s result of unlink: %s\n", fileDelete, unlink_success?"success":"failed");
		}
	}

	closedir(dir);

	if ( map_exists )
	{
		char src[512], dst[512];

		snprintf(src, sizeof(src), "%s/%s.iwd", library_path, mapname);
		snprintf(dst, sizeof(dst), "%s/%s/%s.iwd", fs_homepath->string, fs_game->string, mapname);

		if ( access(src, F_OK) != -1 )
		{
			int link_success = symlink(src, dst) == 0;
			printf("manymaps> NEW LINK: src=%s dst=%s result of link: %s\n", src, dst, link_success?"success":"failed");

			if ( link_success && read == -1 ) // FS_LoadDir is needed when empty.iwd is missing (then .d3dbsp isn't referenced anywhere)
				FS_LoadDir(fs_homepath->string, fs_game->string);
		}
	}
}

int hook_findMap(const char *qpath, void **buffer)
{
	int read = FS_ReadFile(qpath, buffer);
	manymaps_prepare(Cmd_Argv(1), read);

	if ( read != -1 )
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
	
	if( !developer->integer )
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
	
	if( !developer->integer )
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
	
	if( !developer->integer )
		vars->developer = 0;
	
	hook_print_codepos->hook();
}

void hook_Sys_Print(const char *msg)
{
	if ( Scr_IsSystemActive() && con_coloredPrints->boolean )
	{
		Sys_AnsiColorPrint(msg);
	}
	else
	{
		hook_console_print->unhook();
		Sys_Print(msg);
		hook_console_print->hook();
	}
}

void hook_Com_DPrintf(const char *format, ...)
{
	char s[COD2_MAX_STRINGLENGTH];
	va_list va;

	va_start(va, format);
	vsnprintf(s, sizeof(s), format, va);
	va_end(va);
	
	if ( codecallback_dprintf && Scr_IsSystemActive() )
	{
		stackPushString(s);
		short ret = Scr_ExecThread(codecallback_dprintf, 1);
		Scr_FreeThread(ret);
	
		return;
	}

	if ( !developer->integer )
		return;
	
	Com_Printf("%s", s);
}

void Scr_CodeCallback_Error(qboolean terminal, qboolean emit, const char * internal_function, char *message)
{
	if ( codecallback_error && Scr_IsSystemActive() )
	{
		if ( terminal || emit )
		{
			stackPushString(message);
			stackPushString(internal_function);
			stackPushInt(terminal);
			short ret = Scr_ExecThread(codecallback_error, 3);
			Scr_FreeThread(ret);
		}
		else
		{
			/* If the error is non-critical (not stopping the server), save it
			   so we can emit it later at G_RunFrame which is a rather save
			   spot compared to if we emit it directly here within the
			   internals of the scripting engine where we risk crashing it
			   with a segmentation fault */
			if ( scr_errors_index < MAX_ERROR_BUFFER )
			{
				strncpy(scr_errors[scr_errors_index].internal_function, internal_function, sizeof(scr_errors[scr_errors_index].internal_function));
				strncpy(scr_errors[scr_errors_index].message, message, sizeof(scr_errors[scr_errors_index].message));
				scr_errors_index++;
			}
			else
			{
				printf("Warning: errors buffer full, not calling CodeCallback_Error for '%s'\n", message);
			}
		}
	}
}

void custom_Com_Error(int type, const char *format, ...)
{
	Sys_EnterCriticalSectionInternal(2);
	
	#if COD_VERSION == COD2_1_0 // Not tested
		int com_errorEntered = 0x0;
		int va_string_156 = 0x0;
		int unk1 = 0x0;
		int unk2 = 0x0;
		int com_fixedConsolePosition = 0x0;
	#elif COD_VERSION == COD2_1_2 // Not tested
		int com_errorEntered = 0x0;
		int va_string_156 = 0x0;
		int unk1 = 0x0;
		int unk2 = 0x0;
		int com_fixedConsolePosition = 0x0;
	#elif COD_VERSION == COD2_1_3
		int com_errorEntered = 0x081A21C0;
		int va_string_156 = 0x081A2280;
		int unk1 = 0x081A327F;
		int unk2 = 0x081A2264;
		int com_fixedConsolePosition = 0x081A21C4;
	#endif
	
	if ( *(int *)com_errorEntered )
	{
		Sys_Error("recursive error after: %s", *(char *)va_string_156);
	}
	*(int *)com_errorEntered = 1;
	
	va_list va;
	va_start(va, format);
	vsnprintf((char *)va_string_156, 0x1000, format, va);
	va_end(va);
	
	Scr_CodeCallback_Error(scrVmPub.terminal_error, qtrue, "Com_Error", (char *)va_string_156);
	
	*(byte *)unk1 = 0;
	
	if ( type != 1 )
	{
		if ( type == 4 || type == 6 )
			type = 1;
		else if ( type == 5 )
			type = 1;
		else
			*(int *)com_fixedConsolePosition = 0;
	}
	
	*(int *)unk2 = type;
	
	Sys_LeaveCriticalSectionInternal(2);

	longjmp((__jmp_buf_tag*)Sys_GetValue(2), -1);
}

void custom_Scr_ErrorInternal(void)
{
	if ( !scrVarPub.evaluate && !scrCompilePub.script_loading )
	{
		if ( scrVarPub.developer && scrVmGlob.loading )
		{
			scrVmPub.terminal_error = 1;
		}
		if ( scrVmPub.function_count || scrVmPub.debugCode )
		{
			Scr_CodeCallback_Error(qfalse, qfalse, "Scr_ErrorInternal", (char *)scrVarPub.error_message);
			longjmp(&g_script_error[g_script_error_level], -1);
		}
	}
	else if ( !scrVmPub.terminal_error )
	{
		return;
	}
	custom_Com_Error(1, "\x15%s", scrVarPub.error_message);
}

void custom_RuntimeError_Debug(int channel, char *pos, int index, char *message)
{
	int i, j;

	Scr_CodeCallback_Error(qfalse, qfalse, "RuntimeError_Debug", message);
	Com_PrintMessage(channel, custom_va("\n******* script runtime error *******\n%s: ", message));
	custom_Scr_PrintPrevCodePos(channel, pos, index);
	i = scrVmPub.function_count;
	if ( scrVmPub.function_count )
	{
		while ( j = i - 1, 0 < j )
		{
			Com_PrintMessage(channel, "called from:\n");
			custom_Scr_PrintPrevCodePos(channel, (char *)scrVmPub.function_frame_start[i - 1].fs.pos, scrVmPub.function_frame_start[i - 1].fs.localId == 0);
			i = j;
		}
		Com_PrintMessage(channel, "started from:\n");
		custom_Scr_PrintPrevCodePos(channel, (char *)scrVmPub.function_frame_start[0].fs.pos, 1);
	}
	Com_PrintMessage(channel, "************************************\n");
}

void custom_RuntimeError(char *pos, int index, char *message, char *param_4)
{
	int channel_1, channel_2;
	char *local_14;
	char *local_10;
	qboolean terminate;

	if ( scrVarPub.developer || scrVmPub.terminal_error )
	{
		if ( !scrVmPub.debugCode )
		{
			terminate = false;
			if ( scrVmPub.abort_on_error || scrVmPub.terminal_error )
				terminate = true;
			
			if ( terminate )
				channel_2 = 0;
			else
				channel_2 = 4;
			
			custom_RuntimeError_Debug(channel_2, pos, index, message);
			if ( !terminate )
				return;
		}
		else
		{
			Com_Printf("%s\n", message);
			if ( !scrVmPub.terminal_error )
			{
				Scr_CodeCallback_Error(qfalse, qfalse, "RuntimeError", message);
				return;
			}
		}
		if ( !param_4 )
		{
			local_10 = (char *)"";
			local_14 = (char *)"";
			param_4 = local_10;
		}
		else
		{
			local_14 = (char *)"\n";
		}
		if ( !scrVmPub.terminal_error )
			channel_1 = 4;
		else
			channel_1 = 5;
		
		custom_Com_Error(channel_1, "\x15script runtime error\n(see console for details)\n%s%s%s", message, local_14, param_4);
	}
}

void tryFindObject(VariableValue *arg, unsigned int constString)
{
	// Check if our object reference is still valid
	unsigned int objectId = arg->u.pointerValue;
	unsigned int varId = FindVariable(objectId, 0x1fffe);
	if ( varId )
	{
		objectId = FindObject(varId);
		if ( objectId )
			stackPushObject(arg->u.pointerValue);
		else
			stackPushUndefined();
	}
	else
	{
		// If not, try to regain the object from the game entities
		gentity_t *ent;
		for ( int i = 0; i < MAX_GENTITIES; i++ )
		{
			ent = &g_entities[i];
			if ( ent )
			{
				varId = Scr_GetEntityId(i, 0);
				if ( varId && varId == objectId )
				{
					stackPushEntity(&g_entities[i]);
					return;
				}
			}
		}
		printf("Warning: notify param for '%s' with unknown object reference %d\n", SL_ConvertToString(constString), objectId);
		stackPushUndefined();
	}
}

void Scr_CodeCallback_Notify(unsigned int entId, unsigned int constString, unsigned int argc, VariableValue *arguments)
{
	if ( !level.num_entities )
	{
		// Avoid crash in SV_FreeClientScriptPers logic on map switch where Scr_IsSystemActive still returns true
		return;
	}

	if ( Scr_IsSystemActive() )
	{
		unsigned int varId, objectId;

		// Check if our object reference is still valid
		varId = FindVariable(entId, 0x1fffe);
  		if ( varId ) {
    		objectId = FindObject(varId);
			if ( !objectId )
				return;
		} else {
			return;
		}

		if ( !argc || !arguments || !arguments->type || arguments->type == STACK_PRECODEPOS )
		{
			stackPushUndefined();
		}
		else
		{
			stackPushArray();
			for ( unsigned int i = 0; i < argc; i++ )
			{
				VariableValue *arg = arguments + i;
				switch(arg->type) {
					case STACK_UNDEFINED: stackPushUndefined(); break;
					case STACK_OBJECT: tryFindObject(arg, constString); break;
					case STACK_STRING:
					case STACK_LOCALIZED_STRING: stackPushString(SL_ConvertToString(arg->u.stringValue)); break;
					case STACK_VECTOR: stackPushVector((vec_t*)arg->u.vectorValue); break;
					case STACK_FLOAT: stackPushFloat(arg->u.floatValue); break;
					case STACK_INT: stackPushInt(arg->u.intValue); break;
					case STACK_FUNCTION: stackPushFunc(arg->u.codePosValue); break;
					default: printf("Warning: notify with param %d of type 0x%x is currently not supported for CodeCallback_Notify\n", i + 1, arg->type); stackPushUndefined();
				}
				stackPushArrayLast();
			}
		}
		stackPushString(SL_ConvertToString(constString));
		stackPushObject(entId);
		short ret = Scr_ExecThread(codecallback_notify, 3);
		Scr_FreeThread(ret);
	}
}

void custom_G_RunFrame(int levelTime)
{
	int i;
	
	hook_g_runframe->unhook();

	void (*sig)(int levelTime);
	*(int *)&sig = hook_g_runframe->from;
	
	for ( i = 0; i < scr_errors_index; i++ )
		Scr_CodeCallback_Error(qfalse, qtrue, scr_errors[i].internal_function, scr_errors[i].message);
	
	scr_errors_index = 0;

	for ( i = 0; i < scr_notify_index; i++ )
		Scr_CodeCallback_Notify(scr_notify[i].entId, scr_notify[i].constString, scr_notify[i].argc, &scr_notify[i].arguments[0]);
	
	scr_notify_index = 0;
	
	sig(levelTime);
	
	hook_g_runframe->hook();
}

void custom_SV_ArchiveSnapshot(void)
{
	byte *data;
	LargeLocal buf;
	msg_t msg;
	int i, j, k, l, m, n, x;
	int nextCachedSnapClients;
	int nextArchSnapBuffer;
	int nextCachedSnapEnts;
	int nextCachedSnapFrames;
	int cachedFrameIndex2;
	int cachedFrameIndex;
	int freeBytes;
	int index;
	int clientNum;
	archivedEntity_t *archEnt;
	archivedEntity_t to;
	cachedClient_t *cachedClient1;
	cachedClient_t *cachedClient2;
	cachedSnapshot_t *cachedFrame;
	archivedSnapshot_t *archSnap;
	playerState_t ps;
	client_t *client;
	svEntity_t *svEnt;
	gentity_t *gent;

	LargeLocalConstructor(&buf, MAX_MSGLEN);
	data = (byte *)LargeLocalGetBuf(&buf);
	if ( sv.state == SS_GAME )
	{
		if ( !svs.archivedSnapshotEnabled )
		{
			LargeLocalDestructor(&buf);
		}
		else
		{
			MSG_Init(&msg, data, MAX_MSGLEN);
			n = svs.nextCachedSnapshotFrames - 512;
			if ( n < 0 )
			{
				n = 0;
			}
			i = svs.nextCachedSnapshotFrames;
			do {
				m = i - 1;
				if ( m < n ) goto LAB_0809b5f4;
				cachedFrameIndex = m;
				if ( m < 0 )
					cachedFrameIndex = i + 0x1fe;
				cachedFrame = svs.cachedSnapshotFrames + m + (cachedFrameIndex >> 9) * -0x200;
				i = m;
			} while ( cachedFrame->archivedFrame < svs.nextArchivedSnapshotFrames - sv_fps->integer || cachedFrame->usesDelta );
			
			if ( cachedFrame->first_entity < svs.nextCachedSnapshotEntities - 0x4000 || cachedFrame->first_client < svs.nextCachedSnapshotClients - 0x1000 )
			{
LAB_0809b5f4:
				MSG_WriteBit1(&msg);
				MSG_WriteLong(&msg, svs.time);
				
				nextCachedSnapFrames = svs.nextCachedSnapshotFrames;
				if ( svs.nextCachedSnapshotFrames < 0 )
					nextCachedSnapFrames = svs.nextCachedSnapshotFrames + 0x1ff;
				cachedFrame = svs.cachedSnapshotFrames + svs.nextCachedSnapshotFrames + (nextCachedSnapFrames >> 9) * -0x200;
				cachedFrame->archivedFrame = svs.nextArchivedSnapshotFrames;
				cachedFrame->num_entities = 0;
				cachedFrame->first_entity = svs.nextCachedSnapshotEntities;
				cachedFrame->num_clients = 0;
				cachedFrame->first_client = svs.nextCachedSnapshotClients;
				cachedFrame->usesDelta = 0;
				cachedFrame->time = svs.time;
				client = svs.clients;
				for ( m = 0; m < sv_maxclients->integer; m++, client++ )
				{
					if ( CS_ZOMBIE < client->state )
					{
						nextCachedSnapClients = svs.nextCachedSnapshotClients;
						if ( svs.nextCachedSnapshotClients < 0 )
							nextCachedSnapClients = svs.nextCachedSnapshotClients + 0xfff;
						cachedClient1 = svs.cachedSnapshotClients + svs.nextCachedSnapshotClients + (nextCachedSnapClients >> 0xc) * -0x1000;
						cachedClient2 = cachedClient1;
						
						memcpy(&cachedClient1->cs, G_GetClientState(m), 0x5c);
						custom_MSG_WriteDeltaClient(&msg, NULL, &cachedClient2->cs, 1);
						cachedClient1 = cachedClient2;
						i = GetFollowPlayerState(m, &cachedClient2->ps);
						cachedClient1->playerStateExists = i;
						
						if ( !cachedClient2->playerStateExists )
						{
							MSG_WriteBit0(&msg);
						}
						else
						{
							MSG_WriteBit1(&msg);
							custom_MSG_WriteDeltaPlayerstate(&msg, NULL, &cachedClient2->ps);
						}
						i = svs.nextCachedSnapshotClients;
						svs.nextCachedSnapshotClients++;
						if ( i != 0x7ffffffc && 0x7ffffffc < svs.nextCachedSnapshotClients )
							custom_Com_Error(0, "\x15svs.nextCachedSnapshotClients wrapped");
						cachedFrame->num_clients++;
					}
				}
				MSG_WriteBit0(&msg);
				for ( k = 0; i = svs.nextCachedSnapshotFrames, k < sv.num_entities; k++ )
				{
					gent = SV_GentityNum(k);
					if ( gent->r.linked &&
					(( gent->r.broadcastTime || (((gent->r.svFlags & 1) == 0 && ((svEnt = SV_SvEntityForGentity(gent), (gent->r.svFlags & 0x18) || svEnt->numClusters )))))))
					{
						nextCachedSnapEnts = svs.nextCachedSnapshotEntities;
						if (svs.nextCachedSnapshotEntities < 0)
							nextCachedSnapEnts = svs.nextCachedSnapshotEntities + 0x3fff;
						archEnt = svs.cachedSnapshotEntities + svs.nextCachedSnapshotEntities + (nextCachedSnapEnts >> 0xe) * -0x4000;
						memcpy(archEnt, gent, 0xF0);
						archEnt->r.svFlags = gent->r.svFlags;
						if ( gent->r.broadcastTime )
							archEnt->r.svFlags = archEnt->r.svFlags | 8;
						archEnt->r.clientMask[0] = gent->r.clientMask[0];
						archEnt->r.clientMask[1] = gent->r.clientMask[1];
						VectorCopy(gent->r.absmin, archEnt->r.absmin);
						VectorCopy(gent->r.absmax, archEnt->r.absmax);
						
						/* New code start: setEarthquakes */
						if ( (archEnt->s.eType - 10) == EV_EARTHQUAKE )
						{
							for ( l = 0; l < sv_maxclients->integer; l++ )
							{
								if ( player_no_earthquakes[l] )
								{
									if ( l > 31 )
										archEnt->r.clientMask[1] |= 1 << (l - 32);
									else
										archEnt->r.clientMask[0] |= 1 << l;
								}
							}
						}
						/* New code end */
						
						custom_MSG_WriteDeltaArchivedEntity(&msg, &sv.svEntities[gent->s.number].baseline, archEnt, 1);
						i = svs.nextCachedSnapshotEntities;
						svs.nextCachedSnapshotEntities++;
						if ( i != 0x7ffffffc && 0x7ffffffc < svs.nextCachedSnapshotEntities )
							custom_Com_Error(0, "\x15svs.nextCachedSnapshotEntities wrapped");
						cachedFrame->num_entities++;
					}
				}
				svs.nextCachedSnapshotFrames++;
				if ( i != 0x7ffffffc && 0x7ffffffc < svs.nextCachedSnapshotFrames )
					custom_Com_Error(0, "\x15svs.nextCachedSnapshotFrames wrapped");
			}
			else
			{
				MSG_WriteBit0(&msg);
				MSG_WriteLong(&msg, cachedFrame->archivedFrame);
				MSG_WriteLong(&msg, svs.time);
				cachedClient2 = NULL;
				clientNum = 0;
				j = 0;
				while ( clientNum < sv_maxclients->integer || j < cachedFrame->num_clients )
				{
					if ( clientNum < sv_maxclients->integer && svs.clients[clientNum].state < CS_CONNECTED )
					{
						clientNum++;
					}
					else
					{
						if ( j < cachedFrame->num_clients )
						{
							i = cachedFrame->first_client + j;
							cachedFrameIndex2 = i;
							if ( i < 0 )
								cachedFrameIndex2 = i + 0xfff;
							cachedClient2 = svs.cachedSnapshotClients + i + (cachedFrameIndex2 >> 0xc) * -0x1000;
							x = cachedClient2->cs.clientIndex;
						}
						else
						{
							x = 9999;
						}
						if ( clientNum == x )
						{
							custom_MSG_WriteDeltaClient(&msg, &cachedClient2->cs, G_GetClientState(clientNum), 1);
							i = GetFollowPlayerState(clientNum, &ps);
							if ( !i )
							{
								MSG_WriteBit0(&msg);
							}
							else
							{
								MSG_WriteBit1(&msg);
								custom_MSG_WriteDeltaPlayerstate(&msg, &cachedClient2->ps, &ps);
							}
							j++;
							clientNum++;
						}
						else if ( clientNum < x )
						{
							custom_MSG_WriteDeltaClient(&msg, NULL, G_GetClientState(clientNum), 1);
							i = GetFollowPlayerState(clientNum, &ps);
							if ( !i )
							{
								MSG_WriteBit0(&msg);
							}
							else
							{
								MSG_WriteBit1(&msg);
								custom_MSG_WriteDeltaPlayerstate(&msg, NULL, &ps);
							}
							clientNum++;
						}
						else if ( x < clientNum )
						{
							j++;
						}
					}
				}
				MSG_WriteBit0(&msg);
				for ( k = 0; k < sv.num_entities; k++ )
				{
					gent = SV_GentityNum(k);
					if ( gent->r.linked &&
					(( gent->r.broadcastTime != 0 || (((gent->r.svFlags & 1) == 0 && ((svEnt = SV_SvEntityForGentity(gent), (gent->r.svFlags & 0x18) != 0 || svEnt->numClusters != 0 )))))))
					{
						memcpy(&to.s, &gent->s, sizeof(entityState_t));
						to.r.svFlags = gent->r.svFlags;
						if ( gent->r.broadcastTime )
							to.r.svFlags |= 8u;
						to.r.clientMask[0] = gent->r.clientMask[0];
						to.r.clientMask[1] = gent->r.clientMask[1];
						VectorCopy(gent->r.absmin, to.r.absmin);
						VectorCopy(gent->r.absmax, to.r.absmax);
						
						/* New code start: setEarthquakes */
						if ( (to.s.eType - 10) == EV_EARTHQUAKE )
						{
							for ( l = 0; l < sv_maxclients->integer; l++ )
							{
								if ( player_no_earthquakes[l] )
								{
									if ( l > 31 )
										to.r.clientMask[1] |= 1 << (l - 32);
									else
										to.r.clientMask[0] |= 1 << l;
								}
							}
						}
						/* New code end */
						
						custom_MSG_WriteDeltaArchivedEntity(&msg, &sv.svEntities[gent->s.number].baseline, &to, 1);
					}
				}
			}
			MSG_WriteBits(&msg, 0x3ff, 10);
			if ( !msg.overflowed )
			{
				archSnap = &svs.archivedSnapshotFrames[svs.nextArchivedSnapshotFrames % 0x4b0];
				archSnap->start = svs.nextArchivedSnapshotBuffer;
				archSnap->size = msg.cursize;
				nextArchSnapBuffer = svs.nextArchivedSnapshotBuffer;
				if ( svs.nextArchivedSnapshotBuffer < 0 )
					nextArchSnapBuffer = svs.nextArchivedSnapshotBuffer + 0x1ffffff;
				index = svs.nextArchivedSnapshotBuffer + (nextArchSnapBuffer >> 0x19) * -0x2000000;
				svs.nextArchivedSnapshotBuffer += msg.cursize;
				if ( 0x7ffffffd < svs.nextArchivedSnapshotBuffer )
					custom_Com_Error(0, "\x15svs.nextArchivedSnapshotBuffer wrapped");
				freeBytes = ARCHIVEDSSBUF_SIZE - index;
				
				if ( freeBytes < msg.cursize )
				{
					memcpy(&svs.archivedSnapshotBuffer[index], msg.data, freeBytes);
					memcpy(svs.archivedSnapshotBuffer, &msg.data[freeBytes], msg.cursize - freeBytes);
				}
				else
				{
					memcpy(&svs.archivedSnapshotBuffer[index], msg.data, msg.cursize);
				}
				i = svs.nextArchivedSnapshotFrames;
				svs.nextArchivedSnapshotFrames++;
				if ( i != 0x7ffffffc && 0x7ffffffc < svs.nextArchivedSnapshotFrames )
					custom_Com_Error(0, "\x15svs.nextArchivedSnapshotFrames wrapped");
				LargeLocalDestructor(&buf);
			}
			else
			{
				Com_DPrintf("SV_ArchiveSnapshot: ignoring snapshot because it overflowed.\n");
				LargeLocalDestructor(&buf);
			}
		}
	}
	else
	{
		LargeLocalDestructor(&buf);
	}
}

void custom_SV_BuildClientSnapshot(client_t *client)
{
	clientState_t *clientStateSource;
	int i, j;
	int cachedSnapClients;
	int cachedSnapEnts;
	playerState_t *ps;
	int archiveTime;
	cachedClient_t *cachedClient;
	int snapTime;
	archivedEntity_s *aent;
	cachedSnapshot_t *cachedSnap;
	int clientNum;
	gentity_t *clent;
	clientState_t *clientState;
	entityState_t *entState;
	gentity_t *ent;
	snapshotEntityNumbers_t entityNumbers;
	clientSnapshot_t *frame;
	vec3_t org;
	client_t *snapClient;

	frame = &client->frames[ client->netchan.outgoingSequence & PACKET_MASK ];
	
	frame->num_entities = 0;
	frame->num_clients = 0;
	clent = client->gentity;
	if ( clent && client->state != CS_ZOMBIE )
	{
		frame->first_entity = svs.nextSnapshotEntities;
		frame->first_client = svs.nextSnapshotClients;
		if ( sv.state == SS_GAME )
		{
			entityNumbers.numSnapshotEntities = 0;
			clientNum = client - svs.clients;
			archiveTime = G_GetClientArchiveTime(clientNum);
			cachedSnap = SV_GetCachedSnapshot(&archiveTime);
			G_SetClientArchiveTime(clientNum, archiveTime);
			if ( !cachedSnap )
				snapTime = 0;
			else
				snapTime = svs.time - cachedSnap->time;
			ps = SV_GameClientNum(clientNum);
			frame->ps = *ps;
			clientNum = frame->ps.clientNum;
			if ( clientNum < 0 || clientNum >= MAX_GENTITIES )
				custom_Com_Error(1, "\x15SV_BuildClientSnapshot: bad gEnt");
			VectorCopy(ps->origin, org);
			org[2] += ps->viewHeightCurrent;
			AddLeanToPosition(org, ps->viewangles[1], ps->leanf, 16.0, 20.0);
			if ( !cachedSnap )
			{
				SV_AddEntitiesVisibleFromPoint(org, clientNum, &entityNumbers);
				for (i = 0; i < entityNumbers.numSnapshotEntities; i++)
				{
					ent = SV_GentityNum(entityNumbers.snapshotEntities[i]);
					entState = svs.snapshotEntities + svs.nextSnapshotEntities % svs.numSnapshotEntities;
					*entState = ent->s;
					svs.nextSnapshotEntities++;
					if ( svs.nextSnapshotEntities >= 0x7ffffffe )
						custom_Com_Error(0, "\x15svs.nextSnapshotEntities wrapped");
					frame->num_entities++;
				}
				snapClient = svs.clients;
				for (i = 0; i < sv_maxclients->integer; i++, snapClient++)
				{
					if ( CS_ZOMBIE < snapClient->state )
					{
						clientStateSource = G_GetClientState(i);
						clientState = &svs.snapshotClients[svs.nextSnapshotClients % svs.numSnapshotClients];
						*clientState = *clientStateSource;
						
						svs.nextSnapshotClients++;
						if ( svs.nextSnapshotClients >= 0x7ffffffe )
							custom_Com_Error(0, "\x15svs.nextSnapshotClients wrapped");
						frame->num_clients++;
					}
				}
			}
			else
			{
				SV_AddCachedEntitiesVisibleFromPoint(cachedSnap->num_entities, cachedSnap->first_entity, org, clientNum, &entityNumbers);
				for (i = 0; i < entityNumbers.numSnapshotEntities; i++)
				{
					j = cachedSnap->first_entity + entityNumbers.snapshotEntities[i];
					cachedSnapEnts = j;
					if ( j < 0 )
						cachedSnapEnts = j + 0x3fff;
					aent = svs.cachedSnapshotEntities + j + (cachedSnapEnts >> 0xe) * -0x4000;
					entState = svs.snapshotEntities + svs.nextSnapshotEntities % svs.numSnapshotEntities;
					*entState = aent->s;
					if ( entState->pos.trTime )
						entState->pos.trTime += snapTime;
					if ( entState->apos.trTime )
						entState->apos.trTime += snapTime;
					if ( entState->time )
						entState->time += snapTime;
					if ( entState->time2 )
						entState->time2 += snapTime;
					
					/* New code start: setEarthquakes */
					if ( (aent->s.eType - 10) == EV_EARTHQUAKE )
					{
						if ( clientNum > 31 )
						{
							if ( aent->r.clientMask[1] & (1 << (clientNum - 32)) )
								aent->s.eType = EV_NONE;
						}
						else
						{
							if ( aent->r.clientMask[0] & (1 << clientNum ) )
								aent->s.eType = EV_NONE;
						}
					}
					/* New code end */
					
					svs.nextSnapshotEntities++;
					if ( svs.nextSnapshotEntities >= 0x7ffffffe )
						custom_Com_Error(0, "\x15svs.nextSnapshotEntities wrapped");
					frame->num_entities++;
				}
				for (i = 0; i < cachedSnap->num_clients; i++)
				{
					j = cachedSnap->first_client + i;
					cachedSnapClients = j;
					if ( j < 0 )
						cachedSnapClients = j + 0xfff;
					cachedClient = svs.cachedSnapshotClients + j + (cachedSnapClients >> 0xc) * -0x1000;
					clientState = &svs.snapshotClients[svs.nextSnapshotClients % svs.numSnapshotClients];
					*clientState = cachedClient->cs;
					svs.nextSnapshotClients++;
					if ( svs.nextSnapshotClients >= 0x7ffffffe )
						custom_Com_Error(0, "\x15svs.nextSnapshotClients wrapped");
					frame->num_clients++;
				}
			}
		}
	}
}

int num_map_turrets;
map_turret_t map_turrets[MAX_GENTITIES];
void custom_G_SetEntityPlacement(gentity_t *ent)
{
	/* New code start: map weapons callback */
	if ( codecallback_map_turrets_load )
	{
		// Default turret arcs to -1 so we can identify these as undefined in script
		map_turrets[num_map_turrets].toparc = -1;
		map_turrets[num_map_turrets].bottomarc = -1;
		map_turrets[num_map_turrets].leftarc = -1;
		map_turrets[num_map_turrets].rightarc = -1;
	}
	/* New code end */

	for ( int i = 0; i < level.spawnVars.numSpawnVars; i++ )
	{
		/* New code start: map weapons callback */
		if ( codecallback_map_turrets_load )
		{
			if ( !strncmp(level.spawnVars.spawnVars[i].key, "toparc", 6) )
			{
				map_turrets[num_map_turrets].toparc = atoi(level.spawnVars.spawnVars[i].value);
			}
			else if ( !strncmp(level.spawnVars.spawnVars[i].key, "bottomarc", 9) )
			{
				map_turrets[num_map_turrets].bottomarc = atoi(level.spawnVars.spawnVars[i].value);
			}
			else if ( !strncmp(level.spawnVars.spawnVars[i].key, "leftarc", 7) )
			{
				map_turrets[num_map_turrets].leftarc = atoi(level.spawnVars.spawnVars[i].value);
			}
			else if ( !strncmp(level.spawnVars.spawnVars[i].key, "rightarc", 8) )
			{
				map_turrets[num_map_turrets].rightarc = atoi(level.spawnVars.spawnVars[i].value);
			}
			else if ( !strncmp(level.spawnVars.spawnVars[i].key, "export", 6) )
			{
				map_turrets[num_map_turrets].script_export = atoi(level.spawnVars.spawnVars[i].value);
			}
			else if ( !strncmp(level.spawnVars.spawnVars[i].key, "model", 5) )
			{
				strncpy(map_turrets[num_map_turrets].model, level.spawnVars.spawnVars[i].value, sizeof(map_turrets[num_map_turrets].model));
			}
			else if ( !strncmp(level.spawnVars.spawnVars[i].key, "script_gameobjectname", 21) )
			{
				strncpy(map_turrets[num_map_turrets].script_gameobjectname, level.spawnVars.spawnVars[i].value, sizeof(map_turrets[num_map_turrets].script_gameobjectname));
			} 
			else if ( !strncmp(level.spawnVars.spawnVars[i].key, "weaponinfo", 10) )
			{
				strncpy(map_turrets[num_map_turrets].weaponinfo, level.spawnVars.spawnVars[i].value, sizeof(map_turrets[num_map_turrets].weaponinfo));
			}
		}
		/* New code end */
		G_ParseEntityField(level.spawnVars.spawnVars[i].key, level.spawnVars.spawnVars[i].value, ent);
	}
	G_SetOrigin(ent, &ent->r.currentOrigin);
	G_SetAngle(ent, &ent->r.currentAngles);
}

int num_map_weapons;
map_weapon_t map_weapons[MAX_GENTITIES];
void custom_G_CallSpawn(void)
{
	const char *classname;
	gentity_t *ent;
	gitem_t *item;
	int **i;

	G_SpawnString("classname", "", &classname);
	if ( classname == NULL )
	{
		Com_Printf("G_CallSpawn: NULL classname\n");
	}
	else
	{
		/* New code start: map weapons callback */
		if ( !strncmp(classname, "weapon_", 7) && !g_spawnMapWeapons->boolean )
		{
			ent = G_Spawn();
			custom_G_SetEntityPlacement(ent);
			if ( codecallback_map_weapons_load )
			{
				strncpy(map_weapons[num_map_weapons].classname, classname, sizeof(map_weapons[num_map_weapons].classname));
				VectorCopy(ent->r.currentOrigin, map_weapons[num_map_weapons].origin);
				VectorCopy(ent->r.currentAngles, map_weapons[num_map_weapons].angles);
				map_weapons[num_map_weapons].count = ent->count;
				num_map_weapons++;
			}
			G_FreeEntity(ent);
			return;
		}
		/* New code end */

		/* New code start: map turrets callback */
		if ( ( !strncmp(classname, "misc_mg42", 9) || !strncmp(classname, "misc_turret", 11) ) && !g_spawnMapTurrets->boolean )
		{
			ent = G_Spawn();
			custom_G_SetEntityPlacement(ent);
			if ( codecallback_map_turrets_load )
			{
				strncpy(map_turrets[num_map_turrets].classname, classname, sizeof(map_turrets[num_map_turrets].classname));
				VectorCopy(ent->r.currentOrigin, map_turrets[num_map_turrets].origin);
				VectorCopy(ent->r.currentAngles, map_turrets[num_map_turrets].angles);
				num_map_turrets++;
			}
			G_FreeEntity(ent);
			return;
		}
		/* New code end */
		
		item = G_GetItemForClassname(classname);
		if ( item == NULL )
		{
			for ( i = &spawns; *i != NULL; i += 2 )
			{
				if ( !strcmp((const char *)*i, classname) )
				{
					if ( i[1] == (int *)G_FreeEntity ) 
						return; 
					ent = G_Spawn();
					G_SetEntityPlacement(ent);
					((void (*)(gentity_t *))i[1])(ent);
					return;
				}
			}
			ent = G_Spawn();
			G_SetEntityPlacement(ent);
		}
		else
		{
			ent = G_Spawn();
			G_SetEntityPlacement(ent);
			G_SpawnItem(ent, item);
		}
	}
}

void custom_G_SpawnEntitiesFromString(void)
{
	if ( !G_ParseSpawnVars(&level.spawnVars) ) 
		custom_Com_Error(1, "\x15SpawnEntities: no entities"); 
	SP_worldspawn();
	
	/* New code start: map weapons callback */
	if ( codecallback_map_weapons_load )
	{
		num_map_weapons = 0;
		memset(&map_weapons, 0, sizeof(map_weapons));
	}
	/* New code end */

	/* New code start: map turrets callback */
	if ( codecallback_map_turrets_load )
	{
		num_map_turrets = 0;
		memset(&map_turrets, 0, sizeof(map_turrets));
	}
	/* New code end */
	
	while( G_ParseSpawnVars(&level.spawnVars) )
		custom_G_CallSpawn();
}

void custom_Scr_LoadGameType(void)
{
	hook_scr_loadgametype->unhook();

	void (*sig)(void);
	*(int *)&sig = hook_scr_loadgametype->from;

	/* New code start: map weapons callback */
	if ( codecallback_map_weapons_load )
	{
		if ( !num_map_weapons )
		{
			stackPushUndefined();
		}
		else
		{
			stackPushArray();
			for( int i = 0; i < num_map_weapons; i++ )
			{
				stackPushString(map_weapons[i].classname);
				stackPushArrayLast();
				stackPushVector(map_weapons[i].origin);
				stackPushArrayLast();
				stackPushVector(map_weapons[i].angles);
				stackPushArrayLast();
				stackPushInt(map_weapons[i].count);
				stackPushArrayLast();
			}
		}
		stackPushInt(num_map_weapons);
		short ret = Scr_ExecThread(codecallback_map_weapons_load, 2);
		Scr_FreeThread(ret);
	}
	/* New code end */

	/* New code start: map turrets callback */
	if ( codecallback_map_turrets_load )
	{
		if ( !num_map_turrets )
		{
			stackPushUndefined();
		}
		else
		{
			stackPushArray();
			for( int i = 0; i < num_map_turrets; i++ )
			{
				stackPushString(map_turrets[i].classname);
				stackPushArrayLast();
				stackPushVector(map_turrets[i].origin);
				stackPushArrayLast();
				stackPushVector(map_turrets[i].angles);
				stackPushArrayLast();
				stackPushInt(map_turrets[i].toparc);
				stackPushArrayLast();
				stackPushInt(map_turrets[i].bottomarc);
				stackPushArrayLast();
				stackPushInt(map_turrets[i].leftarc);
				stackPushArrayLast();
				stackPushInt(map_turrets[i].rightarc);
				stackPushArrayLast();
				stackPushString(map_turrets[i].model);
				stackPushArrayLast();
				stackPushString(map_turrets[i].weaponinfo);
				stackPushArrayLast();
				stackPushInt(map_turrets[i].script_export);
				stackPushArrayLast();
				stackPushString(map_turrets[i].script_gameobjectname);
				stackPushArrayLast();
			}
		}
		stackPushInt(num_map_turrets);
		short ret = Scr_ExecThread(codecallback_map_turrets_load, 2);
		Scr_FreeThread(ret);
	}
	/* New code end */

	sig();

	hook_scr_loadgametype->hook();
}

bool custom_CM_IsBadStaticModel(cStaticModel_t *model, char *src, float *origin, float *angles, float (*scale) [3])
{
	XModel_t *xmodel;

	if ( src == NULL || *src == '\0' )
		custom_Com_Error(1, "\x15Invalid static model name\n");
	
	if ( (*scale)[0] == 0.0 )
		custom_Com_Error(1, "\x15Static model [%s] has x scale of 0.0\n", src);
	
	if ( (*scale)[1] == 0.0 )
		custom_Com_Error(1, "\x15Static model [%s] has y scale of 0.0\n", src);
	
	if ( (*scale)[2] == 0.0 )
		custom_Com_Error(1, "\x15Static model [%s] has z scale of 0.0\n", src);
	
	xmodel = CM_XModelPrecache(src);
	if ( xmodel != NULL )
	{
		model->xmodel = xmodel;
		// On the server side, scale is only used for trace functions (see model->invScaledAxis)
		// The entity axis scale values are not synced to the players
		CM_InitStaticModel(model, origin, angles, scale);
		if ( g_debugStaticModels->boolean )
			Com_Printf("Initialized static model [%s] with scale (%f, %f, %f) at (%f, %f, %f)\n", src, (*scale)[0], (*scale)[1], (*scale)[2], model->origin[0], model->origin[1], model->origin[2]);
	}
	
	return xmodel != NULL;
}

void custom_Script_obituary(void)
{
	int args;
	int sWeapon;
	uint sMeansOfDeath;
	gentity_t *ent;
	int type;
	gentity_t *victim;
	gentity_t *attacker;
	vec3_t vec3_origin = {0, 0, 0};
	vec3_t origin;
	const char *team_str;
	int team = 0;
	int distance = 0;
	
	args = Scr_GetNumParam();
	if ( args < 4 || args > 7 )
		Scr_Error("Incorrect number of parameters");

	sWeapon = G_GetWeaponIndexForName(Scr_GetString(2));
	sMeansOfDeath = G_IndexForMeansOfDeath(Scr_GetString(3));
	victim = Scr_GetEntity(0);

	// Custom origin
	if ( args < 6 )
	{
		ent = G_TempEntity(&vec3_origin, EV_OBITUARY);
	}
	else
	{
		Scr_GetVector(args - 2, &origin);
		ent = G_TempEntity(&origin, EV_OBITUARY);
	}
	
	// Custom team (default 0 = all)
	if ( args == 5 || args == 7 )
	{
		team_str = Scr_GetString(4);
		if ( !strcmp(team_str, "axis") )
			team = 1;
		else if ( !strcmp(team_str, "allies") )
			team = 2;
		else if ( !strcmp(team_str, "spectator") )
			team = 3;
		else if ( !strcmp(team_str, "allies+axis") || !strcmp(team_str, "axis+allies") )
			team = 4;
		else if ( !strcmp(team_str, "spectator+axis") || !strcmp(team_str, "axis+spectator") )
			team = 5;
		else if ( !strcmp(team_str, "spectator+allies") || !strcmp(team_str, "allies+spectator") )
			team = 6;
	}
	ent->s.scale = team; // Reusing the scale field that is otherwise not used at obituary TempEntities
	
	// Custom max. distance
	if ( args > 5 )
		distance = Scr_GetInt(args - 1);
	ent->s.dmgFlags = distance; // Reusing the dmgFlags field that is otherwise not used at obituary TempEntities
	
	ent->s.otherEntityNum = victim->s.number;
	type = Scr_GetType(1);
	if ( type == 1 )
	{
		type = Scr_GetPointerType(1);
		if ( type == 0x15 )
		{
			attacker = Scr_GetEntity(1);
			ent->s.attackerEntityNum = attacker->s.number;
			goto LAB_081131e1;
		}
	}
	ent->s.attackerEntityNum = 0x3fe;
LAB_081131e1:
	ent->r.svFlags = 8;
	if ( sMeansOfDeath == 7 || sMeansOfDeath == 8 || sMeansOfDeath == 0xc || sMeansOfDeath == 0xb || sMeansOfDeath == 9 )
		ent->s.eventParm = sMeansOfDeath | 0x80;
	else
		ent->s.eventParm = sWeapon;
}

void com_printmessage_caret_patch(void)
{
	#if COD_VERSION == COD2_1_0
		int caret_check_offset = 0x0; // Not tested
	#elif COD_VERSION == COD2_1_2
		int caret_check_offset = 0x0; // Not tested
	#elif COD_VERSION == COD2_1_3
		int caret_check_offset = 0x08060CCA;
	#endif

	// Removes the following piece of code from Com_PrintMessage:
	/*
	if ( *message == '^' && message[1] != '\0' )
	{
		message += 2;
	}
	*/
	memset((int *)caret_check_offset, 0x90, 23);
}

qboolean custom_SV_MapExists(const char *name)
{
	// First try stock mechanism
	qboolean found = FS_ReadFile(custom_va("maps/mp/%s.%s", SV_GetMapBaseName(name), GetBspExtension()), 0) >= 0;
	if ( !found )
	{
		char library_path[512], map_check[512];
		
		cvar_t *fs_homepath = Cvar_FindVar("fs_homepath");
		cvar_t *fs_game = Cvar_FindVar("fs_game");

		if ( strlen(fs_library->string) )
			strncpy(library_path, fs_library->string, sizeof(library_path));
		else
			snprintf(library_path, sizeof(library_path), "%s/%s/Library", fs_homepath->string, fs_game->string);

		snprintf(map_check, sizeof(map_check), "%s/%s.iwd", library_path, name);

		return access(map_check, F_OK) != -1;
	}
	else
	{
		return qtrue;
	}
}

void Scr_QueueNotifyForCallback(unsigned int entId, unsigned int constString, VariableValue *arguments)
{
	if ( scr_notify_index < MAX_NOTIFY_BUFFER )
	{
		VariableValue *arg;
		unsigned int argc = 0;

		scr_notify[scr_notify_index].entId = entId;
		scr_notify[scr_notify_index].constString = constString;
		for ( arg = arguments; arg->type != STACK_PRECODEPOS && argc < MAX_NOTIFY_PARAMS; arg-- )
		{
			memcpy(&scr_notify[scr_notify_index].arguments[argc], arg, sizeof(scr_notify[0].arguments[0]));
			argc++;
		}
		scr_notify[scr_notify_index].argc = argc;
		scr_notify_index++;
	}
	else
	{
		printf("Warning: notify buffer full, not calling CodeCallback_Notify for '%s'\n", SL_ConvertToString(constString));
	}
}

void custom_VM_Notify(unsigned int entId, unsigned int constString, VariableValue *arguments)
{
	if ( codecallback_notify )
		Scr_QueueNotifyForCallback(entId, constString, arguments);

	hook_vm_notify->unhook();

	void (*sig)(unsigned int entId, unsigned int constString, VariableValue *arguments);
	*(int *)&sig = hook_vm_notify->from;
	
	sig(entId, constString, arguments);
	
	hook_vm_notify->hook();
}

class cCallOfDuty2Pro
{
public:
	cCallOfDuty2Pro()
	{
		// Don't inherit lib of parent
		unsetenv("LD_PRELOAD");
		
		// Otherwise the printf()'s are printed at crash/end on older os/compiler versions
		setbuf(stdout, NULL);

		#if COD_VERSION == COD2_1_0
			printf("> [LIBCOD] Compiled for: CoD2 1.0\n");
		#elif COD_VERSION == COD2_1_2
			printf("> [LIBCOD] Compiled for: CoD2 1.2\n");
		#elif COD_VERSION == COD2_1_3
			printf("> [LIBCOD] Compiled for: CoD2 1.3\n");
		#endif

		printf("> [LIBCOD] Compiled %s %s using GCC %s\n", __DATE__, __TIME__, __VERSION__);

		// Allow to write in executable memory
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

			hook_developer_prints = new cHook(0x08060B7C, (int)hook_Com_DPrintf);
			#if COMPILE_UTILS == 1
				hook_console_print = new cHook(0x0, int(hook_Sys_Print)); // Not tested
				hook_console_print->hook();
			#endif

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
			hook_touch_item_auto = new cHook(0x081037F0, int(touch_item_auto));
			hook_touch_item_auto->hook();

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
				cracking_hook_call(0x08094191, (int)hook_SVC_RemoteCommand);
				cracking_hook_call(0x080940C4, (int)hook_SV_GetChallenge);
				cracking_hook_call(0x08094107, (int)hook_SV_DirectConnect);
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

			hook_developer_prints = new cHook(0x08060E42, (int)hook_Com_DPrintf);
			#if COMPILE_UTILS == 1
				hook_console_print = new cHook(0x0, int(hook_Sys_Print)); // Not tested
				hook_console_print->hook();
			#endif

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
			hook_touch_item_auto = new cHook(0x08105B24, int(touch_item_auto));
			hook_touch_item_auto->hook();

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
				cracking_hook_call(0x08095D63, (int)hook_SVC_RemoteCommand);
				cracking_hook_call(0x08095BF8, (int)hook_SV_GetChallenge);
				cracking_hook_call(0x08095CB2, (int)hook_SV_DirectConnect);
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

			hook_gametype_scripts = new cHook(0x08110286, (int)hook_codscript_gametype_scripts);
			hook_gametype_scripts->hook();

			hook_developer_prints = new cHook(0x08060E3A, (int)hook_Com_DPrintf);
			#if COMPILE_UTILS == 1
				hook_console_print = new cHook(0x080d4AE0, int(hook_Sys_Print));
				hook_console_print->hook();
			#endif

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
			hook_touch_item_auto = new cHook(0x08105C80, int(touch_item_auto));
			hook_touch_item_auto->hook();
			hook_g_tempentity = new cHook(0x0811EFC4, (int)custom_G_TempEntity);
			hook_g_tempentity->hook();
			hook_sv_masterheartbeat = new cHook(0x08096ED6, (int)custom_sv_masterheartbeat);
			hook_sv_masterheartbeat->hook();
			hook_g_runframe = new cHook(0x0810A13A, (int)custom_G_RunFrame);
			hook_g_runframe->hook();
			hook_scr_loadgametype = new cHook(0x081182F4, (int)custom_Scr_LoadGameType);
			hook_scr_loadgametype->hook();
			hook_vm_notify = new cHook(0x0808359E, (int)custom_VM_Notify);
			hook_vm_notify->hook();

			#if COMPILE_PLAYER == 1
				hook_play_movement = new cHook(0x08090DAC, (int)play_movement);
				hook_play_movement->hook();
				hook_play_endframe = new cHook(0x080F7516, (int)play_endframe);
				hook_play_endframe->hook();
				hook_set_anim = new cHook(0x080D90D6, (int)set_anim);
				hook_set_anim->hook();
			#endif

			cracking_hook_function(0x08105CAC, (int)custom_Touch_Item);
			cracking_hook_function(0x0811F232, (int)custom_G_AddEvent);
			cracking_hook_function(0x080EBF24, (int)hook_BG_IsWeaponValid);
			cracking_hook_function(0x080DFC78, (int)custom_BG_AddPredictableEventToPlayerstate);
			cracking_hook_function(0x080985C8, (int)custom_SV_WriteSnapshotToClient);
			cracking_hook_function(0x0808F302, (int)custom_SV_SendClientGameState);
			cracking_hook_function(0x0808F02E, (int)custom_SV_DropClient);
			cracking_hook_function(0x0808FDC2, (int)custom_SV_WriteDownloadToClient);
			cracking_hook_function(0x080B7FA6, (int)custom_va);
			cracking_hook_function(0x08090534, (int)hook_SV_VerifyIwds_f);
			cracking_hook_function(0x080907BA, (int)hook_SV_ResetPureClient_f);
			cracking_hook_function(0x080963C8, (int)custom_SV_CalcPings);
			cracking_hook_function(0x0809657E, (int)custom_SV_CheckTimeouts);
			cracking_hook_function(0x08080050, (int)custom_Scr_ErrorInternal);
			cracking_hook_function(0x08061124, (int)custom_Com_Error);
			cracking_hook_function(0x080788D2, (int)custom_RuntimeError);
			cracking_hook_function(0x080787DC, (int)custom_RuntimeError_Debug);
			cracking_hook_function(0x0809B016, (int)custom_SV_ArchiveSnapshot);
			cracking_hook_function(0x0809A408, (int)custom_SV_BuildClientSnapshot);
			cracking_hook_function(0x0811B770, (int)custom_G_SpawnEntitiesFromString);
			cracking_hook_function(0x080584F0, (int)custom_CM_IsBadStaticModel);
			cracking_hook_function(0x08113128, (int)custom_Script_obituary);
			cracking_hook_function(0x08092302, (int)custom_SV_MapExists);

			#if COMPILE_BOTS == 1
				cracking_hook_function(0x0809676C, (int)custom_SV_BotUserMove);
			#endif

			#if COMPILE_RATELIMITER == 1
				cracking_hook_call(0x08095C48, (int)hook_SVC_Info);
				cracking_hook_call(0x08095B94, (int)hook_SVC_Status);
				cracking_hook_call(0x08095E1D, (int)hook_SVC_RemoteCommand);
				cracking_hook_call(0x08095CB2, (int)hook_SV_GetChallenge);
				cracking_hook_call(0x08095D6C, (int)hook_SV_DirectConnect);
			#endif
		#endif

		com_printmessage_caret_patch();
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
// Both now: able to load with wrapper AND directly
// IMPORTANT: file needs "lib" infront of name, otherwise it wont be loaded
// Will be called when LD_PRELOAD is referencing this .so

void __attribute__ ((constructor)) lib_load(void)
{
	pro = new cCallOfDuty2Pro;
}

void __attribute__ ((destructor)) lib_unload(void)
{
	delete pro;
}