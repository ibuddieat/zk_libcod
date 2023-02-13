#include "gsc.hpp"

#if COMPILE_CUSTOM_VOICE == 1
#include <pthread.h>
#endif

// Stock cvars
cvar_t *cl_allowDownload;
cvar_t *com_logfile;
cvar_t *developer;
cvar_t *g_playerCollisionEjectSpeed;
cvar_t *g_voiceChatTalkingDuration;
cvar_t *player_meleeHeight;
cvar_t *player_meleeRange;
cvar_t *player_meleeWidth;
cvar_t *rcon_password;
cvar_t *sv_allowDownload;
cvar_t *sv_floodProtect;
cvar_t *sv_fps;
cvar_t *sv_maxclients;
cvar_t *sv_maxRate;
cvar_t *sv_padPackets;
cvar_t *sv_pure;
cvar_t *sv_showCommands;
cvar_t *sv_timeout;
cvar_t *sv_voice;
cvar_t *sv_voiceQuality;
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
cvar_t *g_dumpVoiceData;
cvar_t *g_logPickup;
cvar_t *g_playerCollision;
cvar_t *g_playerEject;
cvar_t *g_resetSlide;
cvar_t *g_spawnMapTurrets;
cvar_t *g_spawnMapWeapons;
cvar_t *logfileName;
cvar_t *logfileRotate;
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
cvar_t *sv_verifyIwds;
#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
cvar_t *sv_wwwDlDisconnectedMessages;
#endif

cHook *hook_add_opcode;
cHook *hook_clientendframe;
cHook *hook_com_initcvars;
cHook *hook_console_print;
cHook *hook_developer_prints;
cHook *hook_fire_grenade;
cHook *hook_g_processipbans;
cHook *hook_g_runframe;
cHook *hook_g_tempentity;
cHook *hook_gametype_scripts;
cHook *hook_gscr_loadconsts;
cHook *hook_init_opcode;
cHook *hook_play_movement;
cHook *hook_g_freeentity;
cHook *hook_g_initgentity;
cHook *hook_print_codepos;
cHook *hook_scr_loadgametype;
cHook *hook_scr_notify;
cHook *hook_script_cloneplayer;
cHook *hook_bg_playanim;
cHook *hook_sv_init;
cHook *hook_sv_masterheartbeat;
cHook *hook_touch_item_auto;
cHook *hook_vm_notify;
cHook *hook_sv_verifyiwds_f;

int codecallback_client_spam = 0;
int codecallback_dprintf = 0;
int codecallback_error = 0;
int codecallback_fire_grenade = 0;
int codecallback_hitchwarning = 0;
int codecallback_map_turrets_load = 0;
int codecallback_map_weapons_load = 0;
int codecallback_notify = 0;
int codecallback_notifydebug = 0;
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
int codecallback_leanleftbutton = 0;
int codecallback_leanrightbutton = 0;
int codecallback_meleebreathbutton = 0;
int codecallback_meleebutton = 0;
int codecallback_pronebutton = 0;
int codecallback_reloadbutton = 0;
int codecallback_smokebutton = 0;
int codecallback_standbutton = 0;
int codecallback_usebutton = 0;

/*
const entityHandler_t entityHandlers[] =
{
	{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0 },
	{ NULL, NULL, NULL, &Touch_Multi, NULL, NULL, NULL, NULL, 0, 0 },
	{ NULL, NULL, NULL, NULL, &hurt_use, NULL, NULL, NULL, 0, 0 },
	{ NULL, NULL, NULL, &hurt_touch, &hurt_use, NULL, NULL, NULL, 0, 0 },
	{ NULL, NULL, NULL, NULL, &Use_trigger_damage, &Pain_trigger_damage, &Die_trigger_damage, NULL, 0, 0},
	{ NULL, &Reached_ScriptMover, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0 },
	{ NULL, &Reached_ScriptMover, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0 },
	{ &G_ExplodeMissile, NULL, NULL, &Touch_Item_Auto, NULL, NULL, NULL, NULL, 3, 4},
	{ &G_ExplodeMissile, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 5, 6 },
	{ NULL, NULL, NULL, NULL, NULL, NULL, &player_die, &G_PlayerController, 0, 0 },
	{ NULL, NULL, NULL, NULL, NULL, NULL, &player_die, NULL, 0, 0 },
	{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, &G_PlayerController, 0, 0 },
	{ &BodyEnd, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0 },
	{ &turret_think_init, NULL, NULL, NULL, &turret_use, NULL, NULL, &turret_controller, 0, 0},
	{ &turret_think, NULL, NULL, NULL, &turret_use, NULL, NULL, &turret_controller, 0, 0},
	{ &DroppedItemClearOwner, NULL, NULL, &Touch_Item_Auto, NULL, NULL, NULL, NULL, 0, 0},
	{ &FinishSpawningItem, NULL, NULL, &Touch_Item_Auto, NULL, NULL, NULL, NULL, 0, 0},
	{ NULL, NULL, NULL, &Touch_Item_Auto, NULL, NULL, NULL, NULL, 0, 0 },
	{ NULL, NULL, NULL, NULL, &use_trigger_use, NULL, NULL, NULL, 0, 0 },
	{ &G_FreeEntity, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0 },
};
*/

qboolean logRcon = qtrue;
qboolean logHeartbeat = qtrue;

int hitchFrameTime = 0;

char openLogfileName[MAX_OSPATH];

scr_error_t scr_errors[MAX_ERROR_BUFFER];
int scr_errors_index = 0;

scr_notify_t scr_notify[MAX_NOTIFY_DEBUG_BUFFER];
int scr_notify_index = 0;

int num_map_weapons;
map_weapon_t map_weapons[MAX_GENTITIES];

int num_map_turrets;
map_turret_t map_turrets[MAX_GENTITIES];

customEntityState_t customEntityState[MAX_GENTITIES];

FILE *voiceDataDumpFile;
#if COMPILE_CUSTOM_VOICE == 1
pthread_mutex_t loadSoundFileResultLock;
loadSoundFileResult_t loadSoundFileResults[MAX_THREAD_RESULTS_BUFFER];
int loadSoundFileResultsIndex = 0;
int currentMaxSoundIndex = 0;
VoicePacket_t voiceDataStore[MAX_CUSTOMSOUNDS][MAX_STOREDVOICEPACKETS];
float player_pendingVoiceDataFrames[MAX_CLIENTS] = {0.0};
int player_currentSoundTalker[MAX_CLIENTS] = {0};
int player_currentSoundIndex[MAX_CLIENTS] = {0};
int player_sentVoiceDataIndex[MAX_CLIENTS] = {0};
#endif
objective_t player_objectives[MAX_CLIENTS][16];
float player_meleeHeightScale[MAX_CLIENTS] = {0.0}; // Defaults to 1.0 on connect
float player_meleeRangeScale[MAX_CLIENTS] = {0.0}; // Defaults to 1.0 on connect
float player_meleeWidthScale[MAX_CLIENTS] = {0.0}; // Defaults to 1.0 on connect
int player_fireThroughWalls[MAX_CLIENTS] = {0};
float player_fireRangeScale[MAX_CLIENTS] = {0.0}; // Defaults to 1.0 on connect
int player_no_pickup[MAX_CLIENTS] = {0};
int player_no_earthquakes[MAX_CLIENTS] = {0};
collisionTeam_t player_collision[MAX_CLIENTS] = {COLLISION_TEAM_BOTH}; // Defaults to COLLISION_TEAM_BOTH on connect
int player_silent[MAX_CLIENTS] = {0};
int player_g_speed[MAX_CLIENTS] = {0};
int player_g_gravity[MAX_CLIENTS] = {0};
int gamestate_size[MAX_CLIENTS] = {0};
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

void custom_Com_InitCvars(void)
{
	/* Register stock cvars here with different settings, scheme:
	cvar_t *cvar = Cvar_Register<Type>(var_name, default value, [min. value, max. value,] flags); */
	sv_maxRate = Cvar_RegisterInt("sv_maxRate", 0, 0, 25000, CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_UNSAFE);

	// Register custom cvars required early on server start
	logfileName = Cvar_RegisterString("logfileName", "console_mp_server.log", CVAR_ARCHIVE);
	logfileRotate = Cvar_RegisterInt("logfileRotate", 0, 0, 1000, CVAR_ARCHIVE);

	hook_com_initcvars->unhook();
	void (*Com_InitCvars)(void);
	*(int *)&Com_InitCvars = hook_com_initcvars->from;
	Com_InitCvars();
	hook_com_initcvars->hook();

	// Get references to early loaded stock cvars
	com_logfile = Cvar_FindVar("logfile");
}

void common_init_complete_print(const char *format, ...)
{
	char s[MAX_STRINGLENGTH];
	va_list va;

	va_start(va, format);
	Q_vsnprintf(s, sizeof(s), format, va);
	va_end(va);

	Com_Printf("%s", s);

	/* Do stuff after sv has been initialized here */
	
	// Get references to stock cvars
	cl_allowDownload = Cvar_RegisterBool("cl_allowDownload", qtrue, CVAR_ARCHIVE | CVAR_SYSTEMINFO); // Force-enable download for clients
	developer = Cvar_FindVar("developer");
	rcon_password = Cvar_FindVar("rcon_password");
	sv_allowDownload = Cvar_FindVar("sv_allowDownload");
	sv_floodProtect = Cvar_FindVar("sv_floodProtect");
	sv_fps = Cvar_FindVar("sv_fps");
	sv_maxclients = Cvar_FindVar("sv_maxclients");
	sv_padPackets = Cvar_FindVar("sv_padPackets");
	sv_pure = Cvar_FindVar("sv_pure");
	sv_showCommands = Cvar_FindVar("sv_showCommands");
	sv_timeout = Cvar_FindVar("sv_timeout");
	sv_voice = Cvar_FindVar("sv_voice");
	sv_voiceQuality = Cvar_FindVar("sv_voiceQuality");
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
	g_dumpVoiceData = Cvar_RegisterBool("g_dumpVoiceData", qfalse, CVAR_ARCHIVE);
	g_logPickup = Cvar_RegisterBool("g_logPickup", qtrue, CVAR_ARCHIVE);
	g_playerCollision = Cvar_RegisterBool("g_playerCollision", qtrue, CVAR_ARCHIVE);
	g_playerEject = Cvar_RegisterBool("g_playerEject", qtrue, CVAR_ARCHIVE);
	g_resetSlide = Cvar_RegisterBool("g_resetSlide", qfalse, CVAR_ARCHIVE);
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
	sv_verifyIwds = Cvar_RegisterBool("sv_verifyIwds", qtrue, CVAR_ARCHIVE);
	#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
	sv_wwwDlDisconnectedMessages = Cvar_RegisterInt("sv_wwwDlDisconnectedMessages", 1, 0, 2, CVAR_ARCHIVE);
	#endif

	if ( g_dumpVoiceData->boolean )
	{
		voiceDataDumpFile = fopen("voiceData.spx", "ab");	
		if ( !voiceDataDumpFile )
		{
			Com_Printf("Warning: Could not open file voiceData.spx\n");
		}
	}
}

void custom_G_ProcessIPBans(void)
{
	/* This is right after G_RegisterCvars, giving us access to variables that
	are not yet defined at the common_init_complete_print hook */
	g_playerCollisionEjectSpeed = Cvar_FindVar("g_playerCollisionEjectSpeed");
	g_voiceChatTalkingDuration = Cvar_FindVar("g_voiceChatTalkingDuration");
	player_meleeHeight = Cvar_FindVar("player_meleeHeight");
	player_meleeRange = Cvar_FindVar("player_meleeRange");
	player_meleeWidth = Cvar_FindVar("player_meleeWidth");

	hook_g_processipbans->unhook();
	void (*G_ProcessIPBans)(void);
	*(int *)&G_ProcessIPBans = hook_g_processipbans->from;
	G_ProcessIPBans();
	hook_g_processipbans->hook();
}

void custom_GScr_LoadConsts(void)
{
	/* Allocate custom strings for Scr_Notify() here, scheme:
	scr_const.custom = GScr_AllocString("custom_string");
	Note: This new reference also has to be added to stringIndex_t in declarations.hpp
	*/
	scr_const.both = GScr_AllocString("both");
	scr_const.bounce = GScr_AllocString("bounce");
	scr_const.flags = GScr_AllocString("flags");
	scr_const.land = GScr_AllocString("land");
	#if COMPILE_CUSTOM_VOICE == 1
	scr_const.sound_file_done = GScr_AllocString("sound_file_done");
	scr_const.sound_file_stop = GScr_AllocString("sound_file_stop");
	#endif
	scr_const.trigger_radius = GScr_AllocString("trigger_radius");

	hook_gscr_loadconsts->unhook();
	void (*GScr_LoadConsts)(void);
	*(int *)&GScr_LoadConsts = hook_gscr_loadconsts->from;
	GScr_LoadConsts();
	hook_gscr_loadconsts->hook();
}

void hitch_warning_print(const char *message, int frameTime)
{
	/* Called if 500 < frameTime && frameTime < 500000 */
	Com_Printf(message, frameTime);

	hitchFrameTime = frameTime;
}

void hook_bad_printf(const char *format, ...) {}

void hook_sv_spawnserver(const char *format, ...)
{
	char s[MAX_STRINGLENGTH];
	va_list va;

	va_start(va, format);
	Q_vsnprintf(s, sizeof(s), format, va);
	va_end(va);

	Com_Printf("%s", s);
	
	/* Do stuff after sv has been spawned here */

	hook_developer_prints->hook();
}

void custom_sv_masterheartbeat(const char *hbname)
{
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

	hook_sv_masterheartbeat->unhook();
	void (*SV_MasterHeartbeat)(const char *hbname);
	*(int *)&SV_MasterHeartbeat = hook_sv_masterheartbeat->from;
	SV_MasterHeartbeat(hbname);
	hook_sv_masterheartbeat->hook();
}

int custom_GScr_LoadGameTypeScript()
{
	char path_for_cb[512] = "maps/mp/gametypes/_callbacksetup";

	if ( strlen(fs_callbacks->string) )
		strncpy(path_for_cb, fs_callbacks->string, sizeof(path_for_cb));

	codecallback_client_spam = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_CLSpam", 0);
	codecallback_dprintf = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_DPrintf", 0);
	codecallback_error = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_Error", 0);
	codecallback_fire_grenade = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_FireGrenade", 0);
	codecallback_hitchwarning = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_HitchWarning", 0);
	codecallback_map_turrets_load = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_MapTurretsLoad", 0);
	codecallback_map_weapons_load = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_MapWeaponsLoad", 0);
	codecallback_notify = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_Notify", 0);
	codecallback_notifydebug = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_NotifyDebug", 0);
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
 	codecallback_leanleftbutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_LeanLeftButton", 0);
 	codecallback_leanrightbutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_LeanRightButton", 0);
	codecallback_meleebreathbutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_MeleeBreathButton", 0);
	codecallback_meleebutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_MeleeButton", 0);
	codecallback_pronebutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_ProneButton", 0);
	codecallback_reloadbutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_ReloadButton", 0);
	codecallback_smokebutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_SmokeButton", 0);
	codecallback_standbutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_StandButton", 0);
 	codecallback_usebutton = Scr_GetFunctionHandle(path_for_cb, "CodeCallback_UseButton", 0);

	hook_gametype_scripts->unhook();
	int (*GScr_LoadGameTypeScript)();
	*(int *)&GScr_LoadGameTypeScript = hook_gametype_scripts->from;
	int ret = GScr_LoadGameTypeScript();
	hook_gametype_scripts->hook();

	return ret;
}

qboolean SkipCollision(gentity_t *client1, gentity_t *client2)
{
	int id1 = client1 - g_entities;
	int id2 = client2 - g_entities;

	if ( id1 < MAX_CLIENTS && id2 < MAX_CLIENTS && client1->client && client2->client && client1->client->sess.connected == CON_CONNECTED && client2->client->sess.connected == CON_CONNECTED )
	{
		if ( player_collision[id1] == COLLISION_TEAM_NONE || player_collision[id2] == COLLISION_TEAM_NONE )
			return qtrue;

		if ( player_collision[id1] == COLLISION_TEAM_AXIS && (client2->client->sess).team != TEAM_AXIS )
			return qtrue;

		if ( player_collision[id1] == COLLISION_TEAM_ALLIES && (client2->client->sess).team != TEAM_ALLIES )
			return qtrue;

		if ( player_collision[id2] == COLLISION_TEAM_AXIS && (client1->client->sess).team != TEAM_AXIS )
			return qtrue;

		if ( player_collision[id2] == COLLISION_TEAM_ALLIES && (client1->client->sess).team != TEAM_ALLIES )
			return qtrue;

		if ( player_collision[id1] == COLLISION_TEAM_BOTH )
		{
			if ( player_collision[id2] == COLLISION_TEAM_BOTH )
				return qfalse;

			if ( player_collision[id2] == COLLISION_TEAM_AXIS && (client1->client->sess).team == TEAM_AXIS )
				return qfalse;

			if ( player_collision[id2] == COLLISION_TEAM_ALLIES && (client1->client->sess).team == TEAM_ALLIES )
				return qfalse;
		}

		if ( player_collision[id2] == COLLISION_TEAM_BOTH )
		{
			if ( player_collision[id1] == COLLISION_TEAM_AXIS && (client2->client->sess).team == TEAM_AXIS )
				return qfalse;

			if ( player_collision[id1] == COLLISION_TEAM_ALLIES && (client2->client->sess).team == TEAM_ALLIES )
				return qfalse;
		}
	}

	return qfalse;
}

void custom_SV_ClipMoveToEntity(moveclip_t *clip, svEntity_s *entity, trace_t *trace)
{
	int touchNum;
	gentity_t *touch;
	clipHandle_t clipHandle;
	vec3_t maxs;
	vec3_t mins;
	float *angles;
	float fraction;

	touchNum = entity - sv.svEntities;
	touch = SV_GentityNum(touchNum);
	if ( ( ( (touch->r).contents & clip->contentmask) != 0 ) &&
	( clip->passEntityNum == ENTITY_NONE || ( ( touchNum != clip->passEntityNum && (touch->r).ownerNum != clip->passEntityNum ) && ( (touch->r).ownerNum != clip->passOwnerNum ) ) ) )
	{
		/* New code: per-player/team collison */
		if ( SkipCollision(touch, &g_entities[clip->passEntityNum]) )
			return;
		/* New code end */

		VectorAdd((touch->r).absmin, clip->mins, mins);
		VectorAdd((touch->r).absmax, clip->maxs, maxs);

		if ( !CM_TraceBox(&clip->extents, mins, maxs, trace->fraction) )
		{
			clipHandle = SV_ClipHandleForEntity(touch);
			angles = (touch->r).currentAngles;
			if ( (touch->r).bmodel == 0 )
			{
				angles = vec3_origin;
			}
			fraction = trace->fraction;
			CM_TransformedBoxTrace(trace, (clip->extents).start, (clip->extents).end, clip->mins, clip->maxs, clipHandle, clip->contentmask, (touch->r).currentOrigin, angles);
			if ( trace->fraction < fraction )
			{
				(trace->entityNum).entnum = (touch->s).number;
			}
		}
	}
}

void custom_G_SetClientContents(gentity_t *ent)
{
	if ( !g_playerCollision->boolean )
		return;

	if ( ent->client->noclip == 0 )
	{
		if ( ent->client->ufo == 0 )
		{
			if ( (ent->client->sess).state == STATE_DEAD )
			{
				(ent->r).contents = 0;
			}
			else
			{
				if( player_collision[ent - g_entities] != COLLISION_TEAM_BOTH )
					(ent->r).contents = ( CONTENTS_CANSHOTCLIP | CONTENTS_CLIPSHOT );
				else
					(ent->r).contents = CONTENTS_BODY;
			}
		}
		else
		{
			(ent->r).contents = 0;
		}
	}
	else
	{
		(ent->r).contents = 0;
	}
}

qboolean custom_StuckInClient(gentity_t *self)
{
	float fTemp;
	double dTemp;
	float selfEjectSpeed;
	float hitEjectSpeed;
	vec2_t dir;
	gentity_t *hit;
	float hitSpeed;
	float selfSpeed;
	int i;
	int id = self - g_entities;

	/* New code: g_playerEject cvar */
	if ( !g_playerEject->boolean )
		return qfalse;
	/* New code end */

	if ( ( ( ((self->client->ps).pm_flags & PMF_VIEWLOCKED) != 0 ) && ( (self->client->sess).state == STATE_PLAYING ) ) && ( player_collision[id] != COLLISION_TEAM_BOTH /* New condition */ || ( (self->r).contents == CONTENTS_BODY || ( (self->r).contents == CONTENTS_CORPSE ) ) ) )
	{
		hit = g_entities;
		for ( i = 0; i < level.maxclients; i++, hit++)
		{
			if ( ( ( ( ( ( (hit->r).inuse != 0 ) && ( ((hit->client->ps).pm_flags & PMF_VIEWLOCKED) != 0 ) ) && ( (hit->client->sess).state == STATE_PLAYING )  ) && ( (hit != self && hit->client != NULL ) ) ) && ( 0 < hit->healthPoints && ( player_collision[i] != COLLISION_TEAM_BOTH /* New condition */ || ( (hit->r).contents == CONTENTS_BODY || ( (hit->r).contents == CONTENTS_CORPSE ) ) ) ) ) &&
			( (hit->r).absmin[0] <= (self->r).absmax[0] && ( ( ( (self->r).absmin[0] <= (hit->r).absmax[0] && ( (hit->r).absmin[1] <= (self->r).absmax[1] ) ) && ( (self->r).absmin[1] <= (hit->r).absmax[1] ) ) && ( (hit->r).absmin[2] <= (self->r).absmax[2] && ( (self->r).absmin[2] <= (hit->r).absmax[2] ) ) ) ) )
			{
				/* New code: per-player/team collison */
				if ( SkipCollision(self, hit) )
					continue;
				/* New code end */

				VectorSubtract2((hit->r).currentOrigin, (self->r).currentOrigin, dir);
				fTemp = (self->r).maxs[0] + (hit->r).maxs[0];
				dTemp = DotProduct2(dir, dir);
				if ( dTemp <= ( (double)fTemp * (double)fTemp ) )
				{
					VectorSubtract2((hit->r).currentOrigin, (self->r).currentOrigin, dir);
					dTemp = crandom();
					dir[0] = dir[0] + (float)((dTemp + dTemp) - 1.0);
					dTemp = crandom();
					dir[1] = dir[1] + (float)((dTemp + dTemp) - 1.0);
					Vec2Normalize(dir);
					if ( 0.0 < VectorLength2((hit->client->ps).velocity) )
					{
						hitEjectSpeed = (float)g_playerCollisionEjectSpeed->integer;
					}
					else
					{
						hitEjectSpeed = 0.0;
					}
					hitSpeed = hitEjectSpeed;
					if ( 0.0 < VectorLength2((self->client->ps).velocity) )
					{
						selfEjectSpeed = (float)g_playerCollisionEjectSpeed->integer;
					}
					else
					{
						selfEjectSpeed = 0.0;
					}
					selfSpeed = selfEjectSpeed;
					if ( hitEjectSpeed < 0.0001 && selfEjectSpeed < 0.0001 )
					{
						hitSpeed = (float)(hit->client->ps).speed;
						selfSpeed = (float)(self->client->ps).speed;
					}
					VectorScale2(dir, hitSpeed, (hit->client->ps).velocity);
					(hit->client->ps).pm_time = 300;
					(hit->client->ps).pm_flags = (hit->client->ps).pm_flags | PMF_SLIDING;
					VectorScale2(dir, selfSpeed * -1, (self->client->ps).velocity);
					(self->client->ps).pm_time = 300;
					(self->client->ps).pm_flags = (self->client->ps).pm_flags | PMF_SLIDING;
					return qtrue;
				}
			}
		}
	}
	return qfalse;
}

gentity_t* custom_fire_grenade(gentity_t *attacker, vec3_t start, vec3_t dir, int weaponIndex, int fuseTime)
{
	hook_fire_grenade->unhook();
	gentity_t* (*fire_grenade)(gentity_t *attacker, vec3_t start, vec3_t dir, int weaponIndex, int fuseTime);
	*(int *)&fire_grenade = hook_fire_grenade->from;
	gentity_t* grenade = fire_grenade(attacker, start, dir, weaponIndex, fuseTime);
	hook_fire_grenade->hook();

	if ( codecallback_fire_grenade && Scr_IsSystemActive() )
	{
		WeaponDef_t *def = BG_WeaponDefs(weaponIndex);
		stackPushString(def->szInternalName);
		stackPushEntity(grenade);
		short ret = Scr_ExecEntThread(attacker, codecallback_fire_grenade, 2);
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
		char tmp[MAX_STRINGLENGTH];
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
	int i;
	challenge_t *challenge;
	qboolean isBot = qfalse;
	const char *translatedReason;
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

	translatedReason = SEH_StringEd_GetString(reason);

	if ( showIngameMessage )
	{
		if ( !translatedReason )
			SV_SendServerCommand(0, 0, "e \"\x15%s^7 %s%s\"", name, "", reason);
		else
			SV_SendServerCommand(0, 0, "e \"\x15%s^7 %s%s\"", name, "\x14", reason);
	}

	Com_Printf("%i:%s %s\n", drop - svs.clients, name, reason);

	SV_SendServerCommand(0, 1, "J %d", drop - svs.clients);
	if ( !translatedReason )
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

void custom_Touch_Item_Auto(gentity_t * item, gentity_t * entity, int touch)
{
	if ( player_no_pickup[entity->client->ps.clientNum] )
		return;
	
	hook_touch_item_auto->unhook();
	void (*Touch_Item_Auto)(gentity_t *, gentity_t *, int);
	*(int *)&Touch_Item_Auto = hook_touch_item_auto->from;
	Touch_Item_Auto(item, entity, touch);
	hook_touch_item_auto->hook();
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
			if ( !COM_BitCheck(entity->client->ps.weapons, bg_item->giTag) )
			{
				if ( (BG_WeaponDefs(bg_item->giTag)->impactType + ~IMPACT_TYPE_NONE) < 2)
					SV_GameSendServerCommand(entity - g_entities, 0, custom_va("%c \"GAME_CANT_GET_PRIMARY_WEAP_MESSAGE\"", 0x66));
			}
			else
			{
				SV_GameSendServerCommand(entity - g_entities, 0, custom_va("%c \"GAME_PICKUP_CANTCARRYMOREAMMO\x14%s\"", 0x66, BG_WeaponDefs(bg_item->giTag)->szDisplayName));
			}
		}
	}
	else
	{
		I_strncpyz(name, (entity->client->sess).manualModeName, sizeof(name));
		I_CleanStr(name);
		
		if ( g_logPickup->boolean )
		{
			if ( bg_item->giType == IT_WEAPON )
				G_LogPrintf("Weapon;%d;%d;%s;%s\n", SV_GetGuid((entity->s).number), (entity->s).number, name, BG_WeaponDefs(bg_item->giTag)->szInternalName);
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
				stackPushString(BG_WeaponDefs(bg_item->giTag)->szInternalName);
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

void custom_MSG_WriteDeltaStruct(msg_t *msg, entityState_t *from, entityState_t *to, int force, int numFields, int indexBits, netField_t *stateFieldsPointer, int bChangeBit, int clientNum, int spectatorClientNum, int entNum)
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

			if ( numFields == 0x3b )
			{
				/* New code: per-player/team collison */
				if ( clientNum != -1 && i == 31 /* solid */ && entNum >= 0 && entNum < MAX_CLIENTS )
				{
					gentity_t *client1 = &g_entities[clientNum];
					gentity_t *client2 = &g_entities[entNum];

					if ( SkipCollision(client1, client2) )
						*toF = 0;
				}
				/* New code end */
				
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
			}
			
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
						client_t *client = NULL;
						if ( spectatorClientNum != -1 )
							client = &svs.clients[spectatorClientNum];
						else if ( clientNum != -1 )
							client = &svs.clients[clientNum];

						// ClientDisconnect() nulls parts of clientSession_t
						if ( client && client->gentity->client && client->gentity->client->sess.connected == CON_CONNECTED )
						{
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
					if ( ( ammobits[j] >> ( (byte)i & 0x1F ) & 1 ) != 0 )
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
				clipbits = clipbits | 1 << ( (byte)i & 0x1F );
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
				if ( clipbits >> ( (byte)i & 0x1F ) & 1 )
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
		MSG_WriteDeltaHudElems(msg, from->hud.archival, to->hud.archival, 0x1F);
		MSG_WriteDeltaHudElems(msg, from->hud.current, to->hud.current, 0x1F);
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

	custom_MSG_WriteDeltaStruct(msg, (entityState_t *)from, (entityState_t *)to, force, 0x16, 6, &clientStateFields, 1, -1, -1, -1);
}

void custom_MSG_WriteDeltaArchivedEntity(msg_t *msg, archivedEntity_s *from, archivedEntity_s *to, int force)
{
	custom_MSG_WriteDeltaStruct(msg, (entityState_t *)from, (entityState_t *)to, force, 0x44, 10, &archivedEntityFields, 0, -1, -1, -1);
}

void custom_MSG_WriteDeltaEntity(msg_t *msg, entityState_t *from, entityState_t *to, qboolean force, int clientNum, int entNum)
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
					if ( !( to->otherEntityNum == ( spectatorClientNum + 1 ) ) )
						disable = qtrue;
				}
				else
				{
					// Client plays normally
					if ( !( to->otherEntityNum == ( clientNum + 1 ) ) )
						disable = qtrue;
				}
			}
		}
	}

	if ( disable )
		to->eType = (entityType_t)EV_NONE;

	custom_MSG_WriteDeltaStruct(msg, from, to, force, 0x3b, 10, &entityStateFields, 0, clientNum, spectatorClientNum, entNum);
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
			custom_MSG_WriteDeltaEntity(msg, oldent, newent, 0, client, newnum);
			oldindex++;
			newindex++;
		}
		else if ( newnum < oldnum )
		{
			custom_MSG_WriteDeltaEntity(msg, &sv.svEntities[newnum].baseline.s, newent, 1, client, newnum);
			newindex++;
		}
		else if ( oldnum < newnum )
		{
			custom_MSG_WriteDeltaEntity(msg, oldent, NULL, 1, client, newnum);
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
	data = LargeLocalGetBuf(&buf);
	while ( client->state != CS_FREE && client->netchan.unsentFragments )
		SV_Netchan_TransmitNextFragment(&client->netchan);
	
 	Com_DPrintf("SV_SendClientGameState() for %s\n", client->name);
	Com_DPrintf("Going from CS_CONNECTED to CS_PRIMED for %s\n", client->name);
	
	client->state = CS_PRIMED;
	client->pureAuthentic = 0;
	client->gamestateMessageNum = client->netchan.outgoingSequence;
	
	/* New code start */
	#if COMPILE_CUSTOM_VOICE == 1
	player_pendingVoiceDataFrames[client - svs.clients] = 0.0;
	player_currentSoundTalker[client - svs.clients] = 0;
	player_currentSoundIndex[client - svs.clients] = 0;
	player_sentVoiceDataIndex[client - svs.clients] = 0;
	#endif
	memset(&player_objectives[client - svs.clients], 0, sizeof(player_objectives[client - svs.clients]));
	player_meleeHeightScale[client - svs.clients] = 1.0;
	player_meleeRangeScale[client - svs.clients] = 1.0;
	player_meleeWidthScale[client - svs.clients] = 1.0;
	player_fireThroughWalls[client - svs.clients] = 0;
	player_fireRangeScale[client - svs.clients] = 1.0;
	player_no_pickup[client - svs.clients] = 0;
	player_no_earthquakes[client - svs.clients] = 0;
	player_collision[client - svs.clients] = COLLISION_TEAM_BOTH;
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
		custom_MSG_WriteDeltaEntity(&msg, &nullstate, base, qtrue, client - svs.clients, start);
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
	char errorMessage[MAX_STRINGLENGTH];

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

qboolean custom_BG_IsWeaponValid(playerState_t *ps, unsigned int index)
{
	WeaponDef_t *weaponDef;

	if ( !BG_IsWeaponIndexValid(index) )
		return qfalse;

	if ( !COM_BitCheck(ps->weapons, index) ) // Player has weapon?
		return qfalse;

	weaponDef = BG_GetWeaponDef(index);
	/* New code: Fixed potential NULL-pointer crash */
	if ( !weaponDef )
		return 0;
	/* New code end */

	if ( ( ( weaponDef->inventoryType == WEAPINVENTORY_PRIMARY && ps->weaponslots[1] != index ) && ps->weaponslots[2] != index ) && weaponDef->altWeaponIndex != index )
		return qfalse;

	return qtrue;
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
	Q_vsnprintf(info->va_string[index], sizeof(info->va_string[0]), format, va);
	va_end(va);

	info->va_string[index][1023] = 0;

	return info->va_string[index];
}

void custom_SV_VerifyIwds_f(client_t *cl)
{
	if ( sv_pure->boolean )
	{
		if ( sv_verifyIwds->boolean )
		{
			hook_sv_verifyiwds_f->unhook();
			SV_VerifyIwds_f(cl);
			hook_sv_verifyiwds_f->hook();
		}
		else
		{
			cl->pureAuthentic = 1;
		}
	}
}

void custom_SV_ResetPureClient_f(client_t *cl)
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

void hook_RuntimeError_in_VM_Execute(const char *pos, int error_index, const char *error_message, const char *dialog_error_message)
{
	if ( developer->integer == 2 )
		RuntimeError_Debug(0, pos, error_index, error_message); // TODO: This (new) branch causes infinite error loops
	else
		RuntimeError(pos, error_index, error_message, dialog_error_message);
}

#if COMPILE_PLAYER == 1

void hook_gamestate_info(const char *format, ...)
{
	char s[MAX_STRINGLENGTH];
	va_list va;

	va_start(va, format);
	Q_vsnprintf(s, sizeof(s), format, va);
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
		if ( codecallback_standbutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_standbutton, 0);
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

int custom_ClientEndFrame(gentity_t *ent)
{
	hook_clientendframe->unhook();
	int (*ClientEndFrame)(gentity_t *ent);
	*(int *)&ClientEndFrame = hook_clientendframe->from;
	int ret = ClientEndFrame(ent);
	hook_clientendframe->hook();

	if ( ent->client->sess.state == STATE_PLAYING )
	{
		int num = ent - g_entities;

		if ( player_g_speed[num] > 0 )
			ent->client->ps.speed = player_g_speed[num];

		if ( player_g_gravity[num] > 0 )
			ent->client->ps.gravity = player_g_gravity[num];

		// Experimental slide bug fix
		if ( g_resetSlide->boolean )
		{
			if ( ( (ent->client->ps).pm_flags & PMF_SLIDING ) != 0 && (ent->client->ps).pm_time == 0 )
			{
				(ent->client->ps).pm_flags = (ent->client->ps).pm_flags & ~PMF_SLIDING;
			}
		}
	}

	return ret;
}

int custom_BG_PlayAnim(playerState_t *ps, int animNum, animBodyPart_t bodyPart, int forceDuration, qboolean setTimer, qboolean isContinue, qboolean force)
{
	hook_bg_playanim->unhook();

	int (*BG_PlayAnim)(playerState_t *ps, int animNum, animBodyPart_t bodyPart, int forceDuration, qboolean setTimer, qboolean isContinue, qboolean force);
	*(int *)&BG_PlayAnim = hook_bg_playanim->from;

	int duration;

	if ( !custom_animation[ps->clientNum] )
		duration = BG_PlayAnim(ps, animNum, bodyPart, forceDuration, setTimer, isContinue, force);
	else
		duration = BG_PlayAnim(ps, custom_animation[ps->clientNum], bodyPart, forceDuration, qtrue, isContinue, qtrue);

	hook_bg_playanim->hook();

	return duration;
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

bool SVC_callback(const char *str, const char *ip)
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
	if ( SVC_RateLimitAddress(from, 10, 1000) )
	{
		if ( !SVC_callback("RCON:ADDRESS", NET_AdrToString(from)) )
			Com_DPrintf("SVC_RemoteCommand: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
		return true;
	}

	if ( badRconPassword )
	{
		static leakyBucket_t bucket;

		// Make DoS via rcon impractical
		if ( SVC_RateLimit(&bucket, 10, 1000) )
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
	
	bool badRconPassword = !strlen(rcon_password->string) || strcmp(Cmd_Argv(1), rcon_password->string) != 0;
	
	if ( !sv_limitLocalRcon->boolean )
	{
		unsigned char *ip = from.ip;
	
		if ( !( ip[0] == 10 ||									// 10.0.0.0 – 10.255.255.255
			  ( ip[0] == 172 && ( ip[1] >= 16 && ip[1] <= 31 ) ) || // 172.16.0.0 – 172.31.255.255
			  ( ip[0] == 192 && ip[1] == 168 ) ) )					// 192.168.0.0 – 192.168.255.255
		{
			if ( SVC_ApplyRconLimit(from, badRconPassword) )
				return;
		}
	}
	else
	{
		if ( SVC_ApplyRconLimit(from, badRconPassword) )
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
		strcmp(Cmd_Argv(2), "devmap") != 0 && 
		strcmp(Cmd_Argv(2), "fast_restart") != 0 &&
		strcmp(Cmd_Argv(2), "map") != 0 && 
		strcmp(Cmd_Argv(2), "map_restart") != 0 && 
		strcmp(Cmd_Argv(2), "map_rotate") != 0
		)
	{
		/*
		The map commands would crash because the server would continue running
		scripts after executing gsc_utils_remotecommand() (via VM_Resume).
		A potential solution for getting these commands into the callback too
		could be to use something as follows in gsc_utils_remotecommand(), just
		like Script_map() does:

		level.finished = 2;
		level.savePersist = 0;
		Cbuf_ExecuteText(2, custom_va("map %s\n", <mapname>));
		*/

		stackPushInt((int)msg);
		stackPushArray();
		int args = Cmd_Argc();
		for ( int i = 2; i < args; i++ )
		{
			char tmp[MAX_STRINGLENGTH];
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
	if ( SVC_RateLimitAddress(from, 10, 1000) )
	{
		if ( !SVC_callback("CHALLENGE:ADDRESS", NET_AdrToString(from)) )
			Com_DPrintf("SV_GetChallenge: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
		return;
	}

	// Allow getchallenge to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit(&outboundLeakyBucket, 10, 100) )
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
	if ( SVC_RateLimitAddress(from, 10, 1000) )
	{
		Com_DPrintf("SV_DirectConnect: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
		return;
	}

	// Allow connect to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit(&outboundLeakyBucket, 10, 100) )
	{
		Com_DPrintf("SV_DirectConnect: rate limit exceeded, dropping request\n");
		return;
	}

	SV_DirectConnect(from);
}

void hook_SVC_Info(netadr_t from)
{
	// Prevent using getinfo as an amplifier
	if ( SVC_RateLimitAddress(from, 10, 1000) )
	{
		if ( !SVC_callback("INFO:ADDRESS", NET_AdrToString(from)) )
			Com_DPrintf("SVC_Info: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
		return;
	}

	// Allow getinfo to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit(&outboundLeakyBucket, 10, 100) )
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
	if ( SVC_RateLimitAddress(from, 10, 1000) )
	{
		if ( !SVC_callback("STATUS:ADDRESS", NET_AdrToString(from)) )
			Com_DPrintf("SVC_Status: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
		return;
	}

	// Allow getstatus to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit(&outboundLeakyBucket, 10, 100) )
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
	char map_check[MAX_OSPATH];
	char library_path[MAX_OSPATH];
	
	cvar_t *fs_homepath = Cvar_FindVar("fs_homepath");
	cvar_t *fs_game = Cvar_FindVar("fs_game");
	cvar_t *map = Cvar_FindVar("mapname");

	if ( strlen(fs_library->string) )
		strncpy(library_path, fs_library->string, sizeof(library_path));
	else
		snprintf(library_path, sizeof(library_path), "%s/%s/Library", fs_homepath->string, fs_game->string);

	Com_sprintf(map_check, MAX_OSPATH, "%s/%s.iwd", library_path, mapname);

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

	while ( ( dir_ent = readdir(dir) ) != NULL )
	{
		if ( strcmp(dir_ent->d_name, ".") == 0 || strcmp(dir_ent->d_name, "..") == 0 )
			continue;

		char fileDelete[MAX_OSPATH];
		Com_sprintf(fileDelete, MAX_OSPATH, "%s/%s/%s", fs_homepath->string, fs_game->string, dir_ent->d_name);

		if ( access(fileDelete, F_OK) != -1 )
		{
			int unlink_success = unlink(fileDelete) == 0;
			printf("manymaps> REMOVED OLD LINK: %s result of unlink: %s\n", fileDelete, unlink_success?"success":"failed");
		}
	}

	closedir(dir);

	if ( map_exists )
	{
		char src[MAX_OSPATH];
		char dst[MAX_OSPATH];

		Com_sprintf(src, MAX_OSPATH, "%s/%s.iwd", library_path, mapname);
		Com_sprintf(dst, MAX_OSPATH, "%s/%s/%s.iwd", fs_homepath->string, fs_game->string, mapname);

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

void custom_Com_DPrintf(const char *format, ...)
{
	char s[MAX_STRINGLENGTH];
	va_list va;

	va_start(va, format);
	Q_vsnprintf(s, sizeof(s), format, va);
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

void Scr_CodeCallback_Error(qboolean terminal, qboolean emit, const char *internal_function, char *message)
{
	if ( codecallback_error && Scr_IsSystemActive() && !com_errorEntered )
	{
		if ( !strncmp(message, "exceeded maximum number of script variables", 43) )
		{
			/* Since we cannot allocate more script variables, further
			   execution of scripts or script callbacks could lead to an
			   undefined state (in script) or endless error loops, so we stop */
			Com_Error(1, "\x15%s", "exceeded maximum number of script variables");
		}

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
			   so we can emit it later at G_RunFrame which is a rather safe
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

void custom_Com_Error(int errorLevel, const char *error, ...)
{
	Sys_EnterCriticalSectionInternal(CRITSECT_COM_ERROR);
	
	#if COD_VERSION == COD2_1_0 // Not tested
	int va_string_156 = 0x0;
	int unk1 = 0x0;
	#elif COD_VERSION == COD2_1_2 // Not tested
	int va_string_156 = 0x0;
	int unk1 = 0x0;
	#elif COD_VERSION == COD2_1_3
	int va_string_156 = 0x081A2280;
	int unk1 = 0x081A327F;
	#endif
	
	if ( com_errorEntered )
	{
		Sys_Error("recursive error after: %s", (char *)va_string_156);
	}
	com_errorEntered = 1;
	
	va_list va;
	va_start(va, error);
	Q_vsnprintf((char *)va_string_156, 0x1000, error, va);
	va_end(va);
	
	Scr_CodeCallback_Error(scrVmPub.terminal_error, qtrue, "Com_Error", (char *)va_string_156);
	
	*(byte *)unk1 = 0;
	
	if ( errorLevel != 1 )
	{
		if ( errorLevel == 4 || errorLevel == 6 )
			errorLevel = 1;
		else if ( errorLevel == 5 )
			errorLevel = 1;
		else
			com_fixedConsolePosition = 0;
	}
	
	errorcode = errorLevel;
	
	Sys_LeaveCriticalSectionInternal(CRITSECT_COM_ERROR);

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
			Scr_CodeCallback_Error(qfalse, qfalse, "Scr_ErrorInternal", (char *)scrVarPub.error_message); // New
			longjmp(&g_script_error[g_script_error_level], -1);
		}
	}
	else if ( !scrVmPub.terminal_error )
	{
		return;
	}
	Com_Error(1, "\x15%s", scrVarPub.error_message);
}

void custom_RuntimeError_Debug(int channel, const char *pos, int index, const char *message)
{
	int i, j;

	Scr_CodeCallback_Error(qfalse, qfalse, "RuntimeError_Debug", (char *)message); // New
	Com_PrintMessage(channel, custom_va("\n******* script runtime error *******\n%s: ", message));
	Scr_PrintPrevCodePos(channel, pos, index);
	i = scrVmPub.function_count;
	if ( scrVmPub.function_count )
	{
		while ( j = i - 1, 0 < j )
		{
			Com_PrintMessage(channel, "called from:\n");
			Scr_PrintPrevCodePos(channel, (char *)scrVmPub.function_frame_start[i - 1].fs.pos, scrVmPub.function_frame_start[i - 1].fs.localId == 0);
			i = j;
		}
		Com_PrintMessage(channel, "started from:\n");
		Scr_PrintPrevCodePos(channel, (char *)scrVmPub.function_frame_start[0].fs.pos, 1);
	}
	Com_PrintMessage(channel, "************************************\n");
}

void custom_RuntimeError(const char *pos, int index, const char *message, const char *dialog_error)
{
	int channel_1, channel_2;
	char newline[2];
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
				Scr_CodeCallback_Error(qfalse, qfalse, "RuntimeError", (char *)message);
				return;
			}
		}
		if ( !dialog_error )
		{
			newline[0] = '\0';
			dialog_error = "";
		}
		else
		{
			newline[0] = '\n';
			newline[1] = '\0';
		}
		if ( !scrVmPub.terminal_error )
			channel_1 = 4;
		else
			channel_1 = 5;
		
		Com_Error(channel_1, "\x15script runtime error\n(see console for details)\n%s%s%s", message, newline, dialog_error);
	}
}

void Scr_CodeCallback_NotifyDebug(unsigned int entId, char *message, unsigned int argc, SavedVariableValue *arguments)
{
	if ( !level.num_entities )
	{
		// Avoid crash in SV_FreeClientScriptPers logic on map switch where Scr_IsSystemActive still returns true
		return;
	}

	if ( Scr_IsSystemActive() )
	{
		if ( !argc || !arguments || !arguments->type || arguments->type == STACK_PRECODEPOS )
		{
			stackPushUndefined();
		}
		else
		{
			stackPushArray();
			for ( unsigned int i = 0; i < argc; i++ )
			{
				SavedVariableValue *arg = arguments + i;
				switch(arg->type) {
					case STACK_UNDEFINED: stackPushUndefined(); break;
					case STACK_OBJECT: stackPushObject(arg->u.pointerValue); RemoveRefToObject(arg->u.pointerValue); break;
					case STACK_STRING:
					case STACK_LOCALIZED_STRING: stackPushString(arg->u.stringValue); break;
					case STACK_VECTOR: stackPushVector(arg->u.vectorValue); break;
					case STACK_FLOAT: stackPushFloat(arg->u.floatValue); break;
					case STACK_INT: stackPushInt(arg->u.intValue); break;
					case STACK_FUNCTION: stackPushFunc(arg->u.codePosValue); break;
				}
				stackPushArrayLast();
			}
		}
		stackPushString(message);
		stackPushObject(entId);
		short ret = Scr_ExecThread(codecallback_notifydebug, 3);
		Scr_FreeThread(ret);
	}
}

void custom_G_RunFrame(int levelTime)
{
	int i;
	
	hook_g_runframe->unhook();
	void (*G_RunFrame)(int levelTime);
	*(int *)&G_RunFrame = hook_g_runframe->from;

	// Warn about server lag
	if ( hitchFrameTime && Scr_IsSystemActive() )
	{
		stackPushInt(hitchFrameTime);
		short ret = Scr_ExecThread(codecallback_hitchwarning, 1);
		Scr_FreeThread(ret);
		hitchFrameTime = 0;
	}

	// Process CodeCallback_Error queue
	for ( i = 0; i < scr_errors_index; i++ )
	{
		Scr_CodeCallback_Error(qfalse, qtrue, scr_errors[i].internal_function, scr_errors[i].message);
	}
	scr_errors_index = 0;

	// Process CodeCallback_NotifyDebug queue
	for ( i = 0; i < scr_notify_index; i++ )
	{
		Scr_CodeCallback_NotifyDebug(scr_notify[i].entId, scr_notify[i].message, scr_notify[i].argc, &scr_notify[i].arguments[0]);
		RemoveRefToObject(scr_notify[i].entId);
		memset(&scr_notify[i], 0, sizeof(scr_notify[0]));
	}
	scr_notify_index = 0;

#if COMPILE_CUSTOM_VOICE == 1
	// Try process results from Speex encoder tasks
	if ( pthread_mutex_trylock(&loadSoundFileResultLock) == 0 && Scr_IsSystemActive() )
	{
		if ( loadSoundFileResultsIndex == MAX_THREAD_RESULTS_BUFFER )
		{
			Com_Printf("Warning: loadSoundFile results buffer full\n");
		}
		for ( i = 0; i < loadSoundFileResultsIndex; i++ )
		{
			stackPushInt(loadSoundFileResults[i].result);
			stackPushInt(loadSoundFileResults[i].soundIndex);
			short ret = Scr_ExecThread(loadSoundFileResults[i].callback, 2);
			Scr_FreeThread(ret);
		}
		loadSoundFileResultsIndex = 0;
		pthread_mutex_unlock(&loadSoundFileResultLock);
	}

	// Process custom voice data queue
	qboolean aPlayerIsTalking = qfalse;

	if ( sv_voice->boolean )
	{
		int durationSinceLastTalk;
		gclient_t *gclient = level.clients;
		for ( i = 0; i < sv_maxclients->integer; i++, gclient++ )
		{
			durationSinceLastTalk = level.time - gclient->playerTalkTime;
			if ( durationSinceLastTalk >= 0 && g_voiceChatTalkingDuration->integer > durationSinceLastTalk )
			{
				aPlayerIsTalking = qtrue;
				break;
			}
		}
	}

	if ( !aPlayerIsTalking ) // TODO: Use SV_ClientHasClientMuted() here to only pause if a non-muted player talks
	{
		int id;
		client_t *client = svs.clients;
		for ( i = 0; i < sv_maxclients->integer; i++, client++ )
		{
			id = client - svs.clients;
			if ( client->state < CS_CONNECTED )
				continue;

			if ( player_currentSoundIndex[id] )
			{
				player_pendingVoiceDataFrames[id] += MAX_VOICEPACKETSPERFRAME; // 51.2 packets per second @ 20 server fps
				VoicePacket_t *voicePacket;

				for ( ; player_pendingVoiceDataFrames[id] > 1.0 && player_sentVoiceDataIndex[id] < MAX_STOREDVOICEPACKETS; player_sentVoiceDataIndex[id]++, player_pendingVoiceDataFrames[id] -= 1.0 )
				{
					voicePacket = &voiceDataStore[player_currentSoundIndex[id] - 1][player_sentVoiceDataIndex[id]];
					if ( svs.clients[player_currentSoundTalker[id]].state < CS_CONNECTED || !voicePacket->dataLen )
					{
						player_pendingVoiceDataFrames[id] = 0.0;
						player_currentSoundTalker[id] = 0;
						player_currentSoundIndex[id] = 0;
						player_sentVoiceDataIndex[id] = 0;
						Scr_Notify(&g_entities[id], scr_const.sound_file_done, 0);
						break;
					}
					voicePacket->talkerNum = player_currentSoundTalker[id];
					SV_QueueVoicePacket(voicePacket->talkerNum, id, voicePacket);
				}
			}
		}
	}
#endif

	G_RunFrame(levelTime);
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
	data = LargeLocalGetBuf(&buf);
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
							Com_Error(0, "\x15svs.nextCachedSnapshotClients wrapped");
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
							Com_Error(0, "\x15svs.nextCachedSnapshotEntities wrapped");
						cachedFrame->num_entities++;
					}
				}
				svs.nextCachedSnapshotFrames++;
				if ( i != 0x7ffffffc && 0x7ffffffc < svs.nextCachedSnapshotFrames )
					Com_Error(0, "\x15svs.nextCachedSnapshotFrames wrapped");
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
					( gent->r.broadcastTime != 0 || ( ( gent->r.svFlags & 1 ) == 0 && ( svEnt = SV_SvEntityForGentity(gent), ( gent->r.svFlags & 0x18 ) != 0 || svEnt->numClusters != 0 ) ) ) )
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
						if ( ( to.s.eType - 10 ) == EV_EARTHQUAKE )
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
					Com_Error(0, "\x15svs.nextArchivedSnapshotBuffer wrapped");
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
					Com_Error(0, "\x15svs.nextArchivedSnapshotFrames wrapped");
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
				Com_Error(1, "\x15SV_BuildClientSnapshot: bad gEnt");
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
						Com_Error(0, "\x15svs.nextSnapshotEntities wrapped");
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
							Com_Error(0, "\x15svs.nextSnapshotClients wrapped");
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
								aent->s.eType = (entityType_t)EV_NONE;
						}
						else
						{
							if ( aent->r.clientMask[0] & (1 << clientNum ) )
								aent->s.eType = (entityType_t)EV_NONE;
						}
					}
					/* New code end */
					
					svs.nextSnapshotEntities++;
					if ( svs.nextSnapshotEntities >= 0x7ffffffe )
						Com_Error(0, "\x15svs.nextSnapshotEntities wrapped");
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
						Com_Error(0, "\x15svs.nextSnapshotClients wrapped");
					frame->num_clients++;
				}
			}
		}
	}
}

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
		Com_Error(1, "\x15SpawnEntities: no entities");

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
	void (*Scr_LoadGameType)(void);
	*(int *)&Scr_LoadGameType = hook_scr_loadgametype->from;

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

	Scr_LoadGameType();
	hook_scr_loadgametype->hook();
}

bool custom_CM_IsBadStaticModel(cStaticModel_t *model, char *src, float *origin, float *angles, float (*scale) [3])
{
	XModel_t *xmodel;

	if ( src == NULL || *src == '\0' )
		Com_Error(1, "\x15Invalid static model name\n");
	
	if ( (*scale)[0] == 0.0 )
		Com_Error(1, "\x15Static model [%s] has x scale of 0.0\n", src);
	
	if ( (*scale)[1] == 0.0 )
		Com_Error(1, "\x15Static model [%s] has y scale of 0.0\n", src);
	
	if ( (*scale)[2] == 0.0 )
		Com_Error(1, "\x15Static model [%s] has z scale of 0.0\n", src);
	
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

void custom_Player_UpdateCursorHints(gentity_t *player)
{
	int useListSize;
	int temp;
	WeaponDef_t *weaponDef;
	int i;
	int cursorHintString;
	int cursorHint;
	gentity_t *useList[2050];
	gentity_t *ent;
	gclient_t *client;

	client = player->client;
	(client->ps).cursorHint = 0;
	(client->ps).cursorHintString = -1;
	(client->ps).cursorHintEntIndex = ENTITY_NONE;
	if ( 0 < player->healthPoints && ( (player->client->ps).weaponstate < 0x11 || ( 0x16 < (player->client->ps).weaponstate ) ) )
	{
		if ( !player->active )
		{
			useListSize = Player_GetUseList(player, (useList_t *)useList);
			if ( ( (player->client->ps).pm_flags & PMF_MANTLE ) == 0 && useListSize )
			{
				cursorHint = 0;
				cursorHintString = -1;
				for ( i = 0; i < useListSize; i++ )
				{
					ent = useList[i * 2];
					temp = (ent->s).eType;
					if ( temp == ET_ITEM )
					{
						temp = BG_GetItemHintString(player->client, ent);
						if ( temp != 0 )
							goto LAB_08121ee6;
					}
					else if ( temp < ET_MISSILE )
					{
						if ( temp == ET_GENERAL )
						{
							if ( ent->classname != scr_const.trigger_use && ( temp = cursorHint, ent->classname != scr_const.trigger_use_touch ) )
							{
LAB_08121ee6:
								cursorHint = temp;
								(client->ps).cursorHintEntIndex = (ent->s).number;
								(client->ps).cursorHint = cursorHint;
								(client->ps).cursorHintString = cursorHintString;
								if ( (client->ps).cursorHint != 0 )
								{
									return;
								}
								(client->ps).cursorHintEntIndex = ENTITY_NONE;
								return;
							}

							/* New code start: hintString support for trigger_radius */
							if ( customEntityState[(ent->s).number].convertedTrigger )
							{
								Scr_AddEntity(player);
								Scr_Notify(ent, scr_const.trigger, 1);
							}
							/* New code end */

							if ( ( ent->team == 0 || ent->team == (player->client->sess).team ) &&
							( (ent->params).trigger.damage == ENTITY_NONE || (ent->params).trigger.damage == (player->client->ps).clientNum ) )
							{
								temp = (ent->s).dmgFlags;
								if ( (ent->s).dmgFlags != 0 && (ent->s).scale != 0xFF )
								{
									cursorHintString = (ent->s).scale;
								}
								goto LAB_08121ee6;
							}
						}
					}
					else if ( temp == ET_TURRET && G_IsTurretUsable(ent, player) )
					{
						temp = (ent->s).weapon + 4;
						weaponDef = BG_GetWeaponDef((ent->s).weapon);
						if ( *weaponDef->szUseHintString != '\0' )
						{
							weaponDef = BG_GetWeaponDef((ent->s).weapon);
							cursorHintString = weaponDef->iUseHintStringIndex;
						}
						goto LAB_08121ee6;
					}
				}
			}
		}
		else if ( ( (client->ps).eFlags & EF_USETURRET ) != 0 )
		{
			Player_SetTurretDropHintString(player);
		}
	}
}

void custom_Script_clonePlayer(scr_entref_t ref)
{
	int id = ref.entnum;

	hook_script_cloneplayer->unhook();
	void (*Script_clonePlayer)(scr_entref_t ref);
	*(int *)&Script_clonePlayer = hook_script_cloneplayer->from;

	if ( id >= MAX_CLIENTS )
	{
		stackError("clonePlayer() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	gentity_t *entity = &g_entities[id];
	if ( !Com_GetServerDObj((entity->client->ps).clientNum) )
	{
		stackPushUndefined();
	}
	else
	{
		Script_clonePlayer(ref);
	}

	hook_script_cloneplayer->hook();
}

void custom_Script_bulletTrace(void)
{
	int args;
	int hitCharacters;
	int type;
	gentity_t *passEnt;
	vec3_t normal;
	vec3_t position;
	trace_t trace;
	int contentmask;
	int passEntityNum;
	vec3_t end;
	vec3_t start;

	passEntityNum = ENTITY_NONE;
	Scr_GetVector(0, &start);
	Scr_GetVector(1, &end);
	hitCharacters = Scr_GetInt(2);
	if ( hitCharacters == 0 )
	{
		contentmask = MASK_SHOT ^ CONTENTS_BODY;
	}
	else
	{
		contentmask = MASK_SHOT;
	}

	/* New code: bulletTrace contentmask override */
	args = Scr_GetNumParam();
	if ( args > 4 )
		contentmask = Scr_GetInt(4);
	/* New code end */

	type = Scr_GetType(3);
	if ( type == STACK_OBJECT )
	{
		type = Scr_GetPointerType(3);
		if ( type == STACK_ENTITY )
		{
			passEnt = Scr_GetEntity(3);
			passEntityNum = (passEnt->s).number;
		}
	}
	G_LocationalTrace(&trace, &start, &end, passEntityNum, contentmask, NULL);
	Scr_MakeArray();
	Scr_AddFloat(trace.fraction);
	Scr_AddArrayStringIndexed(scr_const.fraction);
	Vec3Lerp(start, end, trace.fraction, position);
	Scr_AddVector(position);
	Scr_AddArrayStringIndexed(scr_const.position);
	if ( trace.entityNum.entnum == ENTITY_NONE || trace.entityNum.entnum == ENTITY_WORLD )
	{
		Scr_AddUndefined();
	}
	else
	{
		Scr_AddEntity(&g_entities[trace.entityNum.entnum]);
	}
	Scr_AddArrayStringIndexed(scr_const.entity);
	if ( trace.fraction < 1.0 )
	{
		Scr_AddVector(trace.normal);
		Scr_AddArrayStringIndexed(scr_const.normal);
		Scr_AddString(Com_SurfaceTypeToName((int)(trace.surfaceFlags & 0x1f00000U) >> 0x14));
		Scr_AddArrayStringIndexed(scr_const.surfacetype);
		Scr_AddInt(trace.surfaceFlags);
		Scr_AddArrayStringIndexed(scr_const.flags);
	}
	else
	{
		VectorSubtract(end, start, normal);
		Vec3Normalize(normal);
		Scr_AddVector(normal);
		Scr_AddArrayStringIndexed(scr_const.normal);
		Scr_AddConstString(scr_const.none);
		Scr_AddArrayStringIndexed(scr_const.surfacetype);
		Scr_AddInt(0);
		Scr_AddArrayStringIndexed(scr_const.flags);
	}
}

void custom_Script_obituary(void)
{
	int args;
	int sWeapon;
	uint sMeansOfDeath;
	gentity_t *ent;
	gentity_t *victim;
	gentity_t *attacker;
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
		ent = G_TempEntity((vec3_t *)vec3_origin, EV_OBITUARY);
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
	if ( Scr_GetType(1) == STACK_OBJECT )
	{
		if ( Scr_GetPointerType(1) == STACK_ENTITY )
		{
			attacker = Scr_GetEntity(1);
			ent->s.attackerEntityNum = attacker->s.number;
			goto LAB_081131e1;
		}
	}
	ent->s.attackerEntityNum = ENTITY_WORLD;
LAB_081131e1:
	ent->r.svFlags = 8;
	if ( sMeansOfDeath == MOD_MELEE || sMeansOfDeath == MOD_HEAD_SHOT || sMeansOfDeath == MOD_SUICIDE || sMeansOfDeath == MOD_FALLING || sMeansOfDeath == MOD_CRUSH )
		ent->s.eventParm = sMeansOfDeath | 0x80;
	else
		ent->s.eventParm = sWeapon;
}

void custom_Script_setHintString(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *ent;
	char hintString[MAX_STRINGLENGTH];
	int index;

	ent = G_GetEntity(id);
	if ( ( ent->classname != scr_const.trigger_radius ) && ( ent->classname != scr_const.trigger_use ) && ( ent->classname != scr_const.trigger_use_touch ) )
	{
		Scr_Error("The setHintString command only works on trigger_radius, trigger_use or trigger_use_touch entities.\n");
	}

	if ( Scr_GetType(0) == STACK_STRING )
	{
		if ( I_stricmp(Scr_GetString(0), "") == 0 )
		{
			(ent->s).scale = 0xff;
			return;
		}
	}

	Scr_ConstructMessageString(0, Scr_GetNumParam() + -1, "Hint String", hintString, MAX_STRINGLENGTH);
	if ( G_GetHintStringIndex(&index, hintString) == 0 )
	{
		Scr_Error(custom_va("Too many different hintstring values. Max allowed is %i different strings", 0x20));
	}
	(ent->s).scale = index;

	// Added trigger_radius support by converting it to a trigger_use_touch
	if ( ent->classname == scr_const.trigger_radius )
	{
		Scr_SetString(&ent->classname, scr_const.trigger_use_touch);
		(ent->params).trigger.damage = ENTITY_NONE;
		(ent->r).contents = CONTENTS_DONOTENTER;
		(ent->r).svFlags = 1;
		(ent->s).dmgFlags = 2;
		// Set it to a 'converted' trigger to still emit 'trigger' notifies
		customEntityState[(ent->s).number].convertedTrigger = qtrue;
	}
}

qboolean custom_SV_MapExists(const char *name)
{
	// First try stock mechanism
	qboolean found = FS_ReadFile(custom_va("maps/mp/%s.%s", SV_GetMapBaseName(name), GetBspExtension()), 0) >= 0;
	if ( !found )
	{
		char map_check[MAX_OSPATH];
		char library_path[MAX_OSPATH];
		
		cvar_t *fs_homepath = Cvar_FindVar("fs_homepath");
		cvar_t *fs_game = Cvar_FindVar("fs_game");

		if ( strlen(fs_library->string) )
			strncpy(library_path, fs_library->string, sizeof(library_path));
		else
			snprintf(library_path, sizeof(library_path), "%s/%s/Library", fs_homepath->string, fs_game->string);

		Com_sprintf(map_check, MAX_OSPATH, "%s/%s.iwd", library_path, name);

		return access(map_check, F_OK) != -1;
	}
	else
	{
		return qtrue;
	}
}

void Scr_QueueNotifyDebugForCallback(unsigned int entId, unsigned int constString, VariableValue *arguments)
{
	if ( scr_notify_index < MAX_NOTIFY_DEBUG_BUFFER )
	{
		char *message = SL_ConvertToString(constString);
		VariableValue *arg;
		SavedVariableValue *savedArg;
		unsigned int argc = 0;
		char *stringValueSrc;

		AddRefToObject(entId);
		scr_notify[scr_notify_index].entId = entId;
		I_strncpyz(scr_notify[scr_notify_index].message, message, strlen(message) + 1);
		for ( arg = arguments; arg->type != STACK_PRECODEPOS && argc < MAX_NOTIFY_DEBUG_PARAMS; arg-- )
		{
			savedArg = &scr_notify[scr_notify_index].arguments[argc];
			savedArg->type = arg->type;
			switch(savedArg->type) {
				case STACK_UNDEFINED: break;
				case STACK_OBJECT: AddRefToObject(arg->u.pointerValue); savedArg->u.pointerValue = arg->u.pointerValue; break;
				case STACK_STRING:
				case STACK_LOCALIZED_STRING:
					stringValueSrc = SL_ConvertToString(arg->u.stringValue);
					I_strncpyz(savedArg->u.stringValue, stringValueSrc, strlen(stringValueSrc) + 1);
					break;
				case STACK_VECTOR: VectorCopy(arg->u.vectorValue, savedArg->u.vectorValue); break;
				case STACK_FLOAT: savedArg->u.floatValue = arg->u.floatValue; break;
				case STACK_INT: savedArg->u.intValue = arg->u.intValue; break;
				case STACK_FUNCTION: savedArg->u.codePosValue = arg->u.codePosValue; break;
				default:
					printf("Warning: Notify debug with param %d of type 0x%x is currently not supported for CodeCallback_NotifyDebug\n", argc + 1, savedArg->type);
					savedArg->type = STACK_UNDEFINED;
			}
			argc++;
		}
		scr_notify[scr_notify_index].argc = argc;
		scr_notify_index++;
	}
	else
	{
		printf("Warning: Notify debug buffer full, not calling CodeCallback_NotifyDebug for '%s'\n", SL_ConvertToString(constString));
	}
}

void custom_VM_Notify(unsigned int entId, unsigned int constString, VariableValue *arguments)
{
	if ( codecallback_notifydebug )
		Scr_QueueNotifyDebugForCallback(entId, constString, arguments);

	hook_vm_notify->unhook();
	void (*VM_Notify)(unsigned int entId, unsigned int constString, VariableValue *arguments);
	*(int *)&VM_Notify = hook_vm_notify->from;
	VM_Notify(entId, constString, arguments);
	hook_vm_notify->hook();
}

void custom_Scr_Notify(gentity_t *ent, unsigned short constString, unsigned int numArgs)
{
	char *message = SL_ConvertToString(constString);
	char messageStr[MAX_STRINGLENGTH] = {0};
	SavedVariableValue savedArgs[numArgs];
	char *stringValueSrc;

	if ( codecallback_notify && Scr_IsSystemActive() )
	{
		// Save message and stack for CodeCallback_Notify
		I_strncpyz(messageStr, message, strlen(message) + 1);
		VariableValue *arg;
		SavedVariableValue *savedArg;
		for ( unsigned int i = 0; i < numArgs; i++ )
		{
			arg = scrVmPub.top - i;
			savedArg = &savedArgs[i];
			savedArg->type = arg->type;
			switch(savedArg->type) {
				case STACK_UNDEFINED: break;
				case STACK_OBJECT: AddRefToObject(arg->u.pointerValue); savedArg->u.pointerValue = arg->u.pointerValue; break;
				case STACK_STRING:
				case STACK_LOCALIZED_STRING:
					stringValueSrc = SL_ConvertToString(arg->u.stringValue);
					I_strncpyz(savedArg->u.stringValue, stringValueSrc, strlen(stringValueSrc) + 1);
					break;
				case STACK_VECTOR: VectorCopy(arg->u.vectorValue, savedArg->u.vectorValue); break;
				case STACK_FLOAT: savedArg->u.floatValue = arg->u.floatValue; break;
				case STACK_INT: savedArg->u.intValue = arg->u.intValue; break;
				case STACK_FUNCTION: savedArg->u.codePosValue = arg->u.codePosValue; break;
				default:
					printf("Warning: Notify with param %d of type 0x%x is currently not supported for CodeCallback_Notify\n", i + 1, savedArg->type);
					savedArg->type = STACK_UNDEFINED;
			}
		}
	}

	hook_scr_notify->unhook();
	void (*Scr_Notify)(gentity_t *ent, unsigned short constString, unsigned int numArgs);
	*(int *)&Scr_Notify = hook_scr_notify->from;
	// Execute Scr_Notify -> Scr_NotifyNum -> VM_Notify
	Scr_Notify(ent, constString, numArgs);

	if ( codecallback_notify && Scr_IsSystemActive() )
	{
		// Restore stack for CodeCallback_Notify
		stackPushArray();
		for ( unsigned int i = 0; i < numArgs; i++ )
		{
			SavedVariableValue *arg = &savedArgs[i];
			switch(arg->type) {
				case STACK_UNDEFINED: stackPushUndefined(); break;
				case STACK_OBJECT: stackPushObject(arg->u.pointerValue); RemoveRefToObject(arg->u.pointerValue); break;
				case STACK_STRING:
				case STACK_LOCALIZED_STRING: stackPushString(arg->u.stringValue); break;
				case STACK_VECTOR: stackPushVector(arg->u.vectorValue); break;
				case STACK_FLOAT: stackPushFloat(arg->u.floatValue); break;
				case STACK_INT: stackPushInt(arg->u.intValue); break;
				case STACK_FUNCTION: stackPushFunc(arg->u.codePosValue); break;
			}
			stackPushArrayLast();
		}
		stackPushString(messageStr);
		short ret = Scr_ExecEntThread(ent, codecallback_notify, 2);
		Scr_FreeThread(ret);
	}
	
	hook_scr_notify->hook();
}

void G_UpdateSingleObjective(objective_t *from, objective_t *to)
{
	from->state = to->state;
	from->origin[0] = to->origin[0];
	from->origin[1] = to->origin[1];
	from->origin[2] = to->origin[2];
	from->entNum = to->entNum;
	from->teamNum = to->teamNum;
	from->icon = to->icon;
}

void custom_G_UpdateObjectives(void)
{
	int i, j;
	gclient_s *client;
	sessionTeam_t team;
	objective_t *obj;

	for ( i = 0; i < level.maxclients; i++ )
	{
		if ( level.gentities[i].r.inuse != 0 )
		{
			client = level.gentities[i].client;
			team = ((level.gentities[i].client)->sess).team;
			for ( j = 0; j < 16; j++ )
			{
				/* New code start: per-player objective functions */
				obj = &player_objectives[i][j];
				if ( obj->state != OBJST_EMPTY )
				{
					G_UpdateSingleObjective(&(client->ps).objective[j], obj);
				}
				else
				{
				/* New code end */
					obj = &level.objectives[j];
					if ( obj->state == OBJST_EMPTY || ( obj->teamNum != 0 &&  obj->teamNum != team ) )
					{
						(client->ps).objective[j].state = OBJST_EMPTY;
					}
					else
					{
						G_UpdateSingleObjective(&(client->ps).objective[j], obj);
					}
				}
			}
		}
	}
}

void custom_FireWeaponMelee(gentity_t *player)
{
	weaponParms wp;
	int id;
	float range, width, height;

	if ( (player->client->ps.eFlags & EF_USETURRET) == 0 || player->active == 0 )
	{
		id = player->client->ps.clientNum;
		wp.weapDef = BG_GetWeaponDef(player->s.weapon);
		G_GetPlayerViewOrigin(player, wp.muzzleTrace);
		G_GetPlayerViewDirection(player, wp.forward, wp.right, wp.up);

		/* Stock values from G_RegisterCvars:
		player_meleeRange = Cvar_RegisterFloat("player_meleeRange", 64.0, 0.0, 1000.0, CVAR_CHEAT | CVAR_UNSAFE);
		player_meleeWidth = Cvar_RegisterFloat("player_meleeWidth", 10.0, 0.0, 1000.0, CVAR_CHEAT | CVAR_UNSAFE);
		player_meleeHeight = Cvar_RegisterFloat("player_meleeHeight", 10.0, 0.0, 1000.0, CVAR_CHEAT | CVAR_UNSAFE);
		*/

		/* New code start: per-player melee values */
		range = player_meleeRange->floatval * player_meleeRangeScale[id];
		width = player_meleeWidth->floatval * player_meleeWidthScale[id];
		height = player_meleeHeight->floatval * player_meleeHeightScale[id];
		/* New code end */

		Weapon_Melee(player, &wp, range, width, height);
	}
}

void bullet_fire_extended_trace(trace_t *results, const vec3_t *start, const vec3_t *end, int passEntityNum, int contentmask, uint8_t *priorityMap)
{
	if ( passEntityNum < 64 && player_fireThroughWalls[passEntityNum] )
	{
		// Recreate bullet hit effect that would otherwise be missing with that mask
		trace_t trace;
		gentity_t *hitEffect;
		vec3_t origin;

		G_LocationalTrace(&trace, start, end, passEntityNum, contentmask, priorityMap);
		Vec3Lerp((float *)start, (float *)end, trace.fraction, origin);
		hitEffect = G_TempEntity(&origin, EV_SHOTGUN_HIT);
		hitEffect->s.eventParm = DirToByte(trace.normal) & 0xFF;
		hitEffect->s.surfType = (trace.surfaceFlags >> 20) & 0x1F;

		// Set contentmask to only hit player bodies (default: MASK_SHOT)
		contentmask = CONTENTS_BODY;
	}
	G_LocationalTrace(results, start, end, passEntityNum, contentmask, priorityMap);
}

void custom_Bullet_Fire_Spread(gentity_t *source, gentity_t *inflictor, weaponParms *wp, int offset, float spread) // Guessed function name
{
	int i;
	vec3_t start, end;

	/* New code start: per-player min. fire distance */
	int id = inflictor->client->ps.clientNum;
	float distance = wp->weapDef->fMinDamageRange * player_fireRangeScale[id];
	/* New code end */

	VectorCopy(wp->muzzleTrace, start);
	for ( i = 0; i < wp->weapDef->shotCount; i++ )
	{
		Bullet_Endpos(spread, end, wp, distance);
		Bullet_Fire_Extended(source, inflictor, &start, &end, 1.0, 0, wp, source, offset);
	}
}

void custom_Bullet_Fire(gentity_t *inflictor, float spread, weaponParms *wp, gentity_t *source, int offset)
{
	antilagClientStore antilagStore;
	vec3_t end;

	G_AntiLagRewindClientPos(offset, &antilagStore);
	if ( wp->weapDef->weapClass == WEAPCLASS_SPREAD )
	{
		custom_Bullet_Fire_Spread(source, inflictor, wp, offset, spread);
	}
	else
	{
		/* New code start: per-player min. fire distance */
		int id = inflictor->client->ps.clientNum;
		float distance = 8192.0 * player_fireRangeScale[id];
		/* New code end */

		Bullet_Endpos(spread, end, wp, distance);
		Bullet_Fire_Extended(source, inflictor, &wp->muzzleTrace, &end, 1.0, 0, wp, source, offset);
	}
	G_AntiLag_RestoreClientPos(&antilagStore);
}

void custom_SV_QueueVoicePacket(int talkerNum, int clientNum, VoicePacket_t *voicePacket)
{
	/* New code start: voice chat dump */
	if ( voiceDataDumpFile && g_dumpVoiceData->boolean )
	{
		char voiceLogData[(256*4)+1];
		char voiceLogEntry[(256*4)+64]; // {"talker": "64", "client": "64", "data": ""}\n

		char *pos = voiceLogData;
		for ( int i = 0; i < voicePacket->dataLen; i++ )
		{
			pos += sprintf(pos, "\\x%02hhX", voicePacket->data[i]);
		}

		snprintf(voiceLogEntry, sizeof(voiceLogEntry), "{\"talker\": \"%02i\", \"client\": \"%02i\", \"data\": \"%s\"}\n", talkerNum, clientNum, voiceLogData);
		fwrite(&voiceLogEntry, strlen(voiceLogEntry), 1, voiceDataDumpFile);
		fflush(voiceDataDumpFile);
	}
	/* New code end */

	client_t *client = svs.clients;
	if ( client[clientNum].unsentVoiceData < MAX_VOICEPACKETS )
	{
		client[clientNum].voicedata[client[clientNum].unsentVoiceData].dataLen = voicePacket->dataLen;
		memcpy(client[clientNum].voicedata[client[clientNum].unsentVoiceData].data, voicePacket->data, voicePacket->dataLen);
		client[clientNum].voicedata[client[clientNum].unsentVoiceData].talkerNum = (char)talkerNum;
		client[clientNum].unsentVoiceData++;
	}
}

void openLogfile(qboolean reopen)
{
	time_t timer;
	struct tm *timeInfo;
	const char *timeString;
	char strippedTime[128];

	opening_qconsole = 1;

	time(&timer);
	timeInfo = localtime(&timer);
	timeString = asctime(timeInfo);
	strncpy(strippedTime, timeString, sizeof(strippedTime));
	strippedTime[strlen(timeString) - 1] = '\0';

	if ( logfile && reopen )
	{
		FS_FCloseFile(logfile);
	}

	if( logfileRotate->integer > 0 )
	{
		char logfilePath[512];
		char newLogfilePath[512];
		cvar_t *fs_homepath = Cvar_FindVar("fs_homepath");
		cvar_t *fs_game = Cvar_FindVar("fs_game");
		int maxFileIndex = logfileRotate->integer;
		int fileIndex = maxFileIndex;

		// Check existance of older log files
		while( fileIndex > 0 )
		{
			snprintf(logfilePath, sizeof(logfilePath), "%s/%s/%s.%d", fs_homepath->string, fs_game->string, logfileName->string, fileIndex);
			if ( access(logfilePath, F_OK) != -1 )
			{
				// Older log file exists, increment file extension unless it already has the max. index
				if ( fileIndex == maxFileIndex )
				{
					// Remove
					if ( unlink(logfilePath) != 0 )
					{
						printf("Warning: Failed to delete old log file '%s', aborting rotation\n", logfilePath);
						break;
					}
				}
				else
				{
					// Rename the file
					snprintf(newLogfilePath, sizeof(newLogfilePath), "%s/%s/%s.%d", fs_homepath->string, fs_game->string, logfileName->string, fileIndex + 1);
					if ( rename(logfilePath, newLogfilePath) != 0 )
					{
						printf("Warning: Failed to rotate existing log file '%s', aborting rotation\n", logfilePath);
						break;
					}
				}
			}
			fileIndex--;
		}

		// Target file already exists? Append .1 to extension
		snprintf(logfilePath, sizeof(logfilePath), "%s/%s/%s", fs_homepath->string, fs_game->string, logfileName->string);
		if ( access(logfilePath, F_OK) != -1 )
		{
			snprintf(newLogfilePath, sizeof(newLogfilePath), "%s/%s/%s.%d", fs_homepath->string, fs_game->string, logfileName->string, fileIndex + 1);
			if( rename(logfilePath, newLogfilePath) != 0 )
				printf("Warning: Failed to rotate existing log file '%s'\n", logfileName->string);
		}
	}

	logfile = FS_FOpenFileWrite(logfileName->string);
	I_strncpyz(openLogfileName, logfileName->string, MAX_OSPATH);

	opening_qconsole = 0;

	Com_Printf("logfile '%s' opened on %s\n", openLogfileName, strippedTime);
}

void custom_Com_PrintMessage(int /* print_msg_type_t */ channel, char *message)
{
	PbCaptureConsoleOutput(message, 0x1000);
	if ( rd_buffer == NULL )
	{
		/* Caret patch
		if ( *message == '^' && message[1] != '\0' )
		{
			message += 2;
		}
		*/
		if ( channel != 4 )
		{
			Sys_Print(message);
		}

		if ( com_logfile != NULL && com_logfile->integer != 0 )
		{
			Sys_EnterCriticalSectionInternal(CRITSECT_CONSOLE);
			if ( FS_Initialized() )
			{
				if ( logfile == 0 && opening_qconsole == 0 && logfileName->string )
				{
					openLogfile(qfalse);
				}
				else if ( logfile && !opening_qconsole && logfileName->string && strncmp(logfileName->string, openLogfileName, strlen(openLogfileName)) )
				{
					// logfileName cvar value changed since log file initialization
					openLogfile(qtrue);
				}
				if ( logfile != 0 )
				{
					FS_Write(message, strlen(message), logfile);
					if ( 1 < com_logfile->integer ) // "logfile" cvar
					{
						FS_Flush(logfile);
					}
				}
			}
			Sys_LeaveCriticalSectionInternal(CRITSECT_CONSOLE);
		}
	}
	else if ( channel != 4 )
	{
		// RCON output buffer
		Sys_EnterCriticalSectionInternal(CRITSECT_RD_BUFFER);
		if ( rd_buffersize - 1U < ( strlen(rd_buffer) + strlen(message) ) )
		{
			rd_flush(rd_buffer);
			*rd_buffer = '\0';
		}
		I_strncat(rd_buffer, rd_buffersize, message);
		Sys_LeaveCriticalSectionInternal(CRITSECT_RD_BUFFER);
	}
}

void custom_G_InitGentity(gentity_t *ent)
{
	memset(&customEntityState[(ent->s).number], 0, sizeof(customEntityState_t));

	hook_g_initgentity->unhook();
	void (*G_InitGentity)(gentity_t *ent);
	*(int *)&G_InitGentity = hook_g_initgentity->from;
	G_InitGentity(ent);
	hook_g_initgentity->hook();
}

void custom_G_FreeEntity(gentity_t *ent)
{
	// Find out from where ent is freed:
	//Com_Printf(">>> G_FreeEntity for entity %d from %p\n", (ent->s).number, __builtin_return_address(0));

	hook_g_freeentity->unhook();
	void (*G_FreeEntity)(gentity_t *ent);
	*(int *)&G_FreeEntity = hook_g_freeentity->from;
	G_FreeEntity(ent);
	hook_g_freeentity->hook();
}

qboolean G_BounceGrenade(gentity_t *ent,trace_t *trace) // G_BounceMissile as base
{
	int contents;
	double length;
	qboolean bounce;
	vec3_t angle;
	vec3_t planeNormal;
	vec3_t velocity;
	float dot;

	contents = SV_PointContents(&(ent->r).currentOrigin, -1, CONTENTS_WATER);
	BG_EvaluateTrajectoryDelta(&(ent->s).pos, 50 + (int)((float)(level.time - level.previousTime) * trace->fraction) + level.previousTime, velocity);
	dot = DotProduct(velocity, trace->normal);
	VectorMA(velocity, dot * -2.0, trace->normal, (ent->s).pos.trDelta);
	if ( 0.7 < trace->normal[2] )
	{
		(ent->s).groundEntityNum = (trace->entityNum).entnum;
	}
	if ( ( (ent->s).eFlags & EF_BOUNCE ) != 0 )
	{
		length = VectorLength(velocity);
		if ( 0.0 < length && ( dot <= 0.0 ) )
		{
			VectorScale((ent->s).pos.trDelta, ( customEntityState[(ent->s).number].perpendicularBounce - customEntityState[(ent->s).number].parallelBounce ) * ( dot / -length ) + customEntityState[(ent->s).number].parallelBounce, (ent->s).pos.trDelta);
		}

		if ( 0.7 < trace->normal[2] && VectorLength((ent->s).pos.trDelta) < 20.0 )
		{
			Scr_AddEntity(&g_entities[(trace->entityNum).entnum]);
			Scr_AddString(Com_SurfaceTypeToName((int)( trace->surfaceFlags & 0x1F00000U ) >> 0x14));
			Scr_Notify(ent, scr_const.land, 2);
			G_SetOrigin(ent, &(ent->r).currentOrigin);
			G_MissileLandAngles(ent, trace, angle, 1);
			G_SetAngle(ent, &angle);
			return qfalse;
		}
	}
	VectorScale(trace->normal, 0.1, planeNormal);
	if ( 0.0 < planeNormal[2] )
	{
		planeNormal[2] = 0.0;
	}
	VectorAdd((ent->r).currentOrigin, planeNormal, (ent->r).currentOrigin);
	VectorCopy((ent->r).currentOrigin, (ent->s).pos.trBase);
	(ent->s).pos.trTime = level.time;
	G_MissileLandAngles(ent, trace, angle, 0);
	VectorCopy(angle, (ent->s).apos.trBase);
	(ent->s).apos.trTime = level.time;
	if ( contents == 0 )
	{
		VectorSubtract((ent->s).pos.trDelta, velocity, velocity);
		if ( 100.0 < VectorLength(velocity) )
		{
			bounce = 1;
		}
		else
		{
			bounce = 0;
		}
	}
	else
	{
		bounce = 0;
	}
	return bounce;
}

void G_RunGravityModelAsGrenade(gentity_t *ent) // G_RunMissile as base
{
	double absDeltaZ;
	vec3_t lerpOrigin;
	trace_t trace2;
	trace_t trace;
	vec3_t origin;
	qboolean bounce;

	if ( ( (ent->s).pos.trType == TR_STATIONARY ) && ( (ent->s).groundEntityNum != ENTITY_WORLD ) )
	{
		VectorCopy((ent->r).currentOrigin, origin);
		origin[2] = origin[2] - 1.5;
		G_StartSolidTrace(&trace, (ent->r).currentOrigin, origin, (ent->s).number, ent->clipmask);
		if ( trace.fraction == 1.0 )
		{
			(ent->s).pos.trType = TR_GRAVITY;
			(ent->s).pos.trTime = level.time;
			(ent->s).pos.trDuration = 0;
			VectorCopy((ent->r).currentOrigin, (ent->s).pos.trBase);
			VectorClear((ent->s).pos.trDelta);
		}
	}
	BG_EvaluateTrajectory(&(ent->s).pos, level.time + 50, origin);
	absDeltaZ = (ent->s).pos.trDelta[2];
	if ( absDeltaZ < 0 )
		absDeltaZ *= -1;
	if ( ( absDeltaZ <= 30.0 ) || SV_PointContents(&(ent->r).currentOrigin, -1, CONTENTS_WATER) )
	{
		G_StartSolidTrace(&trace, (ent->r).currentOrigin, origin, (ent->s).number, ent->clipmask);
	}
	else
	{
		G_StartSolidTrace(&trace, (ent->r).currentOrigin, origin, (ent->s).number, ent->clipmask | CONTENTS_WATER);
	}
	if ( ( trace.surfaceFlags & 0x1F00000 ) == SURF_WATER )
	{
		G_StartSolidTrace(&trace, (ent->r).currentOrigin, origin, (ent->s).number, ent->clipmask);
	}
	if ( ( g_entities[trace.entityNum.entnum].flags & EF_TAGCONNECT ) != 0 )
	{
		G_StartSolidTraceNoContents(&trace, trace.entityNum.entnum, ent, origin);
	}
	Vec3Lerp((ent->r).currentOrigin, origin, trace.fraction, lerpOrigin);
	VectorCopy(lerpOrigin, (ent->r).currentOrigin);
	if ( ( ( (ent->s).eFlags & EF_BOUNCE ) != 0 ) && ( trace.fraction == 1.0 || ( trace.fraction < 1.0 && ( 0.7 < trace.normal[2] ) ) ) )
	{
		VectorCopy((ent->r).currentOrigin, origin);
		origin[2] = origin[2] - 1.5;
		G_StartSolidTrace(&trace2, (ent->r).currentOrigin, origin, (ent->s).number, ent->clipmask);
		if ( ( trace2.fraction != 1.0 ) && ( trace2.entityNum.entnum == ENTITY_WORLD ) )
		{
			trace.fraction = trace2.fraction;
			trace.normal[0] = trace2.normal[0];
			trace.normal[1] = trace2.normal[1];
			trace.normal[2] = trace2.normal[2];
			trace.surfaceFlags = trace2.surfaceFlags;
			trace.contents = trace2.contents;
			trace.material = trace2.material;
			trace.entityNum = trace2.entityNum;
			trace.hitId = trace2.hitId;
			trace.allsolid = trace2.allsolid;
			trace.startsolid = trace2.startsolid;
			Vec3Lerp((ent->r).currentOrigin, origin, trace2.fraction, lerpOrigin);
			(ent->s).pos.trBase[2] = (ent->s).pos.trBase[2] + ( ( lerpOrigin[2] + 1.5 ) - (ent->r).currentOrigin[2]);
			VectorCopy(lerpOrigin, (ent->r).currentOrigin);
			(ent->r).currentOrigin[2] = (ent->r).currentOrigin[2] + 1.5;
		}
	}
	SV_LinkEntity(ent);
	if ( trace.fraction != 1.0 )
	{
		bounce = G_BounceGrenade(ent, &trace);
		if ( bounce && trace.startsolid == 0)
		{
			Scr_AddEntity(&g_entities[trace.entityNum.entnum]);
			Scr_AddString(Com_SurfaceTypeToName((int)( trace.surfaceFlags & 0x1F00000U ) >> 0x14));
			Scr_Notify(ent, scr_const.bounce, 2);
		}
	}
}

void G_RunGravityModelAsItem(gentity_t *ent) // G_RunItem as base
{
	vec3_t subOrigin;
	vec3_t lerpOrigin;
	trace_t trace;
	vec3_t origin;

	if ( ( ( ( (ent->s).groundEntityNum == ENTITY_NONE ) || ( level.gentities[(ent->s).groundEntityNum].s.pos.trType != TR_STATIONARY ) ) && ( (ent->s).pos.trType != TR_GRAVITY ) ) &&
	( ( ( ent->spawnflags ^ 1) & 1) != 0 ) )
	{
		(ent->s).pos.trType = TR_GRAVITY;
		(ent->s).pos.trTime = level.time;
		VectorCopy((ent->r).currentOrigin, (ent->s).pos.trBase);
		VectorClear((ent->s).pos.trDelta);
	}
	if ( ( ( (ent->s).pos.trType == TR_STATIONARY ) || ( (ent->s).pos.trType == TR_GRAVITY_PAUSED ) ) || ( ent->tagInfo != NULL ) )
	{
		// Removed G_RunThink(ent);
	}
	else 
	{
		BG_EvaluateTrajectory(&(ent->s).pos, level.time + 50, origin);
		if ( Vec3DistanceSq(&(ent->r).currentOrigin, &origin) < 0.1 )
		{
			origin[2] = origin[2] - 1.0;
		}

		if ( customEntityState[(ent->s).number].collideModels )
			SV_Trace(&trace, &(ent->r).currentOrigin, &(ent->r).mins, &(ent->r).maxs, &origin, (ent->s).number, ent->clipmask, 1, NULL, 1);
		else
			SV_Trace(&trace, &(ent->r).currentOrigin, &(ent->r).mins, &(ent->r).maxs, &origin, (ent->s).number, ent->clipmask, 0, NULL, 1);
		if ( trace.fraction < 1.0 )
		{
			Vec3Lerp((ent->r).currentOrigin, origin, trace.fraction, lerpOrigin);
			if (( ( trace.startsolid == 0 ) && ( trace.fraction < 0.01 ) ) && ( trace.normal[2] < 0.5 ) )
			{
				VectorSubtract(origin, (ent->r).currentOrigin, subOrigin);
				VectorMA(origin, 1 - DotProduct(subOrigin, trace.normal), trace.normal, origin);
				if ( customEntityState[(ent->s).number].collideModels )
					SV_Trace(&trace, &(ent->r).currentOrigin, &(ent->r).mins, &(ent->r).maxs, &origin, (ent->s).number, ent->clipmask, 1, NULL, 1);
				else
					SV_Trace(&trace, &(ent->r).currentOrigin, &(ent->r).mins, &(ent->r).maxs, &origin, (ent->s).number, ent->clipmask, 0, NULL, 1);
				Vec3Lerp(lerpOrigin, origin, trace.fraction, lerpOrigin);
			}
			(ent->s).pos.trType = TR_LINEAR_STOP;
			(ent->s).pos.trTime = level.time;
			(ent->s).pos.trDuration = 50;
			VectorCopy((ent->r).currentOrigin, (ent->s).pos.trBase);
			VectorSubtract(lerpOrigin, (ent->r).currentOrigin, (ent->s).pos.trDelta);
			VectorScale((ent->s).pos.trDelta, 20.0, (ent->s).pos.trDelta);
			VectorCopy(lerpOrigin, (ent->r).currentOrigin);
		}
		else
		{
			VectorCopy(origin, (ent->r).currentOrigin);
		}
		SV_LinkEntity(ent);
		if ( ( (ent->r).inuse != 0 ) && ( trace.fraction < 0.01 ) )
		{
			if ( trace.normal[2] > 0.0 && !SV_PointContents(&(ent->r).currentOrigin, -1, CONTENTS_NODROP) )
			{
				vec3_t angles;
				vec3_t v1;
				vec3_t v2;
				vec3_t v3;
				
				VectorCopy(trace.normal, v3);
				AngleVectors(&(ent->r).currentAngles, &v1, 0, 0);
				VectorCross(v3, v1, v2);
				VectorCross(v2, v3, v1);
				AxisToAngles(v1, angles);
				G_SetAngle(ent, &angles);
				G_SetOrigin(ent, &lerpOrigin);
				if ( (ent->s).groundEntityNum != trace.entityNum.entnum )
				{
					Scr_AddEntity(&g_entities[trace.entityNum.entnum]);
					Scr_AddString(Com_SurfaceTypeToName((int)( trace.surfaceFlags & 0x1F00000U ) >> 0x14));
					Scr_Notify(ent, scr_const.land, 2);
				}
				(ent->s).groundEntityNum = trace.entityNum.entnum;
				SV_LinkEntity(ent);
			}
			else
			{
				G_FreeEntity(ent);
			}
		}
	}
}

void custom_G_RunFrameForEntity(gentity_t *ent)
{
	if ( ent->framenum != level.framenum )
	{
		ent->framenum = level.framenum;
		if ( ent->client == NULL )
		{
			if ( ( ent->flags & 0x800 ) == 0 )
			{
				(ent->s).eFlags = (ent->s).eFlags & 0xffffffdf;
			}
			else
			{
				(ent->s).eFlags = (ent->s).eFlags | 0x20;
			}
		}
		if ( ( (ent->s).eFlags == 0x10000 ) && ( (ent->s).time2 < level.time ) )
		{
			G_FreeEntity(ent);
		}
		else
		{
			if ( 300 < level.time - ent->eventTime )
			{
				if ( ent->freeAfterEvent != 0 )
				{
					G_FreeEntity(ent);
					return;
				}
				if ( ent->unlinkAfterEvent != 0 )
				{
					ent->unlinkAfterEvent = 0;
					SV_UnlinkEntity(ent);
				}
			}
			if ( ent->freeAfterEvent == 0 )
			{
				if ( (ent->s).eType == ET_MISSILE )
				{
					G_RunMissile(ent);
				}
				else if ( (ent->s).eType == ET_ITEM )
				{
					if ( ent->tagInfo == NULL )
					{
						G_RunItem(ent);
					}
					else
					{
						G_GeneralLink(ent);
						G_RunThink(ent);
					}
				}
				else if ( (ent->s).eType == ET_CORPSE )
				{
					G_RunCorpse(ent);
				}
				else if ( customEntityState[(ent->s).number].gravityType )
				{
					vec3_t oldOrigin;

					VectorCopy((ent->r).currentOrigin, oldOrigin);
					if ( customEntityState[(ent->s).number].gravityType == GRAVITY_ITEM )
						G_RunGravityModelAsItem(ent);
					else if ( customEntityState[(ent->s).number].gravityType == GRAVITY_GRENADE )
						G_RunGravityModelAsGrenade(ent);
					VectorSubtract((ent->r).currentOrigin, oldOrigin, customEntityState[(ent->s).number].velocity);
					VectorScale(customEntityState[(ent->s).number].velocity, 20.0, customEntityState[(ent->s).number].velocity);
				}
				else if ( ent->physicsObject == 0 )
				{
					if ( (ent->s).eType == ET_SCRIPTMOVER )
					{
						G_RunMover(ent);
					}
					else if ( ent->client == NULL )
					{
						if ( ( (ent->s).eType == ET_GENERAL ) && ( ent->tagInfo != NULL ) )
						{
							G_GeneralLink(ent);
						}
						G_RunThink(ent);
					}
					else
					{
						G_RunClient(ent);
					}
				}
				else
				{
					G_RunItem(ent);
				}
			}
		}
	}
}

qboolean custom_SV_ClientCommand(client_t *cl, msg_t *msg)
{
	int seq;
	char *s;
	qboolean clientOk;
	qboolean floodprotect;

	clientOk = 1;
	floodprotect = true;
	seq = MSG_ReadLong(msg);
	s = MSG_ReadString(msg);
	if ( seq <= cl->lastClientCommand )
	{
		return qtrue;
	}
	if ( sv_showCommands->boolean )
	{
		Com_Printf("clientCommand: %i : %s\n", seq, s);
	}
	if ( seq <= cl->lastClientCommand + 1 )
	{
		if ( !I_strncmp("team ", s, 5) || !I_strncmp("score ", s, 6) || !I_strncmp("mr ", s, 3) || !I_strncmp("userinfo ", s, 9) ) // New: Added userinfo
		{
			floodprotect = false;
		}
		if ( ( ( *(legacyHacks + 4) == 0 ) && ( CS_PRIMED < cl->state ) ) && ( sv_floodProtect->boolean && ( svs.time < cl->floodprotect && floodprotect ) ) )
		{
			clientOk = 0;
			Com_DPrintf("client text ignored for %s: %s\n", cl->name, Cmd_Argv(0));
		}
		if ( floodprotect )
		{
			cl->floodprotect = svs.time + 800;
		}
		SV_ExecuteClientCommand(cl, s, clientOk);
		cl->lastClientCommand = seq;
		Com_sprintf(cl->lastClientCommandString, MAX_STRINGLENGTH, "%s", s);
		return qtrue;
	}
	Com_Printf("Client %s lost %i clientCommands\n", cl->name, ( seq - cl->lastClientCommand ) + 1);
	SV_DropClient(cl, "EXE_LOSTRELIABLECOMMANDS");
	return qfalse;
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
		cracking_hook_call(0x08061FE7, (int)common_init_complete_print);
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

		cracking_hook_call(0x08081CFE, (int)hook_RuntimeError_in_VM_Execute);

		hook_gametype_scripts = new cHook(0x0810DDEE, (int)custom_GScr_LoadGameTypeScript);
		hook_gametype_scripts->hook();
		hook_developer_prints = new cHook(0x08060B7C, (int)custom_Com_DPrintf);
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

		hook_fire_grenade = new cHook(0x0810C1F6, (int)custom_fire_grenade);
		hook_fire_grenade->hook();
		hook_touch_item_auto = new cHook(0x081037F0, int(custom_Touch_Item_Auto));
		hook_touch_item_auto->hook();
		hook_sv_verifyiwds_f = new cHook(0x0808EC66, int(custom_SV_VerifyIwds_f));
		hook_sv_verifyiwds_f->hook();

		#if COMPILE_PLAYER == 1
		hook_play_movement = new cHook(0x0808F488, (int)play_movement);
		hook_play_movement->hook();
		hook_clientendframe = new cHook(0x080F4DBE, (int)custom_ClientEndFrame);
		hook_clientendframe->hook();
		hook_bg_playanim = new cHook(0x080D69B2, (int)custom_BG_PlayAnim);
		hook_bg_playanim->hook();
		#endif

		cracking_hook_function(0x080E97F0, (int)custom_BG_IsWeaponValid);
		cracking_hook_function(0x0808E544, (int)custom_SV_WriteDownloadToClient);
		cracking_hook_function(0x080B59CE, (int)custom_va);
		cracking_hook_function(0x0808EEEC, (int)custom_SV_ResetPureClient_f);
		cracking_hook_function(0x0809443E, (int)custom_SV_CalcPings);
		cracking_hook_function(0x080945AC, (int)custom_SV_CheckTimeouts);
		cracking_hook_function(0x080F474A, (int)custom_StuckInClient);
		cracking_hook_function(0x080F2F2E, (int)custom_G_SetClientContents);

		#if COMPILE_JUMP == 1
		cracking_hook_function(0x080D9FF4, (int)Jump_ClearState);
		cracking_hook_function(0x080DA1A6, (int)Jump_ReduceFriction);
		cracking_hook_function(0x080DA238, (int)Jump_ClampVelocity);
		cracking_hook_function(0x080DA0A4, (int)Jump_IsPlayerAboveMax);
		cracking_hook_function(0x080DA016, (int)Jump_GetStepHeight);
		cracking_hook_function(0x080DA584, (int)Jump_Check);
		cracking_hook_function(0x080DA0F4, (int)Jump_ApplySlowdown);
		cracking_hook_function(0x080DA0CA, (int)Jump_ActivateSlowdown);
		cracking_hook_function(0x080D9EE8, (int)Jump_RegisterDvars);
		#endif

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
		cracking_hook_call(0x08062301, (int)common_init_complete_print);
		cracking_hook_call(0x08093572, (int)hook_sv_spawnserver);
		cracking_hook_call(0x08090B0C, (int)hook_ClientCommand);
		cracking_hook_call(0x0808DA52, (int)hook_AuthorizeState);
		cracking_hook_call(0x0808D22E, (int)hook_isLanAddress);
		cracking_hook_call(0x0808BCFC, (int)hook_findMap);
		cracking_hook_call(0x080909BE, (int)hook_ClientUserinfoChanged);
		cracking_hook_call(0x08070B1B, (int)Scr_GetCustomFunction);
		cracking_hook_call(0x08070D3F, (int)Scr_GetCustomMethod);
		cracking_hook_call(0x0808227A, (int)hook_RuntimeError_in_VM_Execute);
		cracking_hook_call(0x0808FCBE, (int)hook_bad_printf);
		cracking_hook_call(0x080EBB14, (int)hook_findWeaponIndex);

		#if COMPILE_PLAYER == 1
		cracking_hook_call(0x0808F533, (int)hook_gamestate_info);
		#endif

		hook_gametype_scripts = new cHook(0x0811012A, (int)custom_GScr_LoadGameTypeScript);
		hook_gametype_scripts->hook();

		hook_developer_prints = new cHook(0x08060E42, (int)custom_Com_DPrintf);
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

		hook_fire_grenade = new cHook(0x0810E532, (int)custom_fire_grenade);
		hook_fire_grenade->hook();
		hook_touch_item_auto = new cHook(0x08105B24, int(custom_Touch_Item_Auto));
		hook_touch_item_auto->hook();
		hook_sv_verifyiwds_f = new cHook(0x080904A0, int(custom_SV_VerifyIwds_f));
		hook_sv_verifyiwds_f->hook();

		#if COMPILE_PLAYER == 1
		hook_play_movement = new cHook(0x08090D18, (int)play_movement);
		hook_play_movement->hook();
		hook_clientendframe = new cHook(0x080F73D2, (int)custom_ClientEndFrame);
		hook_clientendframe->hook();
		hook_bg_playanim = new cHook(0x080D8F92, (int)custom_BG_PlayAnim);
		hook_bg_playanim->hook();
		#endif

		cracking_hook_function(0x080EBDE0, (int)custom_BG_IsWeaponValid);
		cracking_hook_function(0x0808FD2E, (int)custom_SV_WriteDownloadToClient);
		cracking_hook_function(0x080B7E62, (int)custom_va);
		cracking_hook_function(0x08090726, (int)custom_SV_ResetPureClient_f);
		cracking_hook_function(0x0809630E, (int)custom_SV_CalcPings);
		cracking_hook_function(0x080964C4, (int)custom_SV_CheckTimeouts);
		cracking_hook_function(0x080F6D5A, (int)custom_StuckInClient);
		cracking_hook_function(0x080F553E, (int)custom_G_SetClientContents);

		#if COMPILE_JUMP == 1
		cracking_hook_function(0x080DC5D4, (int)Jump_ClearState);
		cracking_hook_function(0x080DC786, (int)Jump_ReduceFriction);
		cracking_hook_function(0x080DC818, (int)Jump_ClampVelocity);
		cracking_hook_function(0x080DC684, (int)Jump_IsPlayerAboveMax);
		cracking_hook_function(0x080DC5F6, (int)Jump_GetStepHeight);
		cracking_hook_function(0x080DCB64, (int)Jump_Check);
		cracking_hook_function(0x080DC6D4, (int)Jump_ApplySlowdown);
		cracking_hook_function(0x080DC6AA, (int)Jump_ActivateSlowdown);
		cracking_hook_function(0x080DC4C8, (int)Jump_RegisterDvars);
		#endif

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
		cracking_hook_call(0x080622F9, (int)common_init_complete_print);
		cracking_hook_call(0x0809362A, (int)hook_sv_spawnserver);
		cracking_hook_call(0x08090BA0, (int)hook_ClientCommand);
		cracking_hook_call(0x0808DB12, (int)hook_AuthorizeState);
		cracking_hook_call(0x0808D2FA, (int)hook_isLanAddress);
		cracking_hook_call(0x0808BDC8, (int)hook_findMap);
		cracking_hook_call(0x08090A52, (int)hook_ClientUserinfoChanged);
		cracking_hook_call(0x08070BE7, (int)Scr_GetCustomFunction);
		cracking_hook_call(0x08070E0B, (int)Scr_GetCustomMethod);
		//cracking_hook_call(0x08082346, (int)hook_RuntimeError_in_VM_Execute);
		cracking_hook_call(0x0808FD52, (int)hook_bad_printf);
		cracking_hook_call(0x080EBC58, (int)hook_findWeaponIndex);
		cracking_hook_call(0x08062644, (int)hitch_warning_print);
		cracking_hook_call(0x0811FF47, (int)bullet_fire_extended_trace);

		hook_gametype_scripts = new cHook(0x08110286, (int)custom_GScr_LoadGameTypeScript);
		hook_gametype_scripts->hook();

		hook_developer_prints = new cHook(0x08060E3A, (int)custom_Com_DPrintf);
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

		hook_fire_grenade = new cHook(0x0810E68E, (int)custom_fire_grenade);
		hook_fire_grenade->hook();
		hook_touch_item_auto = new cHook(0x08105C80, int(custom_Touch_Item_Auto));
		hook_touch_item_auto->hook();
		hook_g_tempentity = new cHook(0x0811EFC4, (int)custom_G_TempEntity);
		hook_g_tempentity->hook();
		hook_gscr_loadconsts = new cHook(0x081224F8, (int)custom_GScr_LoadConsts);
		hook_gscr_loadconsts->hook();
		hook_sv_masterheartbeat = new cHook(0x08096ED6, (int)custom_sv_masterheartbeat);
		hook_sv_masterheartbeat->hook();
		hook_g_runframe = new cHook(0x0810A13A, (int)custom_G_RunFrame);
		hook_g_runframe->hook();
		hook_scr_loadgametype = new cHook(0x081182F4, (int)custom_Scr_LoadGameType);
		hook_scr_loadgametype->hook();
		hook_vm_notify = new cHook(0x0808359E, (int)custom_VM_Notify);
		hook_vm_notify->hook();
		hook_g_processipbans = new cHook(0x0811BB60, (int)custom_G_ProcessIPBans);
		hook_g_processipbans->hook();
		hook_scr_notify = new cHook(0x0811B2DE, (int)custom_Scr_Notify);
		hook_scr_notify->hook();
		hook_script_cloneplayer = new cHook(0x080FCC76, (int)custom_Script_clonePlayer);
		hook_script_cloneplayer->hook();
		hook_com_initcvars = new cHook(0x08061D90, (int)custom_Com_InitCvars);
		hook_com_initcvars->hook();
		hook_sv_verifyiwds_f = new cHook(0x08090534, int(custom_SV_VerifyIwds_f));
		hook_sv_verifyiwds_f->hook();
		hook_g_freeentity = new cHook(0x0811EE50, int(custom_G_FreeEntity));
		hook_g_freeentity->hook();
		hook_g_initgentity = new cHook(0x0811E85C, int(custom_G_InitGentity));
		hook_g_initgentity->hook();

		#if COMPILE_PLAYER == 1
		hook_play_movement = new cHook(0x08090DAC, (int)play_movement);
		hook_play_movement->hook();
		hook_clientendframe = new cHook(0x080F7516, (int)custom_ClientEndFrame);
		hook_clientendframe->hook();
		hook_bg_playanim = new cHook(0x080D90D6, (int)custom_BG_PlayAnim);
		hook_bg_playanim->hook();
		#endif

		cracking_hook_function(0x08105CAC, (int)custom_Touch_Item);
		cracking_hook_function(0x0811F232, (int)custom_G_AddEvent);
		cracking_hook_function(0x080EBF24, (int)custom_BG_IsWeaponValid);
		cracking_hook_function(0x080DFC78, (int)custom_BG_AddPredictableEventToPlayerstate);
		cracking_hook_function(0x080985C8, (int)custom_SV_WriteSnapshotToClient);
		cracking_hook_function(0x0808F302, (int)custom_SV_SendClientGameState);
		cracking_hook_function(0x0808F02E, (int)custom_SV_DropClient);
		cracking_hook_function(0x0808FDC2, (int)custom_SV_WriteDownloadToClient);
		cracking_hook_function(0x080B7FA6, (int)custom_va);
		cracking_hook_function(0x080907BA, (int)custom_SV_ResetPureClient_f);
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
		cracking_hook_function(0x08113818, (int)custom_Script_bulletTrace);
		cracking_hook_function(0x08113128, (int)custom_Script_obituary);
		cracking_hook_function(0x081124F6, (int)custom_Script_setHintString);
		cracking_hook_function(0x08092302, (int)custom_SV_MapExists);
		cracking_hook_function(0x08109CE0, (int)custom_G_UpdateObjectives); // Guessed function name
		cracking_hook_function(0x08120A70, (int)custom_FireWeaponMelee);
		cracking_hook_function(0x08120484, (int)custom_Bullet_Fire);
		cracking_hook_function(0x0809C21C, (int)custom_SV_QueueVoicePacket);
		cracking_hook_function(0x08121BC6, (int)custom_Player_UpdateCursorHints);
		cracking_hook_function(0x08060C20, (int)custom_Com_PrintMessage);
		cracking_hook_function(0x08109F5C, (int)custom_G_RunFrameForEntity);
		cracking_hook_function(0x08090BAC, (int)custom_SV_ClientCommand);
		cracking_hook_function(0x080F6E9E, (int)custom_StuckInClient);
		cracking_hook_function(0x080F5682, (int)custom_G_SetClientContents);
		cracking_hook_function(0x0809CD64, (int)custom_SV_ClipMoveToEntity);

		#if COMPILE_JUMP == 1
		cracking_hook_function(0x080DC718, (int)Jump_ClearState);
		cracking_hook_function(0x080DC8CA, (int)Jump_ReduceFriction);
		cracking_hook_function(0x080DC95C, (int)Jump_ClampVelocity);
		cracking_hook_function(0x080DC7C8, (int)Jump_IsPlayerAboveMax);
		cracking_hook_function(0x080DC73A, (int)Jump_GetStepHeight);
		cracking_hook_function(0x080DCCA8, (int)Jump_Check);
		cracking_hook_function(0x080DC818, (int)Jump_ApplySlowdown);
		cracking_hook_function(0x080DC7EE, (int)Jump_ActivateSlowdown);
		cracking_hook_function(0x080DC60C, (int)Jump_RegisterDvars);
		#endif

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

		gsc_weapons_init();
		printf("> [PLUGIN LOADED]\n");
	}

	~cCallOfDuty2Pro()
	{
		gsc_weapons_free();
		if ( voiceDataDumpFile )
		{
			fclose(voiceDataDumpFile);
		}
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