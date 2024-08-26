#include "gsc.hpp"
#include "libcod.hpp"
#include "proxy/proxy.h"
#include "ratelimiter.hpp"
#include "utils.hpp"

#include <signal.h>

#if COMPILE_CUSTOM_VOICE == 1
#include <pthread.h>
#endif

// Stock dvars
dvar_t *bg_bobMax;
dvar_t *bg_fallDamageMaxHeight;
dvar_t *bg_fallDamageMinHeight;
dvar_t *cl_allowDownload;
dvar_t *cl_paused;
dvar_t *com_dedicated;
dvar_t *com_hunkMegs;
dvar_t *com_logfile;
dvar_t *com_sv_running;
dvar_t *com_timescale;
dvar_t *developer;
dvar_t *fs_game;
dvar_t *fs_homepath;
dvar_t *g_antilag;
dvar_t *g_knockback;
dvar_t *g_mantleBlockTimeBuffer;
dvar_t *g_password;
dvar_t *g_playerCollisionEjectSpeed;
dvar_t *g_synchronousClients;
dvar_t *g_voiceChatTalkingDuration;
dvar_t *jump_height;
dvar_t *jump_stepSize;
dvar_t *jump_slowdownEnable;
dvar_t *jump_ladderPushVel;
dvar_t *jump_spreadAdd;
dvar_t *net_ip;
dvar_t *net_lanauthorize;
dvar_t *net_port;
dvar_t *nextmap;
dvar_t *player_dmgtimer_maxTime;
dvar_t *player_dmgtimer_timePerPoint;
dvar_t *player_meleeHeight;
dvar_t *player_meleeRange;
dvar_t *player_meleeWidth;
dvar_t *rcon_password;
dvar_t *showpackets;
dvar_t *sv_allowAnonymous;
dvar_t *sv_allowDownload;
dvar_t *sv_cheats;
dvar_t *sv_disableClientConsole;
dvar_t *sv_floodProtect;
dvar_t *sv_fps;
dvar_t *sv_gametype;
dvar_t *sv_hostname;
dvar_t *sv_iwdNames;
dvar_t *sv_iwds;
dvar_t *sv_mapname;
dvar_t *sv_maxclients;
dvar_t *sv_maxPing;
dvar_t *sv_maxRate;
dvar_t *sv_minPing;
dvar_t *sv_packet_info;
dvar_t *sv_padPackets;
dvar_t *sv_privateClients;
dvar_t *sv_privatePassword;
dvar_t *sv_pure;
dvar_t *sv_reconnectlimit;
dvar_t *sv_referencedIwdNames;
dvar_t *sv_referencedIwds;
dvar_t *sv_serverid;
dvar_t *sv_showCommands;
dvar_t *sv_timeout;
dvar_t *sv_voice;
dvar_t *sv_voiceQuality;
dvar_t *sv_zombietime;
dvar_t *cl_wwwDownload;
dvar_t *sv_wwwBaseURL;
dvar_t *sv_wwwDlDisconnected;
dvar_t *sv_wwwDownload;

// Custom dvars
#if COMPILE_UTILS == 1
dvar_t *con_coloredPrints;
#endif
dvar_t *fs_callbacks;
dvar_t *fs_library;
dvar_t *fs_mapScriptDirectories;
dvar_t *g_brushModelCollisionTweaks;
dvar_t *g_bulletDrop;
dvar_t *g_bulletDropMaxTime;
dvar_t *g_corpseHit;
dvar_t *g_debugCallbacks;
dvar_t *g_debugEvents;
dvar_t *g_debugStaticModels;
dvar_t *g_forceRate;
dvar_t *g_forceSnaps;
dvar_t *g_logPickup;
dvar_t *g_mantleBlockEnable;
dvar_t *g_playerCollision;
dvar_t *g_playerCollisionEjectDamageAllowed;
dvar_t *g_playerCollisionEjectDuration;
dvar_t *g_playerEject;
dvar_t *g_resetSlide;
dvar_t *g_safePrecache;
dvar_t *g_spawnMapTurrets;
dvar_t *g_spawnMapWeapons;
dvar_t *g_spectateBots;
dvar_t *g_triggerMode;
dvar_t *g_turretMissingTagTerminalError;
dvar_t *libcod;
dvar_t *loc_loadLocalizedMods;
dvar_t *logErrors;
dvar_t *logfileName;
dvar_t *logfileRotate;
dvar_t *logTimestamps;
dvar_t *scr_turretDamageName;
dvar_t *sv_allowRcon;
dvar_t *sv_authorizePort;
dvar_t *sv_authorizeServer;
dvar_t *sv_authorizeTimeout;
dvar_t *sv_botKickMessages;
dvar_t *sv_botReconnectMode;
dvar_t *sv_botUseTriggerUse;
dvar_t *sv_cracked;
dvar_t *sv_disconnectMessages;
dvar_t *sv_downloadMessage;
dvar_t *sv_downloadMessageAtMap;
dvar_t *sv_downloadMessageForLegacyClients;
dvar_t *sv_kickGamestateLimitedClients;
dvar_t *sv_kickMessages;
dvar_t *sv_limitLocalRcon;
dvar_t *sv_logHeartbeats;
dvar_t *sv_logRcon;
dvar_t *sv_masterPort;
dvar_t *sv_masterServer;
dvar_t *sv_minimizeSysteminfo;
dvar_t *sv_noauthorize;
dvar_t *sv_reservedConfigstringBufferSize;
dvar_t *sv_timeoutMessages;
dvar_t *sv_verifyIwds;
dvar_t *sv_version;
dvar_t *sv_wwwDlDisconnectedMessages;

cHook *hook_add_opcode;
cHook *hook_bg_playanim;
cHook *hook_clientendframe;
cHook *hook_com_initdvars;
cHook *hook_console_print;
cHook *hook_developer_prints;
cHook *hook_fire_grenade;
cHook *hook_fs_registerdvars;
cHook *hook_g_freeentity;
cHook *hook_g_initgentity;
cHook *hook_g_processipbans;
cHook *hook_g_runframe;
cHook *hook_g_tempentity;
cHook *hook_gametype_scripts;
cHook *hook_gscr_loadconsts;
cHook *hook_init_opcode;
cHook *hook_play_movement;
cHook *hook_playercmd_cloneplayer;
cHook *hook_pm_beginweaponchange;
cHook *hook_pmove;
cHook *hook_scr_execentthread;
cHook *hook_scr_execthread;
cHook *hook_scr_loadgametype;
cHook *hook_scr_notify;
cHook *hook_sys_quit;
cHook *hook_sv_freeconfigstrings;
cHook *hook_sv_masterheartbeat;
cHook *hook_sv_verifyiwds_f;
cHook *hook_touch_item_auto;
cHook *hook_vm_notify;

// Stock callbacks
int codecallback_startgametype = 0;
int codecallback_playerconnect = 0;
int codecallback_playerdisconnect = 0;
int codecallback_playerdamage = 0;
int codecallback_playerkilled = 0;

// Custom callbacks
int codecallback_client_spam = 0;
int codecallback_dprintf = 0;
int codecallback_entityevent = 0;
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
int codecallback_suicide = 0;
int codecallback_userinfochanged = 0;
int codecallback_vid_restart = 0;
int codecallback_weapon_change = 0;

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

callback_t callbacks[] =
{
	{ &codecallback_startgametype, "CodeCallback_StartGameType" }, // g_scr_data.gametype.startupgametype
	{ &codecallback_playerconnect, "CodeCallback_PlayerConnect" }, // g_scr_data.gametype.playerconnect
	{ &codecallback_playerdisconnect, "CodeCallback_PlayerDisconnect" }, // g_scr_data.gametype.playerdisconnect
	{ &codecallback_playerdamage, "CodeCallback_PlayerDamage" }, // g_scr_data.gametype.playerdamage
	{ &codecallback_playerkilled, "CodeCallback_PlayerKilled" }, // g_scr_data.gametype.playerkilled

	{ &codecallback_client_spam, "CodeCallback_CLSpam"},
	{ &codecallback_dprintf, "CodeCallback_DPrintf"},
	{ &codecallback_entityevent, "CodeCallback_EntityEvent"},
	{ &codecallback_error, "CodeCallback_Error"},
	{ &codecallback_fire_grenade, "CodeCallback_FireGrenade"},
	{ &codecallback_hitchwarning, "CodeCallback_HitchWarning"},
	{ &codecallback_map_turrets_load, "CodeCallback_MapTurretsLoad"},
	{ &codecallback_map_weapons_load, "CodeCallback_MapWeaponsLoad"},
	{ &codecallback_notify, "CodeCallback_Notify"},
	{ &codecallback_notifydebug, "CodeCallback_NotifyDebug"},
	{ &codecallback_pickup, "CodeCallback_Pickup"},
	{ &codecallback_playercommand, "CodeCallback_PlayerCommand"},
	{ &codecallback_remotecommand, "CodeCallback_RemoteCommand"},
	{ &codecallback_suicide, "CodeCallback_Suicide"},
	{ &codecallback_userinfochanged, "CodeCallback_UserInfoChanged"},
	{ &codecallback_vid_restart, "CodeCallback_VidRestart"},
	{ &codecallback_weapon_change, "CodeCallback_WeaponChange"},

	{ &codecallback_adsbutton, "CodeCallback_AdsButton"},
	{ &codecallback_attackbutton, "CodeCallback_AttackButton"},
	{ &codecallback_crouchbutton, "CodeCallback_CrouchButton"},
	{ &codecallback_fragbutton, "CodeCallback_FragButton"},
	{ &codecallback_holdbreathbutton, "CodeCallback_HoldBreathButton"},
	{ &codecallback_leanleftbutton, "CodeCallback_LeanLeftButton"},
	{ &codecallback_leanrightbutton, "CodeCallback_LeanRightButton"},
	{ &codecallback_meleebreathbutton, "CodeCallback_MeleeBreathButton"},
	{ &codecallback_meleebutton, "CodeCallback_MeleeButton"},
	{ &codecallback_pronebutton, "CodeCallback_ProneButton"},
	{ &codecallback_reloadbutton, "CodeCallback_ReloadButton"},
	{ &codecallback_smokebutton, "CodeCallback_SmokeButton"},
	{ &codecallback_standbutton, "CodeCallback_StandButton"},
	{ &codecallback_usebutton, "CodeCallback_UseButton"},
};

const entityHandler_t entityHandlers[] =
{
	/* Null                  */ { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MOD_UNKNOWN, MOD_UNKNOWN },
	/* Trigger Multiple      */ { NULL, NULL, NULL, Touch_Multi, NULL, NULL, NULL, NULL, MOD_UNKNOWN, MOD_UNKNOWN },
	/* Trigger Hurt          */ { NULL, NULL, NULL, NULL, hurt_use, NULL, NULL, NULL, MOD_UNKNOWN, MOD_UNKNOWN },
	/* Trigger Hurt Touch    */ { NULL, NULL, NULL, hurt_touch, hurt_use, NULL, NULL, NULL, MOD_UNKNOWN, MOD_UNKNOWN },
	/* Trigger Damage        */ { NULL, NULL, NULL, NULL, Use_trigger_damage, Pain_trigger_damage, Die_trigger_damage, NULL, MOD_UNKNOWN, MOD_UNKNOWN },
	/* Script Mover          */ { NULL, Reached_ScriptMover, NULL, NULL, NULL, NULL, NULL, NULL, MOD_UNKNOWN, MOD_UNKNOWN },
	/* Script Model          */ { NULL, Reached_ScriptMover, NULL, NULL, NULL, NULL, NULL, NULL, MOD_UNKNOWN, MOD_UNKNOWN },
	/* Grenade               */ { G_ExplodeMissile, NULL, NULL, Touch_Item_Auto, NULL, NULL, NULL, NULL, MOD_GRENADE, MOD_GRENADE_SPLASH },
	/* Rocket                */ { G_ExplodeMissile, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MOD_PROJECTILE, MOD_PROJECTILE_SPLASH },
	/* Client                */ { NULL, NULL, NULL, NULL, NULL, NULL, player_die, G_PlayerController, MOD_UNKNOWN, MOD_UNKNOWN },
	/* Client Spectator      */ { NULL, NULL, NULL, NULL, NULL, NULL, player_die, NULL, MOD_UNKNOWN, MOD_UNKNOWN },
	/* Client Dead           */ { NULL, NULL, NULL, NULL, NULL, NULL, NULL, G_PlayerController, MOD_UNKNOWN, MOD_UNKNOWN },
	/* Player Clone          */ { BodyEnd, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MOD_UNKNOWN, MOD_UNKNOWN },
	/* Turret Init           */ { turret_think_init, NULL, NULL, NULL, turret_use, NULL, NULL, turret_controller, MOD_UNKNOWN, MOD_UNKNOWN },
	/* Turret                */ { turret_think, NULL, NULL, NULL, turret_use, NULL, NULL, turret_controller, MOD_UNKNOWN, MOD_UNKNOWN },
	/* Dropped Item          */ { DroppedItemClearOwner, NULL, NULL, Touch_Item_Auto, NULL, NULL, NULL, NULL, MOD_UNKNOWN, MOD_UNKNOWN },
	/* Item Init             */ { FinishSpawningItem, NULL, NULL, Touch_Item_Auto, NULL, NULL, NULL, NULL, MOD_UNKNOWN, MOD_UNKNOWN },
	/* Item                  */ { NULL, NULL, NULL, Touch_Item_Auto, NULL, NULL, NULL, NULL, MOD_UNKNOWN, MOD_UNKNOWN },
	/* Trigger Use           */ { NULL, NULL, NULL, NULL, use_trigger_use, NULL, NULL, NULL, MOD_UNKNOWN, MOD_UNKNOWN },
	/* Player Mantling Block */ { G_FreeEntity, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MOD_UNKNOWN, MOD_UNKNOWN },
};

customEntityState_t customEntityState[MAX_GENTITIES];
customPlayerState_t customPlayerState[MAX_CLIENTS];

char altSystemInfo[BIG_INFO_STRING];
char altIwds[MAX_STRINGLENGTH];
char altReferencedIwds[MAX_STRINGLENGTH];

// We use a custom (extended) list of mutexes. All references to the original
// crit_sections list are hooked away
pthread_mutex_t crit_sections[CRITSECT_COUNT];


void custom_Com_InitDvars(void)
{
	// Register custom dvars required early on server start
	libcod = Dvar_RegisterBool("libcod", qtrue, DVAR_ROM);
	logfileName = Dvar_RegisterString("logfileName", "console_mp_server.log", DVAR_ARCHIVE);
	logfileRotate = Dvar_RegisterInt("logfileRotate", 0, 0, 1000, DVAR_ARCHIVE);
	logTimestamps = Dvar_RegisterBool("logTimestamps", qfalse, DVAR_ARCHIVE);
	sv_reservedConfigstringBufferSize = Dvar_RegisterInt("sv_reservedConfigstringBufferSize", 256, 0, 8192, DVAR_ARCHIVE);
	sv_authorizePort = Dvar_RegisterInt("sv_authorizePort", 20700, 0, 65535, DVAR_ARCHIVE);
	sv_authorizeServer = Dvar_RegisterString("sv_authorizeServer", "cod2master.activision.com", DVAR_ARCHIVE);
	sv_authorizeTimeout = Dvar_RegisterInt("sv_authorizeTimeout", 3000, 0, 1200000, DVAR_ARCHIVE);
	sv_masterPort = Dvar_RegisterInt("sv_masterPort", 20710, 0, 65535, DVAR_ARCHIVE);
	sv_masterServer = Dvar_RegisterString("sv_masterServer", "cod2master.activision.com", DVAR_ARCHIVE);
	sv_version = Dvar_RegisterString("sv_version", "1.3", DVAR_ARCHIVE | DVAR_LATCH | DVAR_CHANGEABLE_RESET);

	/* Register stock dvars here with different settings, scheme:
	dvar_t *dvar = Dvar_Register<Type>(var_name, default value, [min. value, max. value,] flags); */
	Dvar_RegisterInt("protocol", 118, 115, 118, DVAR_CHANGEABLE_RESET | DVAR_ROM | DVAR_SERVERINFO);

	/* Force server memory setting for clients to be able to counter 
	 Hunk_AllocateTempMemory failures */
	com_hunkMegs = Dvar_RegisterInt("com_hunkMegs", 160, 1, 512, DVAR_CHANGEABLE_RESET | DVAR_LATCH | DVAR_SYSTEMINFO | DVAR_ARCHIVE);

	hook_com_initdvars->unhook();
	void (*Com_InitDvars)(void);
	*(int *)&Com_InitDvars = hook_com_initdvars->from;
	Com_InitDvars();
	hook_com_initdvars->hook();

	// Get references to early loaded stock dvars
	cl_paused = Dvar_FindVar("cl_paused");
	com_dedicated = Dvar_FindVar("dedicated");
	com_logfile = Dvar_FindVar("logfile");
	com_sv_running = Dvar_FindVar("sv_running");
}

void common_init_complete_print(const char *format, ...)
{
	/* We are in Com_Init_Try_Block_Function, after executing Com_InitDvars()
	 and SV_Init() where a big chunk of dvars are defined. However, there is
	 still some that are defined later, see custom_G_ProcessIPBans */
	Com_Printf("--- Common Initialization Complete ---\n");

	// Get references to stock dvars
	cl_allowDownload = Dvar_RegisterBool("cl_allowDownload", qtrue, DVAR_ARCHIVE | DVAR_SYSTEMINFO); // Force-enable download for clients
	developer = Dvar_FindVar("developer");
	net_ip = Dvar_FindVar("net_ip");
	net_lanauthorize = Dvar_FindVar("net_lanauthorize");
	net_port = Dvar_FindVar("net_port");
	nextmap = Dvar_FindVar("nextmap");
	rcon_password = Dvar_FindVar("rcon_password");
	showpackets = Dvar_FindVar("showpackets");
	sv_disableClientConsole = Dvar_FindVar("sv_disableClientConsole");
	sv_allowAnonymous = Dvar_FindVar("sv_allowAnonymous");
	sv_allowDownload = Dvar_FindVar("sv_allowDownload");
	sv_cheats = Dvar_FindVar("sv_cheats");
	sv_floodProtect = Dvar_FindVar("sv_floodProtect");
	sv_fps = Dvar_FindVar("sv_fps");
	sv_gametype = Dvar_FindVar("g_gametype");
	sv_hostname = Dvar_FindVar("sv_hostname");
	sv_iwdNames = Dvar_FindVar("sv_iwdNames");
	sv_iwds = Dvar_FindVar("sv_iwds");
	sv_mapname = Dvar_FindVar("mapname");
	sv_maxclients = Dvar_FindVar("sv_maxclients");
	sv_maxPing = Dvar_FindVar("sv_maxPing");
	sv_minPing = Dvar_FindVar("sv_minPing");
	sv_packet_info = Dvar_FindVar("sv_packet_info");
	sv_padPackets = Dvar_FindVar("sv_padPackets");
	sv_privateClients = Dvar_FindVar("sv_privateClients");
	sv_privatePassword = Dvar_FindVar("sv_privatePassword");
	sv_pure = Dvar_FindVar("sv_pure");
	sv_reconnectlimit = Dvar_FindVar("sv_reconnectlimit");
	sv_referencedIwdNames = Dvar_FindVar("sv_referencedIwdNames");
	sv_referencedIwds = Dvar_FindVar("sv_referencedIwds");
	sv_serverid = Dvar_FindVar("sv_serverid");
	sv_showCommands = Dvar_FindVar("sv_showCommands");
	sv_timeout = Dvar_FindVar("sv_timeout");
	sv_voice = Dvar_FindVar("sv_voice");
	sv_voiceQuality = Dvar_FindVar("sv_voiceQuality");
	sv_zombietime = Dvar_FindVar("sv_zombietime");
	cl_wwwDownload = Dvar_RegisterBool("cl_wwwDownload", qtrue, DVAR_ARCHIVE | DVAR_SYSTEMINFO); // Force-enable wwwDownload for clients that support it
	sv_wwwBaseURL = Dvar_FindVar("sv_wwwBaseURL");
	sv_wwwDlDisconnected = Dvar_FindVar("sv_wwwDlDisconnected");
	sv_wwwDownload = Dvar_FindVar("sv_wwwDownload");

	// Register custom dvars
	#if COMPILE_UTILS == 1
	con_coloredPrints = Dvar_RegisterBool("con_coloredPrints", qfalse, DVAR_ARCHIVE);
	#endif
	fs_callbacks = Dvar_RegisterString("fs_callbacks", "", DVAR_ARCHIVE);
	fs_library = Dvar_RegisterString("fs_library", "", DVAR_ARCHIVE);
	fs_mapScriptDirectories = Dvar_RegisterInt("fs_mapScriptDirectories", 0, 0, 2, DVAR_ARCHIVE);
	g_brushModelCollisionTweaks = Dvar_RegisterBool("g_brushModelCollisionTweaks", qfalse, DVAR_ARCHIVE);
	g_bulletDrop = Dvar_RegisterBool("g_bulletDrop", qfalse, DVAR_ARCHIVE);
	g_bulletDropMaxTime = Dvar_RegisterInt("g_bulletDropMaxTime", 10000, 50, 60000, DVAR_ARCHIVE);
	g_corpseHit = Dvar_RegisterBool("g_corpseHit", qtrue, DVAR_ARCHIVE);
	g_debugCallbacks = Dvar_RegisterBool("g_debugCallbacks", qfalse, DVAR_ARCHIVE);
	g_debugEvents = Dvar_RegisterBool("g_debugEvents", qfalse, DVAR_ARCHIVE);
	g_debugStaticModels = Dvar_RegisterBool("g_debugStaticModels", qfalse, DVAR_ARCHIVE);
	g_forceRate = Dvar_RegisterInt("g_forceRate", 0, 0, 25000, DVAR_ARCHIVE);
	g_forceSnaps = Dvar_RegisterInt("g_forceSnaps", 0, 0, 30, DVAR_ARCHIVE);
	g_logPickup = Dvar_RegisterBool("g_logPickup", qtrue, DVAR_ARCHIVE);
	g_mantleBlockEnable = Dvar_RegisterBool("g_mantleBlockEnable", qtrue, DVAR_ARCHIVE);
	g_playerCollision = Dvar_RegisterBool("g_playerCollision", qtrue, DVAR_ARCHIVE);
	g_playerCollisionEjectDamageAllowed = Dvar_RegisterBool("g_playerCollisionEjectDamageAllowed", qfalse, DVAR_ARCHIVE);
	g_playerCollisionEjectDuration = Dvar_RegisterInt("g_playerCollisionEjectDuration", 300, 50, 1000, DVAR_ARCHIVE);
	g_playerEject = Dvar_RegisterBool("g_playerEject", qtrue, DVAR_ARCHIVE);
	g_resetSlide = Dvar_RegisterBool("g_resetSlide", qfalse, DVAR_ARCHIVE);
	g_spawnMapTurrets = Dvar_RegisterBool("g_spawnMapTurrets", qtrue, DVAR_ARCHIVE);
	g_spawnMapWeapons = Dvar_RegisterBool("g_spawnMapWeapons", qtrue, DVAR_ARCHIVE);
	g_triggerMode = Dvar_RegisterInt("g_triggerMode", 1, 0, 2, DVAR_ARCHIVE);
	g_turretMissingTagTerminalError = Dvar_RegisterBool("g_turretMissingTagTerminalError", qtrue, DVAR_ARCHIVE);
	g_spectateBots = Dvar_RegisterBool("g_spectateBots", qtrue, DVAR_ARCHIVE);
	loc_loadLocalizedMods = Dvar_RegisterBool("loc_loadLocalizedMods", qfalse, DVAR_ARCHIVE);
	logErrors = Dvar_RegisterBool("logErrors", qfalse, DVAR_ARCHIVE);
	scr_turretDamageName = Dvar_RegisterBool("scr_turretDamageName", qfalse, DVAR_ARCHIVE);
	sv_allowRcon = Dvar_RegisterBool("sv_allowRcon", qtrue, DVAR_ARCHIVE);
	sv_botKickMessages = Dvar_RegisterBool("sv_botKickMessages", qtrue, DVAR_ARCHIVE);
	sv_botReconnectMode = Dvar_RegisterInt("sv_botReconnectMode", 0, 0, 2, DVAR_ARCHIVE);
	sv_botUseTriggerUse = Dvar_RegisterBool("sv_botUseTriggerUse", qfalse, DVAR_ARCHIVE);
	sv_cracked = Dvar_RegisterBool("sv_cracked", qfalse, DVAR_ARCHIVE);
	sv_disconnectMessages = Dvar_RegisterBool("sv_disconnectMessages", qtrue, DVAR_ARCHIVE);
	sv_downloadMessage = Dvar_RegisterString("sv_downloadMessage", "", DVAR_ARCHIVE);
	sv_downloadMessageAtMap = Dvar_RegisterBool("sv_downloadMessageAtMap", qtrue, DVAR_ARCHIVE);
	sv_downloadMessageForLegacyClients = Dvar_RegisterString("sv_downloadMessageForLegacyClients", "", DVAR_ARCHIVE);
	sv_kickGamestateLimitedClients = Dvar_RegisterBool("sv_kickGamestateLimitedClients", qtrue, DVAR_ARCHIVE);
	sv_kickMessages = Dvar_RegisterBool("sv_kickMessages", qtrue, DVAR_ARCHIVE);
	sv_limitLocalRcon = Dvar_RegisterBool("sv_limitLocalRcon", qtrue, DVAR_ARCHIVE);
	sv_logHeartbeats = Dvar_RegisterBool("sv_logHeartbeats", qtrue, DVAR_ARCHIVE);
	sv_logRcon = Dvar_RegisterBool("sv_logRcon", qtrue, DVAR_ARCHIVE);
	sv_noauthorize = Dvar_RegisterBool("sv_noauthorize", qfalse, DVAR_ARCHIVE);
	sv_timeoutMessages = Dvar_RegisterBool("sv_timeoutMessages", qtrue, DVAR_ARCHIVE);
	sv_verifyIwds = Dvar_RegisterBool("sv_verifyIwds", qtrue, DVAR_ARCHIVE);
	sv_wwwDlDisconnectedMessages = Dvar_RegisterInt("sv_wwwDlDisconnectedMessages", 1, 0, 2, DVAR_ARCHIVE);

	/* Register (thus override) dvars that would otherwise be defined later in
	 G_RegisterDvars, example:
	g_gravity = Dvar_RegisterFloat("g_gravity", 800.0, 1.0, 3.402823e+38, DVAR_CHANGEABLE_RESET);
	*/
}

qboolean custom_FS_RegisterDvars(void)
{
	int ret;

	hook_fs_registerdvars->unhook();
	qboolean (*FS_RegisterDvars)(void);
	*(int *)&FS_RegisterDvars = hook_fs_registerdvars->from;
	ret = FS_RegisterDvars();
	hook_fs_registerdvars->hook();

	// Get references to file system dvars
	fs_game = Dvar_FindVar("fs_game");
	fs_homepath = Dvar_FindVar("fs_homepath");

	return ret;
}

void custom_G_ProcessIPBans(void)
{
	/* This is right after G_RegisterDvars() and BG_RegisterDvars(), giving us
	 access to variables that are not yet defined at the
	 common_init_complete_print hook */
	bg_bobMax = Dvar_FindVar("bg_bobMax");
	g_antilag = Dvar_FindVar("g_antilag");
	g_knockback = Dvar_FindVar("g_knockback");
	g_mantleBlockTimeBuffer = Dvar_FindVar("g_mantleBlockTimeBuffer");
	g_password = Dvar_FindVar("g_password");
	g_playerCollisionEjectSpeed = Dvar_FindVar("g_playerCollisionEjectSpeed");
	g_voiceChatTalkingDuration = Dvar_FindVar("g_voiceChatTalkingDuration");
	player_dmgtimer_maxTime = Dvar_FindVar("player_dmgtimer_maxTime");
	player_dmgtimer_timePerPoint = Dvar_FindVar("player_dmgtimer_timePerPoint");
	player_meleeHeight = Dvar_FindVar("player_meleeHeight");
	player_meleeRange = Dvar_FindVar("player_meleeRange");
	player_meleeWidth = Dvar_FindVar("player_meleeWidth");

	hook_g_processipbans->unhook();
	void (*G_ProcessIPBans)(void);
	*(int *)&G_ProcessIPBans = hook_g_processipbans->from;
	G_ProcessIPBans();
	hook_g_processipbans->hook();
}

customStringIndex_t custom_scr_const;
void custom_GScr_LoadConsts(void)
{
	/* Allocate custom strings for Scr_Notify() here, scheme:
	custom_scr_const.custom = GScr_AllocString("custom_string");
	 Note: This new reference also has to be added to customStringIndex_t in
	 declarations.hpp */
	custom_scr_const.all = GScr_AllocString("all");
	custom_scr_const.artist = GScr_AllocString("artist");
	custom_scr_const.axis_allies = GScr_AllocString("axis_allies");
	custom_scr_const.bot_trigger = GScr_AllocString("bot_trigger");
	custom_scr_const.bounce = GScr_AllocString("bounce");
	custom_scr_const.bullet = GScr_AllocString("bullet");
	custom_scr_const.flags = GScr_AllocString("flags");
	custom_scr_const.land = GScr_AllocString("land");
	#if COMPILE_CUSTOM_VOICE == 1
	custom_scr_const.sound_file_done = GScr_AllocString("sound_file_done");
	custom_scr_const.sound_file_stop = GScr_AllocString("sound_file_stop");
	#endif
	custom_scr_const.title = GScr_AllocString("title");
	custom_scr_const.trigger_radius = GScr_AllocString("trigger_radius");

	hook_gscr_loadconsts->unhook();
	void (*GScr_LoadConsts)(void);
	*(int *)&GScr_LoadConsts = hook_gscr_loadconsts->from;
	GScr_LoadConsts();
	hook_gscr_loadconsts->hook();
}

void custom_Dvar_SetFromStringFromSource(dvar_t *dvar, const char *string, DvarSetSource source)
{
	char buf[MAX_STRINGLENGTH];
	DvarValue newValue;

	Sys_EnterCriticalSection(CRITSECT_DVAR);

	/* New: sv_version dvar value sanitization */
	if ( sv_version && dvar == sv_version )
		string = getShortVersionFromProtocol(getProtocolFromShortVersion(string));
	/* New code end */

	I_strncpyz(buf, string, sizeof(buf));
	newValue = Dvar_StringToValue(dvar->type, dvar->domain, buf);
	if ( dvar->type == DVAR_TYPE_ENUM && newValue.integer == DVAR_INVALID_ENUM_INDEX )
	{
		Com_Printf("'%s' is not a valid value for dvar '%s'\n", buf, dvar->name);
		Dvar_PrintDomain(dvar->type, dvar->domain);
		newValue = dvar->reset;
	}

	Dvar_SetVariant(dvar, newValue, source);

	Sys_LeaveCriticalSection(CRITSECT_DVAR);
}

int hitchFrameTime = 0;
void hitch_warning_print(const char *message, int frameTime)
{
	// Called if 500 < frameTime && frameTime < 500000
	Com_Printf(message, frameTime);

	hitchFrameTime = frameTime;
}

void hook_bad_printf(const char *format, ...) {}

const char * custom_ClientConnect(unsigned int clientNum, unsigned int scriptPersId)
{
	XAnimTree *tree;
	clientInfo_t *ci;
	gentity_t *ent;
	char userinfo[1024];
	gclient_t *gclient;
	const char *password;
	uint16_t id;

	id = scriptPersId;
	ent = &g_entities[clientNum];
	gclient = &level.clients[clientNum];
	memset(gclient, 0, sizeof(gclient_s));
	ci = &level_bgs.clientinfo[clientNum];
	tree = level_bgs.clientinfo[clientNum].pXAnimTree;
	memset(ci, 0, sizeof(clientInfo_t));
	ci->pXAnimTree = tree;
	ci->infoValid = 1;
	ci->nextValid = 1;
	gclient->sess.connected = CON_CONNECTING;
	gclient->sess.pers = id;
	gclient->sess.state.team = TEAM_SPECTATOR;
	gclient->sess.sessionState = STATE_SPECTATOR;
	gclient->spectatorClient = -1;
	gclient->sess.forceSpectatorClient = -1;
	G_InitGentity(ent);
	ent->handler = ENT_HANDLER_NULL;
	ent->client = gclient;
	gclient->useHoldEntity = ENTITY_NONE;
	gclient->sess.state.clientIndex = clientNum;
	gclient->ps.clientNum = clientNum;
	ClientUserinfoChanged(clientNum);
	SV_GetUserinfo(clientNum, userinfo, MAX_STRINGLENGTH);
	if ( gclient->sess.localClient
	        || (password = Info_ValueForKey(userinfo, "password"), !*g_password->current.string)
	        || !I_stricmp(g_password->current.string, "none")
			// New: Time-constant string comparison for g_password dvar
	        || strcmp_constant_time(g_password->current.string, password) )
	{
		/* New code start: Save original client IP if provided by proxy */
		char preProxyIP[16];
	
		memset(&customPlayerState[clientNum].preProxyIP, 0, sizeof(customPlayerState[0].preProxyIP));
		I_strncpyz(preProxyIP, Info_ValueForKey(userinfo, "ip"), sizeof(customPlayerState[0].preProxyIP));
		if ( strlen(preProxyIP) != 0 )
		{
			I_strncpyz(customPlayerState[clientNum].preProxyIP, preProxyIP, sizeof(customPlayerState[0].preProxyIP));
			customPlayerState[clientNum].preProxyPort = atoi(Info_ValueForKey(userinfo, "port"));
		}
		/* New code end */

		Scr_PlayerConnect(ent);
		CalculateRanks();
		return 0;
	}
	else
	{
		G_FreeEntity(ent);
		return "GAME_INVALIDPASSWORD";
	}

	return 0;
}

qboolean IsNeededIwd(searchpath_t *search)
{
	// Include all non-localized files
	if ( search->localized == 0 )
		return qtrue;

	// Exclude localized files from main folder
	// iwdFilename: Absolute path to .iwd file, including suffix
	// iwdBasename: .iwd file name without suffix
	// iwdGamename: folder name (main and/or fs_game)
	if ( !strncmp(search->iwd->iwdGamename, "main", 4) )
		return qfalse;

	// If configured, include localized mod files
	if ( loc_loadLocalizedMods->current.boolean )
		return qtrue;
	else
		return qfalse;
}

int GetIW15Checksum(qboolean flipped)
{
	int checksum;

	if ( !strncmp(sv_version->current.string, "1.2", 3) )
	{
		checksum = 2103914635;
		if ( flipped )
			checksum = 181429573;
	}
	else
	{
		checksum = 181429573; // Fine for 1.0 and 1.3
		if ( flipped )
			checksum = 2103914635;
	}

	return checksum;
}

const char * custom_FS_LoadedIwdChecksums(qboolean flipped)
{
	static char info[BIG_INFO_STRING];
	searchpath_t *search;
	int checksum;

	info[0] = '\0';

	for ( search = fs_searchpaths; search != (searchpath_t *)0x0; search = search->next )
	{
		// New: Replaced "search->localized == 0" with "IsNeededIwd(search)"
		if ( search->iwd != NULL && IsNeededIwd(search) )
		{
			checksum = search->iwd->checksum;

			/* New code start: Multi-version support without iwd_15.iwd file
			 error on game version 1.2 */
			if ( !strncmp(search->iwd->iwdBasename, "iw_15", 5) )
				checksum = GetIW15Checksum(flipped);
			/* New code end */

			I_strncat(info, sizeof(info), custom_va("%i ", checksum));
		}
	}

	return info;
}

const char * custom_FS_LoadedIwdNames(void)
{
	static char info[BIG_INFO_STRING];
	searchpath_t *search;

	info[0] = '\0';

	for ( search = fs_searchpaths; search != (searchpath_t *)0x0; search = search->next )
	{
		// New: Replaced "search->localized == 0" with "IsNeededIwd(search)"
		if ( search->iwd != NULL && IsNeededIwd(search) )
		{
			if ( info[0] != '\0' )
			{
				I_strncat(info, BIG_INFO_STRING, " ");
			}
			I_strncat(info, sizeof(info), search->iwd->iwdBasename);
		}
	}

	return info;
}

const char * custom_FS_ReferencedIwdChecksums(qboolean flipped)
{
	searchpath_t *search;
	static char info[BIG_INFO_STRING];
	int checksum;

	info[0] = '\0';

	for ( search = fs_searchpaths ; search != (searchpath_t *)0x0 ; search = search->next )
	{
		if ( search->iwd != NULL && ( search->iwd->referenced || I_strnicmp(search->iwd->iwdGamename, "main", 4) ) )
		{
			checksum = search->iwd->checksum;

			/* New code start: Multi-version support without iwd_15.iwd file
			 error on game version 1.2 */
			if ( !strncmp(search->iwd->iwdBasename, "iw_15", 5) )
				checksum = GetIW15Checksum(flipped);
			/* New code end */

			I_strncat(info, sizeof(info), custom_va("%i ", checksum));
		}
	}

	return info;
}

void custom_SV_SaveSystemInfo()
{
	char info[BIG_INFO_STRING];
	char tempIwds[MAX_STRINGLENGTH];
	char tempReferencedIwds[MAX_STRINGLENGTH];

	I_strncpyz(info, Dvar_InfoString_Big(DVAR_SYSTEMINFO), sizeof(info));

	/* New code start: Alternative config string for clients connecting via
	 a different game version than what is set via the sv_version dvar */
	I_strncpyz(tempIwds, sv_iwds->current.string, sizeof(tempIwds));
	Dvar_SetString(sv_iwds, altIwds);
	I_strncpyz(tempReferencedIwds, sv_referencedIwds->current.string, sizeof(tempReferencedIwds));
	Dvar_SetString(sv_referencedIwds, altReferencedIwds);
	I_strncpyz(altSystemInfo, Dvar_InfoString_Big(DVAR_SYSTEMINFO), sizeof(altSystemInfo));
	Dvar_SetString(sv_iwds, tempIwds);
	Dvar_SetString(sv_referencedIwds, tempReferencedIwds);

	// We (re)register the dvar here so that any latched value is applied
	sv_version = Dvar_RegisterString("sv_version", "1.3", DVAR_ARCHIVE | DVAR_LATCH | DVAR_CHANGEABLE_RESET);

	// Update related dvars accordingly
	Dvar_SetIntByName("protocol", getProtocolFromShortVersion(sv_version->current.string));
	Dvar_SetStringByName("shortversion", sv_version->current.string);
	/* New code end */

	dvar_modifiedFlags &= ~DVAR_SYSTEMINFO;
	SV_SetConfigstring(CS_SYSTEMINFO, info);

	SV_SetConfigstring(CS_SERVERINFO, Dvar_InfoString(DVAR_SERVERINFO | DVAR_NORESTART));
	dvar_modifiedFlags &= ~(DVAR_SERVERINFO | DVAR_NORESTART);

	SV_SetConfig(142, 96, 256);
	dvar_modifiedFlags &= ~DVAR_CODINFO;
}

void custom_SV_SpawnServer(char *server)
{
	char mapname[64];
	int persist;
	client_t *cl;
	const char *dropreason;
	const char *iwdChecksums;
	const char *iwdNames;
	int checksum;
	int index;
	int i;

	Scr_ParseGameTypeList();
	SV_SetGametype();

	if ( com_sv_running->current.boolean )
	{
		persist = G_GetSavePersist();
		index = 0;
		cl = svs.clients;

		while ( index < sv_maxclients->current.integer )
		{
			if ( cl->state > CS_CONNECTED )
			{
				Com_sprintf(mapname, sizeof(mapname), "loadingnewmap\n%s\n%s", server, sv_gametype->current.string);
				NET_OutOfBandPrint(NS_SERVER, cl->netchan.remoteAddress, mapname);
			}

			++index;
			++cl;
		}

		NET_Sleep(250);
	}
	else
	{
		persist = 0;
	}

	Dvar_SetStringByName("mapname", server);
	SV_ShutdownGameProgs();
	Com_Printf("------ Server Initialization ------\n");
	Com_Printf("Server: %s\n", server);

	hook_developer_prints->hook(); // New

	SV_ClearServer();

	if ( com_dedicated->current.integer )
		FX_FreeSystem();

	FS_Shutdown();
	FS_ClearIwdReferences();
	Com_Restart();

	if ( com_sv_running->current.boolean )
		SV_ChangeMaxClients();
	else
		SV_Startup();

	I_strncpyz(sv.gametype, sv_gametype->current.string, 64);

	srand(Sys_MillisecondsRaw());
	sv.checksumFeed = rand() ^ rand() << 16 ^ Sys_Milliseconds();

	FS_Restart(sv.checksumFeed);
	Com_sprintf(mapname, 64, "maps/mp/%s.%s", server, GetBspExtension());
	SV_SetExpectedHunkUsage(mapname);

	for ( index = 0; index < MAX_CONFIGSTRINGS; ++index )
	{
		sv.configstrings[index] = CopyStringInternal("");
	}

	Dvar_ResetScriptInfo();

	svs.snapshotEntities = (entityState_t *)Hunk_AllocInternal(sizeof(entityState_t) * svs.numSnapshotEntities);
	svs.nextSnapshotEntities = 0;
	svs.snapshotClients = (clientState_t *)Hunk_AllocInternal(sizeof(clientState_t) * svs.numSnapshotClients);
	svs.nextSnapshotClients = 0;

	SV_InitArchivedSnapshot();

	svs.snapFlagServerBit ^= 4u;
	Dvar_SetString(nextmap, "map_restart");
	Dvar_SetInt(cl_paused, 0);

	Com_sprintf(mapname, 64, "maps/mp/%s.%s", server, GetBspExtension());

	Com_LoadBsp(mapname);
	CM_LoadMap(mapname, &checksum);
	Com_UnloadBsp();
	CM_LinkWorld();

	sv_serverId_value = (byte)(sv_serverId_value + 16);

	if ( (sv_serverId_value & 0xF0) == 0 )
		sv_serverId_value += 16;

	Dvar_SetInt(sv_serverid, sv_serverId_value);
	sv.start_frameTime = com_frameTime;
	sv.state = SS_LOADING;

	Com_sprintf(mapname, 64, "maps/mp/%s.%s", server, GetBspExtension());
	Com_LoadSoundAliases(mapname, "all_mp", SASYS_GAME);

	SV_InitGameProgs(persist);
	if ( com_dedicated->current.integer )
	{
		FX_InitSystem(0);
		FX_CreateDefaultEffect();
	}

	for ( i = 0; i < 3; ++i )
	{
		svs.time += 100;
		SV_RunFrame();
	}

	SV_CreateBaseline();

	for ( i = 0, cl = svs.clients; i < sv_maxclients->current.integer; ++i, ++cl )
	{
		if ( cl->state < CS_CONNECTED )
		{
			continue;
		}

		/* New: sv_botReconnectMode dvar */
		if ( cl->bIsTestClient )
		{
			if ( sv_botReconnectMode->current.integer == 1 )
			{
				SV_DropClient(cl, "EXE_DISCONNECTED");
				continue;
			}
			else if ( sv_botReconnectMode->current.integer == 2 )
			{
				usercmd_t ucmd;

				ClientConnect(i, cl->clscriptid);
				cl->state = CS_CONNECTED;
				SV_SendClientGameState(cl);
				memset(&ucmd, 0, sizeof(ucmd));
				SV_ClientEnterWorld(cl, &ucmd);
				continue;
			}
		}
		/* New code end */

		dropreason = ClientConnect(i, cl->clscriptid);

		if ( dropreason )
		{
			SV_DropClient(cl, dropreason);
			Com_Printf("SV_SpawnServer: dropped client %i - denied!\n", i); // New
		}
		else
		{
			cl->state = CS_CONNECTED;
		}
	}

	if ( sv_pure->current.boolean )
	{
		iwdChecksums = custom_FS_LoadedIwdChecksums(qfalse);
		Dvar_SetString(sv_iwds, iwdChecksums);

		if ( !*iwdChecksums )
			Com_Printf("WARNING: sv_pure set but no IWD files loaded\n");

		I_strncpyz(altIwds, custom_FS_LoadedIwdChecksums(qtrue), sizeof(altIwds)); // New

		iwdNames = custom_FS_LoadedIwdNames();
		Dvar_SetString(sv_iwdNames, iwdNames);
	}
	else
	{
		Dvar_SetString(sv_iwds, "");
		Dvar_SetString(sv_iwdNames, "");
	}

	Dvar_SetString(sv_referencedIwds, custom_FS_ReferencedIwdChecksums(qfalse));
	I_strncpyz(altReferencedIwds, custom_FS_ReferencedIwdChecksums(qtrue), sizeof(altReferencedIwds)); // New
	Dvar_SetString(sv_referencedIwdNames, FS_ReferencedIwdNames());

	/* New code start: sv_minimizeSysteminfo dvar */
	sv_minimizeSysteminfo = Dvar_RegisterInt("sv_minimizeSysteminfo", 0, 0, 3, DVAR_LATCH | DVAR_ARCHIVE);
	if ( sv_minimizeSysteminfo->current.integer )
	{
		// Get references to dvars if not done yet elsewhere
		bg_fallDamageMaxHeight = Dvar_FindVar("bg_fallDamageMaxHeight");
		bg_fallDamageMinHeight = Dvar_FindVar("bg_fallDamageMinHeight");
		com_timescale = Dvar_FindVar("timescale");
		g_synchronousClients = Dvar_FindVar("g_synchronousClients");

		// Remove SYSTEMINFO flag
		bg_fallDamageMaxHeight->flags &= ~DVAR_SYSTEMINFO;
		bg_fallDamageMinHeight->flags &= ~DVAR_SYSTEMINFO;
		com_timescale->flags &= ~DVAR_SYSTEMINFO;
		g_synchronousClients->flags &= ~DVAR_SYSTEMINFO;
		sv_cheats->flags &= ~DVAR_SYSTEMINFO;
		sv_disableClientConsole->flags &= ~DVAR_SYSTEMINFO;
		sv_voice->flags &= ~DVAR_SYSTEMINFO;
		sv_voiceQuality->flags &= ~DVAR_SYSTEMINFO;

		// Set CODINFO flag instead
		bg_fallDamageMaxHeight->flags |= DVAR_CODINFO;
		bg_fallDamageMinHeight->flags |= DVAR_CODINFO;
		com_timescale->flags |= DVAR_CODINFO;
		g_synchronousClients->flags |= DVAR_CODINFO;
		sv_cheats->flags |= DVAR_CODINFO;
		sv_disableClientConsole->flags |= DVAR_CODINFO;
		sv_voice->flags |= DVAR_CODINFO;
		sv_voiceQuality->flags |= DVAR_CODINFO;

		if ( sv_minimizeSysteminfo->current.integer > 1 )
		{
			jump_height->flags &= ~DVAR_SYSTEMINFO;
			jump_stepSize->flags &= ~DVAR_SYSTEMINFO;
			jump_slowdownEnable->flags &= ~DVAR_SYSTEMINFO;
			jump_ladderPushVel->flags &= ~DVAR_SYSTEMINFO;
			jump_spreadAdd->flags &= ~DVAR_SYSTEMINFO;

			jump_height->flags |= DVAR_CODINFO;
			jump_stepSize->flags |= DVAR_CODINFO;
			jump_slowdownEnable->flags |= DVAR_CODINFO;
			jump_ladderPushVel->flags |= DVAR_CODINFO;
			jump_spreadAdd->flags |= DVAR_CODINFO;

			if ( sv_minimizeSysteminfo->current.integer > 2 )
			{
				cl_allowDownload->flags &= ~DVAR_SYSTEMINFO;
				cl_wwwDownload->flags &= ~DVAR_SYSTEMINFO;

				cl_allowDownload->flags |= DVAR_CODINFO;
				cl_wwwDownload->flags |= DVAR_CODINFO;
			}
		}
	}
	/* New code end */

	custom_SV_SaveSystemInfo();

	sv.state = SS_GAME;
	SV_Heartbeat_f();

	SV_SetupProxies(); // New

	Com_Printf("-----------------------------------\n");

	if ( Dvar_GetBool("sv_punkbuster") == 0 )
		DisablePbSv();
	else
		EnablePbSv();
}

snd_alias_build_s *customSoundAliasInfo = NULL;
int customSoundAliasInfoCount = 0;
void hook_Com_MakeSoundAliasesPermanent(snd_alias_list_t *aliasList, SoundFileInfo *fileInfo)
{
	/* Save detailed sound alias info before Hunk_ClearTempMemory() wipes this
	  after Com_MakeSoundAliasesPermanent is called */
	if ( customSoundAliasInfo )
		Z_FreeInternal(customSoundAliasInfo);

	customSoundAliasInfo = (snd_alias_build_s *)Z_MallocInternal(sizeof(snd_alias_build_s) * saLoadedObjs);
	customSoundAliasInfoCount = 0;

	/* Members of saLoadObjGlob are not stored in a consecutive way in memory,
	  so we cannot just go with a single memcpy. The new list is stored that
	  way and the linked list pointers are updated */
	snd_alias_build_s *previousBuild = NULL;
	snd_alias_build_s *newBuild = NULL;
	for ( snd_alias_build_s *build = saLoadObjGlob; build; build = build->pNext, customSoundAliasInfoCount++ )
	{
		newBuild = customSoundAliasInfo + customSoundAliasInfoCount;
		memcpy(newBuild, build, sizeof(snd_alias_build_s));
		if ( previousBuild )
			previousBuild->pNext = newBuild;
		previousBuild = newBuild;
	}
	if ( newBuild )
		newBuild->pNext = NULL;

	// Call the original function at this address
	Com_MakeSoundAliasesPermanent(aliasList, fileInfo);
}

void custom_Sys_InitializeCriticalSections(void)
{
	int i;
	pthread_mutexattr_t muxattr;

	pthread_mutexattr_init(&muxattr);
	// New: PTHREAD_MUTEX_RECURSIVE instead of PTHREAD_MUTEX_NORMAL
	pthread_mutexattr_settype(&muxattr, PTHREAD_MUTEX_RECURSIVE);

	// New: Support for more mutexes as defined via criticalSection_t
	for ( i = 0; i < CRITSECT_COUNT; i++ )
	{
		pthread_mutex_init(&crit_sections[i], &muxattr);
	}

	pthread_mutexattr_destroy(&muxattr);

}

void custom_Sys_EnterCriticalSection(criticalSection_t section)
{
	pthread_mutex_lock(&crit_sections[section]);
}

void custom_Sys_LeaveCriticalSection(criticalSection_t section)
{
	pthread_mutex_unlock(&crit_sections[section]);
}

int Sys_TryEnterCriticalSection(criticalSection_t section)
{
	return pthread_mutex_trylock(&crit_sections[section]);
}

void custom_Sys_Quit(void)
{
	// Free stuff that is meant to be cleared on server (not map) quit
	if ( customSoundAliasInfo )
		Z_FreeInternal(customSoundAliasInfo);

	// Remove existing links to map library
	manymaps_cleanup();

	// Any proxy threads to cleanup?
	SV_ShutdownProxies();

	// Continue exit routines
	hook_sys_quit->unhook();
	void (*Sys_Quit)(void);
	*(int *)&Sys_Quit = hook_sys_quit->from;
	Sys_Quit();
	hook_sys_quit->hook();
}

void custom_SV_DirectConnect(netadr_t from)
{
	char userinfo[MAX_INFO_STRING];
	int i;
	client_t *cl, *newcl;
	gentity_t *ent;
	int clientNum;
	int version;
	int qport;
	int challenge;
	const char *PbAuthAddress;
	const char *PbAuthResult;
	const char *password;
	int startIndex;
	const char *denied;
	int count;
	int guid;
	char PBguid[33];
	char clientPBguid[33];

	/* New code start: Rate-limiting */
	if ( !from.type == NA_BOT && SVC_ApplyConnectLimit(from, OUTBOUND_BUCKET_MAIN) )
		return;
	/* New code end */

	memset(PBguid, 0, sizeof(PBguid));
	memset(clientPBguid, 0, sizeof(clientPBguid));

	Com_DPrintf("SV_DirectConnect()\n");

	I_strncpyz(userinfo, SV_Cmd_Argv(1), sizeof(userinfo));

	/* Some comments on stock connect variables:
	 - The "challenge" and "qport" variables are not present at bots
	 - The "name" client dvar can be set to an empty string without any issues
	   on the client side, so we do not require it to have a value here
	 - The "protocol" value is validated below
	*/

	version = atoi(Info_ValueForKey(userinfo, "protocol"));

	if ( version < 115 || version > 118 )
	{
		NET_OutOfBandPrint(NS_SERVER, from, custom_va("error\nEXE_SERVER_IS_DIFFERENT_VER\x15%s\n", "1.3"));
		Com_DPrintf("    rejected connect from protocol version %i (should be between %i and %i)\n", version, 115, 118);
		return;
	}

	challenge = atoi(Info_ValueForKey(userinfo, "challenge"));
	qport = atoi(Info_ValueForKey(userinfo, "qport"));

	for ( i = 0, cl = svs.clients; i < sv_maxclients->current.integer; i++, cl++ )
	{
		if ( NET_CompareBaseAdr(from, cl->netchan.remoteAddress)
		        && ( cl->netchan.qport == qport
		             || from.port == cl->netchan.remoteAddress.port ) )
		{
			if ( ( svs.time - cl->lastConnectTime )
			        < ( sv_reconnectlimit->current.integer * 1000 ) )
			{
				Com_DPrintf("%s:reconnect rejected : too soon\n", NET_AdrToString(from));
				return;
			}
			break;
		}
	}

	guid = 0;

	if ( !NET_IsLocalAddress(from) )
	{
		int ping;

		for ( i = 0 ; i < MAX_CHALLENGES ; i++ )
		{
			if ( NET_CompareAdr(from, svs.challenges[i].adr) )
			{
				if ( challenge == svs.challenges[i].challenge )
				{
					guid = svs.challenges[i].guid;
					I_strncpyz(PBguid, svs.challenges[i].PBguid, sizeof(PBguid));
					I_strncpyz(clientPBguid, svs.challenges[i].clientPBguid, sizeof(clientPBguid));
					break;
				}
			}
		}
		if ( i == MAX_CHALLENGES )
		{
			NET_OutOfBandPrint(NS_SERVER, from, "error\nEXE_BAD_CHALLENGE");
			return;
		}

		if ( svs.challenges[i].firstPing == 0 )
		{
			ping = svs.time - svs.challenges[i].pingTime;
			svs.challenges[i].firstPing = ping;
		}
		else
		{
			ping = svs.challenges[i].firstPing;
		}

		Com_Printf("Client %i connecting with %i challenge ping from %s\n", i, ping, NET_AdrToString(from));
		svs.challenges[i].connected = qtrue;

		if ( !Sys_IsLANAddress(from) )
		{
			if ( sv_minPing->current.integer && ping < sv_minPing->current.integer )
			{
				NET_OutOfBandPrint(NS_SERVER, from, "error\nEXE_ERR_HIGH_PING_ONLY");
				Com_DPrintf("Client %i rejected on a too low ping\n", i);
				return;
			}

			if ( sv_maxPing->current.integer && ping > sv_maxPing->current.integer )
			{
				NET_OutOfBandPrint(NS_SERVER, from, "error\nEXE_ERR_LOW_PING_ONLY");
				Com_DPrintf("Client %i rejected on a too high ping: %i\n", i, ping);
				return;
			}
		}
	}

	if ( !NET_IsLocalAddress(from) )
	{
		PbAuthAddress = NET_AdrToString(from);
	}
	else
	{
		PbAuthAddress = "localhost";
	}
	PbAuthResult = PbAuthClient(PbAuthAddress, atoi(Info_ValueForKey(userinfo, "cl_punkbuster")), PBguid);
	if ( !PbAuthResult )
	{
		for ( i = 0, cl = svs.clients; i < sv_maxclients->current.integer; i++, cl++ )
		{
			if ( cl->state == CS_FREE )
			{
				continue;
			}
			if ( NET_CompareBaseAdr(from, cl->netchan.remoteAddress)
					&& ( cl->netchan.qport == qport
						|| from.port == cl->netchan.remoteAddress.port ) )
			{
				Com_Printf("%s:reconnect\n", NET_AdrToString(from));

				if ( cl->state > CS_ZOMBIE )
					SV_FreeClient(cl);

				newcl = cl;
				goto LAB_0808ec36;
			}
		}

		password = Info_ValueForKey(userinfo, "password");

		// New: Time-constant string comparison for sv_privatePassword dvar
		if ( !strcmp_constant_time(password, sv_privatePassword->current.string) )
		{
			startIndex = 0;
		}
		else
		{
			startIndex = sv_privateClients->current.integer;
		}

		newcl = NULL;
		for ( i = startIndex; i < sv_maxclients->current.integer ; i++ )
		{
			cl = &svs.clients[i];
			if ( cl->state == CS_FREE )
			{
				newcl = cl;
				break;
			}
		}

		if ( !newcl )
		{
			NET_OutOfBandPrint(NS_SERVER, from, "error\nEXE_SERVERISFULL");
			Com_DPrintf("Rejected a connection.\n");
			return;
		}

		cl->reliableAcknowledge = 0;
		cl->reliableSequence = 0;
LAB_0808ec36:
		memset(newcl, 0, sizeof(client_t));
		clientNum = newcl - svs.clients;
		ent = SV_GentityNum(clientNum);
		newcl->gentity = ent;
		newcl->clscriptid = Scr_AllocArray();
		newcl->challenge = challenge;

		/* New code start: Save client protocol version */
		customPlayerState[clientNum].protocolVersion = version;
		Com_Printf("Connecting player #%i runs on version %s\n", clientNum, getShortVersionFromProtocol(version));
		/* New code end */

		if ( guid == 0 )
		{
			Com_Printf("Connecting player #%i has a zero GUID\n", clientNum);
		}
		newcl->guid = guid;
		Netchan_Setup(NS_SERVER, &newcl->netchan, from, qport);
		newcl->voicePacketCount = 0;
		newcl->sendVoice = 1;
		I_strncpyz(newcl->userinfo, userinfo, sizeof(newcl->userinfo));
		denied = ClientConnect(clientNum, newcl->clscriptid);
		if ( !denied )
		{
			Com_Printf("Going from CS_FREE to CS_CONNECTED for %s (num %i guid %i)\n", newcl->name, clientNum, newcl->guid);

			newcl->state = CS_CONNECTED;
			newcl->nextSnapshotTime = svs.time;
			newcl->lastPacketTime = svs.time;
			newcl->lastConnectTime = svs.time;

			SV_UserinfoChanged(newcl);

			svs.challenges[i].firstPing = 0;

			NET_OutOfBandPrint(NS_SERVER, from, "connectResponse");

			newcl->gamestateMessageNum = -1;
			count = 0;
			for ( i = 0, cl = svs.clients; i < sv_maxclients->current.integer; i++, cl++ )
			{
				if ( svs.clients[i].state >= CS_CONNECTED )
				{
					count++;
				}
			}
			if ( count == 1 || count == sv_maxclients->current.integer )
			{
				SV_Heartbeat_f();
			}
		}
		else
		{
			NET_OutOfBandPrint(NS_SERVER, from, custom_va("error\n%s", denied));
			Com_DPrintf("Game rejected a connection: %s.\n", denied);
			SV_FreeClientScriptId(newcl);

			/* New code start: Remove rejected client from scoreboard. This
			 does not cover rejection by pbsv.m_AuthClient (PunkBuster) */
			gclient_t *gclient = &level.clients[newcl - svs.clients];

			gclient->sess.connected = CON_DISCONNECTED;
			CalculateRanks();
			/* New code end */
			return;
		}
	}
	else
	{
		if ( !I_strnicmp(PbAuthResult, "error\n", 6) )
		{
			NET_OutOfBandPrint(NS_SERVER, from, PbAuthResult);
		}
	}
}

qboolean logHeartbeat = qtrue;
void custom_SV_MasterHeartbeat(const char *hbname)
{
	int sending_heartbeat_string_offset = 0x0814BBC0;

	if ( logHeartbeat && !sv_logHeartbeats->current.boolean )
	{
		byte disable = 0;
		memcpy((void *)sending_heartbeat_string_offset, &disable, 1);
		logHeartbeat = qfalse;
		Com_DPrintf("Disabled heartbeat logging\n");
	}
	else if ( !logHeartbeat && sv_logHeartbeats->current.boolean )
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
	unsigned int i;
	char path_for_cb[512] = "maps/mp/gametypes/_callbacksetup";

	hook_gametype_scripts->unhook();
	int (*GScr_LoadGameTypeScript)();
	*(int *)&GScr_LoadGameTypeScript = hook_gametype_scripts->from;
	int ret = GScr_LoadGameTypeScript();
	hook_gametype_scripts->hook();

	if ( strlen(fs_callbacks->current.string) )
		strncpy(path_for_cb, fs_callbacks->current.string, sizeof(path_for_cb));

	for ( i = 0; i < sizeof(callbacks)/sizeof(callbacks[0]); i++ )
	{
		*callbacks[i].pos = Scr_GetFunctionHandle(path_for_cb, callbacks[i].name, 0);
		if ( *callbacks[i].pos && g_debugCallbacks->current.boolean )
			Com_Printf("%s found @ %p\n", callbacks[i].name, scrVarPub.programBuffer + *callbacks[i].pos);
	}

	return ret;
}

void custom_G_AddPlayerMantleBlockage(float *endPos, int duration, pmove_t *pm)
{
	gentity_t *owner;
	gentity_t *ent;

	/* New code start: g_mantleBlockEnable dvar */
	if ( !g_mantleBlockEnable->current.boolean )
		return;
	/* New code end */

	owner = &g_entities[pm->ps->clientNum];
	ent = G_Spawn();
	ent->parent = owner;
	ent->r.ownerNum = pm->ps->clientNum;
	ent->r.contents = CONTENTS_PLAYERCLIP;
	ent->clipmask = CONTENTS_PLAYERCLIP;
	ent->r.svFlags = 33;
	ent->s.eType = ET_INVISIBLE;
	ent->handler = ENT_HANDLER_PLAYER_BLOCK;
	VectorCopy(owner->r.mins, ent->r.mins);
	VectorCopy(owner->r.maxs, ent->r.maxs);
	G_SetOrigin(ent, endPos);
	SV_LinkEntity(ent);
	ent->nextthink = g_mantleBlockTimeBuffer->current.integer + level.time + duration;
}

qboolean SkipCollision(gentity_t *client1, gentity_t *client2)
{
	int id1 = client1 - g_entities;
	int id2 = client2 - g_entities;

	if ( id1 < MAX_CLIENTS && id2 < MAX_CLIENTS && client1->client && client2->client && client1->client->sess.connected == CON_CONNECTED && client2->client->sess.connected == CON_CONNECTED )
	{
		if ( customPlayerState[id1].collisionTeam == CUSTOM_TEAM_NONE || customPlayerState[id2].collisionTeam == CUSTOM_TEAM_NONE )
			return qtrue;

		if ( customPlayerState[id1].collisionTeam == CUSTOM_TEAM_AXIS && (client2->client->sess.state).team != TEAM_AXIS )
			return qtrue;

		if ( customPlayerState[id1].collisionTeam == CUSTOM_TEAM_ALLIES && (client2->client->sess.state).team != TEAM_ALLIES )
			return qtrue;

		if ( customPlayerState[id2].collisionTeam == CUSTOM_TEAM_AXIS && (client1->client->sess.state).team != TEAM_AXIS )
			return qtrue;

		if ( customPlayerState[id2].collisionTeam == CUSTOM_TEAM_ALLIES && (client1->client->sess.state).team != TEAM_ALLIES )
			return qtrue;
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
		/* New code start: per-player/team collison */
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
				trace->entityNum = (touch->s).number;
			}
		}
	}
}

void custom_G_SetClientContents(gentity_t *ent)
{
	int id = ent - g_entities;

	/* New code start: g_playerCollision dvar */
	if ( !g_playerCollision->current.boolean )
		return;
	/* New code end */

	if ( ent->client->noclip == 0 )
	{
		if ( ent->client->ufo == 0 )
		{
			if ( (ent->client->sess).sessionState == STATE_DEAD )
			{
				(ent->r).contents = 0;
			}
			else
			{
				(ent->r).contents = CONTENTS_BODY;

				/* New code start: per-player/team collison */
				if ( customPlayerState[id].overrideContents )
					(ent->r).contents = customPlayerState[id].contents;
				/* New code end */
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

	/* New code start: g_playerEject dvar */
	if ( !g_playerEject->current.boolean )
		return qfalse;
	/* New code end */

	if ( ( ( ((self->client->ps).pm_flags & PMF_VIEWLOCKED) != 0 ) && ( (self->client->sess).sessionState == STATE_PLAYING ) ) && ( customPlayerState[id].collisionTeam != CUSTOM_TEAM_AXIS_ALLIES /* New condition */ || ( ((self->r).contents & CONTENTS_BODY) != 0 || ( (self->r).contents == CONTENTS_CORPSE ) ) ) )
	{
		hit = g_entities;
		for ( i = 0; i < level.maxclients; i++, hit++)
		{
			if ( ( ( ( ( ( (hit->r).inuse != 0 ) && ( ((hit->client->ps).pm_flags & PMF_VIEWLOCKED) != 0 ) ) && ( (hit->client->sess).sessionState == STATE_PLAYING )  ) && ( (hit != self && hit->client != NULL ) ) ) && ( 0 < hit->health && ( /* New condition */ customPlayerState[i].collisionTeam != CUSTOM_TEAM_AXIS_ALLIES || ( ((hit->r).contents & CONTENTS_BODY) != 0 || ( (hit->r).contents == CONTENTS_CORPSE ) ) ) ) ) &&
			( (hit->r).absmin[0] <= (self->r).absmax[0] && ( ( ( (self->r).absmin[0] <= (hit->r).absmax[0] && ( (hit->r).absmin[1] <= (self->r).absmax[1] ) ) && ( (self->r).absmin[1] <= (hit->r).absmax[1] ) ) && ( (hit->r).absmin[2] <= (self->r).absmax[2] && ( (self->r).absmin[2] <= (hit->r).absmax[2] ) ) ) ) )
			{
				/* New code start: per-player/team collison */
				if ( SkipCollision(self, hit) )
					continue;
				/* New code end */

				VectorSubtract2((hit->r).currentOrigin, (self->r).currentOrigin, dir);
				fTemp = (self->r).maxs[0] + (hit->r).maxs[0];
				dTemp = DotProduct2(dir, dir);
				if ( dTemp <= ( (double)fTemp * (double)fTemp ) )
				{
					VectorSubtract2((hit->r).currentOrigin, (self->r).currentOrigin, dir);
					fTemp = G_random();
					dir[0] = dir[0] + ((fTemp + fTemp) - 1.0);
					fTemp = G_random();
					dir[1] = dir[1] + ((fTemp + fTemp) - 1.0);
					Vec2Normalize(dir);
					if ( 0.0 < VectorLength2((hit->client->ps).velocity) )
					{
						hitEjectSpeed = (float)g_playerCollisionEjectSpeed->current.integer;
					}
					else
					{
						hitEjectSpeed = 0.0;
					}
					hitSpeed = hitEjectSpeed;
					if ( 0.0 < VectorLength2((self->client->ps).velocity) )
					{
						selfEjectSpeed = (float)g_playerCollisionEjectSpeed->current.integer;
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
					(hit->client->ps).pm_time = g_playerCollisionEjectDuration->current.integer; // New: g_playerCollisionEjectDuration dvar
					(hit->client->ps).pm_flags = (hit->client->ps).pm_flags | PMF_SLIDING;
					VectorScale2(dir, selfSpeed * -1, (self->client->ps).velocity);
					(self->client->ps).pm_time = g_playerCollisionEjectDuration->current.integer; // New: g_playerCollisionEjectDuration dvar
					(self->client->ps).pm_flags = (self->client->ps).pm_flags | PMF_SLIDING;
					if ( !g_playerCollisionEjectDamageAllowed->current.boolean ) // New: g_playerCollisionEjectDamageAllowed dvar
						return qtrue;
					else
						return qfalse;
				}
			}
		}
	}
	return qfalse;
}

gentity_t * custom_fire_grenade(gentity_t *attacker, vec3_t start, vec3_t dir, int weaponIndex, int fuseTime)
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
		if ( i == 1 && tmp[0] >= 20 && tmp[0] <= 22 )
		{
			char *part = strtok(tmp + 1, " ");
			while ( part != NULL )
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

const char * hook_AuthorizeState(int arg)
{
	const char *s = Cmd_Argv(arg);

	if ( sv_cracked->current.boolean && strcmp(s, "deny") == 0 )
		return "accept";

	return s;
}

void custom_ClientUserinfoChanged(int clientNum)
{
	if ( !Scr_IsSystemActive() || sv.state != SS_GAME )
		return;

	if ( codecallback_userinfochanged )
	{	
		short ret;
		client_t *cl = &svs.clients[clientNum];

		// Do not call the callback right on connect as at that point the
		// client is not yet active from a script perspective. Therefore,
		// CodeCallback_PlayerConnect remains the first script callback that is
		// called on a new client.
		if ( cl->state >= CS_CONNECTED )
		{
			ret = Scr_ExecEntThread(&g_entities[clientNum], codecallback_userinfochanged, 0);
			Scr_FreeThread(ret);
			return;
		}
	}

	// This contains all logic from stock ClientUserinfoChanged, and more
	ProcessClientUserinfoChange(clientNum);
}

void ForceServerSnapsAndRate(client_t *client)
{
	if ( g_forceSnaps->current.integer > 0 )
	{
		client->snapshotMsec = 1000 / g_forceSnaps->current.integer;
	}
	if ( g_forceRate->current.integer > 1000 )
	{
		client->rate = g_forceRate->current.integer;
	}
}

void ProcessClientUserinfoChange(int clientNum)
{
	char userinfo[MAX_STRINGLENGTH];
	char oldname[MAX_STRINGLENGTH];
	const char *value;
	gentity_t *entity = &g_entities[clientNum];
	gclient_t *client = entity->client;
	client_t *cl = &svs.clients[clientNum];

	SV_GetUserinfo(clientNum, userinfo, MAX_STRINGLENGTH);

	if ( !Info_Validate(userinfo) )
		strcpy(userinfo, "\\name\\badinfo");

	client->sess.localClient = SV_IsLocalClient(clientNum);
	value = Info_ValueForKey(userinfo, "cg_predictItems");

	if ( atoi(value) )
		client->sess.predictItemPickup = 1;
	else
		client->sess.predictItemPickup = 0;

	if ( client->sess.connected == CON_CONNECTED && level.manualNameChange )
	{
		value = Info_ValueForKey(userinfo, "name");
		ClientCleanName(value, client->sess.name, 32);
	}
	else
	{
		I_strncpyz(oldname, client->sess.state.name, MAX_STRINGLENGTH);
		value = Info_ValueForKey(userinfo, "name");
		ClientCleanName(value, client->sess.state.name, 32);
		I_strncpyz(client->sess.name, client->sess.state.name, 32);
	}

	level_bgs.clientinfo[clientNum].clientNum = clientNum;
	I_strncpyz(level_bgs.clientinfo[clientNum].name, client->sess.state.name, 32);
	level_bgs.clientinfo[clientNum].team = client->sess.state.team;

	ForceServerSnapsAndRate(cl); // New
}

void custom_DeathmatchScoreboardMessage(gentity_t *ent)
{
	int ping;
	int clientNum;
	int numSorted;
	gclient_t *client;
	int len;
	int i;
	int stringlength;
	char string[1400];
	char entry[1024];
	int visiblePlayers; // New: Used for ability to hide players in scoreboard

	string[0] = 0;
	stringlength = 0;
	visiblePlayers = 0;

	numSorted = level.numConnectedClients;

	if ( level.numConnectedClients > MAX_CLIENTS )
		numSorted = MAX_CLIENTS;

	for ( i = 0; i < numSorted; ++i )
	{
		clientNum = level.sortedClients[i];
		client = &level.clients[clientNum];

		/* New code start: Ability to hide players in scoreboard */
		if ( customPlayerState[clientNum].hiddenFromScoreboard )
			continue;
		/* New code end */

		if ( client->sess.connected == CON_CONNECTING )
		{
			Com_sprintf(
			    entry,
			    0x400u,
			    " %i %i %i %i %i",
			    level.sortedClients[i],
			    client->sess.score,
			    -1,
			    client->sess.deaths,
			    client->sess.statusIcon);
		}
		else
		{
			ping = SV_GetClientPing(clientNum);

			Com_sprintf(
			    entry,
			    0x400u,
			    " %i %i %i %i %i",
			    level.sortedClients[i],
			    client->sess.score,
			    ping,
			    client->sess.deaths,
			    client->sess.statusIcon);
		}

		len = strlen(entry);

		if ( stringlength + len > 1024 )
			break;

		strcpy(&string[stringlength], entry);
		stringlength += len;
		visiblePlayers++;
	}

	SV_GameSendServerCommand(ent - g_entities, SV_CMD_RELIABLE, custom_va("b %i %i %i%s", visiblePlayers, level.teamScores[1], level.teamScores[2], string));
}

void custom_SV_DropClient(client_t *drop, const char *reason)
{
	int i;
	challenge_t *challenge;
	qboolean isBot = qfalse;
	const char *translatedReason;
	qboolean showIngameMessage = qtrue; // New
	char name[32];
	
	Com_DPrintf("SV_DropClient for %s\n", drop->name);

	if ( drop->state == CS_ZOMBIE )
		return;	// Already dropped

	/* New code start: libcod cleanup */
	for ( i = 0 ; i < sv_maxclients->current.integer; i++ )
		customPlayerState[i].talkerIcons[drop - svs.clients] = 0;
	/* New code end */

	drop->dropReason = NULL;
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

	if ( isBot && !sv_botKickMessages->current.boolean && ( I_stricmp(reason, "EXE_DISCONNECTED") == 0 || I_stricmp(reason, "EXE_PLAYERKICKED") == 0 ) )
		showIngameMessage = qfalse;

	if ( !sv_kickMessages->current.boolean && I_stricmp(reason, "EXE_PLAYERKICKED") == 0 )
		showIngameMessage = qfalse; // This overrides enabled sv_botKickMessages

	if ( !sv_timeoutMessages->current.boolean && I_stricmp(reason, "EXE_TIMEDOUT") == 0 )
		showIngameMessage = qfalse;

	if ( !sv_disconnectMessages->current.boolean && I_stricmp(reason, "EXE_DISCONNECTED") == 0 )
		showIngameMessage = qfalse;

	if ( sv_wwwDlDisconnectedMessages->current.integer != 1 && I_stricmp(reason, "PC_PATCH_1_1_DOWNLOADDISCONNECTED") == 0 )
		showIngameMessage = qfalse;

	translatedReason = SEH_StringEd_GetString(reason);

	if ( showIngameMessage )
	{
		if ( !translatedReason )
			SV_SendServerCommand(0, SV_CMD_CAN_IGNORE, "e \"\x15%s^7 %s%s\"", name, "", reason);
		else
			SV_SendServerCommand(0, SV_CMD_CAN_IGNORE, "e \"\x15%s^7 %s%s\"", name, "\x14", reason);
	}

	Com_Printf("%i:%s %s\n", drop - svs.clients, name, reason);

	SV_SendServerCommand(0, SV_CMD_RELIABLE, "J %d", drop - svs.clients);
	if ( !translatedReason )
		SV_SendServerCommand(drop, SV_CMD_RELIABLE, "w \"%s^7 %s\" PB", name, reason);
	else
		SV_SendServerCommand(drop, SV_CMD_RELIABLE, "w \"%s\"", reason);

	/* If this was the last client on the server, send a heartbeat to the
	 master so it is known the server is empty send a heartbeat now so the
	 master will get up to date info if there is already a slot for this ip,
	 reuse it */
	for ( i = 0 ; i < sv_maxclients->current.integer ; i++ )
	{
		if ( svs.clients[i].state >= CS_CONNECTED )
			break;
	}
	if ( i == sv_maxclients->current.integer )
		SV_Heartbeat_f();
}

void custom_Touch_Item_Auto(gentity_t * item, gentity_t * entity, int touch)
{
	if ( customPlayerState[entity->client->ps.clientNum].noPickup )
		return;
	
	hook_touch_item_auto->unhook();
	Touch_Item_Auto(item, entity, touch);
	hook_touch_item_auto->hook();
}

void custom_Touch_Item(gentity_t *item, gentity_t *entity, int touch)
{
	gclient_t *client;
	entity_event_t event;
	gitem_t *bg_item;
	char name[0x40];
	int respawn;
	itemType_t type;
	
	if ( !item->active )
		return;
	
	item->active = 0;
	client = entity->client;
	
	if ( !client || entity->health <= 0 || level.clientIsSpawning )
		return;

	event = EV_ITEM_PICKUP;
	bg_item = &bg_itemlist;
	bg_item += item->item[0].index;
	
	if ( !BG_CanItemBeGrabbed(&item->s, &entity->client->ps, touch) )
	{
		if ( ( !touch && (item->s).clientNum != (entity->s).number ) && bg_item->giType == IT_WEAPON )
		{
			if ( !COM_BitCheck(entity->client->ps.weapons, bg_item->giTag) )
			{
				if ( ( BG_WeaponDefs(bg_item->giTag)->weapSlot + ~WEAPSLOT_NONE ) < 2 )
					SV_GameSendServerCommand(entity - g_entities, SV_CMD_CAN_IGNORE, custom_va("%c \"GAME_CANT_GET_PRIMARY_WEAP_MESSAGE\"", 0x66));
			}
			else
			{
				SV_GameSendServerCommand(entity - g_entities, SV_CMD_CAN_IGNORE, custom_va("%c \"GAME_PICKUP_CANTCARRYMOREAMMO\x14%s\"", 0x66, BG_WeaponDefs(bg_item->giTag)->szDisplayName));
			}
		}
	}
	else
	{
		I_strncpyz(name, (entity->client->sess.state).name, sizeof(name));
		I_CleanStr(name);
		
		// New: g_logPickup dvar
		if ( g_logPickup->current.boolean )
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
			/* New code start: CodeCallback_Pickup */
			if ( codecallback_pickup && Scr_IsSystemActive() )
			{
				stackPushString("ammo");
				short ret = Scr_ExecEntThread(entity, codecallback_pickup, 1);
				Scr_FreeThread(ret);
			}
			/* New code end */
			else
			{
				respawn = Pickup_Ammo(item, entity);
			}
		}
		else if ( type < 3 )
		{
			if ( type != IT_WEAPON )
				return;

			/* New code start: CodeCallback_Pickup */
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
			/* New code end */
			else
			{
				respawn = Pickup_Weapon(item, entity, &event, touch);
			}
		}
		else
		{
			if ( type != IT_HEALTH )
				return;
			
			/* New code start: CodeCallback_Pickup */
			if ( codecallback_pickup && Scr_IsSystemActive() )
			{
				stackPushString("health");
				short ret = Scr_ExecEntThread(entity, codecallback_pickup, 1);
				Scr_FreeThread(ret);
			}
			/* New code end */
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
		if ( g_debugEvents->current.boolean )
			Com_DPrintf("BG_AddPredictableEventToPlayerstate() event %26s for client %2d\n", *(&entity_event_names + event), ps->clientNum);
		
		/* New code start: silent */
		if ( ( ( event >= EV_FOOTSTEP_RUN_DEFAULT && event <= EV_FOLIAGE_SOUND ) || event == EV_NOAMMO ) && customPlayerState[ps->clientNum].silent )
			return;
		/* New code end */
		
		ps->events[ps->eventSequence & ( MAX_EVENTS - 1 )] = event & 0xff;
		ps->eventParms[ps->eventSequence & ( MAX_EVENTS - 1 )] = eventParm & 0xff;
		ps->eventSequence++;
	}
}

void custom_G_AddEvent(gentity_t *ent, int event, int eventParm)
{
	if ( ent->client )
	{
		if ( g_debugEvents->current.boolean )
			Com_DPrintf("G_AddEvent() event %26s for client %2d\n", *(&entity_event_names + event), ent->client->ps.clientNum);
		ent->client->ps.events[ent->client->ps.eventSequence & ( MAX_EVENTS - 1 )] = event;
		ent->client->ps.eventParms[ent->client->ps.eventSequence & ( MAX_EVENTS - 1 )] = eventParm;
		ent->client->ps.eventSequence++;
	}
	else
	{
		if ( g_debugEvents->current.boolean )
			Com_DPrintf("G_AddEvent() event %26s for entity %2d\n", *(&entity_event_names + event), ent->s.number);
		ent->s.events[ent->s.eventSequence & ( MAX_EVENTS - 1 )] = event;
		ent->s.eventParms[ent->s.eventSequence & ( MAX_EVENTS - 1 )] = eventParm;
		ent->s.eventSequence++;
	}
	ent->eventTime = level.time;
	ent->r.eventTime = level.time;

	/* New code start: CodeCallback_EntityEvent */
	if ( codecallback_entityevent && Scr_IsSystemActive() )
	{
		stackPushInt(eventParm);
		stackPushInt(event);
		short ret = Scr_ExecEntThread(ent, codecallback_entityevent, 2);
		Scr_FreeThread(ret);
	}
	/* New code end */
}

gentity_t * custom_G_TempEntity(vec3_t origin, int event)
{
	hook_g_tempentity->unhook();

	gentity_t* (*sig)(vec3_t origin, int event);
	*(int *)&sig = hook_g_tempentity->from;

	if ( g_debugEvents->current.boolean )
		Com_DPrintf("G_TempEntity() event %26s at (%f,%f,%f)\n", *(&entity_event_names + event), origin[0], origin[1], origin[2]);

	/* Filter example:
	if (event == EV_PLAY_FX)
		event = EV_NONE;
	 Note: This can cause script runtime errors (or worse) on some events,
	 e.g., when using the obituary function, due to (then) undefined
	 parameters */
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
			while ( absbits )
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
	// New variables from here
	int playFxOnTagForPlayerTargetNum = 0; // For playFxOnTagForPlayer
	int team; // For custom obituary
	vec3_t origin; 	// For custom obituary
	int maxDistance; // For custom obituary
	customTeam_t headIconTeam = CUSTOM_TEAM_NONE; // For custom headIconTeam

	if ( !to )
	{
		if ( bChangeBit )
			MSG_WriteBit1(msg);
		MSG_WriteBits(msg, from->number, indexBits);
		MSG_WriteBit1(msg);
	}
	else
	{
		/* New code start: Collect data for custom entity field usage */
		field = stateFieldsPointer;
		for ( i = 0; i < numFields; i++, field++ )
		{
			fromF = ( int * )( (byte *)from + field->offset );
			toF = ( int * )( (byte *)to + field->offset );

			// entityStateFields
			if ( numFields == 0x3b )
			{
				/* New code start: Custom headIconTeam */
				if ( i == 17 ) // scale
					headIconTeam = (customTeam_t)*toF;
				/* New code end */
				
				/* New code start: playFxOnTagForPlayer */
				if ( i == 49 ) // attackerEntityNum
					playFxOnTagForPlayerTargetNum = *toF;
				/* New code end */
				
				/* New code start: obituary */
				if ( i == 17 ) // scale
					team = *toF;
				else if ( i == 58 ) // dmgFlags
					maxDistance = *toF;
				else if ( i == 2 ) // pos.trBase[0]
					origin[0] = *(vec_t *)toF;
				else if ( i == 1 ) // pos.trBase[1]
					origin[1] = *(vec_t *)toF;
				else if ( i == 8 ) // pos.trBase[2]
					origin[2] = *(vec_t *)toF;
				/* New code end */
			}
		}
		/* New code end */

		lc = 0;
		field = stateFieldsPointer;
		for ( i = 0; i < numFields; i++, field++ )
		{
			fromF = ( int * )( (byte *)from + field->offset );
			toF = ( int * )( (byte *)to + field->offset );

			/* New code start: Apply data for custom entity field usage
			 Here in the optimization loop we apply the changes to entity
			 fields where necessary, so that lc is updated accordingly */
			if ( numFields == 0x3b )
			{
				/* New code start: (not)SolidForPlayer */
				if ( i == 13 ) // eFlags
				{
					if ( clientNum != -1 && entNum >= 0 )
					{
						if ( customEntityState[entNum].notSolidBrushModel )
						{
							if ( customEntityState[entNum].clientMask[clientNum >> 5] & (1 << (clientNum & 0x1F)) )
								*toF |= 0x1;
						}
					}
				}
				/* New code end */

				/* New code start: Custom headIconTeam */
				// entityStateFields: iHeadIconTeam
				if ( i == 46 )
				{
					client_t *client = NULL;
					if ( spectatorClientNum != -1 )
						client = &svs.clients[spectatorClientNum];
					else if ( clientNum != -1 )
						client = &svs.clients[clientNum];

					// ClientDisconnect() nulls parts of clientSession_t
					if ( client && client->gentity && client->gentity->client && client->gentity->client->sess.connected == CON_CONNECTED )
					{
						int clientTeam = (client->gentity->client->sess.state).team;
						if ( headIconTeam && headIconTeam != CUSTOM_TEAM_NONE )
						{
							if ( clientTeam == TEAM_AXIS )
							{
								if ( headIconTeam == CUSTOM_TEAM_AXIS ||
									headIconTeam == CUSTOM_TEAM_AXIS_ALLIES ||
									headIconTeam == CUSTOM_TEAM_AXIS_SPECTATOR ||
									headIconTeam == CUSTOM_TEAM_ALL )
									*toF = TEAM_AXIS;
								else
									*toF = TEAM_NONE;
							}
							else if ( clientTeam == TEAM_ALLIES )
							{
								if ( headIconTeam == CUSTOM_TEAM_ALLIES ||
									headIconTeam == CUSTOM_TEAM_AXIS_ALLIES ||
									headIconTeam == CUSTOM_TEAM_ALLIES_SPECTATOR ||
									headIconTeam == CUSTOM_TEAM_ALL )
									*toF = TEAM_ALLIES;
								else
									*toF = TEAM_NONE;
							}
							else if ( clientTeam == TEAM_SPECTATOR )
							{
								if ( headIconTeam == CUSTOM_TEAM_SPECTATOR ||
									headIconTeam == CUSTOM_TEAM_AXIS_SPECTATOR ||
									headIconTeam == CUSTOM_TEAM_ALLIES_SPECTATOR ||
									headIconTeam == CUSTOM_TEAM_ALL )
									*toF = TEAM_SPECTATOR;
								else
									*toF = TEAM_NONE;
							}
							else // TEAM_NONE
							{
								*toF = TEAM_NONE;
							}
						}
					}
				}
				/* New code end */

				/* New code start: per-player/team collison */
				if ( i == 31 ) // solid
				{
					if ( clientNum != -1 && entNum >= 0 && entNum < MAX_CLIENTS )
					{
						if ( SkipCollision(&g_entities[clientNum], &g_entities[entNum]) )
							*toF = 0;
					}
				}
				/* New code end */

				/* New code start: playFxOnTagForPlayer */
				// entityStateFields: events[0], ..., events[3]
				if ( i == 21 || i == 22 || i == 23 || i == 25 )
				{
					toF = ( int * )( (byte *)to + field->offset );
					if ( *toF == EV_PLAY_FX_ON_TAG && clientNum != playFxOnTagForPlayerTargetNum - 1 && spectatorClientNum != playFxOnTagForPlayerTargetNum - 1 )
						*toF = EV_NONE;
				}
				/* New code end */
				
				/* New code start: obituary */
				// entityStateFields: eType
				if ( i == 12 )
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
						if ( client && client->gentity && client->gentity->client && client->gentity->client->sess.connected == CON_CONNECTED )
						{
							int clientTeam = (client->gentity->client->sess.state).team;
							if ( team )
							{
								if ( team != clientTeam && (
									( clientTeam == TEAM_AXIS && team == CUSTOM_TEAM_ALLIES_SPECTATOR ) || 
									( clientTeam == TEAM_ALLIES && team == CUSTOM_TEAM_AXIS_SPECTATOR ) || 
									( clientTeam == TEAM_SPECTATOR && team == CUSTOM_TEAM_AXIS_ALLIES ) ) )
									*toF = EV_NONE;
							}
							
							if ( maxDistance > 0 )
							{
								double distance = Vec3DistanceSq(client->gentity->r.currentOrigin, origin);
								if ( (int)distance > ( maxDistance * maxDistance ) )
									*toF = EV_NONE;
							}
						}
					}
				}
			}
			/* New code end (obituary) */
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
				custom_MSG_WriteDeltaField(msg, (byte *)from, (byte *)to, field);
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
					while ( absbits )
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
					if ( customPlayerState[spectatorClientNum].noEarthquakes )
						disable = qtrue;
				}
			}
			else
			{
				// Client plays normally
				if ( customPlayerState[clientNum].noEarthquakes )
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
		client->sendAsActive = 1;
	else if ( client->state != CS_ZOMBIE )
		client->sendAsActive = 0;

	if ( client->sendAsActive == 0 )
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
	
	for ( i = 0; i < sv_padPackets->current.integer; i++ )
		MSG_WriteByte(msg, 0);
}

qboolean custom_Netchan_TransmitNextFragment(netchan_t *chan)
{
	msg_t send;
	byte send_buf[MAX_PACKETLEN];
	int fragmentLength;
	int status;
	int temp = (int)chan - 0x6E6C4;
	client_t *client = (client_t *)temp;
	int id = client - svs.clients;

	NetProf_PrepProfiling(&chan->pProf);
	MSG_Init(&send, send_buf, sizeof(send_buf));
	MSG_WriteLong(&send, chan->outgoingSequence | FRAGMENT_BIT);

	if ( chan->sock == NS_CLIENT )
	{
		MSG_WriteShort(&send, chan->qport);
	}

	fragmentLength = FRAGMENT_SIZE;

	if ( chan->unsentFragmentStart  + fragmentLength > chan->unsentLength )
	{
		fragmentLength = chan->unsentLength - chan->unsentFragmentStart;
	}

	/* New code start: Multi version support */
	if ( customPlayerState[id].protocolVersion < 118 )
		MSG_WriteShort(&send, chan->unsentFragmentStart);
	else
		MSG_WriteLong(&send, chan->unsentFragmentStart);
	/* New code end */

	MSG_WriteShort(&send, fragmentLength);
	MSG_WriteData(&send, chan->unsentBuffer + chan->unsentFragmentStart, fragmentLength);

	status = NET_SendPacket(chan->sock, send.cursize, send.data, chan->remoteAddress);
	NetProf_SendProfile(chan, send.cursize, 1);

	if ( showpackets->current.boolean )
	{
		Com_Printf("%s send %4i : s=%i fragment=%i,%i\n"
		            , netsrcString[chan->sock]
		            , send.cursize
		            , chan->outgoingSequence
		            , chan->unsentFragmentStart, fragmentLength);
	}

	chan->unsentFragmentStart += fragmentLength;

	if ( chan->unsentFragmentStart == chan->unsentLength && fragmentLength != FRAGMENT_SIZE )
	{
		chan->outgoingSequence++;
		chan->unsentFragments = qfalse;
	}

	return status > 0;
}

void custom_SV_ClientEnterWorld(client_t *client, usercmd_t *cmd)
{
	gentity_s *ent;
	int clientNum;

	Com_DPrintf("Going from CS_PRIMED to CS_ACTIVE for %s\n", client->name);
	client->state = CS_ACTIVE;
	clientNum = client - svs.clients;
	ent = SV_GentityNum(clientNum);
	ent->s.number = clientNum;
	client->gentity = ent;
	client->deltaMessage = -1;
	client->nextSnapshotTime = svs.time;
	client->lastUsercmd = *cmd;
	ClientBegin(client - svs.clients);

	/* New code start: Multi version support */
	if ( customPlayerState[clientNum].resourceLimitedState > LIMITED_GAMESTATE && sv_kickGamestateLimitedClients->current.boolean )
	{
		Com_Printf("WARNING: Kicking client %i due to insufficient gamestate on map %s\n", clientNum, sv_mapname->current.string);
		SV_DelayDropClient(client, "This map-mod combination requires game version 1.3");
	}
	/* New code end */
}

void custom_SV_SendClientGameState(client_t *client)
{
	int start;
	entityState_t *base, nullstate;
	msg_t msg;
	byte *data;
	LargeLocal buf;
	int id = client - svs.clients;
	int protocolVersion;
	char preProxyIP[16];
	int preProxyPort;
	int currentConfigstringSize = 0;
	int clientGamestateDataCount = 1;
	char *configstring;

	LargeLocalConstructor(&buf, MAX_MSGLEN);
	data = LargeLocalGetBuf(&buf);
	while ( client->state != CS_FREE && client->netchan.unsentFragments )
		SV_Netchan_TransmitNextFragment(&client->netchan);
	
 	Com_DPrintf("SV_SendClientGameState() for %s\n", client->name);
	Com_DPrintf("Going from CS_CONNECTED to CS_PRIMED for %s\n", client->name);
	
	client->state = CS_PRIMED;
	client->pureAuthentic = 0;
	client->gamestateMessageNum = client->netchan.outgoingSequence;
	
	/* New code start: libcod client state */

	// Save relevant data before clearing custom player state
	protocolVersion = customPlayerState[id].protocolVersion;
	memcpy(&preProxyIP, &customPlayerState[id].preProxyIP, 16);
	preProxyPort = customPlayerState[id].preProxyPort;

	// Reset custom player state to default values
	memset(&customPlayerState[id], 0, sizeof(customPlayerState_t));
	customPlayerState[id].collisionTeam = CUSTOM_TEAM_AXIS_ALLIES;
	customPlayerState[id].meleeHeightScale = 1.0;
	customPlayerState[id].meleeRangeScale = 1.0;
	customPlayerState[id].meleeWidthScale = 1.0;
	customPlayerState[id].fireRangeScale = 1.0;
	customPlayerState[id].turretSpreadScale = 1.0;
	customPlayerState[id].weaponSpreadScale = 1.0;
	customPlayerState[id].droppingBulletDrag = 0.01; // 20% drag per second @ 20 server FPS
	customPlayerState[id].droppingBulletVelocity = 31500.0; // About 800 m/s

	// Restore previously saved values
	customPlayerState[id].protocolVersion = protocolVersion;
	memcpy(&customPlayerState[id].preProxyIP, &preProxyIP, 16);
	customPlayerState[id].preProxyPort = preProxyPort;

	// Restore user-provided rate and snaps after download
	SV_UserinfoChanged(client);

	// Enforce rate and snaps, overriding user-provided settings
	ForceServerSnapsAndRate(client);

	// Update entity-related stuff
	ClearNotSolidForPlayerFlags(id);
	/* New code end */

	// New: End here for bots
	if ( client->bIsTestClient )
	{
		LargeLocalDestructor(&buf);
		return;
	}

	MSG_Init(&msg, data, MAX_MSGLEN);
	MSG_WriteLong(&msg, client->lastClientCommand);
	SV_UpdateServerCommandsToClient(client, &msg);
	MSG_WriteByte(&msg, svc_gamestate);
	MSG_WriteLong(&msg, client->reliableSequence);

	// New: Moved configstring loading after entity baseline
	memset(&nullstate, 0, sizeof(nullstate));
	for ( start = 0; start < MAX_GENTITIES; start++ )
	{
		base = &sv.svEntities[start].baseline.s;
		if ( !base->number )
			continue;
		MSG_WriteByte(&msg, svc_baseline);
		custom_MSG_WriteDeltaEntity(&msg, &nullstate, base, qtrue, id, start);
	}

	for ( start = 0; start < MAX_CONFIGSTRINGS; start++ )
	{
		if ( sv.configstrings[start][0] )
		{
			configstring = sv.configstrings[start];

			/* New code start: Multi version support */

			// Fix iw_15.iwd checksum mismatch for version 1.2
			if ( start == CS_SYSTEMINFO )
			{
				if ( protocolVersion == 117 && getProtocolFromShortVersion(sv_version->current.string) != 117 )
					configstring = altSystemInfo;
				else if ( protocolVersion != 117 && getProtocolFromShortVersion(sv_version->current.string) == 117 )
					configstring = altSystemInfo;
			}

			// Older protocol versions: Sending overflowing config strings
			// afterwards via reliable server commands is constrained by
			// another client-sided limit, in addition to the gamestate
			// (message size) limit: 16k bytes for all configstrings together.
			//
			// Still, we can let the client connect without loading all
			// resources, but that will at least cause some precached shaders
			// not to be loaded. The order that decides which type of resource
			// (shader, string, model, etc.) could be set on the server side,
			// if needed.
			//
			// Furthermore, we should reserve some buffer for configstrings
			// that are allocated at runtime (e.g., those populated by
			// playFxOnTag or when playing sounds). For this we use the
			// sv_reservedConfigstringBufferSize dvar. Its value needs to be
			// set higher in case clients with older game versions tend to run
			// into "MAX_GAMESTATE_CHARS exceeded" errors.
			currentConfigstringSize = strlen(configstring);
			if ( protocolVersion < 118 )
			{
				if ( ( msg.cursize + currentConfigstringSize + 3 + 10 ) > 0x4000 )
				{
					Com_Printf("Client %i ran into gamestate limit at configstring %i\n", id, start);
					customPlayerState[id].resourceLimitedState = LIMITED_GAMESTATE;
					break;
				}

				if ( ( clientGamestateDataCount + currentConfigstringSize + 1 ) > ( 16000 - sv_reservedConfigstringBufferSize->current.integer ) )
				{
					Com_Printf("Client %i ran into configstring limit at configstring %i\n", id, start);
					customPlayerState[id].resourceLimitedState = LIMITED_CONFIGSTRING;
					break;
				}
			}
			/* New code end */

			MSG_WriteByte(&msg, svc_configstring);
			MSG_WriteShort(&msg, start);
			MSG_WriteBigString(&msg, configstring);

			clientGamestateDataCount += strlen(configstring) + 1; // New code
		}
	}

	MSG_WriteByte(&msg, svc_EOF);
	MSG_WriteLong(&msg, id);
	MSG_WriteLong(&msg, sv.checksumFeed);
	MSG_WriteByte(&msg, svc_EOF);
	
	Com_DPrintf("Sending %i bytes in gamestate to client: %i\n", msg.cursize, id);
	
	customPlayerState[id].gamestateSize = msg.cursize; // New code
	
	SV_SendMessageToClient(&msg, client);
	LargeLocalDestructor(&buf);

	/* New code start: Gamestate splitting for multi version support */
	if ( start != MAX_CONFIGSTRINGS && customPlayerState[id].resourceLimitedState == LIMITED_GAMESTATE )
	{
		// Reliable commands are limited to MAX_STRINGLENGTH
		char cmd[MAX_STRINGLENGTH];

		Com_DPrintf("Queueing remaining configstrings for client: %i, beginning at %i\n", id, start);
		for ( ; start < MAX_CONFIGSTRINGS; start++ )
		{
			if ( sv.configstrings[start][0] )
			{
				// We do not apply the iw_15.iwd checksum mismatch fix here
				// (again, see above) as we assume that the entity baseline
				// will not consume all the available gamestate space

				currentConfigstringSize = strlen(sv.configstrings[start]);
				if ( ( clientGamestateDataCount + currentConfigstringSize + 1 ) > ( 16000 - sv_reservedConfigstringBufferSize->current.integer ) )
				{
					Com_Printf("Aborting configstring queue at %i as client %i ran into configstring limit\n", start, id);
					customPlayerState[id].resourceLimitedState = LIMITED_CONFIGSTRING;
					return;
				}

				// Leave one command free to avoid "Out of range 
				// reliableAcknowledge message" spam
				if ( client->reliableSequence - client->reliableAcknowledge == ( MAX_RELIABLE_COMMANDS - 1 ) )
				{
					// This could potentially be delayed further, to avoid
					// filling up the command queue at once.
					Com_Printf("Aborting configstring queue at %i as client %i command queue is full\n", start, id);
					customPlayerState[id].resourceLimitedState = LIMITED_CONFIGSTRING;
					return;
				}

				Com_sprintf(cmd, MAX_STRINGLENGTH, "d %i %s", start, sv.configstrings[start]);
				SV_AddServerCommand(client, SV_CMD_RELIABLE, cmd);

				clientGamestateDataCount += strlen(sv.configstrings[start]) + 1;

				// Update gamestate size: Size of byte + size of byte + length of configstring
				customPlayerState[id].gamestateSize += (3 + strlen(sv.configstrings[start]));
			}
		}
		Com_DPrintf("Sending another %i bytes in gamestate as reliable commands to client: %i\n", customPlayerState[id].gamestateSize - msg.cursize, id);
	}
	/* New code end */
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
		I_strncpyz(cl->downloadURL, custom_va("%s/%s", sv_wwwBaseURL->current.string, cl->downloadName), MAX_OSPATH);
		Com_Printf("Redirecting client \'%s\'^7 to %s\n", cl->name, cl->downloadURL);
		cl->downloadingWWW = 1;
		MSG_WriteByte(msg, svc_download);
		MSG_WriteShort(msg, -1);
		MSG_WriteString(msg, cl->downloadURL);
		MSG_WriteLong(msg, size);
		MSG_WriteLong(msg, sv_wwwDlDisconnected->current.boolean != 0);

		/* New code start */
		if ( sv_wwwDlDisconnectedMessages->current.integer == 2 )
			SV_SendServerCommand(0, SV_CMD_CAN_IGNORE, "f \"%s^7 downloads %s\"", cl->name, cl->downloadName);
		/* New code end */

		cl->downloadName[0] = '\0';
	}
	return size != 0;
}

void SV_WriteDownloadErrorToClient(client_t *cl, msg_t *msg, char *errorMessage)
{
	MSG_WriteByte(msg, svc_download);
	MSG_WriteShort(msg, 0); // Client is expecting block zero
	MSG_WriteLong(msg, -1); // Illegal file size
	MSG_WriteString(msg, errorMessage);

	*cl->downloadName = 0;
}

void custom_SV_WriteDownloadToClient(client_t *cl, msg_t *msg)
{
	int curindex;
	char downloadNameNoExt[MAX_QPATH];
	char errorMessage[MAX_STRINGLENGTH];

	// Block download attempts if the client is already playing
	// At this point, all relevant downloads should have been processed already
	if ( cl->state == CS_ACTIVE )
		return;

	// Block empty filename
	if ( !*cl->downloadName )
		return;

	// Block filenames that are shorter than .iwd
	if ( strlen(cl->downloadName) < 4 )
		return;

	// Block filenames that do not end with .iwd
	if ( strcmp(&cl->downloadName[strlen(cl->downloadName) - 4], ".iwd") != 0 )
		return;

	// WWW download has been acknowledged
	if ( cl->clientDownloadingWWW )
		return;

	// If set, download custom message instead of download. Applies for every
	// client
	if ( strlen(sv_downloadMessage->current.string) )
	{
		if ( ( strstr(cl->downloadName, "mp_") || strstr(cl->downloadName, "empty") ) && !sv_downloadMessageAtMap->current.boolean )
		{
			// We might not want to push a custom message if the client is
			// about to load smaller files such as those provided via manymaps
		}
		else
		{
			Com_sprintf(errorMessage, sizeof(errorMessage), sv_downloadMessage->current.string);
			SV_WriteDownloadErrorToClient(cl, msg, errorMessage);
			return;
		}
	}

	// If set, download custom message instead of download. Here for clients on
	// version 1.0 that do not support HTTP download
	if ( strlen(sv_downloadMessageForLegacyClients->current.string) && customPlayerState[cl - svs.clients].protocolVersion == 115 )
	{
		if ( ( strstr(cl->downloadName, "mp_") || strstr(cl->downloadName, "empty") ) && !sv_downloadMessageAtMap->current.boolean )
		{
			// We might not want to push a custom message if the client is
			// about to load smaller files such as those provided via manymaps
		}
		else
		{
			Com_sprintf(errorMessage, sizeof(errorMessage), sv_downloadMessageForLegacyClients->current.string);
			SV_WriteDownloadErrorToClient(cl, msg, errorMessage);
			return;
		}
	}

	if ( sv_wwwDownload->current.boolean && cl->wwwOk )
	{
		if ( !cl->wwwFallback )
		{
			// Redirect to HTTP server
			custom_SV_WWWRedirectClient(cl, msg);
			return;
		}
	}

	// Hardcode client variables to make max download speed for everyone
	cl->state = CS_CONNECTED;
	cl->rate = 25000;
	cl->snapshotMsec = 50;

	if ( !cl->download )
	{
		// Check if direct server downloading is enabled
		if ( !sv_allowDownload->current.boolean )
		{
			Com_Printf("clientDownload: %d : \"%s\" download disabled\n", cl - svs.clients, cl->downloadName);

			if ( sv_pure->current.boolean )
				Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_AUTODL_SERVERDISABLED_PURE\x15%s", cl->downloadName);
			else
				Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_AUTODL_SERVERDISABLED\x15%s", cl->downloadName);

			SV_WriteDownloadErrorToClient(cl, msg, errorMessage);
			return;
		}

		// Block download of stock .iwd files from main folder
		// FS_iwIwd expects a relative .iwd file path without file extension
		I_strncpyz(downloadNameNoExt, cl->downloadName, strlen(cl->downloadName) - 3);
		if ( FS_iwIwd(downloadNameNoExt, "main") )
		{
			Com_Printf("clientDownload: %d : \"%s\" cannot download IW iwd files\n", cl - svs.clients, cl->downloadName);
			Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_CANTAUTODLGAMEPAK\x15%s", cl->downloadName);
			SV_WriteDownloadErrorToClient(cl, msg, errorMessage);
			return;
		}

		// Block download of _svr_ files
		if ( FS_svrIwd(cl->downloadName) )
		{
			Com_Printf("clientDownload: %d : \"%s\" cannot download server-only iwd files\n", cl - svs.clients, cl->downloadName);
			Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_CANTAUTODLGAMEPAK\x15%s", cl->downloadName);
			SV_WriteDownloadErrorToClient(cl, msg, errorMessage);
			return;
		}

		// Block directory traversal outside of valid paths
		if ( strstr(cl->downloadName, "..") )
		{
			Com_Printf("clientDownload: %d blocked from accessing relative path \"%s\"\n", cl - svs.clients, cl->downloadName);
			Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_AUTODL_FILENOTONSERVER\x15%s", cl->downloadName);
			SV_WriteDownloadErrorToClient(cl, msg, errorMessage);
			return;
		}

		// Check if file exists in the game paths
		if ( ( cl->downloadSize = FS_SV_FOpenFileRead( cl->downloadName, &cl->download ) ) <= 0 )
		{
			Com_Printf("clientDownload: %d : \"%s\" file not found on server\n", cl - svs.clients, cl->downloadName);
			Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_AUTODL_FILENOTONSERVER\x15%s", cl->downloadName);
			SV_WriteDownloadErrorToClient(cl, msg, errorMessage);
			return;
		}

		// Init download
		Com_Printf("clientDownload: %d : beginning \"%s\"\n", cl - svs.clients, cl->downloadName);
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

	MSG_WriteByte(msg, svc_download);
	MSG_WriteShort(msg, cl->downloadXmitBlock);

	// Block zero is special, contains file size
	if ( cl->downloadXmitBlock == 0 )
		MSG_WriteLong(msg, cl->downloadSize);

	MSG_WriteShort(msg, cl->downloadBlockSize[curindex]);

	// Write the block
	if ( cl->downloadBlockSize[curindex] )
		MSG_WriteData(msg, cl->downloadBlocks[curindex], cl->downloadBlockSize[curindex]);

	Com_DPrintf("clientDownload: %d : writing block %d\n", cl - svs.clients, cl->downloadXmitBlock);

	// Move on to the next block
	// It will get sent with next snap shot.  The rate will keep us in line.
	cl->downloadXmitBlock++;

	cl->downloadSendTime = svs.time;
}

qboolean custom_BG_IsWeaponValid(playerState_t *ps, unsigned int index)
{
	WeaponDef_t *weaponDef;

	if ( !BG_ValidateWeaponNumber(index) )
		return qfalse;

	if ( !COM_BitCheck(ps->weapons, index) ) // Player has weapon?
		return qfalse;

	weaponDef = BG_GetWeaponDef(index);

	/* New code start: Fixed potential NULL-pointer crash */
	if ( !weaponDef )
		return 0;
	/* New code end */

	if ( ( ( weaponDef->offhandClass == OFFHAND_CLASS_NONE && ps->weaponslots[1] != index ) && ps->weaponslots[2] != index ) && weaponDef->iAltWeaponIndex != index )
		return qfalse;

	return qtrue;
}

char * custom_va(const char *format, ...)
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

	// New: Removed call to Com_Error with "Attempted to overrun string in call to va()"

	return info->va_string[index];
}

void custom_SV_VerifyIwds_f(client_t *cl)
{
	if ( sv_pure->current.boolean )
	{
		if ( sv_verifyIwds->current.boolean )
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

	/* New code start: CodeCallback_VidRestart */
	if ( codecallback_vid_restart && Scr_IsSystemActive() )
	{	
		if ( cl->gentity == NULL )
			return;

		short ret = Scr_ExecEntThread(cl->gentity, codecallback_vid_restart, 0);
		Scr_FreeThread(ret);
	}
	/* New code end */
}

// Adds bot pings and removes spam on 1.2 and 1.3
void custom_SV_CalcPings(void)
{
	int i, j;
	client_t *cl;
	int total, count;
	int delta;

	for ( i = 0 ; i < sv_maxclients->current.integer ; i++ )
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

	droppoint = svs.time - 1000 * sv_timeout->current.integer;
	zombiepoint = svs.time - 1000 * sv_zombietime->current.integer;

	for ( i = 0, cl = svs.clients; i < sv_maxclients->current.integer; i++, cl++ )
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

	if ( customPlayerState[num].botWeapon )
		ucmd.weapon = (byte)(customPlayerState[num].botWeapon & 0xFF);
	else
		ucmd.weapon = (byte)(ps->weapon & 0xFF);

	if ( ent->client == NULL )
		return;

	if ( ent->client->sess.archiveTime == 0 )
	{
		ucmd.buttons = customPlayerState[num].botButtons;
		ucmd.forwardmove = customPlayerState[num].botForwardMove;
		ucmd.rightmove = customPlayerState[num].botRightMove;

		VectorCopy(ent->client->sess.cmd.angles, ucmd.angles);
	}

	client->deltaMessage = client->netchan.outgoingSequence - 1;
	SV_ClientThink(client, &ucmd);
}
#endif

void hook_RuntimeError_in_VM_Execute(const char *pos, int error_index, const char *error_message, const char *dialog_error_message)
{
	RuntimeError(pos, error_index, error_message, dialog_error_message);

	/* If enabled, log errors even if developer mode is off. Skip errors during
	 initialization since the source buffer lookup tables might be corrupt at
	 that point if developer mode is off */
	if ( developer->current.integer == 0 && logErrors->current.boolean && !level.initializing )
		RuntimeError_Debug(0, pos, error_index, error_message);
}

#if COMPILE_PLAYER == 1

void custom_SV_ClientThink(client_t *cl, usercmd_t *ucmd)
{
	hook_play_movement->unhook();
	SV_ClientThink(cl, ucmd);
	hook_play_movement->hook();

	int clientnum = cl - svs.clients;

	customPlayerState[clientnum].frames++;

	if ( Sys_Milliseconds64() - customPlayerState[clientnum].frameTime >= 1000 )
	{
		if ( customPlayerState[clientnum].frames > 1000 )
			customPlayerState[clientnum].frames = 1000;

		customPlayerState[clientnum].fps = customPlayerState[clientnum].frames;
		customPlayerState[clientnum].frameTime = Sys_Milliseconds64();
		customPlayerState[clientnum].frames = 0;
	}
	
	if ( ucmd->buttons & KEY_MASK_FIRE && !(customPlayerState[clientnum].previousButtons & KEY_MASK_FIRE) )
	{
		if ( codecallback_attackbutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_attackbutton, 0);
			Scr_FreeThread(ret);
		}
	}
	
	if ( ucmd->buttons & KEY_MASK_MELEE && !(customPlayerState[clientnum].previousButtons & KEY_MASK_MELEE) )
	{
		if ( codecallback_meleebutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_meleebutton, 0);
			Scr_FreeThread(ret);
		}
	}

	if ( ucmd->buttons & KEY_MASK_USE && !(customPlayerState[clientnum].previousButtons & KEY_MASK_USE) )
	{
		if ( codecallback_usebutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_usebutton, 0);
			Scr_FreeThread(ret);
		}
	}

	if ( ucmd->buttons & KEY_MASK_RELOAD && !(customPlayerState[clientnum].previousButtons & KEY_MASK_RELOAD) )
	{
		if ( codecallback_reloadbutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_reloadbutton, 0);
			Scr_FreeThread(ret);
		}
	}
	
	if ( ucmd->buttons & KEY_MASK_LEANLEFT && !(customPlayerState[clientnum].previousButtons & KEY_MASK_LEANLEFT) )
	{
		if ( codecallback_leanleftbutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_leanleftbutton, 0);
			Scr_FreeThread(ret);
		}
	}
	
	if ( ucmd->buttons & KEY_MASK_LEANRIGHT && !(customPlayerState[clientnum].previousButtons & KEY_MASK_LEANRIGHT) )
	{
		if ( codecallback_leanrightbutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_leanrightbutton, 0);
			Scr_FreeThread(ret);
		}
	}
	
	if ( ucmd->buttons & KEY_MASK_PRONE && !(customPlayerState[clientnum].previousButtons & KEY_MASK_PRONE) )
	{
		if ( codecallback_pronebutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_pronebutton, 0);
			Scr_FreeThread(ret);
		}
	}
	
	if ( ucmd->buttons & KEY_MASK_CROUCH && !(customPlayerState[clientnum].previousButtons & KEY_MASK_CROUCH) )
	{
		if ( codecallback_crouchbutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_crouchbutton, 0);
			Scr_FreeThread(ret);
		}
	}
	
	if ( ucmd->buttons & KEY_MASK_JUMP && !(customPlayerState[clientnum].previousButtons & KEY_MASK_JUMP) )
	{
		if ( codecallback_standbutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_standbutton, 0);
			Scr_FreeThread(ret);
		}
	}
	
	if ( ucmd->buttons & KEY_MASK_ADS_MODE && !(customPlayerState[clientnum].previousButtons & KEY_MASK_ADS_MODE) )
	{
		if ( codecallback_adsbutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_adsbutton, 0);
			Scr_FreeThread(ret);
		}
	}
	
	if ( ucmd->buttons & KEY_MASK_MELEE_BREATH && !(customPlayerState[clientnum].previousButtons & KEY_MASK_MELEE_BREATH) )
	{
		if ( codecallback_meleebreathbutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_meleebreathbutton, 0);
			Scr_FreeThread(ret);
		}
	}
	
	if ( ucmd->buttons & KEY_MASK_HOLDBREATH && !(customPlayerState[clientnum].previousButtons & KEY_MASK_HOLDBREATH) )
	{
		if ( codecallback_holdbreathbutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_holdbreathbutton, 0);
			Scr_FreeThread(ret);
		}
	}
	
	if ( ucmd->buttons & KEY_MASK_FRAG && !(customPlayerState[clientnum].previousButtons & KEY_MASK_FRAG) )
	{
		if ( codecallback_fragbutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_fragbutton, 0);
			Scr_FreeThread(ret);
		}
	}
	
	if ( ucmd->buttons & KEY_MASK_SMOKE && !(customPlayerState[clientnum].previousButtons & KEY_MASK_SMOKE) )
	{
		if ( codecallback_smokebutton )
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_smokebutton, 0);
			Scr_FreeThread(ret);
		}
	}

	customPlayerState[clientnum].previousButtons = ucmd->buttons;
}

int custom_ClientEndFrame(gentity_t *ent)
{
	hook_clientendframe->unhook();
	int (*ClientEndFrame)(gentity_t *ent);
	*(int *)&ClientEndFrame = hook_clientendframe->from;
	int ret = ClientEndFrame(ent);
	hook_clientendframe->hook();

	if ( ent->client->sess.sessionState == STATE_PLAYING )
	{
		int num = ent - g_entities;

		if ( customPlayerState[num].speed > 0 )
			ent->client->ps.speed = customPlayerState[num].speed;

		if ( customPlayerState[num].gravity > 0 )
			ent->client->ps.gravity = customPlayerState[num].gravity;

		// Experimental slide bug fix
		if ( g_resetSlide->current.boolean )
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

	if ( !customPlayerState[ps->clientNum].animation )
		duration = BG_PlayAnim(ps, animNum, bodyPart, forceDuration, setTimer, isContinue, force);
	else
		duration = BG_PlayAnim(ps, customPlayerState[ps->clientNum].animation, bodyPart, forceDuration, qtrue, isContinue, qtrue);

	hook_bg_playanim->hook();

	return duration;
}
#endif

bool SVC_SpamCallback(const char *str, const char *ip)
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

void custom_SV_AuthorizeIpPacket(netadr_t from)
{
    /* New: Rate limiting */
	if ( SVC_ApplyAuthorizeIpPacketLimit(from, OUTBOUND_BUCKET_MAIN) )
		return;
	/* New code end */

	SV_AuthorizeIpPacket(from);
}

void custom_SVC_Info(netadr_t from)
{
	int i, count;
	char infostring[MAX_INFO_STRING];
	char infosend[MAX_INFO_STRING];
	const char *gamedir;
	const char *password;
	int friendlyfire;
	int killcam;
	int serverModded;
	const char *referencedIwdNames;
	char *iwd;

	/* New: Rate limiting */
	if ( SVC_ApplyInfoLimit(from, OUTBOUND_BUCKET_MAIN) )
		return;
	/* New code end */

	count = 0;

	for ( i = sv_privateClients->current.integer ; i < sv_maxclients->current.integer ; i++ )
	{
		if ( svs.clients[i].state >= CS_CONNECTED )
		{
			count++;
		}
	}

	infostring[0] = 0;

	Info_SetValueForKey(infostring, "challenge", Cmd_Argv(1));

	// New: Configurable version response
	Info_SetValueForKey(infostring, "protocol", custom_va("%i", getProtocolFromShortVersion(sv_version->current.string)));

	Info_SetValueForKey(infostring, "hostname", sv_hostname->current.string);
	Info_SetValueForKey(infostring, "mapname", sv_mapname->current.string);
	Info_SetValueForKey(infostring, "clients", custom_va("%i", count));
	Info_SetValueForKey(infostring, "sv_maxclients", custom_va("%i", sv_maxclients->current.integer - sv_privateClients->current.integer));
	Info_SetValueForKey(infostring, "gametype", Dvar_GetString("g_gametype"));
	Info_SetValueForKey(infostring, "pure", custom_va("%i", sv_pure->current.boolean));
	if ( sv_minPing->current.integer )
	{
		Info_SetValueForKey(infostring, "minPing", custom_va("%i", sv_minPing->current.integer));
	}
	if ( sv_maxPing->current.integer )
	{
		Info_SetValueForKey(infostring, "maxPing", custom_va("%i", sv_maxPing->current.integer));
	}
	gamedir = Dvar_GetString("fs_game");
	if ( *gamedir )
	{
		Info_SetValueForKey(infostring, "game", gamedir);
	}
	Info_SetValueForKey( infostring, "sv_allowAnonymous", custom_va("%i", sv_allowAnonymous->current.boolean));
	if ( sv_disableClientConsole->current.boolean )
	{
		Info_SetValueForKey(infostring, "con_disabled", custom_va("%i", sv_disableClientConsole->current.boolean));
	}
	password = Dvar_GetString("g_password");
	if ( password && *password )
		Info_SetValueForKey(infostring, "pswrd", "1");

	friendlyfire = Dvar_GetInt("scr_friendlyfire");
	if ( friendlyfire )
		Info_SetValueForKey(infostring, "ff", custom_va("%i", friendlyfire));

	killcam = Dvar_GetInt("scr_killcam");
	if ( killcam )
		Info_SetValueForKey(infostring, "kc", custom_va("%i", killcam));

	Info_SetValueForKey(infostring, "hw", custom_va("%i", 1));
	serverModded = 0;
	if ( !sv_pure->current.boolean || ( gamedir && *gamedir ) )
	{
		serverModded = 1;
	}
	else
	{
		referencedIwdNames = Dvar_GetString("sv_referencedIwdNames");
		if ( *referencedIwdNames )
		{
			SV_Cmd_TokenizeString(referencedIwdNames);
			count = SV_Cmd_Argc();

			for ( i = 0; i < count; ++i )
			{
				iwd = (char *)SV_Cmd_Argv(i);

				if ( !FS_iwIwd(iwd, "main") )
				{
					serverModded = 1;
					break;
				}
			}
		}
	}

	Info_SetValueForKey(infostring, "mod", custom_va("%i", serverModded));
	Info_SetValueForKey(infostring, "voice", custom_va("%i", sv_voice->current.boolean));
	I_strncpyz(infosend, "infoResponse\n", MAX_INFO_STRING);
	I_strncat(infosend, MAX_INFO_STRING, infostring);
	NET_OutOfBandPrint(NS_SERVER, from, infosend);
}

void custom_SVC_Status(netadr_t from)
{
	LargeLocal buf;
	char *status;
	char infostring[BIG_INFO_STRING];
	char keywords[MAX_INFO_STRING];
	int statusLength;
	int i;
	client_t *cl;
	int ping;
	char player[MAX_INFO_STRING];
	size_t playerLength;
	char *password;
	qboolean serverModded;
	char *gamedir;
	char *referencedIwdNames;
	int count;
	char *iwd;
	char msg[BIG_INFO_STRING];

	/* New: Rate limiting */
	if ( SVC_ApplyStatusLimit(from, OUTBOUND_BUCKET_MAIN) )
		return;
	/* New code end */

	LargeLocalConstructor(&buf, MAX_MSGLEN);
	status = (char *)LargeLocalGetBuf(&buf);

	strcpy(infostring, Dvar_InfoString(DVAR_SERVERINFO | DVAR_NORESTART));
	Info_SetValueForKey(infostring, "challenge", SV_Cmd_Argv(1));

	if ( Dvar_GetBool("fs_restrict") )
	{
		Com_sprintf(keywords, MAX_INFO_STRING, "demo %s", Info_ValueForKey(infostring, "sv_keywords"));
		Info_SetValueForKey(infostring, "sv_keywords", keywords);
	}

	status[0] = 0;
	statusLength = 0;
	for ( i = 0; i < (sv_maxclients->current).integer; i++ )
	{
		cl = svs.clients + i;
		if ( CS_ZOMBIE < cl->state )
		{
			/* New: Custom ping value for status responses */
			ping = cl->ping;
			if ( customPlayerState[i].overrideStatusPing )
				ping = customPlayerState[i].statusPing;
			/* New code end */

			if ( gameInitialized )
				Com_sprintf(player, MAX_INFO_STRING, "%i %i \"%s\"\n", G_GetClientScore(cl - svs.clients), ping, cl->name);
			else
				Com_sprintf(player, MAX_INFO_STRING, "%i %i \"%s\"\n", 0, ping, cl->name);

			playerLength = strlen(player);
			if ( ( MAX_MSGLEN - 1 ) < playerLength + statusLength )
			{
				break;
			}
			strcpy(status + statusLength, player);
			statusLength = statusLength + playerLength;
		}
	}

	password = Dvar_GetString("g_password");
	if ( password && *password )
		Info_SetValueForKey(infostring, "pswrd", "1");
	else
		Info_SetValueForKey(infostring, "pswrd", "0");

	serverModded = 0;
	gamedir = Dvar_GetString("fs_game");
	if ( !sv_pure->current.boolean || ( gamedir && *gamedir ) )
	{
		serverModded = 1;
	}
	else
	{
		referencedIwdNames = Dvar_GetString("sv_referencedIwdNames");
		if ( *referencedIwdNames )
		{
			SV_Cmd_TokenizeString(referencedIwdNames);
			count = SV_Cmd_Argc();
			for ( i = 0; i < count; i++ )
			{
				iwd = SV_Cmd_Argv(i);
				if ( !FS_iwIwd(iwd, "main") )
				{
					serverModded = 1;
					break;
				}
			}
		}
	}

	Info_SetValueForKey(infostring, "mod", custom_va("%i", serverModded));
	Com_sprintf(msg, BIG_INFO_STRING, "statusResponse\n%s\n%s", infostring, status);
	NET_OutOfBandPrint(NS_SERVER, from, msg);
	LargeLocalDestructor(&buf);
}

void custom_SVC_RemoteCommand(netadr_t from, msg_t *msg, qboolean from_script)
{
	char *sv_outputbuf;
	int argc;
	char *argv;
	LargeLocal buf;
	char *password;
	char cmd_aux[1024];
	int len;
	int max_len;
	int arg;
	int args;
	int i;
	qboolean valid;

	/* New: sv_allowRcon dvar */
	if ( !sv_allowRcon->current.boolean )
		return;
	/* New code end */

	LargeLocalConstructor(&buf, SV_OUTPUTBUF_LENGTH);
	sv_outputbuf = (char *)LargeLocalGetBuf(&buf);

	/* New: Patched out half-second limit
	int time = Com_Milliseconds();
	if ( rcon_lasttime != 0 && ( time - rcon_lasttime < 500 ) )
	{
		LargeLocalDestructor(&buf);
		return;
	}
	rcon_lasttime = time;
	*/

	password = SV_Cmd_Argv(1);
	// New: Time-constant string comparison for rcon_password dvar
	qboolean badRconPassword = !strlen(rcon_password->current.string) || !strcmp_constant_time(password, rcon_password->current.string);

	/* New: Rate limiting and sv_limitLocalRcon dvar */
	if ( sv_limitLocalRcon->current.boolean || !IsLocalIPAddress(from.ip) )
	{
		if ( SVC_ApplyRconLimit(from, badRconPassword) )
		{
			LargeLocalDestructor(&buf);
			return;
		}
	}
	/* New code end */

	if ( badRconPassword )
	{
		valid = 0;
		if ( sv_logRcon->current.boolean ) // New: sv_logRcon dvar
			Com_Printf("Bad rcon from %s:\n%s\n", NET_AdrToString(from), SV_Cmd_Argv(2));
	}
	else
	{
		valid = 1;
		if ( sv_logRcon->current.boolean ) // New: sv_logRcon dvar
			Com_Printf("Rcon from %s: %s ", NET_AdrToString(from), SV_Cmd_Argv(2));
	}

	/* New: CodeCallback_RemoteCommand */
	if (
		!from_script &&
		codecallback_remotecommand && 
		!badRconPassword && 
		Scr_IsSystemActive()
		)
	{
		stackPushInt((int)msg);
		stackPushArray();
		args = SV_Cmd_Argc();
		for ( i = 2; i < args; i++ )
		{
			char tmp[MAX_STRINGLENGTH];
			SV_Cmd_ArgvBuffer(i, tmp, sizeof(tmp));
			stackPushString(tmp);
			stackPushArrayLast();
		}
		stackPushString(NET_AdrToString(from));
	
		short ret = Scr_ExecThread(codecallback_remotecommand, 3);
		Scr_FreeThread(ret);
		LargeLocalDestructor(&buf);
		return;
	}
	/* New code end */

	svs.redirectAddress = from;
	Com_BeginRedirect(sv_outputbuf, SV_OUTPUTBUF_LENGTH, SV_FlushRedirect);
	if ( !strlen(rcon_password->current.string) )
	{
		Com_Printf("The server must set \'rcon_password\' for clients to use \'rcon\'.\n");
	}
	else if ( !valid )
	{
		if ( *password == '\0' )
		{
			Com_Printf("You must log in with \'rcon login <password>\' before using \'rcon\'.\n");
		}
		else
		{
			Com_Printf("Invalid password.\n");
		}
	}
	else
	{
		len = 0;
		max_len = MAX_STRINGLENGTH;
		arg = 2;
		while ( argc = SV_Cmd_Argc(), arg < argc )
		{
			argv = SV_Cmd_Argv(arg);
			len = Com_AddToString(argv, cmd_aux, len, max_len, 1);
			len = Com_AddToString(" ", cmd_aux, len, max_len, 0);
			arg++;
		}
		if ( len < max_len )
		{
			cmd_aux[len] = 0;

			/* New: Delayed processing of map-related commands when passed 
			 through CodeCallback_RemoteCommand. Note that Com_Printf output
			 here between Com_BeginRedirect and Com_EndRedirect is passed to
			 the rcon client */
			if ( from_script )
			{
				if ( strcmp(SV_Cmd_Argv(2), "fast_restart") == 0 )
				{
					if ( level.finished )
					{
						if ( level.finished == 1 )
							Com_Printf("fast_restart or map_restart already called\n");
						else
							Com_Printf("exitlevel or map_rotate already called\n");
					}
					else
					{
						level.finished = 1;
						level.savePersist = 0;
						if ( SV_Cmd_Argc() > 3 )
						{
							level.savePersist = atoi(SV_Cmd_Argv(3));
						}
						Cbuf_ExecuteText(2, cmd_aux);
					}
				}
				else if ( strcmp(SV_Cmd_Argv(2), "map_restart") == 0 )
				{
					/* Note: The operator command map_restart behaves 
					 differently than the map_restart script function, as the
					 latter actually performs a fast_restart. Here we follow
					 the stock operator command logic. This implies that
					 level.savePersist is always false here. Use the map
					 command instead if level.savePersist is required to be
					 true */
					if ( level.finished )
					{
						if ( level.finished == 1 )
							Com_Printf("fast_restart or map_restart already called\n");
						else
							Com_Printf("exitlevel or map_rotate already called\n");
					}
					else
					{
						level.finished = 1;
						Cbuf_ExecuteText(2, cmd_aux);
					}
				}
				else if ( strcmp(SV_Cmd_Argv(2), "devmap") == 0 )
				{
					if ( SV_Cmd_Argc() > 3 && custom_SV_MapExists(SV_Cmd_Argv(3)) )
					{
						if ( level.finished )
						{
							if ( level.finished == 2 )
								Com_Printf("devmap or map already called\n");
							else
								Com_Printf("exitlevel or map_rotate already called\n");
						}
						else
						{
							level.finished = 2;
							level.savePersist = 0;
							if ( SV_Cmd_Argc() > 4 )
							{
								level.savePersist = atoi(SV_Cmd_Argv(4));
							}
							Dvar_SetBool(sv_cheats, 1);
							Cbuf_ExecuteText(2, cmd_aux);
						}
					}
				}
				else if ( strcmp(SV_Cmd_Argv(2), "map") == 0 )
				{
					if ( SV_Cmd_Argc() > 3 && custom_SV_MapExists(SV_Cmd_Argv(3)) )
					{
						if ( level.finished )
						{
							if ( level.finished == 2 )
								Com_Printf("map or devmap already called\n");
							else
								Com_Printf("exitlevel or map_rotate already called\n");
						}
						else
						{
							level.finished = 2;
							level.savePersist = 0;
							if ( SV_Cmd_Argc() > 4 )
							{
								level.savePersist = atoi(SV_Cmd_Argv(4));
							}
							Cbuf_ExecuteText(2, cmd_aux);
						}
					}
				}
				else if ( strcmp(SV_Cmd_Argv(2), "map_rotate") == 0 )
				{
					/* Note: This does not do a map existence check, just like
					 the ExitLevel script function. If that is required, check
					 out sv_mapRotationCurrent dvar parsing in
					 SV_MapRotate_f */
					if ( level.finished )
					{
						if ( level.finished == 1 )
							Com_Printf("map_restart or fast_restart already called\n");
						else
							Com_Printf("map_rotate or exitlevel already called\n");
					}
					else
					{
						level.finished = 3;
						level.savePersist = 0;
						if ( SV_Cmd_Argc() > 3 )
						{
							level.savePersist = atoi(SV_Cmd_Argv(3));
						}
						Cbuf_ExecuteText(2, cmd_aux);
						level.teamScores[1] = 0;
						level.teamScores[2] = 0;
						for ( i = 0; i < sv_maxclients->current.integer; i++ )
						{
							if ( level.clients[i].sess.connected == CON_CONNECTED )
							{
								level.clients[i].sess.score = 0;
							}
						}
						for ( i = 0; i < sv_maxclients->current.integer; i++ )
						{
							if ( level.clients[i].sess.connected == CON_CONNECTED )
							{
								level.clients[i].sess.connected = CON_CONNECTING;
							}
						}
					}
				}
				else
				{
					SV_Cmd_ExecuteString(cmd_aux);
				}
			}
			/* New code end */
			else
			{
				SV_Cmd_ExecuteString(cmd_aux);
			}
			
			if ( I_strnicmp(cmd_aux, "pb_sv_", 6) == 0 )
			{
				PbServerForceProcess();
			}
		}
	}
	Com_EndRedirect();
	LargeLocalDestructor(&buf);
}

netadr_t * custom_SV_MasterAddress(void)
{
	if ( sv_masterAddress.type == NA_BOT )
	{
		Com_Printf("Resolving %s\n", sv_masterServer->current.string);
		if ( !NET_StringToAdr(sv_masterServer->current.string, &sv_masterAddress) )
		{
			Com_Printf("Couldn't resolve address: %s\n", sv_masterServer->current.string);
		}
		else
		{
			if ( !strstr(":", sv_masterServer->current.string) )
			{
				sv_masterAddress.port = BigShort(sv_masterPort->current.integer);
			}
			Com_Printf("%s resolved to %i.%i.%i.%i:%i\n",
						sv_masterServer->current.string,
						sv_masterAddress.ip[0],
						sv_masterAddress.ip[1],
						sv_masterAddress.ip[2],
						sv_masterAddress.ip[3],
						BigShort(sv_masterAddress.port));
		}
	}
	return &sv_masterAddress;
}

void custom_SV_GetChallenge(netadr_t from)
{
	int i;
	int oldest;
	int oldestTime;
	challenge_t *challenge;
	netadr_t *master;

	/* New: Rate limiting */
	if ( SVC_ApplyChallengeLimit(from, OUTBOUND_BUCKET_MAIN) )
		return;
	/* New code end */

	oldest = 0;
	oldestTime = 0x7FFFFFFF;
	challenge = &svs.challenges[0];
	for ( i = 0; i < MAX_CHALLENGES; i++, challenge++ )
	{
		if ( !challenge->connected && NET_CompareAdr(from, challenge->adr) )
		{
			break;
		}
		if ( challenge->time < oldestTime )
		{
			oldestTime = challenge->time;
			oldest = i;
		}
	}

	if ( i == MAX_CHALLENGES )
	{
		challenge = &svs.challenges[oldest];

		challenge->challenge = ( ( rand() << 16 ) ^ rand() ) ^ svs.time;
		challenge->adr = from;
		challenge->firstTime = svs.time;
		challenge->firstPing = 0;
		challenge->time = svs.time;
		challenge->connected = qfalse;
		i = oldest;
	}

	// New: sv_noauthorize dvar
	if ( sv_noauthorize->current.boolean || ( !net_lanauthorize->current.boolean && Sys_IsLANAddress(from) ) )
	{
		challenge->pingTime = svs.time;
		NET_OutOfBandPrint(NS_SERVER, from, custom_va("challengeResponse %i", challenge->challenge));
		return;
	}

	if ( !svs.authorizeAddress.ip[0] && svs.authorizeAddress.type != NA_BAD )
	{
		Com_Printf("Resolving %s\n", sv_authorizeServer->current.string);
		if ( !NET_StringToAdr(sv_authorizeServer->current.string, &svs.authorizeAddress))
		{
			Com_Printf("Couldn't resolve address\n");
			return;
		}
		svs.authorizeAddress.port = BigShort(sv_authorizePort->current.integer);
		Com_Printf("%s resolved to %i.%i.%i.%i:%i\n",
					sv_authorizeServer->current.string,
					svs.authorizeAddress.ip[0],
					svs.authorizeAddress.ip[1],
					svs.authorizeAddress.ip[2],
					svs.authorizeAddress.ip[3],
					BigShort(svs.authorizeAddress.port));
	}

	// New: The original authorize server timeout is 20 minutes. This is way
	// too long, so we default it to a few seconds to not have the game go dead
	// once the authorize server goes offline (again). Timeout in milliseconds.
	if ( ( sv_authorizeTimeout->current.integer < svs.time - svs.sv_lastTimeMasterServerCommunicated ) && ( sv_authorizeTimeout->current.integer < svs.time - challenge->firstTime ) )
	{
		master = custom_SV_MasterAddress();
		if ( !NET_CompareAdr(from, *master) )
		{
			Com_DPrintf("authorize server timed out\n");
			challenge->pingTime = svs.time;
			NET_OutOfBandPrint(NS_SERVER, challenge->adr, custom_va("challengeResponse %i", challenge->challenge));
			return;
		}
	}

	const char *clientPBguid = NULL;
	if ( SV_Cmd_Argc() == 3 )
	{
		clientPBguid = SV_Cmd_Argv(2);
		I_strncpyz(svs.challenges[i].clientPBguid, clientPBguid, sizeof(svs.challenges[i].clientPBguid));
	}

	SV_AuthorizeRequest(from, svs.challenges[i].challenge, clientPBguid);
}

void manymaps_get_library_path(char *library_path, size_t library_path_size)
{
	if ( strlen(fs_library->current.string) )
		strncpy(library_path, fs_library->current.string, library_path_size);
	else
		snprintf(library_path, library_path_size, "%s/%s/Library", fs_homepath->current.string, fs_game->current.string);
}

void manymaps_cleanup(void)
{
	DIR *dir;
	struct dirent *dir_ent;
	char library_path[MAX_OSPATH];

	manymaps_get_library_path(library_path, MAX_OSPATH);

	dir = opendir(library_path);
	if ( !dir )
		return;

	// Walk through all files in the library folder
	while ( ( dir_ent = readdir(dir) ) != NULL )
	{
		// Skip directory references
		if ( strcmp(dir_ent->d_name, ".") == 0 || strcmp(dir_ent->d_name, "..") == 0 )
			continue;

		// Check if a file with same name exists in the current fs_game folder
		char fileDelete[MAX_OSPATH];
		Com_sprintf(fileDelete, MAX_OSPATH, "%s/%s/%s", fs_homepath->current.string, fs_game->current.string, dir_ent->d_name);
		if ( access(fileDelete, F_OK) != -1 )
		{
			// Skip file if we cannot determine if it is a symbolic link
			struct stat lstat_buf;
			if ( lstat(fileDelete, &lstat_buf) )
				continue;

			// Only delete symbolic links
			if ( !S_ISLNK(lstat_buf.st_mode) )
				continue;

			int unlink_success = unlink(fileDelete) == 0;
			printf("> [LIBCOD] Manymaps: Removing old link %s: %s\n", fileDelete, unlink_success ? "success" : "failed");
		}
	}

	closedir(dir);
}

void manymaps_prepare(const char *mapname, int read)
{
	char map_check[MAX_OSPATH];
	char library_path[MAX_OSPATH];
	const char *stock_maps[] = {"mp_breakout", "mp_brecourt", "mp_burgundy", "mp_carentan", "mp_dawnville", "mp_decoy", "mp_downtown", "mp_farmhouse", "mp_leningrad", "mp_matmata", "mp_railyard", "mp_toujane", "mp_trainstation", "mp_rhine", "mp_harbor"};
	int map_num;

	manymaps_get_library_path(library_path, MAX_OSPATH);

	Com_sprintf(map_check, MAX_OSPATH, "%s/%s.iwd", library_path, mapname);

	// Version 1.0 does not have mp_rhine and mp_harbor
	// But they could be added as standalone .iwd files
	map_num = int(sizeof(stock_maps) / sizeof(stock_maps[0]));
	if ( getProtocolFromShortVersion(sv_version->current.string) < 117 )
		map_num -= 2;

	bool map_found = false;
	bool from_stock_map = false;

	for ( int i = 0; i < map_num; i++ )
	{
		if ( strcmp(sv_mapname->current.string, stock_maps[i]) == 0 )
		{
			from_stock_map = true;
			break;
		}
	}

	for ( int i = 0; i < map_num; i++ )
	{
		if ( strcmp(mapname, stock_maps[i]) == 0 )
		{
			map_found = true;

			// When changing from stock map to stock map, do not trigger
			// manymaps
			if ( from_stock_map )
				return;
			else
				break;
		}
	}

	int map_exists = access(map_check, F_OK) != -1;
	if ( !map_exists && !map_found )
		return;

	// Remove existing links to map library
	manymaps_cleanup();

	// Create new link to map library
	if ( map_exists )
	{
		char src[MAX_OSPATH];
		char dst[MAX_OSPATH];

		Com_sprintf(src, MAX_OSPATH, "%s/%s.iwd", library_path, mapname);
		Com_sprintf(dst, MAX_OSPATH, "%s/%s/%s.iwd", fs_homepath->current.string, fs_game->current.string, mapname);

		if ( access(src, F_OK) != -1 )
		{
			int link_success = symlink(src, dst) == 0;
			printf("> [LIBCOD] Manymaps: New link from %s to %s: %s\n", src, dst, link_success ? "success" : "failed");

			// FS_AddIwdFilesForGameDirectory() is needed when 000empty.iwd is
			// missing as then .d3dbsp is not referenced anywhere
			if ( link_success && read == -1 )
				FS_AddIwdFilesForGameDirectory(fs_homepath->current.string, fs_game->current.string);
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

void custom_GScr_LoadLevelScript()
{
	dvar_t *sv_mapname;
	char s[128]; // New: Original size was 64 chars

	sv_mapname = Dvar_RegisterString("mapname", "", DVAR_SERVERINFO | DVAR_ROM | DVAR_CHANGEABLE_RESET);

	/* New code start: Logic for fs_mapScriptDirectories dvar */
	switch ( fs_mapScriptDirectories->current.integer )
	{
	case 1:
		Com_sprintf(s, sizeof(s), "maps/mp/%s/%s", sv_mapname->current.string, sv_mapname->current.string);
		g_scr_data.levelscript = Scr_GetFunctionHandle(s, "main", 0);
		break;
	case 2:
		Com_sprintf(s, sizeof(s), "maps/mp/%s/%s", sv_mapname->current.string, sv_mapname->current.string);
		g_scr_data.levelscript = Scr_GetFunctionHandle(s, "main", 0);
		if ( !g_scr_data.levelscript )
		{
			Com_sprintf(s, sizeof(s), "maps/mp/%s", sv_mapname->current.string);
			g_scr_data.levelscript = Scr_GetFunctionHandle(s, "main", 0);
		}
		break;
	default:
		/* New code end */
		Com_sprintf(s, sizeof(s), "maps/mp/%s", sv_mapname->current.string);
		g_scr_data.levelscript = Scr_GetFunctionHandle(s, "main", 0);
	}
}

void custom_Scr_InitOpcodeLookup()
{
	hook_init_opcode->unhook();
	
	void (*GE_Scr_InitOpcodeLookup)();
	*(int *)&GE_Scr_InitOpcodeLookup = hook_init_opcode->from;
	
	scrVarPub_t *vars = &scrVarPub;
	
	vars->developer = 1;
	GE_Scr_InitOpcodeLookup();
	
	if ( !developer->current.integer )
		vars->developer = 0;
	
	hook_init_opcode->hook();
}

void custom_AddOpcodePos(int a1, int a2)
{
	hook_add_opcode->unhook();
	
	void (*AddOpcodePos)(int, int);
	*(int *)&AddOpcodePos = hook_add_opcode->from;
	
	scrVarPub_t *vars = &scrVarPub;
	
	vars->developer = 1;
	AddOpcodePos(a1, a2);
	
	if ( !developer->current.integer )
		vars->developer = 0;
	
	hook_add_opcode->hook();
}

void custom_Scr_PrintPrevCodePos(conChannel_t channel, const char *codePos, unsigned int index)
{
	unsigned int bufferIndex;
	unsigned int pos;

	if ( !codePos )
	{
		Com_PrintMessage(channel, "<frozen thread>\n");
		return;
	}

	if ( codePos == &g_EndPos )
	{
		Com_PrintMessage(channel, "<removed thread>\n");
	}
	else
	{
		/* Changed logic here so that in non-developer mode as much debug
		  information as available is still logged */
		if ( scrVarPub.programBuffer && Scr_IsInOpcodeMemory(codePos) )
		{
			pos = Scr_GetPrevSourcePos(codePos - 1, index);
			if ( (int)pos < 0 )
			{
				Com_PrintMessage(channel, "<unknown thread, enable developer mode to see the full trace>\n");
			}
			else
			{
				bufferIndex = Scr_GetSourceBuffer(codePos - 1);
				Scr_PrintSourcePos(
					channel,
					scrParserPub.sourceBufferLookup[bufferIndex].buf,
					scrParserPub.sourceBufferLookup[bufferIndex].sourceBuf,
					pos);
			}
			return;
		}
		Com_PrintMessage(channel, custom_va("%s\n\n", codePos));
	}
}

void hook_Sys_Print(const char *msg)
{
	if ( Scr_IsSystemActive() && con_coloredPrints->current.boolean )
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

	if ( !developer->current.integer )
		return;
	
	Com_Printf("%s", s);
}

scr_error_t scr_errors[MAX_ERROR_BUFFER];
int scr_errors_index = 0;
void Scr_CodeCallback_Error(qboolean terminal, qboolean emit, const char *internal_function, char *message)
{
	if ( codecallback_error && Scr_IsSystemActive() && !com_errorEntered )
	{
		if ( !strncmp(message, "exceeded maximum number of script variables", 43) )
		{
			/* Since we cannot allocate more script variables, further
			 execution of scripts or script callbacks could lead to an
			 undefined state (in script) or endless error loops, so we stop */
			Com_Error(ERR_DROP, "\x15%s", "exceeded maximum number of script variables");
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
				printf("Warning: Errors buffer full, not calling CodeCallback_Error for '%s'\n", message);
			}
		}
	}
}

void custom_Com_Error(errorParm_t code, const char *format, ...)
{
	va_list va;

	Sys_EnterCriticalSection(CRITSECT_COM_ERROR);

	if ( com_errorEntered )
	{
		Sys_Error("recursive error after: %s", &com_errorMessage);
	}
	com_errorEntered = 1;

	va_start(va, format);
	Q_vsnprintf(&com_errorMessage, 0x1000, format, va);
	va_end(va);
	memset((void *)((int)&com_errorMessage + 4095), 0, 1);

	Scr_CodeCallback_Error(scrVmPub.terminal_error, qtrue, "Com_Error", &com_errorMessage); // New
	
	if ( code != ERR_DROP )
	{
		if ( code == ERR_SCRIPT || code == ERR_LOCALIZATION )
			code = ERR_DROP;
		else if ( code == ERR_SCRIPT_DROP )
			code = ERR_DROP;
		else
			com_fixedConsolePosition = 0;
	}
	
	com_errorType = code;
	
	Sys_LeaveCriticalSection(CRITSECT_COM_ERROR);

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
	Com_Error(ERR_DROP, "\x15%s", scrVarPub.error_message);
}

void custom_SV_Status_f(void)
{
	int i, j, l;
	client_t *cl;
	int ping;

	/* New code start: Log only one message timestamp at status info */
	byte logTimestampsValue = logTimestamps->current.boolean;

	Com_Printf("status:"); // This message still has a timestamp
	logTimestamps->current.boolean = 0;
	/* New code end */

	if ( !com_sv_running->current.boolean )
	{
		Com_Printf("Server is not running.\n");
		return;
	}

	Com_Printf("map: %s\n", sv_mapname->current.string);
	Com_Printf("num score ping guid   name            lastmsg address               qport rate\n");
	Com_Printf("--- ----- ---- ------ --------------- ------- --------------------- ----- -----\n");

	for ( i = 0, cl = svs.clients; i < sv_maxclients->current.integer; i++, cl++ )
	{
		if ( !cl->state )
		{
			continue;
		}
		Com_Printf("%3i ", i);
		Com_Printf("%5i ", G_GetClientScore(cl - svs.clients));

		if ( cl->state == CS_CONNECTED )
		{
			Com_Printf("CNCT ");
		}
		else if ( cl->state == CS_ZOMBIE )
		{
			Com_Printf("ZMBI ");
		}
		else
		{
			ping = cl->ping < 9999 ? cl->ping : 9999;
			Com_Printf("%4i ", ping);
		}

		Com_Printf("%6i ", cl->guid);
		Com_Printf("%s^7", cl->name);
		l = 16 - I_DrawStrlen(cl->name);
		for ( j = 0; j < l; j++ )
			Com_Printf(" ");

		Com_Printf("%7i ", svs.time - cl->lastPacketTime);

		/* New code start: Print external client IP in case of proxying */
		if ( strlen(customPlayerState[i].preProxyIP) )
		{
			char s[INET_ADDRSTRLEN + 8] = {0};
			snprintf(s, sizeof(s), "%s:%i", customPlayerState[i].preProxyIP, customPlayerState[i].preProxyPort);
			Com_Printf("%s", s);
			l = 22 - strlen(s);
		}
		else
		/* New code end */
		{
			const char *s = NET_AdrToString(cl->netchan.remoteAddress);
			Com_Printf("%s", s);
			l = 22 - strlen(s);
		}
		for ( j = 0; j < l; j++ )
			Com_Printf(" ");

		Com_Printf("%5i", cl->netchan.qport);

		Com_Printf(" %5i", cl->rate);

		Com_Printf("\n");
	}
	Com_Printf("\n");

	logTimestamps->current.boolean = logTimestampsValue; // New
}

void custom_RuntimeError_Debug(conChannel_t channel, const char *pos, int index, const char *message)
{
	int i, j;
	byte logTimestampsValue;

	Scr_CodeCallback_Error(qfalse, qfalse, "RuntimeError_Debug", (char *)message); // New

	// New: Fix: Do not print log message timestamps in runtime error information
	logTimestampsValue = logTimestamps->current.boolean;
	logTimestamps->current.boolean = 0;

	Com_PrintMessage(channel, custom_va("\n******* script runtime error *******\n%s: ", message));
	custom_Scr_PrintPrevCodePos(channel, pos, index);
	i = scrVmPub.function_count;
	if ( scrVmPub.function_count )
	{
		while ( j = i - 1, 0 < j )
		{
			Com_PrintMessage(channel, "called from:\n");
			custom_Scr_PrintPrevCodePos(channel, scrVmPub.function_frame_start[i - 1].fs.pos, scrVmPub.function_frame_start[i - 1].fs.localId == 0);
			i = j;
		}
		Com_PrintMessage(channel, "started from:\n");
		custom_Scr_PrintPrevCodePos(channel, scrVmPub.function_frame_start[0].fs.pos, 1);
	}
	Com_PrintMessage(channel, "************************************\n");

	logTimestamps->current.boolean = logTimestampsValue;
}

void custom_RuntimeError(const char *pos, int index, const char *message, const char *dialog_error)
{
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
				custom_RuntimeError_Debug(CON_CHANNEL_DONT_FILTER, pos, index, message);
			else
				custom_RuntimeError_Debug(CON_CHANNEL_LOGFILEONLY, pos, index, message);
			
			
			if ( !terminate )
				return;
		}
		else
		{
			Com_Printf("%s\n", message);
			if ( !scrVmPub.terminal_error )
			{
				Scr_CodeCallback_Error(qfalse, qfalse, "RuntimeError", (char *)message); // New
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
			Com_Error(ERR_SCRIPT, "\x15script runtime error\n(see console for details)\n%s%s%s", message, newline, dialog_error);
		else
			Com_Error(ERR_SCRIPT_DROP, "\x15script runtime error\n(see console for details)\n%s%s%s", message, newline, dialog_error);
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
				switch ( arg->type )
				{
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

scr_notify_t scr_notify[MAX_NOTIFY_DEBUG_BUFFER];
int scr_notify_index = 0;
#if COMPILE_CUSTOM_VOICE == 1
loadSoundFileResult_t loadSoundFileResults[MAX_THREAD_RESULTS_BUFFER];
int loadSoundFileResultsIndex = 0;
int currentMaxSoundIndex = 0;
VoicePacket_t voiceDataStore[MAX_CUSTOMSOUNDS][MAX_STOREDVOICEPACKETS];
#endif
void custom_G_RunFrame(int levelTime)
{
	int i, j;
	client_t *client = svs.clients;
	
	hook_g_runframe->unhook();
	void (*G_RunFrame)(int levelTime);
	*(int *)&G_RunFrame = hook_g_runframe->from;

	// Warn about server lag
	if ( codecallback_hitchwarning && hitchFrameTime && Scr_IsSystemActive() )
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

	// Process voice data tweaks
	gclient_t *gclient = level.clients;
	int durationSinceLastTalk;
	VoicePacket_t fakeVoicePacket;
	fakeVoicePacket.data[0] = 0xFF; // Magic :)
	fakeVoicePacket.dataLen = 1;

	for ( i = 0; i < sv_maxclients->current.integer; i++, client++ )
	{
		if ( client->state < CS_CONNECTED )
			continue;

		// Client talker icon trigger mechanism
		for ( j = 0; j < sv_maxclients->current.integer; j++ )
		{
			if ( customPlayerState[i].talkerIcons[j] )
			{
				// No fake voice data if the player is actually talking
				gclient = level.clients + j;
				durationSinceLastTalk = level.time - gclient->lastVoiceTime;
				if ( durationSinceLastTalk >= 0 && g_voiceChatTalkingDuration->current.integer > durationSinceLastTalk )
					continue;
#if COMPILE_CUSTOM_VOICE == 1
				// No fake voice data if the player is streaming sound already
				if ( customPlayerState[i].currentSoundIndex && customPlayerState[i].currentSoundTalker == j )
					continue;
#endif
				fakeVoicePacket.talkerNum = j;
				SV_QueueVoicePacket(j, i, &fakeVoicePacket);
			}
		}
	}

#if COMPILE_CUSTOM_VOICE == 1
	// Try process results from Speex encoder tasks
	if ( Scr_IsSystemActive() )
	{
		if ( Sys_TryEnterCriticalSection(CRITSECT_LOAD_SOUND_FILE) == 0 )
		{
			if ( loadSoundFileResultsIndex == MAX_THREAD_RESULTS_BUFFER )
			{
				Com_Printf("Warning: LoadSoundFile results buffer full\n");
			}
			for ( i = 0; i < loadSoundFileResultsIndex; i++ )
			{
				stackPushInt(loadSoundFileResults[i].result);
				stackPushInt(loadSoundFileResults[i].soundIndex);
				short ret = Scr_ExecThread(loadSoundFileResults[i].callback, 2);
				Scr_FreeThread(ret);
			}
			loadSoundFileResultsIndex = 0;
			Sys_LeaveCriticalSection(CRITSECT_LOAD_SOUND_FILE);
		}
	}

	// Process custom voice data queue
	qboolean aPlayerIsTalking = qfalse;

	if ( sv_voice->current.boolean )
	{
		gclient = level.clients;
		for ( i = 0; i < sv_maxclients->current.integer; i++, gclient++ )
		{
			durationSinceLastTalk = level.time - gclient->lastVoiceTime;
			if ( durationSinceLastTalk >= 0 && g_voiceChatTalkingDuration->current.integer > durationSinceLastTalk )
			{
				aPlayerIsTalking = qtrue;
				break;
			}
		}
	}

	if ( !aPlayerIsTalking ) // TODO: Use SV_ClientHasClientMuted() here to only pause if a non-muted player talks
	{
		client = svs.clients;
		for ( i = 0; i < sv_maxclients->current.integer; i++, client++ )
		{
			if ( client->state < CS_CONNECTED )
				continue;

			if ( customPlayerState[i].currentSoundIndex )
			{
				customPlayerState[i].pendingVoiceDataFrames += MAX_VOICEPACKETSPERFRAME; // 51.2 packets per second @ 20 server fps
				VoicePacket_t *voicePacket;

				for ( ; customPlayerState[i].pendingVoiceDataFrames > 1.0 && customPlayerState[i].sentVoiceDataIndex < MAX_STOREDVOICEPACKETS; customPlayerState[i].sentVoiceDataIndex++, customPlayerState[i].pendingVoiceDataFrames -= 1.0 )
				{
					voicePacket = &voiceDataStore[customPlayerState[i].currentSoundIndex - 1][customPlayerState[i].sentVoiceDataIndex];
					if ( svs.clients[customPlayerState[i].currentSoundTalker].state < CS_CONNECTED || !voicePacket->dataLen )
					{
						customPlayerState[i].pendingVoiceDataFrames = 0.0;
						customPlayerState[i].currentSoundTalker = 0;
						customPlayerState[i].currentSoundIndex = 0;
						customPlayerState[i].sentVoiceDataIndex = 0;
						if ( Scr_IsSystemActive() )
							Scr_Notify(&g_entities[i], custom_scr_const.sound_file_done, 0);
						break;
					}
					voicePacket->talkerNum = customPlayerState[i].currentSoundTalker;
					SV_QueueVoicePacket(voicePacket->talkerNum, i, voicePacket);
				}
			}
		}
	}
#endif

	// Process bullet drop
	if ( g_bulletDrop->current.boolean )
	{
		for ( i = 0, client = svs.clients; i < sv_maxclients->current.integer; i++, client++ )
		{
			if ( client->state < CS_CONNECTED || customPlayerState[i].droppingBulletsCount < 1 )
				continue;

			for ( j = 0; j < MAX_DROPPING_BULLETS; j++ )
			{
				droppingBullet_t *bullet = &customPlayerState[i].droppingBullets[j];
				if ( bullet->inUse )
				{
					// Enforce max. bullet lifetime
					if ( ( level.time - bullet->startTime ) > g_bulletDropMaxTime->current.integer )
					{
						if ( bullet->visualBullet )
							custom_Bullet_Drop_Finalize_Visual(bullet, qtrue);

						custom_Bullet_Drop_Free(i, bullet);
						continue;
					}

					// Process bullet travel
					if ( g_antilag->current.boolean )
						custom_Bullet_Fire_Drop_Think_AntiLag(i, bullet);
					else
						custom_Bullet_Fire_Drop_Think(i, bullet);
				}
			}
		}
	}

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

	LargeLocalConstructor(&buf, MAX_LARGE_MSGLEN);
	data = LargeLocalGetBuf(&buf);
	if ( sv.state == SS_GAME )
	{
		if ( !svs.archiveEnabled )
		{
			LargeLocalDestructor(&buf);
		}
		else
		{
			MSG_Init(&msg, data, MAX_LARGE_MSGLEN);
			n = svs.nextCachedSnapshotFrames - 512;
			if ( n < 0 )
			{
				n = 0;
			}
			i = svs.nextCachedSnapshotFrames;
			do {
				m = i - 1;
				if ( m < n )
					goto LAB_0809b5f4;
				cachedFrameIndex = m;
				if ( m < 0 )
					cachedFrameIndex = i + 0x1fe;
				cachedFrame = svs.cachedSnapshotFrames + m + (cachedFrameIndex >> 9) * -0x200;
				i = m;
			} while ( cachedFrame->archivedFrame < svs.nextArchivedSnapshotFrames - sv_fps->current.integer || cachedFrame->usesDelta );
			
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
				for ( m = 0; m < sv_maxclients->current.integer; m++, client++ )
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
							Com_Error(ERR_FATAL, "\x15svs.nextCachedSnapshotClients wrapped");
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
							for ( l = 0; l < sv_maxclients->current.integer; l++ )
							{
								if ( customPlayerState[l].noEarthquakes )
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
							Com_Error(ERR_FATAL, "\x15svs.nextCachedSnapshotEntities wrapped");
						cachedFrame->num_entities++;
					}
				}
				svs.nextCachedSnapshotFrames++;
				if ( i != 0x7ffffffc && 0x7ffffffc < svs.nextCachedSnapshotFrames )
					Com_Error(ERR_FATAL, "\x15svs.nextCachedSnapshotFrames wrapped");
			}
			else
			{
				MSG_WriteBit0(&msg);
				MSG_WriteLong(&msg, cachedFrame->archivedFrame);
				MSG_WriteLong(&msg, svs.time);
				cachedClient2 = NULL;
				clientNum = 0;
				j = 0;
				while ( clientNum < sv_maxclients->current.integer || j < cachedFrame->num_clients )
				{
					if ( clientNum < sv_maxclients->current.integer && svs.clients[clientNum].state < CS_CONNECTED )
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
							for ( l = 0; l < sv_maxclients->current.integer; l++ )
							{
								if ( customPlayerState[l].noEarthquakes )
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
					Com_Error(ERR_FATAL, "\x15svs.nextArchivedSnapshotBuffer wrapped");
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
					Com_Error(ERR_FATAL, "\x15svs.nextArchivedSnapshotFrames wrapped");
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
				Com_Error(ERR_DROP, "\x15SV_BuildClientSnapshot: bad gEnt");
			VectorCopy(ps->origin, org);
			org[2] += ps->viewHeightCurrent;
			AddLeanToPosition(org, ps->viewangles[1], ps->leanf, 16.0, 20.0);
			if ( !cachedSnap )
			{
				SV_AddEntitiesVisibleFromPoint(org, clientNum, &entityNumbers);
				for ( i = 0; i < entityNumbers.numSnapshotEntities; i++ )
				{
					ent = SV_GentityNum(entityNumbers.snapshotEntities[i]);
					entState = svs.snapshotEntities + svs.nextSnapshotEntities % svs.numSnapshotEntities;
					*entState = ent->s;
					svs.nextSnapshotEntities++;
					if ( svs.nextSnapshotEntities >= 0x7ffffffe )
						Com_Error(ERR_FATAL, "\x15svs.nextSnapshotEntities wrapped");
					frame->num_entities++;
				}
				snapClient = svs.clients;
				for ( i = 0; i < sv_maxclients->current.integer; i++, snapClient++ )
				{
					if ( CS_ZOMBIE < snapClient->state )
					{
						clientStateSource = G_GetClientState(i);
						clientState = &svs.snapshotClients[svs.nextSnapshotClients % svs.numSnapshotClients];
						*clientState = *clientStateSource;
						
						svs.nextSnapshotClients++;
						if ( svs.nextSnapshotClients >= 0x7ffffffe )
							Com_Error(ERR_FATAL, "\x15svs.nextSnapshotClients wrapped");
						frame->num_clients++;
					}
				}
			}
			else
			{
				SV_AddCachedEntitiesVisibleFromPoint(cachedSnap->num_entities, cachedSnap->first_entity, org, clientNum, &entityNumbers);
				for ( i = 0; i < entityNumbers.numSnapshotEntities; i++ )
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
						Com_Error(ERR_FATAL, "\x15svs.nextSnapshotEntities wrapped");
					frame->num_entities++;
				}
				for ( i = 0; i < cachedSnap->num_clients; i++ )
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
						Com_Error(ERR_FATAL, "\x15svs.nextSnapshotClients wrapped");
					frame->num_clients++;
				}
			}
		}
	}
}

void custom_G_GetPlayerViewOrigin(gentity_t *ent, float *origin)
{
	float speed;
	float cycle;
	float hFactor;
	float vFactor;
	vec3_t direction;
	gclient_t *client;

	client = ent->client;

	if ( (client->ps.eFlags & EF_TURRET_STAND) != 0 )
	{
		if ( G_DObjGetWorldTagPos(&g_entities[client->ps.viewlocked_entNum], scr_const.tag_player, origin) )
		{
			return;
		}
		else
		{
			// New: g_turretMissingTagTerminalError dvar
			if ( g_turretMissingTagTerminalError->current.boolean )
				Com_Error(ERR_DROP, "G_GetPlayerViewOrigin: Couldn't find [tag_player] on turret");
			else
				Scr_CodeCallback_Error(qfalse, qfalse, "G_GetPlayerViewOrigin", custom_va("Turret removed for client %d while alive and in killcam", ent - g_entities));
		}
	}

	VectorCopy(client->ps.origin, origin);
	origin[2] = origin[2] + client->ps.viewHeightCurrent;
	cycle = BG_GetBobCycle(client);
	speed = BG_GetSpeed(&client->ps, level.time);
	vFactor = BG_GetVerticalBobFactor(&client->ps, cycle, speed, bg_bobMax->current.decimal);
	origin[2] = origin[2] + vFactor;
	hFactor = BG_GetHorizontalBobFactor(&client->ps, cycle, speed, bg_bobMax->current.decimal);
	G_GetPlayerViewDirection(ent, 0, direction, 0);
	VectorMA(origin, hFactor, direction, origin);
	AddLeanToPosition(origin, client->ps.viewangles[1], client->ps.leanf, 16.0, 20.0);

	if ( client->ps.origin[2] + 8.0 > origin[2] )
		origin[2] = client->ps.origin[2] + 8.0;
}

void custom_G_ClientStopUsingTurret(gentity_t *self)
{
	turretInfo_t *info;
	gentity_t *owner;

	info = self->pTurretInfo;

	/* New code start: Return if turret info has already been freed, see
	 g_turretMissingTagTerminalError dvar docs for when this can happen */
	if ( !info )
		return;
	/* New code end */

	owner = &g_entities[self->r.ownerNum];
	info->fireSndDelay = 0;
	self->s.loopSound = 0;

	if ( info->prevStance != ~WEAPSTANCE_STAND )
	{
		if ( info->prevStance == WEAPSTANCE_PRONE )
		{
			G_AddEvent(owner, EV_STANCE_FORCE_PRONE, 0);
		}
		else if ( info->prevStance == WEAPSTANCE_DUCK )
		{
			G_AddEvent(owner, EV_STANCE_FORCE_CROUCH, 0);
		}
		else
		{
			G_AddEvent(owner, EV_STANCE_FORCE_STAND, 0);
		}

		info->prevStance = (weapStance_t)~WEAPSTANCE_STAND;
	}

	TeleportPlayer(owner, info->userOrigin, owner->r.currentAngles);
	owner->client->ps.eFlags &= ~EF_TURRET_STAND;
	owner->client->ps.viewlocked = PLAYERVIEWLOCK_NONE;
	owner->client->ps.viewlocked_entNum = ENTITY_NONE;
	owner->active = 0;
	owner->s.otherEntityNum = 0;
	self->active = 0;
	self->r.ownerNum = ENTITY_NONE;
	info->flags &= ~0x800u;
}

int num_map_turrets;
map_turret_t map_turrets[MAX_GENTITIES];
void custom_G_ParseEntityFields(gentity_t *ent)
{
	/* New code start: map weapons callback */
	if ( codecallback_map_turrets_load )
	{
		// Default turret arcs to -1 so we can identify these as undefined in
		// script
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
	G_SetOrigin(ent, ent->r.currentOrigin);
	G_SetAngle(ent, ent->r.currentAngles);
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
		if ( !strncmp(classname, "weapon_", 7) && !g_spawnMapWeapons->current.boolean )
		{
			ent = G_Spawn();
			custom_G_ParseEntityFields(ent);
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
		if ( ( !strncmp(classname, "misc_mg42", 9) || !strncmp(classname, "misc_turret", 11) ) && !g_spawnMapTurrets->current.boolean )
		{
			ent = G_Spawn();
			custom_G_ParseEntityFields(ent);
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
					G_ParseEntityFields(ent);
					((void (*)(gentity_t *))i[1])(ent);
					return;
				}
			}
			ent = G_Spawn();
			G_ParseEntityFields(ent);
		}
		else
		{
			ent = G_Spawn();
			G_ParseEntityFields(ent);
			G_SpawnItem(ent, item);
		}
	}
}

void ClearAllNotSolidForPlayerFlags(void)
{
	for ( int i = 72; i < MAX_GENTITIES; i++ )
	{
		customEntityState[i].notSolidBrushModel = qfalse;
		customEntityState[i].clientMask[0] = 0;
		customEntityState[i].clientMask[1] = 0;
	}
}

void ClearNotSolidForPlayerFlags(int clientNum)
{
	gentity_t *ent;
	int id;

	// Skip reserved entity slots: Players, player clones
	for ( int i = 72; i < level.num_entities; i++ )
	{
		ent = &g_entities[i];
		id = ent - g_entities;

		if ( ent->r.bmodel )
		{
			customEntityState[id].clientMask[clientNum >> 5] &= ~(1 << (clientNum & 0x1F));
			if ( !customEntityState[id].clientMask[0] && !customEntityState[id].clientMask[1] )
				customEntityState[id].notSolidBrushModel = qfalse;
		}
	}
}

savedBrushModelContents_t savedBrushModelContents[MAX_GENTITIES];
int savedBrushModelContentsCount = 0;
void SaveBrushModelContents(void)
{
	gentity_t *ent;

	savedBrushModelContentsCount = 0;

	// Skip reserved entity slots: Players, player clones
	for ( int i = 72; i < level.num_entities; i++ )
	{
		ent = &g_entities[i];
		if ( ent->r.bmodel )
		{
			savedBrushModelContents[savedBrushModelContentsCount].num = i;
			savedBrushModelContents[savedBrushModelContentsCount].contents = ent->r.contents;
			savedBrushModelContentsCount++;
		}
	}
}

void UpdateSavedBrushModelContents(int num, int contents)
{
	for ( int i = 0; i < savedBrushModelContentsCount; i++ )
	{
		savedBrushModelContents_t *ref = &savedBrushModelContents[i];
		gentity_t *ent = &g_entities[ref->num];

		if ( num == ent - g_entities )
		{
			savedBrushModelContents[i].contents = contents;
			return;
		}
	}
}

void custom_ScrCmd_SetContents(scr_entref_t entref)
{
	gentity_t *ent;
	int contents;

	ent = GetEntity(entref.entnum);
	contents = Scr_GetInt(0);
	Scr_AddInt((ent->r).contents);
	(ent->r).contents = contents;
	SV_LinkEntity(ent);

	// New: Update saved brush model contents list
	UpdateSavedBrushModelContents(entref.entnum, contents);
}

void custom_G_SpawnEntitiesFromString(void)
{
	if ( !G_ParseSpawnVars(&level.spawnVars) ) 
		Com_Error(ERR_DROP, "\x15SpawnEntities: no entities");

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
	
	while ( G_ParseSpawnVars(&level.spawnVars) )
		custom_G_CallSpawn();

	/* New: Collect list of brush model entities after map load so that we do
	 not have to loop through all entities multiple times per player and
	 server frame when modifying the entity contents in player movement
	 trace calls */
	ClearAllNotSolidForPlayerFlags();
	SaveBrushModelContents();
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
			for ( int i = 0; i < num_map_weapons; i++ )
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
			for ( int i = 0; i < num_map_turrets; i++ )
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
	XModel *xmodel;

	if ( src == NULL || *src == '\0' )
		Com_Error(ERR_DROP, "\x15Invalid static model name\n");
	
	if ( (*scale)[0] == 0.0 )
		Com_Error(ERR_DROP, "\x15Static model [%s] has x scale of 0.0\n", src);
	
	if ( (*scale)[1] == 0.0 )
		Com_Error(ERR_DROP, "\x15Static model [%s] has y scale of 0.0\n", src);
	
	if ( (*scale)[2] == 0.0 )
		Com_Error(ERR_DROP, "\x15Static model [%s] has z scale of 0.0\n", src);
	
	xmodel = CM_XModelPrecache(src);
	if ( xmodel != NULL )
	{
		model->xmodel = xmodel;
		// On the server side, scale is only used for trace functions (see model->invScaledAxis)
		// The entity axis scale values are not synced to the players
		CM_InitStaticModel(model, origin, angles, scale);
		if ( g_debugStaticModels->current.boolean )
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
	client_t *cl = svs.clients + player->s.number;

	client = player->client;
	(client->ps).cursorHint = 0;
	(client->ps).cursorHintString = -1;
	(client->ps).cursorHintEntIndex = ENTITY_NONE;
	if ( 0 < player->health && ( (player->client->ps).weaponstate < 0x11 || ( 0x16 < (player->client->ps).weaponstate ) ) )
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
						temp = Player_GetItemCursorHint(player->client, ent);
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

								/* New code start: sv_botUseTriggerUse dvar */
								if ( cl->bIsTestClient && sv_botUseTriggerUse->current.boolean && Scr_IsSystemActive() )
								{
									Scr_AddEntity(ent);
									Scr_Notify(player, custom_scr_const.bot_trigger, 1);
								}
								/* New code end */

								if ( (client->ps).cursorHint != 0 )
								{
									return;
								}
								(client->ps).cursorHintEntIndex = ENTITY_NONE;
								return;
							}

							/* New code start: hintString support for trigger_radius */
							if ( customEntityState[(ent->s).number].convertedTrigger && Scr_IsSystemActive() )
							{
								Scr_AddEntity(player);
								Scr_Notify(ent, scr_const.trigger, 1);
							}
							/* New code end */

							if ( ( ent->team == 0 || ent->team == (player->client->sess.state).team ) &&
							( ent->trigger.singleUserEntIndex == ENTITY_NONE || ent->trigger.singleUserEntIndex == (player->client->ps).clientNum ) )
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
		else if ( ( (client->ps).eFlags & EF_TURRET_STAND ) != 0 )
		{
			Player_SetTurretDropHint(player);
		}
	}
}

void custom_PlayerCmd_ClonePlayer(scr_entref_t entref)
{
	int id = entref.entnum;

	hook_playercmd_cloneplayer->unhook();
	void (*PlayerCmd_ClonePlayer)(scr_entref_t entref);
	*(int *)&PlayerCmd_ClonePlayer = hook_playercmd_cloneplayer->from;

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
		PlayerCmd_ClonePlayer(entref);
	}

	hook_playercmd_cloneplayer->hook();
}

void custom_player_die(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, meansOfDeath_t meansOfDeath, int iWeapon, const float *vDir, hitLocation_t hitLoc, int psTimeOffset)
{
	pmtype_t type;
	gentity_t *turret;
	gclient_t *client;
	vec3_t origin;
	vec3_t dir;
	int deathAnimDuration;
	int i;

	if ( Com_GetServerDObj(self->client->ps.clientNum)
	        && self->client->ps.pm_type <= PM_NORMAL_LINKED
	        && (self->client->ps.pm_flags & 0x400000) == 0 )
	{
		bgs = &level_bgs;

		if ( attacker->s.eType == ET_TURRET && attacker->r.ownerNum != ENTITY_NONE )
			attacker = &g_entities[attacker->r.ownerNum];
		
		Scr_AddEntity(attacker);
		Scr_Notify(self, scr_const.death, 1);

		if ( !scr_turretDamageName->current.boolean && iWeapon ) // New: scr_turretDamageName dvar condition
		{
			if ( attacker->client )
			{
				if ( (attacker->client->ps.eFlags & EF_TURRET_STAND) != 0 )
				{
					turret = &g_entities[attacker->s.otherEntityNum];

					if ( turret->s.eType == ET_TURRET )
						iWeapon = turret->s.weapon;
				}
			}
		}

		if ( self->client->ps.grenadeTimeLeft )
		{
			dir[0] = G_crandom();
			dir[1] = G_crandom();
			dir[2] = G_random();
			VectorScale(dir, 160.0, dir);
			VectorCopy(self->r.currentOrigin, origin);
			origin[2] = origin[2] + 40.0;
			fire_grenade(self, origin, dir, self->client->ps.offHandIndex, self->client->ps.grenadeTimeLeft);
		}

		if ( self->client->ps.pm_type == PM_NORMAL_LINKED )
			type = PM_DEAD_LINKED;
		else
			type = PM_DEAD;

		self->client->ps.pm_type = type;
		deathAnimDuration = BG_AnimScriptEvent(&self->client->ps, ANIM_ET_DEATH, 0, 1);

		Scr_PlayerKilled(
		    self,
		    inflictor,
		    attacker,
		    damage,
		    meansOfDeath,
		    iWeapon,
		    vDir,
		    hitLoc,
		    psTimeOffset,
		    deathAnimDuration);

		for ( i = 0; i < level.maxclients; ++i )
		{
			client = &level.clients[i];

			if ( client->sess.connected == CON_CONNECTED
			        && client->sess.sessionState == STATE_SPECTATOR
			        && client->spectatorClient == self->s.number )
			{
				Cmd_Score_f(&g_entities[i]);
			}
		}

		self->takedamage = 1;
		self->r.contents = CONTENTS_CORPSE;
		self->r.currentAngles[2] = 0.0;
		LookAtKiller(self, inflictor, attacker);
		VectorCopy(self->r.currentAngles, self->client->ps.viewangles);
		self->s.loopSound = 0;
		SV_UnlinkEntity(self);
		self->r.maxs[2] = 30.0;
		SV_LinkEntity(self);
		self->health = 0;
		self->handler = ENT_HANDLER_CLIENT_DEAD;
	}
}

void custom_PlayerCmd_finishPlayerDamage(scr_entref_t entref)
{
	const char *mod;
	const char *weaponName;
	unsigned short hitlocStr;
	gentity_t *tempEnt;
	gclient_t *client;
	float knockback;
	gentity_t *ent;
	float flinch;
	float dmgTime;
	float maxTime;
	void (*pain)(gentity_t *, gentity_t *, int, const float *, meansOfDeath_t, const float *, hitLocation_t);
	void (*die)(gentity_t *, gentity_t *, gentity_t *, int, meansOfDeath_t, const int, const float *, hitLocation_t, int);
	int bodyBulletImpacts;
	int psOffsetTime;
	int maxDmg;
	vec3_t velocaityScale;
	vec3_t localdir;
	float dmgRange;
	int minDmg;
	hitLocation_t hitlocIndex;
	int weaponIndex;
	meansOfDeath_t modIndex;
	int dflags;
	int damage;
	const float *point;
	vec3_t vPoint;
	vec_t *dir;
	vec3_t vDir;
	gentity_t *attacker;
	gentity_t *inflictor;

	inflictor = &g_entities[1022];
	attacker = &g_entities[1022];
	dir = 0;
	point = 0;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entref.entnum];

		if ( !ent->client )
		{
			Scr_ObjectError(custom_va("entity %i is not a player", entref.entnum));
		}
	}

	damage = Scr_GetInt(2u);

	if ( damage > 0 )
	{
		if ( Scr_GetType(0) && Scr_GetPointerType(0) == VAR_ENTITY )
			inflictor = Scr_GetEntity(1u);

		if ( Scr_GetType(1u) && Scr_GetPointerType(1u) == VAR_ENTITY )
			attacker = Scr_GetEntity(1u);

		dflags = Scr_GetInt(3u);
		mod = Scr_GetString(4u);
		modIndex = G_IndexForMeansOfDeath(mod);
		weaponName = Scr_GetString(5u);
		weaponIndex = G_GetWeaponIndexForName(weaponName);

		if ( Scr_GetType(6u) )
		{
			Scr_GetVector(6u, &vPoint);
			point = vPoint;
		}

		if ( Scr_GetType(7u) )
		{
			Scr_GetVector(7u, &vDir);
			dir = vDir;
		}

		hitlocStr = Scr_GetConstString(8u);
		hitlocIndex = G_GetHitLocationIndexFromString(hitlocStr);
		psOffsetTime = Scr_GetInt(9u);
		/* New code start: Additional parameter to skip body bullet impacts */
		bodyBulletImpacts = 1;
		if ( Scr_GetNumParam() > 10 )
			bodyBulletImpacts = Scr_GetInt(10u);
		/* New code end */

		if ( dir )
			Vec3NormalizeTo(dir, localdir);
		else
			VectorClear(localdir);

		if ( (ent->flags & 8) != 0 || (dflags & 4) != 0 )
		{
			minDmg = 0;
		}
		else
		{
			dmgRange = 0.30000001;

			if ( (ent->client->ps.pm_flags & 1) != 0 )
			{
				dmgRange = 0.02;
			}
			else if ( (ent->client->ps.pm_flags & 2) != 0 )
			{
				dmgRange = 0.15000001;
			}

			minDmg = (int)((float)damage * dmgRange);

			if ( minDmg > 60 )
				minDmg = 60;

			if ( minDmg )
			{
				if ( (ent->client->ps.eFlags & 0x300) == 0 )
				{
					knockback = (float)minDmg * g_knockback->current.decimal / 250.0;
					VectorScale(localdir, knockback, velocaityScale);
					VectorAdd(ent->client->ps.velocity, velocaityScale, ent->client->ps.velocity);

					if ( !ent->client->ps.pm_time )
					{
						maxDmg = 2 * minDmg;

						if ( 2 * minDmg <= 49 )
							maxDmg = 50;

						if ( maxDmg > 200 )
							maxDmg = 200;

						ent->client->ps.pm_time = maxDmg;
						ent->client->ps.pm_flags |= 0x400u;
					}
				}
			}
		}

		if ( (ent->flags & 1) == 0 )
		{
			if ( weaponIndex && BG_GetWeaponDef(weaponIndex)->weapType == WEAPTYPE_BULLET && bodyBulletImpacts )
			{
				if ( BG_GetWeaponDef(weaponIndex)->bRifleBullet )
					tempEnt = G_TempEntity(vPoint, EV_SHOTGUN_HIT);
				else
					tempEnt = G_TempEntity(vPoint, EV_BULLET_HIT_LARGE);

				tempEnt->s.eventParm = DirToByte(localdir);
				tempEnt->s.scale = DirToByte(localdir);
				tempEnt->s.surfType = 7;
				tempEnt->s.otherEntityNum = attacker->s.number;
				tempEnt->r.clientMask[ent->client->ps.clientNum >> 5] |= 1 << (ent->client->ps.clientNum & 0x1F);

				if ( BG_GetWeaponDef(weaponIndex)->bRifleBullet )
					tempEnt = G_TempEntity(vPoint, EV_BULLET_HIT_CLIENT_LARGE);
				else
					tempEnt = G_TempEntity(vPoint, EV_BULLET_HIT_CLIENT_SMALL);

				tempEnt->s.surfType = 7;
				tempEnt->s.otherEntityNum = attacker->s.number;
				tempEnt->s.clientNum = ent->client->ps.clientNum;
				tempEnt->r.clientMask[0] = -1;
				tempEnt->r.clientMask[1] = -1;
				tempEnt->r.clientMask[ent->client->ps.clientNum >> 5] &= ~(1 << (ent->client->ps.clientNum & 0x1F));
			}

			ent->client->damage_blood += damage;

			if ( dir )
			{
				VectorCopy(localdir, ent->client->damage_from);
				ent->client->damage_fromWorld = 0;
			}
			else
			{
				VectorCopy(ent->r.currentOrigin, ent->client->damage_from);
				ent->client->damage_fromWorld = 1;
			}

			if ( (ent->flags & 2) != 0 && ent->health - damage <= 0 )
				damage = ent->health - 1;

			maxTime = player_dmgtimer_maxTime->current.decimal;
			dmgTime = (float)damage * player_dmgtimer_timePerPoint->current.decimal;
			ent->client->ps.damageTimer += (int)dmgTime;

			if ( dir )
			{
				client = ent->client;
				client->ps.flinchYaw = (int)vectoyaw(dir);
			}
			else
			{
				ent->client->ps.flinchYaw = 0;
			}

			flinch = ent->client->ps.viewangles[1];

			if ( flinch < 0.0 )
				flinch = flinch + 360.0;

			ent->client->ps.flinchYaw -= (int)flinch;

			if ( (float)ent->client->ps.damageTimer > maxTime )
				ent->client->ps.damageTimer = (int)maxTime;

			ent->client->ps.damageDuration = ent->client->ps.damageTimer;
			ent->health -= damage;
			Scr_AddEntity(attacker);
			Scr_AddInt(damage);
			Scr_Notify(ent, scr_const.damage, 2u);

			if ( ent->health > 0 )
			{
				pain = entityHandlers[ent->handler].pain;

				if ( pain )
					pain(ent, attacker, damage, point, modIndex, localdir, hitlocIndex);
			}
			else
			{
				if ( ent->health < -999 )
					ent->health = -999;

				die = entityHandlers[ent->handler].die;

				if ( die )
					die(ent, inflictor, attacker, damage, modIndex, weaponIndex, localdir, hitlocIndex, psOffsetTime);

				if ( !ent->r.inuse )
					return;
			}

			ent->client->ps.stats[0] = ent->health;
		}
	}
}

void custom_PlayerCmd_Suicide(scr_entref_t entref)
{
	gentity_t *pSelf;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		pSelf = 0;
	}
	else
	{
		pSelf = &g_entities[entref.entnum];

		if ( !pSelf->client )
		{
			Scr_ObjectError(custom_va("entity %i is not a player", entref.entnum));
		}
	}

	/* New code start: CodeCallback_Suicide */
	if ( codecallback_suicide && Scr_IsSystemActive() )
	{	
		short ret = Scr_ExecEntThread(pSelf, codecallback_suicide, 0);
		Scr_FreeThread(ret);
		return;
	}
	/* New code end */
	else
	{
		pSelf->flags &= 0xfffffffc;
		pSelf->health = 0;
		pSelf->client->ps.stats[STAT_HEALTH] = 0;

		player_die(pSelf, pSelf, pSelf, 100000, MOD_SUICIDE, 0, 0, HITLOC_NONE, 0);
	}
}

void custom_PM_BeginWeaponChange(playerState_t *ps, unsigned int newweapon)
{
	hook_pm_beginweaponchange->unhook();
	void (*PM_BeginWeaponChange)(playerState_t *ps, unsigned int newweapon);
	*(int *)&PM_BeginWeaponChange = hook_pm_beginweaponchange->from;

	/* New code start: CodeCallback_WeaponChange */
	if ( codecallback_weapon_change && newweapon != customPlayerState[ps->clientNum].weapon && Scr_IsSystemActive() )
	{
		stackPushInt(newweapon);
		short ret = Scr_ExecEntThread(&g_entities[ps->clientNum], codecallback_weapon_change, 1);
		Scr_FreeThread(ret);
	}
	customPlayerState[ps->clientNum].weapon = newweapon;
	/* New code end */

	PM_BeginWeaponChange(ps, newweapon);

	hook_pm_beginweaponchange->hook();
}

void custom_Scr_BulletTrace(void)
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
	vec3_t start;
	vec3_t end;

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

	/* New code start: bulletTrace contentmask override */
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
	G_LocationalTrace(&trace, start, end, passEntityNum, contentmask, NULL);
	Scr_MakeArray();
	Scr_AddFloat(trace.fraction);
	Scr_AddArrayStringIndexed(scr_const.fraction);
	Vec3Lerp(start, end, trace.fraction, position);
	Scr_AddVector(position);
	Scr_AddArrayStringIndexed(scr_const.position);
	if ( trace.entityNum == ENTITY_NONE || trace.entityNum == ENTITY_WORLD )
	{
		Scr_AddUndefined();
	}
	else
	{
		Scr_AddEntity(&g_entities[trace.entityNum]);
	}
	Scr_AddArrayStringIndexed(scr_const.entity);
	if ( trace.fraction < 1.0 )
	{
		Scr_AddVector(trace.normal);
		Scr_AddArrayStringIndexed(scr_const.normal);
		Scr_AddString(Com_SurfaceTypeToName((int)(trace.surfaceFlags & 0x1f00000U) >> 0x14));
		Scr_AddArrayStringIndexed(scr_const.surfacetype);
		Scr_AddInt(trace.surfaceFlags);
		Scr_AddArrayStringIndexed(custom_scr_const.flags);
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
		Scr_AddArrayStringIndexed(custom_scr_const.flags);
	}
}

void custom_Scr_BulletTracePassed(void)
{
	int args;
	int hitCharacters;
	int type;
	gentity_t *passEnt;
	int contentmask;
	int passEntityNum;
	vec3_t start;
	vec3_t end;

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

	/* New code start: bulletTracePassed contentmask override */
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

	Scr_AddBool(G_LocationalTracePassed(start, end, passEntityNum, contentmask));
}

void custom_Scr_SightTracePassed(void)
{
	int args;
	int type;
	int hitNum;
	uint contentmask;
	int passEntityNum;
	gentity_t *passEnt;
	int hitCharacters;
	vec3_t end;
	vec3_t start;
	float visibility;

	passEntityNum = ENTITY_NONE;
	Scr_GetVector(0, &start);
	Scr_GetVector(1, &end);
	hitCharacters = Scr_GetInt(2);

	if ( hitCharacters == 0 )
	{
		contentmask = MASK_OPAQUE_AI ^ CONTENTS_BODY;
	}
	else
	{
		contentmask = MASK_OPAQUE_AI;
	}

	/* New code start: sightTracePassed contentmask override */
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
	G_SightTrace(&hitNum, start, end, passEntityNum, contentmask);
	visibility = SV_FX_GetVisibility(start, end);
	if ( !hitNum && visibility < 0.2 )
	{
		hitNum = 1;
	}
	Scr_AddBool(hitNum == 0);
}

void custom_GScr_KickPlayer()
{
	int id;
	char* msg;
	char tmp[128];

	// New: Added 2nd parameter for kick message and some error handling

	if ( Scr_GetNumParam() > 1 )
	{
		if ( !stackGetParams("is", &id, &msg) )
		{
			stackError("custom_GScr_KickPlayer() one or more arguments is undefined or has a wrong type");
			stackPushUndefined();
			return;
		}
	}
	else
	{
		if ( !stackGetParams("i", &id) )
		{
			stackError("custom_GScr_KickPlayer() one or more arguments is undefined or has a wrong type");
			stackPushUndefined();
			return;
		}
		else
		{
			Cbuf_ExecuteText(2, custom_va("tempBanClient %i\n", id));
			return;
		}
	}

	if ( id >= MAX_CLIENTS )
	{
		stackError("custom_GScr_KickPlayer() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	if ( client == NULL )
	{
		stackError("custom_GScr_KickPlayer() entity %i is no longer connected", id);
		stackPushUndefined();
		return;
	}

	if ( client->netchan.remoteAddress.type == NA_LOOPBACK )
	{
		stackError("custom_GScr_KickPlayer() cannot kick host");
		stackPushUndefined();
		return;
	}

	strncpy(tmp, msg, sizeof(tmp));
	tmp[sizeof(tmp) - 1] = '\0';
	SV_DropClient(client, tmp);

	stackPushBool(qtrue);
}

void custom_GScr_Obituary(void)
{
	int args;
	int sWeapon;
	uint sMeansOfDeath;
	gentity_t *ent;
	gentity_t *victim;
	gentity_t *attacker;
	vec3_t origin;
	const char *team_str;
	customTeam_t team = CUSTOM_TEAM_NONE;
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
		ent = G_TempEntity(vec3_origin, EV_OBITUARY);
	}
	else
	{
		Scr_GetVector(args - 2, &origin);
		ent = G_TempEntity(origin, EV_OBITUARY);
	}
	
	// Custom team (default 0 = all)
	if ( args == 5 || args == 7 )
	{
		team_str = Scr_GetString(4);
		if ( !strcmp(team_str, "axis") )
			team = CUSTOM_TEAM_AXIS;
		else if ( !strcmp(team_str, "allies") )
			team = CUSTOM_TEAM_ALLIES;
		else if ( !strcmp(team_str, "spectator") )
			team = CUSTOM_TEAM_SPECTATOR;
		else if ( !strcmp(team_str, "allies+axis") || !strcmp(team_str, "axis+allies") )
			team = CUSTOM_TEAM_AXIS_ALLIES;
		else if ( !strcmp(team_str, "spectator+axis") || !strcmp(team_str, "axis+spectator") )
			team = CUSTOM_TEAM_AXIS_SPECTATOR;
		else if ( !strcmp(team_str, "spectator+allies") || !strcmp(team_str, "allies+spectator") )
			team = CUSTOM_TEAM_ALLIES_SPECTATOR;
	}
	ent->s.scale = (int)team; // Reusing the scale field that is otherwise not used at obituary TempEntities
	
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

void custom_GScr_SetHintString(scr_entref_t entref)
{
	int id = entref.entnum;
	gentity_t *ent;
	char hintString[MAX_STRINGLENGTH];
	int index;

	ent = GetEntity(id);
	if ( ( ent->classname != custom_scr_const.trigger_radius ) && ( ent->classname != scr_const.trigger_use ) && ( ent->classname != scr_const.trigger_use_touch ) )
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
	if ( ent->classname == custom_scr_const.trigger_radius )
	{
		Scr_SetString(&ent->classname, scr_const.trigger_use_touch);
		ent->trigger.singleUserEntIndex = ENTITY_NONE;
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

		manymaps_get_library_path(library_path, MAX_OSPATH);

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
			switch ( savedArg->type )
			{
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
			switch ( savedArg->type )
			{
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
			switch ( arg->type )
			{
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

void custom_G_UpdateObjectiveToClients(void)
{
	int i, j;
	gclient_t *client;
	team_t team;
	objective_t *obj;

	for ( i = 0; i < level.maxclients; i++ )
	{
		if ( level.gentities[i].r.inuse != 0 )
		{
			client = level.gentities[i].client;
			team = ((level.gentities[i].client)->sess.state).team;
			for ( j = 0; j < 16; j++ )
			{
				/* New code start: per-player objective functions */
				obj = &customPlayerState[i].objectives[j];
				if ( obj->state != OBJST_EMPTY )
				{
					G_UpdateSingleObjective(&(client->ps).objective[j], obj);
				}
				else
				{
				/* New code end */
					obj = &level.objectives[j];
					if ( obj->state == OBJST_EMPTY || ( obj->teamNum != 0 && obj->teamNum != team ) )
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

void custom_G_DamageClient(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, const float *vDir, const float *vPoint, int damage, int dFlags, meansOfDeath_t meansOfDeath, hitLocation_t hitLoc, int timeOffset)
{
	int iWeapon;
	int dmg;

	if ( self->takedamage && !self->client->noclip && !self->client->ufo && self->client->sess.connected == CON_CONNECTED )
	{
		if ( inflictor )
		{
			/* New code start: scr_turretDamageName dvar */
			if ( scr_turretDamageName->current.boolean && inflictor->s.eFlags & EF_TURRET_STAND )
			{
				gentity_t *turret = &level.gentities[inflictor->client->ps.viewlocked_entNum];

				iWeapon = turret->s.weapon;
			}
			else
			/* New code end */
			{
				iWeapon = inflictor->s.weapon;
			}
		}
		else if ( attacker )
		{
			iWeapon = attacker->s.weapon;
		}
		else
		{
			iWeapon = 0;
		}

		dmg = (int)(G_GetWeaponHitLocationMultiplier(hitLoc, iWeapon) * (float)damage);

		if ( dmg > 0 )
			Scr_PlayerDamage(self, inflictor, attacker, dmg, dFlags, meansOfDeath, iWeapon, vPoint, vDir, hitLoc, timeOffset);
	}
}

void custom_FireWeaponMelee(gentity_t *player)
{
	weaponParms wp;
	int id;
	float range, width, height;

	if ( (player->client->ps.eFlags & EF_TURRET_STAND) == 0 || player->active == 0 )
	{
		id = player->client->ps.clientNum;
		wp.weapDef = BG_GetWeaponDef(player->s.weapon);
		G_GetPlayerViewOrigin(player, wp.muzzleTrace);
		G_GetPlayerViewDirection(player, wp.forward, wp.right, wp.up);

		/* Stock values from G_RegisterDvars:
		player_meleeRange = Dvar_RegisterFloat("player_meleeRange", 64.0, 0.0, 1000.0, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
		player_meleeWidth = Dvar_RegisterFloat("player_meleeWidth", 10.0, 0.0, 1000.0, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
		player_meleeHeight = Dvar_RegisterFloat("player_meleeHeight", 10.0, 0.0, 1000.0, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
		*/

		/* New code start: per-player melee values */
		range = player_meleeRange->current.decimal * customPlayerState[id].meleeRangeScale;
		width = player_meleeWidth->current.decimal * customPlayerState[id].meleeWidthScale;
		height = player_meleeHeight->current.decimal * customPlayerState[id].meleeHeightScale;
		/* New code end */

		Weapon_Melee(player, &wp, range, width, height);
	}
}

void custom_Bullet_Fire_Extended(const gentity_t *inflictor, gentity_t *attacker, float *start, float *end, float dmgScale, int callCount, const weaponParms *wp, const gentity_t *weaponEnt, int gameTime)
{
	float scale;
	float scaleSquared;
	float dirScale;
	int surfaceType;
	int event;
	float dot;
	vec3_t dir;
	vec3_t origin;
	meansOfDeath_t meansOfDeath;
	int dflags;
	int damage;
	float dist;
	vec3_t temp;
	gentity_t *self;
	gentity_t *tempEnt;
	trace_t trace;
	int contentMask;
	uint8_t *priorityMap;
	qboolean noBulletImpacts;

	dflags = 0;

	if ( callCount <= 12 )
	{
		if ( wp->weapDef->bRifleBullet )
		{
			meansOfDeath = MOD_RIFLE_BULLET;
			dflags = 32;
		}
		else
		{
			meansOfDeath = MOD_PISTOL_BULLET;
		}

		if ( wp->weapDef->armorPiercing )
			dflags |= 2u;

		if ( wp->weapDef->bRifleBullet )
			priorityMap = &riflePriorityMap;
		else
			priorityMap = &bulletPriorityMap;

		contentMask = MASK_SHOT;

		/* New code start: Custom bullet mask and firing through walls */
		if ( attacker->s.number < 64 )
		{
			if ( customPlayerState[attacker->s.number].overrideBulletMask )
			{
				contentMask = customPlayerState[attacker->s.number].bulletMask;
			}
			else if ( customPlayerState[attacker->s.number].fireThroughWalls )
			{
				// Set contentmask to only hit player bodies
				contentMask = CONTENTS_BODY;
			}
		}
		/* New code end */

		G_LocationalTrace(&trace, start, end, inflictor->s.number, contentMask, priorityMap);
		Vec3Lerp(start, end, trace.fraction, origin);
		custom_G_CheckHitTriggerDamage(attacker, start, origin, wp->weapDef->damage, meansOfDeath);
		self = &g_entities[trace.entityNum];
		VectorSubtract(end, start, dir);
		Vec3Normalize(dir);
		dot = DotProduct(dir, trace.normal) * -2.0;
		VectorMA(dir, dot, trace.normal, dir);

		if ( (trace.surfaceFlags & SURF_SKY) == 0 && !self->client && trace.fraction < 1.0 )
		{
			if ( wp->weapDef->weapClass == WEAPCLASS_SPREAD )
			{
				event = EV_BULLET_HIT_SMALL;
			}
			else if ( wp->weapDef->bRifleBullet )
			{
				event = EV_SHOTGUN_HIT;
			}
			else
			{
				event = EV_BULLET_HIT_LARGE;
			}

			if ( wp->weapDef->bRifleBullet )
				event = EV_SHOTGUN_HIT;
			else
				event = EV_BULLET_HIT_LARGE;

			/* New code start: Bullet impact control, firing through walls */
			if ( attacker->s.number < 64 )
				noBulletImpacts = customPlayerState[attacker->s.number].noBulletImpacts;
			else
				noBulletImpacts = qfalse;

			if ( attacker->s.number < 64 && customPlayerState[attacker->s.number].fireThroughWalls )
			{
				if ( !noBulletImpacts )
				{
					// Recreate bullet hit effect that would otherwise be missing with a player-only mask
					gentity_t *hitEffect;

					hitEffect = G_TempEntity(origin, event);
					hitEffect->s.eventParm = DirToByte(trace.normal) & 0xFF;
					hitEffect->s.surfType = (trace.surfaceFlags >> 20) & 0x1F;
				}
			}
			else if ( !noBulletImpacts )
			/* New code end */
			{
				tempEnt = G_TempEntity(origin, event);
				tempEnt->s.eventParm = DirToByte(trace.normal);
				tempEnt->s.scale = DirToByte(dir);

				if ( self->s.eType == ET_PLAYER_CORPSE && g_corpseHit->current.boolean ) // New: g_corpseHit dvar
					surfaceType = 7;
				else
					surfaceType = (trace.surfaceFlags & 0x1F00000) >> 20;

				tempEnt->s.surfType = surfaceType;
				tempEnt->s.otherEntityNum = weaponEnt->s.number;
			}
		}

		if ( (trace.contents & CONTENTS_GLASS) != 0 )
		{
			VectorSubtract(end, start, dir);
			Vec3Normalize(dir);
			scaleSquared = DotProduct(trace.normal, dir);

			if ( -scaleSquared < 0.125 )
				dirScale = 0.0;
			else
				dirScale = 0.25 / -scaleSquared;

			VectorMA(origin, dirScale, dir, start);
			custom_Bullet_Fire_Extended(inflictor, attacker, start, end, dmgScale, callCount + 1, wp, weaponEnt, gameTime);
		}
		else if ( self->takedamage )
		{
			if ( self != attacker )
			{
				VectorSubtract(start, origin, temp);
				dist = VectorLength(temp);
				damage = (int)((float)Bullet_CalcDamageRange(wp, dist) * dmgScale);

				G_Damage(self, attacker, attacker, wp->forward, origin, damage, dflags, meansOfDeath, (hitLocation_t)trace.partGroup, level.time - gameTime);

				if ( self->client )
				{
					if ( (dflags & 0x20) != 0 && ( Dvar_GetInt("scr_friendlyfire") || !OnSameTeam(self, attacker) ) )
					{
						scale = dmgScale * 0.5;
						custom_Bullet_Fire_Extended(self, attacker, origin, end, scale, callCount + 1, wp, weaponEnt, gameTime);
					}
				}
			}
		}
	}
	else
	{
		Com_DPrintf("Bullet_Fire_Extended: Too many resursions, bullet aborted\n");
	}
}

qboolean custom_Bullet_Fire_Drop(droppingBullet_t *bullet, const gentity_t *inflictor, gentity_t *attacker, float *start, float *end, float dmgScale, const weaponParms *wp, const gentity_t *weaponEnt)
{
	int surfaceType;
	int event;
	float dot;
	vec3_t dir;
	vec3_t origin;
	meansOfDeath_t meansOfDeath;
	int dflags;
	int damage;
	float dist;
	vec3_t temp;
	gentity_t *self;
	gentity_t *tempEnt;
	trace_t trace;
	int contentMask;
	uint8_t *priorityMap;
	qboolean noBulletImpacts;
	float scaleSquared;
	float dirScale;

	dflags = 0;

	if ( wp->weapDef->bRifleBullet )
	{
		meansOfDeath = MOD_RIFLE_BULLET;
		dflags = 32;
	}
	else
	{
		meansOfDeath = MOD_PISTOL_BULLET;
	}

	if ( wp->weapDef->armorPiercing )
		dflags |= 2u;

	if ( wp->weapDef->bRifleBullet )
		priorityMap = &riflePriorityMap;
	else
		priorityMap = &bulletPriorityMap;

	contentMask = MASK_SHOT;

	if ( bullet->lastHitEnt )
		G_LocationalTrace(&trace, start, end, bullet->lastHitEnt->s.number, contentMask, priorityMap);
	else
		G_LocationalTrace(&trace, start, end, inflictor->s.number, contentMask, priorityMap);
	Vec3Lerp(start, end, trace.fraction, origin);
	custom_G_CheckHitTriggerDamage(attacker, start, origin, wp->weapDef->damage, meansOfDeath);
	self = &g_entities[trace.entityNum];
	VectorSubtract(end, start, dir);
	Vec3Normalize(dir);

	// Update bullet trajectory values
	bullet->distance += Vec3Distance(bullet->position, origin);
	VectorCopy(origin, bullet->position);
	VectorCopy(dir, bullet->direction);
	bullet->dmgScale = dmgScale;

	// Apply (linear) bullet drag
	bullet->velocity *= 1.0 - ((1.0 - bullet->drag) * trace.fraction);

	dot = DotProduct(dir, trace.normal) * -2.0;
	VectorMA(dir, dot, trace.normal, dir);

	// Bullet holes
	if ( (trace.surfaceFlags & SURF_SKY) == 0 && !self->client && trace.fraction < 1.0 )
	{
		if ( wp->weapDef->weapClass == WEAPCLASS_SPREAD )
		{
			event = EV_BULLET_HIT_SMALL;
		}
		else if ( wp->weapDef->bRifleBullet )
		{
			event = EV_SHOTGUN_HIT;
		}
		else
		{
			event = EV_BULLET_HIT_LARGE;
		}

		if ( wp->weapDef->bRifleBullet )
			event = EV_SHOTGUN_HIT;
		else
			event = EV_BULLET_HIT_LARGE;

		if ( attacker->s.number < 64 )
			noBulletImpacts = customPlayerState[attacker->s.number].noBulletImpacts;
		else
			noBulletImpacts = qfalse;

		if ( attacker->s.number < 64 && customPlayerState[attacker->s.number].fireThroughWalls )
		{
			if ( !noBulletImpacts )
			{
				// Recreate bullet hit effect that would otherwise be missing with a player-only mask
				gentity_t *hitEffect;

				hitEffect = G_TempEntity(origin, event);
				hitEffect->s.eventParm = DirToByte(trace.normal) & 0xFF;
				hitEffect->s.surfType = (trace.surfaceFlags >> 20) & 0x1F;
			}
		}
		else if ( !noBulletImpacts )
		{
			tempEnt = G_TempEntity(origin, event);
			tempEnt->s.eventParm = DirToByte(trace.normal);
			tempEnt->s.scale = DirToByte(dir);

			if ( self->s.eType == ET_PLAYER_CORPSE && g_corpseHit->current.boolean ) // New: g_corpseHit dvar
				surfaceType = 7;
			else
				surfaceType = (trace.surfaceFlags & 0x1F00000) >> 20;

			tempEnt->s.surfType = surfaceType;
			tempEnt->s.otherEntityNum = weaponEnt->s.number;
		}
	}

	// If glass was hit, keep the bullet alive
	if ( (trace.contents & CONTENTS_GLASS) != 0 )
	{
		VectorSubtract(end, start, dir);
		Vec3Normalize(dir);
		scaleSquared = DotProduct(trace.normal, dir);

		if ( -scaleSquared < 0.125 )
			dirScale = 0.0;
		else
			dirScale = 0.25 / -scaleSquared;

		VectorMA(origin, dirScale, dir, bullet->position);
		return qfalse;
	}
	else if ( self->takedamage )
	{
		// An entity that can take damage was hit
		if ( self != attacker )
		{
			VectorSubtract(start, origin, temp);
			dist = VectorLength(temp);
			damage = (int)((float)Bullet_CalcDamageRange(wp, dist) * dmgScale);

			// Player could execute some sort of team kill if they switch team while the bullet travels
			G_Damage(self, attacker, attacker, wp->forward, origin, damage, dflags, meansOfDeath, (hitLocation_t)trace.partGroup, bullet->timeOffset);

			// Save that entity reference so we can ignore it for subsequent traces
			bullet->lastHitEnt = self;

			if ( self->client )
			{
				if ( (dflags & 0x20) != 0 && ( Dvar_GetInt("scr_friendlyfire") || !OnSameTeam(self, attacker) ) )
				{
					bullet->dmgScale = dmgScale * 0.5;
					return qfalse;
				}
			}
		}
	}

	// Bullet did not hit anything
	if ( trace.fraction == 1.0 && trace.entityNum == ENTITY_NONE )
		return qfalse;

	return qtrue;
}

void custom_Bullet_Fire_Spread(const gentity_t *source, gentity_t *inflictor, const weaponParms *wp, int offset, float spread)
{
	int i;
	vec3_t start, end;

	/* New code start: per-player min. fire distance */
	int id = inflictor->client->ps.clientNum;
	float distance = wp->weapDef->fMinDamageRange * customPlayerState[id].fireRangeScale;
	/* New code end */

	VectorCopy(wp->muzzleTrace, start);
	for ( i = 0; i < wp->weapDef->shotCount; i++ )
	{
		Bullet_Endpos(spread, end, wp, distance);
		custom_Bullet_Fire_Extended(source, inflictor, start, end, 1.0, 0, wp, source, offset);
	}
}

void custom_Bullet_Fire_Drop_Think(int clientNum, droppingBullet_t *bullet)
{
	vec3_t end;

	custom_Bullet_Drop_Nextpos(end, bullet);
	if ( custom_Bullet_Fire_Drop(bullet, bullet->attacker, bullet->inflictor, bullet->position, end, bullet->dmgScale, &bullet->wp, bullet->attacker) )
	{
		if ( bullet->visualBullet )
			custom_Bullet_Drop_Finalize_Visual(bullet, qfalse);

		custom_Bullet_Drop_Free(clientNum, bullet);
	}
	else
	{
		if ( bullet->visualBullet )
			custom_Bullet_Drop_Update_Visual(bullet, end);
	}
}

void custom_Bullet_Fire_Drop_Think_AntiLag(int clientNum, droppingBullet_t *bullet)
{
	antilagClientStore antilagStore;
	vec3_t end;

	G_AntiLagRewindClientPos(level.time - bullet->timeOffset, &antilagStore);
	custom_Bullet_Drop_Nextpos(end, bullet);
	if ( custom_Bullet_Fire_Drop(bullet, bullet->attacker, bullet->inflictor, bullet->position, end, bullet->dmgScale, &bullet->wp, bullet->attacker) )
	{
		if ( bullet->visualBullet )
			custom_Bullet_Drop_Finalize_Visual(bullet, qfalse);

		custom_Bullet_Drop_Free(clientNum, bullet);
	}
	else
	{
		if ( bullet->visualBullet )
			custom_Bullet_Drop_Update_Visual(bullet, end);
	}
	G_AntiLag_RestoreClientPos(&antilagStore);
}

void custom_Bullet_Drop_Firstpos(float spread, float *end, const weaponParms *wp, float distance, droppingBullet_t *bullet)
{
	float timeDelta;
	float zDelta;

	Bullet_Endpos(spread, end, wp, distance);
	timeDelta = 1.0 / (float)sv_fps->current.integer;
	zDelta = 0.5 * (386.08858267717 * (timeDelta * timeDelta));
	// Do we want drag in gravity direction too?
	// A terminal falling velocity could be implemented here too
	//zDelta *= (1.0 - bullet->drag);
	bullet->zVelocity = bullet->zVelocity - (386.08858267717 * timeDelta);
	end[2] += zDelta;
}

void custom_Bullet_Drop_Nextpos(float *end, droppingBullet_t *bullet)
{
	float timeDelta;
	float zDelta;

	VectorMA(bullet->position, bullet->velocity, bullet->direction, end);
	timeDelta = 1.0 / (float)sv_fps->current.integer;
	zDelta = (bullet->zVelocity * timeDelta) + (0.5 * (386.08858267717 * (timeDelta * timeDelta)));
	// Do we want drag in gravity direction too?
	// A terminal falling velocity could be implemented here too
	//zDelta *= (1.0 - bullet->drag);
	bullet->zVelocity = bullet->zVelocity - (386.08858267717 * timeDelta);
	end[2] += zDelta;
}

void custom_Bullet_Drop_Free(int clientNum, droppingBullet_t *bullet)
{
	customPlayerState[clientNum].droppingBulletsCount--;
	Com_DPrintf("Bullet_Drop_Free: Done after %.2fs and %f units of distance, %d bullets still active\n", (float)(level.time - bullet->startTime)/1000, bullet->distance, customPlayerState[clientNum].droppingBulletsCount);
	memset(bullet, 0, sizeof(droppingBullet_t));
}

gentity_t * custom_Bullet_Drop_Create_Visual(int clientNum, droppingBullet_t *bullet, float *start, float *end)
{
	gentity_t *ent;
	vec3_t dir;
	vec3_t angles;

	ent = G_Spawn();
	Scr_SetString(&ent->classname, custom_scr_const.bullet);
	ent->r.contents = 0;
	ent->clipmask = 0;
	ent->s.eType = ET_GENERAL;
	ent->freeAfterEvent = 1;
	ent->eventTime = level.time + 50;
	G_SetModel(ent, G_ModelName(bullet->visualBulletModelIndex));
	G_DObjUpdate(ent);

	G_SetOrigin(ent, start);
	VectorSubtract(end, (ent->r).currentOrigin, dir);
	VecToAngles(dir, angles);
	G_SetAngle(ent, angles);

	(ent->s).pos.trType = TR_LINEAR;
	(ent->s).pos.trDuration = 50;
	(ent->s).pos.trTime = level.time - 50;
	VectorCopy((ent->r).currentOrigin, (ent->s).pos.trBase);
	VectorScale(dir, 20.0, ent->s.pos.trDelta);
	SV_LinkEntity(ent);

	return ent;
}

void custom_Bullet_Drop_Update_Visual(droppingBullet_t *bullet, float *end)
{
	gentity_t *ent = bullet->visualBullet;
	vec3_t angles;

	ent->eventTime += 50;

	VectorCopy(bullet->position, (ent->r).currentOrigin);
	VecToAngles(bullet->direction, angles);
	G_SetAngle(ent, angles);

	(ent->s).pos.trType = TR_LINEAR;
	(ent->s).pos.trDuration = 50;
	(ent->s).pos.trTime = level.time - 50;
	VectorCopy((ent->r).currentOrigin, (ent->s).pos.trBase);
	VectorScale(bullet->direction, 20.0, ent->s.pos.trDelta);
}

void custom_Bullet_Drop_Finalize_Visual(droppingBullet_t *bullet, qboolean remove)
{
	gentity_t *ent = bullet->visualBullet;
	vec3_t angles;

	G_SetOrigin(ent, bullet->position); // Sets TR_STATIONARY
	VecToAngles(bullet->direction, angles);
	G_SetAngle(ent, angles);

	// Keep bullet visuals visible for while after final hit?
	if ( bullet->visualTime > 0 && !remove )
		bullet->visualBullet->eventTime = level.time + bullet->visualTime;
	else
		G_FreeEntity(bullet->visualBullet);
}

void custom_Bullet_Fire(gentity_t *inflictor, float spread, weaponParms *wp, const gentity_t *source, int offset)
{
	antilagClientStore antilagStore;
	float distance;
	vec3_t end;
	int i;
	int id;

	G_AntiLagRewindClientPos(offset, &antilagStore);
	if ( wp->weapDef->weapClass == WEAPCLASS_SPREAD )
	{
		custom_Bullet_Fire_Spread(source, inflictor, wp, offset, spread);
	}
	else
	{
		id = inflictor->client->ps.clientNum;

		/* New code start: bullet drop */
		if ( g_bulletDrop->current.boolean && customPlayerState[id].droppingBulletsEnabled )
		{
			if ( customPlayerState[id].droppingBulletsCount >= MAX_DROPPING_BULLETS )
			{
				Com_DPrintf("Bullet_Fire: Too many bullets still on their way for player %d, bullet aborted\n", id);
				return;
			}

			droppingBullet_t bullet;
			memset(&bullet, 0, sizeof(droppingBullet_t));

			// Factor applied on velocity, each frame; could be added as a weaponDef field
			bullet.drag = customPlayerState[id].droppingBulletDrag;

			// Get next bullet position
			distance = customPlayerState[id].droppingBulletVelocity / (float)sv_fps->current.integer;
			if ( distance > 8192.0 ) // Clamp to common stock value
				distance = 8192.0;
			custom_Bullet_Drop_Firstpos(spread, end, wp, distance, &bullet);

			// Visualize bullet trajectory
			bullet.visualBullet = NULL;
			if ( customPlayerState[id].droppingBulletVisuals )
			{
				bullet.visualBulletModelIndex = customPlayerState[id].droppingBulletVisualModelIndex;
				bullet.visualBullet = custom_Bullet_Drop_Create_Visual(id, &bullet, wp->muzzleTrace, end);
				bullet.visualTime = customPlayerState[id].droppingBulletVisualTime;
			}

			// If bullet did not hit an obstacle, queue it for next server frame
			if ( !custom_Bullet_Fire_Drop(&bullet, source, inflictor, wp->muzzleTrace, end, 1.0, wp, source) )
			{
				for ( i = 0; i < MAX_DROPPING_BULLETS; i++ )
				{
					if ( !customPlayerState[id].droppingBullets[i].inUse )
					{
						bullet.attacker = source;
						bullet.distance = distance;
						bullet.inflictor = inflictor;
						bullet.inUse = qtrue;
						bullet.startTime = level.time;
						bullet.timeOffset = level.time - offset;
						bullet.velocity = distance;
						bullet.weaponEnt = source;
						memcpy(&bullet.wp, wp, sizeof(weaponParms));
						memcpy(&customPlayerState[id].droppingBullets[i], &bullet, sizeof(droppingBullet_t));
						customPlayerState[id].droppingBulletsCount++;
						break;
					}
				}
			} else {
				if ( bullet.visualBullet )
					custom_Bullet_Drop_Finalize_Visual(&bullet, qfalse);
			}
		}
		/* New code end */
		else
		{
			/* New code start: per-player min. fire distance */
			distance = 8192.0 * customPlayerState[id].fireRangeScale;
			/* New code end */

			Bullet_Endpos(spread, end, wp, distance);
			custom_Bullet_Fire_Extended(source, inflictor, wp->muzzleTrace, end, 1.0, 0, wp, source, offset);
		}
	}
	G_AntiLag_RestoreClientPos(&antilagStore);
}

void custom_Fire_Lead(gentity_t *ent, gentity_t *activator)
{
	gentity_t *attacker;
	weaponParms wp;
	float spread;

	spread = ent->pTurretInfo->playerSpread;
	if ( activator == g_entities + ENTITY_NONE )
	{
		attacker = g_entities + ENTITY_WORLD;
	}
	else
	{
		attacker = activator;

		/* New code start: per-player turret spread scale */
		if ( attacker->client )
			spread = ent->pTurretInfo->playerSpread * customPlayerState[attacker->client->ps.clientNum].turretSpreadScale;
		/* New code end */
	}

	Turret_FillWeaponParms(ent, attacker, &wp);
	wp.weapDef = BG_GetWeaponDef((ent->s).weapon);
	if ( (wp.weapDef)->weapType == WEAPTYPE_BULLET )
		custom_Bullet_Fire(attacker, spread, &wp, ent, level.time);
	else
		Weapon_RocketLauncher_Fire(ent, 0.0, &wp);

	G_AddEvent(ent, EV_FIRE_WEAPON_MG42, (attacker->s).number);
}

void custom_FireWeaponAntiLag(gentity_t *player, int time)
{
	float maxSpread;
	float minSpread;
	float spread;
	float currentAimSpreadScale;
	weaponParms wp;

	if ( ((player->client->ps).eFlags & EF_TURRET_STAND) == 0 || player->active == 0 )
	{
		wp.weapDef = BG_GetWeaponDef((player->s).weapon);
		G_CalcMuzzlePoints(player, &wp);
		currentAimSpreadScale = player->client->currentAimSpreadScale;
		BG_GetSpreadForWeapon(&player->client->ps, (player->s).weapon, &minSpread, &maxSpread);
		if ( (player->client->ps).fWeaponPosFrac == 1.0 )
		{
			spread = (wp.weapDef)->accuracy + (maxSpread - (wp.weapDef)->accuracy) * currentAimSpreadScale;
		}
		else
		{
			spread = minSpread + (maxSpread - minSpread) * currentAimSpreadScale;
		}

		/* New code start: per-player weapon spread scale */
		spread *= customPlayerState[player->client->ps.clientNum].weaponSpreadScale;
		/* New code end */

		if ( (wp.weapDef)->weapType == WEAPTYPE_BULLET )
		{
			custom_Bullet_Fire(player, spread, &wp, player, time);
		}
		else if ( (wp.weapDef)->weapType == WEAPTYPE_GRENADE )
		{
			weapon_grenadelauncher_fire(player, (player->s).weapon, &wp);
		}
		else if ( (wp.weapDef)->weapType == WEAPTYPE_PROJECTILE )
		{
			Weapon_RocketLauncher_Fire(player, spread, &wp);
		}
		else
		{
			Com_Error(ERR_DROP, "\x15Unknown weapon type %i for %s\n", (wp.weapDef)->weapType, (wp.weapDef)->szInternalName);
		}
	}
}

char openLogfileName[MAX_OSPATH];
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

	if ( logfileRotate->current.integer > 0 )
	{
		char logfilePath[512];
		char newLogfilePath[512];
		int maxFileIndex = logfileRotate->current.integer;
		int fileIndex = maxFileIndex;

		// Check existance of older log files
		while ( fileIndex > 0 )
		{
			snprintf(logfilePath, sizeof(logfilePath), "%s/%s/%s.%d", fs_homepath->current.string, fs_game->current.string, logfileName->current.string, fileIndex);
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
					snprintf(newLogfilePath, sizeof(newLogfilePath), "%s/%s/%s.%d", fs_homepath->current.string, fs_game->current.string, logfileName->current.string, fileIndex + 1);
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
		snprintf(logfilePath, sizeof(logfilePath), "%s/%s/%s", fs_homepath->current.string, fs_game->current.string, logfileName->current.string);
		if ( access(logfilePath, F_OK) != -1 )
		{
			snprintf(newLogfilePath, sizeof(newLogfilePath), "%s/%s/%s.%d", fs_homepath->current.string, fs_game->current.string, logfileName->current.string, fileIndex + 1);
			if ( rename(logfilePath, newLogfilePath) != 0 )
				printf("Warning: Failed to rotate existing log file '%s'\n", logfileName->current.string);
		}
	}

	logfile = FS_FOpenFileWrite(logfileName->current.string);
	I_strncpyz(openLogfileName, logfileName->current.string, MAX_OSPATH);

	opening_qconsole = 0;

	Com_Printf("logfile '%s' opened on %s\n", openLogfileName, strippedTime);
}

void custom_Com_PrintMessage(int /* print_msg_type_t */ channel, char *message)
{
	PbCaptureConsoleOutput(message, 0x1000);
	if ( rd_buffer == NULL )
	{
		/* Caret patch: Removed the following code:
		if ( *message == '^' && message[1] != '\0' )
		{
			message += 2;
		}
		*/
		if ( channel != 4 )
		{
			Sys_EnterCriticalSection(CRITSECT_PRINT);
			Sys_Print(message);
			Sys_LeaveCriticalSection(CRITSECT_PRINT);
		}

		if ( com_logfile != NULL && com_logfile->current.integer != 0 )
		{
			Sys_EnterCriticalSection(CRITSECT_CONSOLE);
			if ( FS_Initialized() )
			{
				if ( logfile == 0 && opening_qconsole == 0 && logfileName->current.string )
				{
					openLogfile(qfalse);
				}
				else if ( logfile && !opening_qconsole && logfileName->current.string && strncmp(logfileName->current.string, openLogfileName, strlen(openLogfileName)) )
				{
					// logfileName dvar value changed since log file initialization
					openLogfile(qtrue);
				}
				if ( logfile != 0 )
				{
					if ( logTimestamps->current.boolean && strlen(message) && strcmp(message, " ") != 0 )
					{
						time_t timer;
						struct tm *timeInfo;
						const char *timeString;
						char timedmessage[0x2000];

						time(&timer);
						timeInfo = localtime(&timer);
						timeString = asctime(timeInfo);
						Com_sprintf(timedmessage, 0x2000, "%s%s", timeString, message);
						timedmessage[strlen(timeString) - 1] = ' ';
						FS_Write(timedmessage, strlen(timedmessage), logfile);
					}
					else
					{
						FS_Write(message, strlen(message), logfile);
					}
					if ( 1 < com_logfile->current.integer ) // "logfile" dvar
					{
						FS_Flush(logfile);
					}
				}
			}
			Sys_LeaveCriticalSection(CRITSECT_CONSOLE);
		}
	}
	else if ( channel != 4 )
	{
		// RCON output buffer
		Sys_EnterCriticalSection(CRITSECT_RD_BUFFER);
		if ( rd_buffersize - 1U < ( strlen(rd_buffer) + strlen(message) ) )
		{
			rd_flush(rd_buffer);
			*rd_buffer = '\0';
		}
		I_strncat(rd_buffer, rd_buffersize, message);
		Sys_LeaveCriticalSection(CRITSECT_RD_BUFFER);
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

qboolean G_BounceGravityModel(gentity_t *ent, trace_t *trace) // G_BounceMissile as base
{
	int contents;
	double length;
	qboolean bounce;
	vec3_t angle;
	vec3_t planeNormal;
	vec3_t velocity;
	float dot;

	contents = SV_PointContents((ent->r).currentOrigin, -1, CONTENTS_WATER);
	BG_EvaluateTrajectoryDelta(&(ent->s).pos, 50 + (int)((float)(level.time - level.previousTime) * trace->fraction) + level.previousTime, velocity);
	dot = DotProduct(velocity, trace->normal);
	VectorMA(velocity, dot * -2.0, trace->normal, (ent->s).pos.trDelta);
	if ( 0.7 < trace->normal[2] )
	{
		(ent->s).groundEntityNum = trace->entityNum;
	}
	if ( ( (ent->s).eFlags & EF_BOUNCE ) != 0 )
	{
		length = VectorLength(velocity);
		if ( 0.0 < length && ( dot <= 0.0 ) )
		{
			VectorScale((ent->s).pos.trDelta, ( customEntityState[(ent->s).number].perpendicularBounce - customEntityState[(ent->s).number].parallelBounce ) * ( dot / -length ) + customEntityState[(ent->s).number].parallelBounce, (ent->s).pos.trDelta);
			if ( customEntityState[(ent->s).number].maxVelocity > 0.0 )
				VectorClampLength((ent->s).pos.trDelta, customEntityState[(ent->s).number].maxVelocity);
		}

		if ( 0.7 < trace->normal[2] && VectorLength((ent->s).pos.trDelta) < 20.0 )
		{
			if ( Scr_IsSystemActive() )
			{
				Scr_AddEntity(&g_entities[trace->entityNum]);
				Scr_AddString(Com_SurfaceTypeToName((int)(trace->surfaceFlags & 0x1F00000U) >> 0x14));
				Scr_Notify(ent, custom_scr_const.land, 2);
			}
			G_SetOrigin(ent, (ent->r).currentOrigin);
			G_MissileLandAngles(ent, trace, angle, 1);
			G_SetAngle(ent, angle);
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

void G_RunGravityModelWithBounce(gentity_t *ent) // G_RunMissile as base
{
	double absDeltaZ;
	vec3_t lerpOrigin;
	trace_t trace2;
	trace_t trace;
	vec3_t origin;
	vec3_t maxLerpVector;
	qboolean bounce;

	if ( ( (ent->s).pos.trType == TR_STATIONARY ) && ( (ent->s).groundEntityNum != ENTITY_WORLD ) )
	{
		VectorCopy((ent->r).currentOrigin, origin);
		origin[2] = origin[2] - 1.5;
		G_MissileTrace(&trace, (ent->r).currentOrigin, origin, (ent->s).number, ent->clipmask);
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

	if ( customEntityState[(ent->s).number].maxVelocity > 0.0 )
	{
		VectorSubtract(origin, (ent->r).currentOrigin, maxLerpVector);
		VectorClampLength(maxLerpVector, customEntityState[(ent->s).number].maxVelocity / sv_fps->current.integer);
		VectorAdd((ent->r).currentOrigin, maxLerpVector, origin);
	}

	absDeltaZ = (ent->s).pos.trDelta[2];
	if ( absDeltaZ < 0 )
		absDeltaZ *= -1;
	if ( ( absDeltaZ <= 30.0 ) || SV_PointContents((ent->r).currentOrigin, -1, CONTENTS_WATER) )
	{
		G_MissileTrace(&trace, (ent->r).currentOrigin, origin, (ent->s).number, ent->clipmask);
	}
	else
	{
		G_MissileTrace(&trace, (ent->r).currentOrigin, origin, (ent->s).number, ent->clipmask | CONTENTS_WATER);
	}
	if ( ( trace.surfaceFlags & 0x1F00000 ) == SURF_WATER )
	{
		G_MissileTrace(&trace, (ent->r).currentOrigin, origin, (ent->s).number, ent->clipmask);
	}
	if ( ( g_entities[trace.entityNum].flags & EF_TAGCONNECT ) != 0 )
	{
		Missile_TraceNoContents(&trace, trace.entityNum, ent, origin);
	}
	Vec3Lerp((ent->r).currentOrigin, origin, trace.fraction, lerpOrigin);
	VectorCopy(lerpOrigin, (ent->r).currentOrigin);
	if ( ( ( (ent->s).eFlags & EF_BOUNCE ) != 0 ) && ( trace.fraction == 1.0 || ( trace.fraction < 1.0 && ( 0.7 < trace.normal[2] ) ) ) )
	{
		VectorCopy((ent->r).currentOrigin, origin);
		origin[2] = origin[2] - 1.5;
		G_MissileTrace(&trace2, (ent->r).currentOrigin, origin, (ent->s).number, ent->clipmask);
		if ( ( trace2.fraction != 1.0 ) && ( trace2.entityNum == ENTITY_WORLD ) )
		{
			trace.fraction = trace2.fraction;
			trace.normal[0] = trace2.normal[0];
			trace.normal[1] = trace2.normal[1];
			trace.normal[2] = trace2.normal[2];
			trace.surfaceFlags = trace2.surfaceFlags;
			trace.contents = trace2.contents;
			trace.material = trace2.material;
			trace.entityNum = trace2.entityNum;
			trace.partName = trace2.partName;
			trace.partGroup = trace2.partGroup;
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
		bounce = G_BounceGravityModel(ent, &trace);
		if ( bounce && trace.startsolid == 0 && Scr_IsSystemActive() )
		{
			Scr_AddEntity(&g_entities[trace.entityNum]);
			Scr_AddString(Com_SurfaceTypeToName((int)(trace.surfaceFlags & 0x1F00000U) >> 0x14));
			Scr_Notify(ent, custom_scr_const.bounce, 2);
		}
	}
}

void G_RunGravityModelNoBounce(gentity_t *ent) // G_RunItem as base
{
	vec3_t subOrigin;
	vec3_t lerpOrigin;
	trace_t trace;
	vec3_t origin;
	vec3_t maxLerpVector;

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

		if ( customEntityState[(ent->s).number].maxVelocity > 0.0 )
		{
			VectorSubtract(origin, (ent->r).currentOrigin, maxLerpVector);
			VectorClampLength(maxLerpVector, customEntityState[(ent->s).number].maxVelocity / sv_fps->current.integer);
			VectorAdd((ent->r).currentOrigin, maxLerpVector, origin);
		}

		if ( Vec3DistanceSq((ent->r).currentOrigin, origin) < 0.1 )
		{
			origin[2] = origin[2] - 1.0;
		}

		if ( customEntityState[(ent->s).number].collideModels )
			SV_Trace(&trace, (ent->r).currentOrigin, (ent->r).mins, (ent->r).maxs, origin, (ent->s).number, ent->clipmask, 1, NULL, 1);
		else
			SV_Trace(&trace, (ent->r).currentOrigin, (ent->r).mins, (ent->r).maxs, origin, (ent->s).number, ent->clipmask, 0, NULL, 1);

		if ( trace.fraction < 1.0 )
		{
			Vec3Lerp((ent->r).currentOrigin, origin, trace.fraction, lerpOrigin);
			if ( ( trace.startsolid == 0 && ( trace.fraction < 0.01 ) ) && ( trace.normal[2] < 0.5 ) )
			{
				VectorSubtract(origin, (ent->r).currentOrigin, subOrigin);
				VectorMA(origin, 1 - DotProduct(subOrigin, trace.normal), trace.normal, origin);
				if ( customEntityState[(ent->s).number].collideModels )
					SV_Trace(&trace, (ent->r).currentOrigin, (ent->r).mins, (ent->r).maxs, origin, (ent->s).number, ent->clipmask, 1, NULL, 1);
				else
					SV_Trace(&trace, (ent->r).currentOrigin, (ent->r).mins, (ent->r).maxs, origin, (ent->s).number, ent->clipmask, 0, NULL, 1);
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
		if ( (ent->r).inuse != 0 && ( trace.fraction < 0.01 ) )
		{
			if ( trace.normal[2] > 0.0 )
			{
				if ( customEntityState[(ent->s).number].angledGravity )
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
					G_SetAngle(ent, angles);
				}
				G_SetOrigin(ent, lerpOrigin);
				if ( (ent->s).groundEntityNum != trace.entityNum && Scr_IsSystemActive() )
				{
					Scr_AddEntity(&g_entities[trace.entityNum]);
					Scr_AddString(Com_SurfaceTypeToName((int)(trace.surfaceFlags & 0x1F00000U) >> 0x14));
					Scr_Notify(ent, custom_scr_const.land, 2);
				}
				(ent->s).groundEntityNum = trace.entityNum;
				SV_LinkEntity(ent);
			}
		}
	}
}

void custom_G_RunFrameForEntity(gentity_t *ent)
{
	if ( ent->processedFrame != level.framenum )
	{
		ent->processedFrame = level.framenum;
		if ( ent->client == NULL )
		{
			if ( ( ent->flags & 0x800 ) == 0 )
			{
				(ent->s).eFlags = (ent->s).eFlags & 0xFFFFFFDF;
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
				else if ( (ent->s).eType == ET_PLAYER_CORPSE )
				{
					G_RunCorpse(ent);
				}
				/* New code start: Entity gravity */
				else if ( customEntityState[(ent->s).number].gravityType )
				{
					vec3_t oldOrigin;

					VectorCopy((ent->r).currentOrigin, oldOrigin);
					if ( customEntityState[(ent->s).number].gravityType == GRAVITY_NO_BOUNCE )
						G_RunGravityModelNoBounce(ent);
					else if ( customEntityState[(ent->s).number].gravityType == GRAVITY_BOUNCE )
						G_RunGravityModelWithBounce(ent);
					VectorSubtract((ent->r).currentOrigin, oldOrigin, customEntityState[(ent->s).number].velocity);
					VectorScale(customEntityState[(ent->s).number].velocity, 20.0, customEntityState[(ent->s).number].velocity);
				}
				/* New code end */
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
	s = MSG_ReadCommandString(msg);
	if ( seq <= cl->lastClientCommand )
	{
		return qtrue;
	}
	if ( sv_showCommands->current.boolean )
	{
		Com_Printf("clientCommand: %i : %s\n", seq, s);
	}
	if ( seq <= cl->lastClientCommand + 1 )
	{
		// New: Added userinfo
		if ( !I_strncmp("team ", s, 5) || !I_strncmp("score ", s, 6) || !I_strncmp("mr ", s, 3) || !I_strncmp("userinfo ", s, 9) )
		{
			floodprotect = false;
		}
		if ( ( ( *(legacyHacks + 4) == 0 ) && ( CS_PRIMED < cl->state ) ) && ( sv_floodProtect->current.boolean && ( svs.time < cl->nextReliableTime && floodprotect ) ) )
		{
			clientOk = 0;
			Com_DPrintf("client text ignored for %s: %s\n", cl->name, Cmd_Argv(0));
		}
		if ( floodprotect )
		{
			cl->nextReliableTime = svs.time + 800;
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

void custom_G_CheckHitTriggerDamage(gentity_t *pActivator, float *vStart, float *vEnd, int iDamage, meansOfDeath_t iMOD)
{
	// Function has stock logic
	gentity_t *ent;
	vec3_t maxs;
	vec3_t mins;
	int entityList[1026];
	int count;
	int i;
	int entNum;

	VectorCopy(vStart, mins);
	VectorCopy(vStart, maxs);
	AddPointToBounds(vEnd, mins, maxs);
	count = CM_AreaEntities(mins, maxs, entityList, MAX_GENTITIES, CONTENTS_DONOTENTER_LARGE);
	for ( i = 0; i < count; i++)
	{
		entNum = entityList[i];
		ent = g_entities + entNum;
		if ( g_entities[entNum].classname == scr_const.trigger_damage )
		{
			if ( SV_SightTraceToEntity(vStart, vec3_origin, vec3_origin, vEnd, (ent->s).number, -1) )
			{
				Scr_AddEntity(pActivator);
				Scr_AddInt(iDamage);
				Scr_Notify(ent, scr_const.damage, 2);
				Activate_trigger_damage(ent, pActivator, iDamage, iMOD);
				if ( g_entities[entNum].trigger.accumulate == 0 )
				{
					g_entities[entNum].health = 32000;
				}
			}
		}
	}
}

void custom_G_GrenadeTouchTriggerDamage(gentity_t *pActivator, float *vStart, float *vEnd, int iDamage, meansOfDeath_t iMOD)
{
	// Function has stock logic
	gentity_t *ent;
	vec3_t maxs;
	vec3_t mins;
	int entityList[1026];
	int count;
	int i;
	int entNum;

	VectorCopy(vStart, mins);
	VectorCopy(vStart, maxs);
	AddPointToBounds(vEnd, mins, maxs);
	count = CM_AreaEntities(mins, maxs, entityList, MAX_GENTITIES, CONTENTS_DONOTENTER_LARGE);
	for ( i = 0; i < count; i++ )
	{
		entNum = entityList[i];
		ent = g_entities + entNum;
		if ( g_entities[entNum].classname == scr_const.trigger_damage && (g_entities[entNum].flags & FL_GRENADE_TOUCH_DAMAGE) != 0 )
		{
			if ( SV_SightTraceToEntity(vStart, vec3_origin, vec3_origin, vEnd, (ent->s).number, -1) )
			{
				Scr_AddEntity(pActivator);
				Scr_AddInt(iDamage);
				Scr_Notify(ent, scr_const.damage, 2);
				Activate_trigger_damage(ent, pActivator, iDamage, iMOD);
				if ( g_entities[entNum].trigger.accumulate == 0 )
				{
					g_entities[entNum].health = 32000;
				}
			}
		}
	}
}

void custom_SV_LinkEntity(gentity_t *gEnt)
{
	// Function has stock logic
	svEntity_t *ent;
	vec_t *angles;
	vec_t *origin;
	clipHandle_t clip;
	DObj *dobj;
	double radius;
	int i;
	vec3_t max;
	vec3_t min;
	int lastLeaf;
	int c;
	int b;
	int a;
	int num_leafs;
	int cluster;
	int leafs[128];

	ent = SV_SvEntityForGentity(gEnt);
	if ( !(gEnt->r).bmodel )
	{
		if ( ((gEnt->r).contents & ( CONTENTS_SOLID | CONTENTS_BODY )) == 0 )
		{
			(gEnt->s).solid = 0;
		}
		else
		{
			a = (int)((gEnt->r).maxs[0]);
			if ( a < 1 )
			{
				a = 1;
			}
			if ( 0xFF < a )
			{
				a = 0xFF;
			}
			b = (int)(1.0 - (gEnt->r).mins[2]);
			if ( b < 1 )
			{
				b = 1;
			}
			if ( 0xFF < b )
			{
				b = 0xFF;
			}
			c = (int)((gEnt->r).maxs[2] + 32.0);
			if ( c < 1 )
			{
				c = 1;
			}
			if ( 0xFF < c )
			{
				c = 0xFF;
			}
			(gEnt->s).solid = b << 8 | c << 0x10 | a;
		}
	}
	else
	{
		(gEnt->s).solid = 0xffffff;
	}
	angles = (gEnt->r).currentAngles;
	origin = (gEnt->r).currentOrigin;
	SnapAngles(angles);
	if ( !(gEnt->r).bmodel || ( *angles == 0.0 && (gEnt->r).currentAngles[1] == 0.0 && (gEnt->r).currentAngles[2] == 0.0 ) )
	{
		VectorAdd(origin, (gEnt->r).mins, (gEnt->r).absmin);
		VectorAdd(origin, (gEnt->r).maxs, (gEnt->r).absmax);
	}
	else if ( *angles == 0.0 && (gEnt->r).currentAngles[2] == 0.0 )
	{
		radius = RadiusFromBounds2((gEnt->r).mins, (gEnt->r).maxs);
		for ( i = 0; i < 2; i++ )
		{
			(gEnt->r).absmin[i] = origin[i] - (float)radius;
			(gEnt->r).absmax[i] = origin[i] + (float)radius;
		}
		(gEnt->r).absmin[2] = (gEnt->r).currentOrigin[2] + (gEnt->r).mins[2];
		(gEnt->r).absmax[2] = (gEnt->r).currentOrigin[2] + (gEnt->r).maxs[2];
	}
	else
	{
		radius = RadiusFromBounds((gEnt->r).mins, (gEnt->r).maxs);
		for ( i = 0; i < 3; i++ )
		{
			(gEnt->r).absmin[i] = origin[i] - (float)radius;
			(gEnt->r).absmax[i] = origin[i] + (float)radius;
		}
	}

	(gEnt->r).absmin[0] = (gEnt->r).absmin[0] - 1.0;
	(gEnt->r).absmin[1] = (gEnt->r).absmin[1] - 1.0;
	(gEnt->r).absmin[2] = (gEnt->r).absmin[2] - 1.0;
	(gEnt->r).absmax[0] = (gEnt->r).absmax[0] + 1.0;
	(gEnt->r).absmax[1] = (gEnt->r).absmax[1] + 1.0;
	(gEnt->r).absmax[2] = (gEnt->r).absmax[2] + 1.0;
	ent->numClusters = 0;
	ent->lastCluster = 0;
	if ( ((gEnt->r).svFlags & 0x19) == 0 )
	{
		num_leafs = CM_BoxLeafnums((gEnt->r).absmin, (gEnt->r).absmax, leafs, MAX_TOTAL_ENT_LEAFS, &lastLeaf);
		if ( num_leafs == 0 )
		{
			CM_UnlinkEntity(ent);
			return;
		}
		for ( i = 0; i < num_leafs; i++ )
		{
			cluster = CM_LeafCluster(leafs[i]);
			if ( cluster != -1 )
			{
				ent->clusternums[ent->numClusters++] = cluster;
				if ( ent->numClusters == MAX_ENT_CLUSTERS )
					break;
			}
		}
		if ( i != num_leafs )
			ent->lastCluster = CM_LeafCluster(lastLeaf);
	}
	(gEnt->r).linked = 1;
	if ( (gEnt->r).contents == 0 )
	{
		CM_UnlinkEntity(ent);
	}
	else
	{
		clip = SV_ClipHandleForEntity(gEnt);
		dobj = Com_GetServerDObj((gEnt->s).number);
		if ( dobj == NULL || ( ((gEnt->r).svFlags & 6) == 0 ) )
		{
			CM_LinkEntity(ent, (gEnt->r).absmin, (gEnt->r).absmax, clip);
		}
		else
		{
			if ( ((gEnt->r).svFlags & 2) == 0 )
			{
				DObjGetBounds(dobj, min, max);
				VectorAdd(origin, min, min);
				VectorAdd(origin, max, max);
			}
			else
			{
				VectorAdd(origin, actorLocationalMins, min);
				VectorAdd(origin, actorLocationalMaxs, max);
			}
			CM_LinkEntity(ent, min, max, clip);
		}
	}
}

void custom_CM_AreaEntities_r(unsigned int nodeIndex, areaParms_t *ap)
{
	// Function has stock logic, but different loop structure
	struct worldSector_s *node;
	gentity_t *gcheck;
	int en;
	unsigned int nextNodeIndex;
	int gnum;

	for ( node = &cm_world.sectors[nodeIndex]; node->contents.contentsEntities & ap->contentmask; node = &cm_world.sectors[nodeIndex] )
	{
		for ( en = node->contents.entities; en > 0; en = sv.svEntities[gnum].nextEntityInWorldSector )
		{
			gnum = en - 1;
			gcheck = SV_GentityNum(gnum);

			if ( gcheck->r.contents & ap->contentmask )
			{
				if ( gcheck->r.absmin[0] > ap->maxs[0]
				|| gcheck->r.absmin[1] > ap->maxs[1]
				|| gcheck->r.absmin[2] > ap->maxs[2]
				|| gcheck->r.absmax[0] < ap->mins[0]
				|| gcheck->r.absmax[1] < ap->mins[1]
				|| gcheck->r.absmax[2] < ap->mins[2] )
				{
					continue;
				}
				if ( ap->count == ap->maxcount )
				{
					Com_DPrintf("CM_AreaEntities: MAXCOUNT\n");
					return;
				}
				ap->list[ap->count] = gnum;
				++ap->count;
			}
		}

		if ( node->tree.dist >= ap->maxs[node->tree.axis] )
		{
			nodeIndex = node->tree.child[1];
			if ( node->tree.dist <= ap->mins[node->tree.axis] )
			{
				return;
			}
		}
		else if ( node->tree.dist <= ap->mins[node->tree.axis] )
		{
			nodeIndex = node->tree.child[0];
		}
		else
		{
			nextNodeIndex = node->tree.child[1];
			custom_CM_AreaEntities_r(node->tree.child[0], ap);
			nodeIndex = nextNodeIndex;
		}
	}
}

int TriggerDamageEntities(int *entityList)
{
	int i;
	int count = 0;

	// Skip reserved entity slots: Players, player clones
	for ( i = 72; i < level.num_entities; i++ )
	{
		if ( g_entities[i].classname == scr_const.trigger_damage )
		{
			entityList[count++] = i;
		}
	}

	return count;
}

int TriggerTouchEntities(const float *mins, const float *maxs, int *entityList)
{
	int i;
	int count = 0;

	// Skip reserved entity slots: Players, player clones
	for ( i = 72; i < level.num_entities; i++ )
	{
		if ( entityHandlers[g_entities[i].handler].touch || SV_EntityContact(mins, maxs, &g_entities[i]) )
		{
			entityList[count++] = i;
		}
	}

	return count;
}

int custom_CM_AreaEntities(const float *mins, const float *maxs, int *entityList, int maxcount, int contentmask)
{
	areaParms_t ae;

	/* Known content masks on call:
	 0x00000020	G_BounceMissile
	 0x00000020	G_ExplodeMissile
	 0x00000020	G_RunMissile
	 0x00200000	Player_GetUseList
	 0x00400000	G_GrenadeTouchTriggerDamage
	 0x00400000	G_CheckHitTriggerDamage
	 0x02000000	GScr_positionWouldTelefrag
	 0x02000000	? indirection
	 0x02000180	G_MoverPush
	 0x405c0008	G_TouchTriggers
	 0x80000000	G_RunCorpseMove
	 0x80000000	G_RunItem
	 0xFFFFFFFF	G_RadiusDamage
	*/

	/* New code start: g_triggerMode dvar */
	if ( g_triggerMode->current.integer == 0 && ( contentmask == 0x400000 || contentmask == 0x405C0008 ) )
	{
		return 0;
	}
	else if ( g_triggerMode->current.integer == 2 && contentmask == 0x400000 )
	{
		return TriggerDamageEntities(entityList);
	}
	// Currently disabled for touch triggers as it can activate trigger_hurt
	// entities in unwanted situations:
	//else if ( g_triggerMode->current.integer == 2 && contentmask == 0x405C0008 )
	//{
	//	return TriggerTouchEntities(mins, maxs, entityList);
	//}
	/* New code end */

	ae.mins = mins;
	ae.maxs = maxs;
	ae.list = entityList;
	ae.count = 0;
	ae.maxcount = maxcount;
	ae.contentmask = contentmask;
	custom_CM_AreaEntities_r(1, &ae);
	return ae.count;
}

void PrintCallbackInfo(gentity_t *ent, int callbackHook, unsigned int numArgs)
{
	unsigned int i;

	for ( i = 0; i < sizeof(callbacks)/sizeof(callbacks[0]); i++ )
	{
		if ( *callbacks[i].pos == callbackHook )
		{
			if ( !ent )
				Com_Printf("Calling %s with %d argument(s)\n", callbacks[i].name, numArgs);
			else
				Com_Printf("Calling %s with %d argument(s) on entity %d\n", callbacks[i].name, numArgs, ent - g_entities);
			return;
		}
	}
	if ( callbackHook == g_scr_data.gametype.main )
		Com_Printf("Calling gametype::main\n");
	else if ( callbackHook == g_scr_data.levelscript )
		Com_Printf("Calling map::main\n");
	else if ( callbackHook == g_scr_data.delete_ )
		Com_Printf("Calling codescripts/delete::main\n");
	else if ( callbackHook == g_scr_data.initstructs )
		Com_Printf("Calling codescripts/struct::initstructs\n");
	else if ( callbackHook == g_scr_data.createstruct )
		Com_Printf("Calling codescripts/struct::createstruct\n");
	else
		Com_Printf("Calling unknown callback @ 0x%x with %d argument(s)\n", callbackHook, numArgs);
}

short custom_Scr_ExecEntThread(gentity_t *ent, int callbackHook, unsigned int numArgs)
{
	if ( g_debugCallbacks->current.boolean )
		PrintCallbackInfo(ent, callbackHook, numArgs);

	hook_scr_execentthread->unhook();
	short (*Scr_ExecEntThread)(gentity_t *ent, int callbackHook, unsigned int numArgs);
	*(int *)&Scr_ExecEntThread = hook_scr_execentthread->from;
	short ret = Scr_ExecEntThread(ent, callbackHook, numArgs);
	hook_scr_execentthread->hook();

	return ret;
}

short custom_Scr_ExecThread(int callbackHook, unsigned int numArgs)
{
	if ( g_debugCallbacks->current.boolean )
		PrintCallbackInfo(NULL, callbackHook, numArgs);

	hook_scr_execthread->unhook();
	short (*Scr_ExecThread)(int callbackHook, unsigned int numArgs);
	*(int *)&Scr_ExecThread = hook_scr_execthread->from;
	short ret = Scr_ExecThread(callbackHook, numArgs);
	hook_scr_execthread->hook();

	return ret;
}

void ServerCrash(int sig)
{
	int fd;
	FILE *fp;
	void *array[20];
	size_t size = backtrace(array, 20);

	// Write to crash log ...
	fp = fopen("./crash.log", "a");
	if ( fp )
	{
		fd = fileno(fp);
		fseek(fp, 0, SEEK_END);
		fprintf(fp, "Error: Server crashed with signal 0x%x {%d}\n", sig, sig);
		fflush(fp);
		backtrace_symbols_fd(array, size, fd);
	}
	// ... and stderr
	fprintf(stderr, "Error: Server crashed with signal 0x%x {%d}\n", sig, sig);
	backtrace_symbols_fd(array, size, STDERR_FILENO);
	exit(1);
}

void custom_PlayerCmd_DeactivateReverb(scr_entref_t entref)
{
	unsigned short priorityString;
	int paramNum;
	float fadetime;
	int priority;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
	}
	else if ( !g_entities[entref.entnum].client )
	{
		Scr_ObjectError(custom_va("entity %i is not a player", entref.entnum));
	}

	fadetime = 0.0;
	paramNum = Scr_GetNumParam();
	if ( paramNum != 1 )
	{
		if ( paramNum != 2 )
		{
			Scr_Error(
			"USAGE: player deactivateReverb(\"priority\", fadetime = 0);\nValid priorities are \"snd_enveffectsprio_level\" or \"snd_enveffectsprio_shellshock\", fadetime is the time spent fading t o the next lowest active reverb priority level in seconds\n"
			);
			return;
		}
		fadetime = Scr_GetFloat(1);
	}
	priorityString = Scr_GetConstString(0);
	priority = 1;
	if ( priorityString == scr_const.snd_enveffectsprio_level )
	{
		priority = 1;
	}
	else if ( priorityString == scr_const.snd_enveffectsprio_shellshock )
	{
		priority = 2;
	}
	else
	{
		Scr_Error("priority must be \'snd_enveffectsprio_level\' or \'snd_enveffectsprio_shellshock\'\n");
	}

	// New: Fixed command that is broken in original code
	SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, custom_va("%c %i %g", 68, priority, fadetime));
}

void custom_PlayerCmd_DeactivateChannelVolumes(scr_entref_t entref)
{
	unsigned short priorityString;
	int paramNum;
	float fadetime;
	int priority;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
	}
	else if ( !g_entities[entref.entnum].client )
	{
		Scr_ObjectError(custom_va("entity %i is not a player", entref.entnum));
	}

	fadetime = 0.0;
	paramNum = Scr_GetNumParam();
	if ( paramNum != 1 )
	{
		if ( paramNum != 2 )
		{
			Scr_Error(
			"USAGE: player deactivatechannelvolumes(\"priority\", fadetime = 0);\nValid priorities are \"snd_channelvolprio_holdbreath\", \"snd_channelvolprio_pain\", or \"snd_channelvolprio_shells hock\", fadetime is the time spent fading to the next lowest active reverb priority level in seconds\n"
			);
			return;
		}
		fadetime = Scr_GetFloat(1);
	}
	priorityString = Scr_GetConstString(0);
	priority = 1;
	if ( priorityString == scr_const.snd_channelvolprio_holdbreath )
	{
		priority = 1;
	}
	else if ( priorityString == scr_const.snd_channelvolprio_pain )
	{
		priority = 2;
	}
	else if ( priorityString == scr_const.snd_channelvolprio_shellshock )
	{
		priority = 3;
	}
	else
	{
		Scr_Error("priority must be \'snd_channelvolprio_holdbreath\', \'snd_channelvolprio_pain\', or \'snd_channelvolprio_shellshock\'\n");
	}

	// New: Fixed command that is broken in original code
	SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, custom_va("%c %i %g", 70, priority, fadetime));
}

void custom_Cmd_PrintEntities_f(void)
{
	// New: Omit request if cheats are disabled as this could lag servers with
	// low IOPS
	if ( sv_cheats->current.boolean )
		G_PrintEntities();
}

int custom_Cmd_FollowCycle_f(gentity_t *ent, int dir)
{
	clientState_t cstate;
	playerState_s pstate;
	int startClientNum;
	int clientNum;

	if ( dir != 1 && dir != -1 )
		Com_Error(ERR_DROP, "Cmd_FollowCycle_f: bad dir %i", dir);

	if ( ent->client->sess.sessionState != STATE_SPECTATOR )
		return 0;

	if ( ent->client->sess.forceSpectatorClient >= 0 )
		return 0;

	clientNum = ent->client->spectatorClient;

	if ( clientNum < 0 )
		clientNum = 0;

	startClientNum = clientNum;

	do
	{
		clientNum += dir;

		if ( clientNum >= level.maxclients )
			clientNum = 0;

		if ( clientNum < 0 )
			clientNum = level.maxclients - 1;

		if ( SV_GetArchivedClientInfo(clientNum, &ent->client->sess.archiveTime, &pstate, &cstate)
		        && G_ClientCanSpectateTeam(ent->client, (team_t)cstate.team) )
		{
			client_t *client = &svs.clients[clientNum];

			/* New code start: g_spectateBots dvar */
			if ( client->bIsTestClient && !g_spectateBots->current.boolean )
				continue;
			/* New code end */
			
			/* New code start: setAllowSpectators */
			if ( customPlayerState[clientNum].notAllowingSpectators )
				continue;
			/* New code end */

			ent->client->spectatorClient = clientNum;
			ent->client->sess.sessionState = STATE_SPECTATOR;
			return 1;
		}
	}
	while ( clientNum != startClientNum );

	return 0;
}

void custom_GScr_LogPrint(void)
{
	unsigned int len;
	int iStringLen;
	char string[1024];
	int iNumParms;
	int i;
	const char *pszToken;

	string[0] = 0;
	iStringLen = 0;
	iNumParms = Scr_GetNumParam();

	for ( i = 0; i < iNumParms; ++i )
	{
		pszToken = Scr_GetString(i);
		len = strlen(pszToken);

		if ( (int)(len + iStringLen) >= MAX_STRINGLENGTH )
			break;

		I_strncat(string, MAX_STRINGLENGTH, pszToken);
		iStringLen += len;
	}

	// New: Fixed potential format string crash
	G_LogPrintf("%s", string);
}

void custom_Huff_offsetReceive(node_t *node, int *ch, byte *fin, int readsize, int *offset)
{
	// New: Fixed a design flaw (OOB access), see https://github.com/callofduty4x/CoD4x_Server/pull/396
	// Related reference: https://github.com/ioquake/ioq3/commit/d2b1d124d4055c2fcbe5126863487c52fd58cca1
	bloc = *offset;
	while ( node && node->symbol == INTERNAL_NODE )
	{
		if ( bloc >= readsize )
		{
			*ch = 0;
			*offset = readsize + 1;
			return;
		}
		if ( !get_bit(fin) )
		{
			node = node->left;
		}
		else
		{
			node = node->right;
		}
	}
	if ( !node )
	{
		*ch = 0;
	}
	else
	{
		*ch = node->symbol;
		*offset = bloc;
	}
}

int custom_MSG_ReadBitsCompress(const byte* input, byte* outputBuf, int readsize, int outputBufSize)
{
	// New: Added readsize parameter to Huff_offsetReceive call
	// New: Added outputBufSize for overflow check (see custom_SV_ExecuteClientMessage)
	readsize = readsize * 8;
	byte *outptr = outputBuf;

	int get;
	int offset;
	int i;

	if ( readsize <= 0 )
	{
		return 0;
	}

	for ( offset = 0, i = 0; offset < readsize && i < outputBufSize; i++ )
	{
		custom_Huff_offsetReceive(msgHuff.decompressor.tree, &get, (byte*)input, readsize, &offset);
		*outptr = (byte)get;
		outptr++;
	}

	return i;
}

void custom_SV_ExecuteClientMessage(client_t *cl, msg_t *msg)
{
	byte *outputBuf;
	LargeLocal buf;
	msg_t decompressMsg;
	int c;
	
	LargeLocalConstructor(&buf, MAX_MSGLEN);
	outputBuf = LargeLocalGetBuf(&buf);
	MSG_Init(&decompressMsg, outputBuf, MAX_MSGLEN);

	// New: Fix for potential decompression exploit, see CVE-2018-10718
	// Note: This happens after packet defragmentation
	decompressMsg.cursize = custom_MSG_ReadBitsCompress(msg->data + msg->readcount, outputBuf, msg->cursize - msg->readcount, decompressMsg.maxsize);
	if ( decompressMsg.cursize == decompressMsg.maxsize )
	{
		SV_DropClient(cl, "SV_ExecuteClientMessage: Client sent oversize message");
		return;
	}

	// New: Fix for clients getting stuck in the file download process if the
	// server executes a map load while downloading is still in progress, with
	// sv_wwwDlDisconnected disabled
	// https://github.com/ioquake/ioq3/blob/1fc83e4845b53a9b259f5980468c8db15fce8de7/code/server/sv_client.c#L1941
	if ( ( cl->serverId == sv_serverId_value || cl->downloadName[0] )
		|| ( !cl->downloadName[0] && strstr(cl->lastClientCommandString, "nextdl") )
		|| cl->clientDownloadingWWW )
	{
		do {
			c = MSG_ReadBits(&decompressMsg, 3);
			if ( c == clc_EOF || c != clc_clientCommand )
			{
				if ( sv_pure->current.boolean && cl->pureAuthentic == 2 )
				{
					cl->nextSnapshotTime = -1;
					SV_DropClient(cl, "EXE_UNPURECLIENTDETECTED");
					cl->state = CS_ACTIVE;
					SV_SendClientSnapshot(cl);
					cl->state = CS_ZOMBIE;
				}
				if ( c == clc_move )
				{
					SV_UserMove(cl, &decompressMsg, 1);
				}
				else if ( c == clc_moveNoDelta )
				{
					SV_UserMove(cl, &decompressMsg, 0);
				}
				else if ( c != clc_EOF )
				{
					Com_Printf("WARNING: bad command byte %i for client %i\n", c, cl - svs.clients);
				}
				LargeLocalDestructor(&buf);
				return;
			}
			if ( !SV_ClientCommand(cl, &decompressMsg) )
			{
				LargeLocalDestructor(&buf);
				return;
			}
		} while ( cl->state != CS_ZOMBIE );
		LargeLocalDestructor(&buf);
	}
	else if ( (cl->serverId & 0xF0) == (sv_serverId_value & 0xF0) )
	{
		if ( cl->state == CS_PRIMED )
		{
			SV_ClientEnterWorld(cl, &cl->lastUsercmd);
		}
		LargeLocalDestructor(&buf);
	}
	else
	{
		if ( cl->gamestateMessageNum < cl->messageAcknowledge )
		{
			Com_DPrintf("%s : dropped gamestate, resending\n", cl->name);
			SV_SendClientGameState(cl);
		}
		LargeLocalDestructor(&buf);
	}
}

void custom_SV_ConnectionlessPacket(netadr_t from, msg_t *msg)
{
	char *s;
	const char *c;
	client_t *cl;
	int i;
	int clientNum;
	
	clientNum = -1;
	MSG_BeginReading(msg);
	MSG_ReadLong(msg);
	SV_Netchan_AddOOBProfilePacket(msg->cursize);
	if ( !I_strnicmp((const char *)msg->data + 4, "pb_", 3) )
	{
		cl = svs.clients;
		for ( i = 0; i < sv_maxclients->current.integer; i++, cl++ )
		{
			if ( cl->state != CS_FREE && NET_CompareBaseAdr(from, (cl->netchan).remoteAddress) && (cl->netchan).remoteAddress.port == from.port )
			{
				clientNum = i;
				break;
			}
		}
		if ( msg->data[7] != 0x43 && msg->data[7] != 0x31 && msg->data[7] != 0x4a )
		{
			PbSvAddEvent(0xd, clientNum, msg->cursize + -4, msg->data + 4);
		}
	}
	else
	{
		s = MSG_ReadStringLine(msg);
		SV_Cmd_TokenizeString(s);
		c = SV_Cmd_Argv(0);

		if ( sv_packet_info->current.boolean )
		{
			Com_Printf("SV packet %s : %s\n", NET_AdrToString(from), c);
		}

		if ( !I_stricmp(c, "getstatus") )
		{
			SV_UpdateLastTimeMasterServerCommunicated(from);
			// New: Replaced call to original SVC_Status
			custom_SVC_Status(from);
		}
		else if ( !I_stricmp(c, "getinfo") )
		{
			SV_UpdateLastTimeMasterServerCommunicated(from);
			// New: Replaced call to original SVC_Info
			custom_SVC_Info(from);
		}
		else if ( !I_stricmp(c, "getchallenge") )
		{
			SV_UpdateLastTimeMasterServerCommunicated(from);
			// New: Replaced call to original SV_GetChallenge
			custom_SV_GetChallenge(from);
		}
		else if ( !I_stricmp(c, "connect") )
		{
			if ( !NET_IsLocalAddress(from) )
			{
				PbPassConnectString(NET_AdrToString(from), msg->data);
			}
			else
			{
				PbPassConnectString("localhost", msg->data);
			}
			// New: Replaced call to original SV_DirectConnect
			custom_SV_DirectConnect(from);
		}
		else if ( !I_stricmp(c, "ipAuthorize") )
		{
			SV_UpdateLastTimeMasterServerCommunicated(from);
			// New: Replaced call to original SV_AuthorizeIpPacket
			custom_SV_AuthorizeIpPacket(from);
		}
		else if ( !I_stricmp(c, "rcon") )
		{
			// New: Replaced call to original SVC_RemoteCommand
			custom_SVC_RemoteCommand(from, msg, qfalse);
		}
		else if ( !I_stricmp(c , "v") )
		{
			SV_VoicePacket(from, msg);
		}
		else if ( !I_stricmp(c, "disconnect") )
		{
			// if a client starts up a local server, we may see some spurious
			// server disconnect messages when their new server sees our final
			// sequenced messages to the old client
		}
		else
		{
			// bad connectionless packet
		}
	}
}

void custom_SV_PacketEvent(netadr_t from, msg_t *msg)
{
	int qport;
	client_t *cl;
	int i;

	if ( msg->cursize < 4 || *(int *)msg->data != -1 )
	{
		SV_ResetSkeletonCache();
		MSG_BeginReading(msg);
		MSG_ReadLong(msg);
		qport = MSG_ReadShort(msg);
		cl = svs.clients;
		for ( i = 0; i < sv_maxclients->current.integer; i++, cl++ )
		{
			if ( cl->state != CS_FREE && NET_CompareBaseAdr(from, (cl->netchan).remoteAddress) && (cl->netchan).qport == (qport & 0xFFFF) )
			{
				if ( (cl->netchan).remoteAddress.port != from.port )
				{
					Com_Printf("SV_ReadPackets: fixing up a translated port\n");
					(cl->netchan).remoteAddress.port = from.port;
				}

				if ( !Netchan_Process(&cl->netchan, msg) )
				{
					return;
				}

				cl->serverId = MSG_ReadByte(msg);
				cl->messageAcknowledge = MSG_ReadLong(msg);
				if ( cl->messageAcknowledge < 0 )
				{
					Com_Printf("Invalid reliableAcknowledge message from %s - reliableAcknowledge is %i\n", cl->name, cl->reliableAcknowledge);
					return;
				}

				cl->reliableAcknowledge = MSG_ReadLong(msg);
				// New: Fix for invalid reliableAcknowledge exploit, see https://github.com/callofduty4x/CoD4x_Server/pull/407
				if ( ( cl->reliableSequence - cl->reliableAcknowledge ) > ( MAX_RELIABLE_COMMANDS - 1 ) || cl->reliableAcknowledge < 0 || ( cl->reliableSequence - cl->reliableAcknowledge ) < 0 )
				{
					Com_Printf("Out of range reliableAcknowledge message from %s - cl->reliableSequence is %i, reliableAcknowledge is %i\n",
					cl->name, cl->reliableSequence, cl->reliableAcknowledge);
					cl->reliableAcknowledge = cl->reliableSequence;
					return;
				}

				SV_Netchan_Decode(cl, msg->data + msg->readcount, msg->cursize - msg->readcount);

				if ( cl->state == CS_ZOMBIE )
				{
					return;
				}
				cl->lastPacketTime = svs.time;
				SV_ExecuteClientMessage(cl, msg);
				bgs = NULL;
				return;
			}
		}
		NET_OutOfBandPrint(NS_SERVER, from, "disconnect");
	}
	else
	{
		custom_SV_ConnectionlessPacket(from, msg);
	}
}

int custom_SV_CanReplaceServerCommand(client_t *client, const char *command)
{
	// New: Exploit fix, taken over from IW4x
	return -1;
}

void custom_SV_FreeConfigstrings(void)
{
	hook_sv_freeconfigstrings->unhook();
	SV_FreeConfigstrings();
	hook_sv_freeconfigstrings->hook();

	// We (re)register the dvar here so that any latched value is applied in time
	g_safePrecache = Dvar_RegisterBool("g_safePrecache", qfalse, DVAR_ARCHIVE | DVAR_LATCH | DVAR_CHANGEABLE_RESET);
	if ( g_safePrecache->current.boolean )
	{
		SV_SetConfigstring(847, "fx/misc/missing_fx.efx");
		SV_SetConfigstring(335, "xmodel/default_static_model");
		cached_models[1] = SV_XModelGet("xmodel/default_static_model");
	}
}

int custom_G_FindConfigstringIndex(const char *name, int start, int max, qboolean create, const char *fieldname)
{
	const char *s1;
	int i;

	if ( !name || !*name )
		return 0;

	for ( i = 1; i < max; ++i )
	{
		s1 = SV_GetConfigstringConst(start + i);

		if ( !*s1 )
			break;

		if ( !strcasecmp(s1, name) )
			return i;
	}

	if ( create )
	{
		if ( i == max )
		{
			// New: Added g_safePrecache dvar logic
			if ( !g_safePrecache->current.boolean )
			{
				Com_Error(ERR_DROP, custom_va("G_FindConfigstringIndex: overflow (%d): %s", start, name));
			}
			else
			{
				if ( fieldname && !strncmp(fieldname, "effect", 6) )
				{
					Com_Printf("Warning: Exceeded number of effect files, defaulting \"%s\"\n", name);
					return 1;
				}
				else
				{
					Com_Error(ERR_DROP, custom_va("G_FindConfigstringIndex: overflow (%d): %s", start, name));
				}
			}
		}

		SV_SetConfigstring(start + i, name);
		return i;
	}
	else
	{
		if ( fieldname )
		{
			Scr_Error(custom_va("%s \"%s\" not precached", fieldname, name));
		}

		return 0;
	}
}

unsigned int custom_G_ModelIndex(const char *name)
{
	const char *modelName;
	signed int i;

	if ( !*name )
		return 0;

	for ( i = 1; i < MAX_MODELS; ++i )
	{
		modelName = SV_GetConfigstringConst(i + 334);

		if ( !*modelName )
			break;

		if ( !strcasecmp(modelName, name) )
			return i;
	}

	if ( !level.initializing )
	{
		// New: Added g_safePrecache dvar logic
		if ( !g_safePrecache->current.boolean )
		{
			Scr_Error(custom_va("model '%s' not precached", name));
		}
		else
		{
			Com_Printf("Warning: Model '%s' not precached\n", name);
			Scr_CodeCallback_Error(qfalse, qfalse, "G_ModelIndex", custom_va("Model '%s' not precached", name));
			return 1;
		}
	}

	if ( i == MAX_MODELS )
	{
		// New: Added g_safePrecache dvar logic
		if ( !g_safePrecache->current.boolean )
		{
			Com_Error(ERR_DROP, "G_ModelIndex: overflow");
		}
		else
		{
			Com_Printf("Warning: Exceeded number of models, defaulting '%s'\n", name);
			return 1;
		}
	}

	cached_models[i] = SV_XModelGet(name);
	SV_SetConfigstring(i + 334, name);

	return i;
}

int custom_SV_GetClientPing(int clientNum)
{
	if ( customPlayerState[clientNum].overridePing )
		return customPlayerState[clientNum].ping;
	return svs.clients[clientNum].ping;
}

void custom_ClientScr_SetHeadIconTeam(gclient_t *pSelf, const game_client_field_t *pField)
{
	unsigned short str;
	gentity_t *player = &g_entities[pSelf - level.clients];

	str = Scr_GetConstString(0);

	/* No matter what is set here, spectators can always see all headicons.
	 This might be a bug in the client, especially since the server part was
	 broken too (spectator and invalid branch were flipped) */
	if ( scr_const.none == str )
	{
		player->s.iHeadIconTeam = TEAM_NONE;

		/* Reusing the scale field that is otherwise only used at trigger
		 entities. This way we have it archived, for correct killcam data.
		 At this field, only the first byte is synchronized between players */
		player->s.scale = CUSTOM_TEAM_NONE;
	}
	else if ( scr_const.allies == str )
	{
		player->s.iHeadIconTeam = TEAM_ALLIES;
		player->s.scale = CUSTOM_TEAM_NONE;
	}
	else if ( scr_const.axis == str )
	{
		player->s.iHeadIconTeam = TEAM_AXIS;
		player->s.scale = CUSTOM_TEAM_NONE;
	}
	else if ( scr_const.spectator == str )
	{
		player->s.iHeadIconTeam = TEAM_SPECTATOR;
		player->s.scale = CUSTOM_TEAM_NONE;
	}
	else if ( custom_scr_const.axis_allies == str )
	{
		player->s.iHeadIconTeam = TEAM_NONE;
		player->s.scale = CUSTOM_TEAM_AXIS_ALLIES;
	}
	else if ( custom_scr_const.all == str )
	{
		player->s.iHeadIconTeam = TEAM_NONE;
		player->s.scale = CUSTOM_TEAM_ALL;
	}
	else
	{
		Scr_Error(custom_va("'%s' is an illegal head icon team string. Must be none, allies, axis, or spectator.", SL_ConvertToString(str)));
	}
}

void custom_ClientScr_GetHeadIconTeam(gclient_t *pSelf, const game_client_field_t *pField)
{
	unsigned int str;
	gentity_t *player = &g_entities[pSelf - level.clients];

	switch ( player->s.iHeadIconTeam )
	{
	case 1:
		str = scr_const.axis;
		break;

	case 2:
		str = scr_const.allies;
		break;

	case 3:
		str = scr_const.spectator;
		break;

	default:
		if ( player->s.scale == CUSTOM_TEAM_AXIS_ALLIES )
			str = custom_scr_const.axis_allies;
		else if ( player->s.scale == CUSTOM_TEAM_ALL )
			str = custom_scr_const.all;
		else
			str = scr_const.none;
		break;
	}

	Scr_AddConstString(str);
}

void SetupBrushModelContents(int clientNum)
{
	for ( int i = 0; i < savedBrushModelContentsCount; i++ )
	{
		savedBrushModelContents_t *ref = &savedBrushModelContents[i];
		gentity_t *ent = &g_entities[ref->num];

		if ( customEntityState[ref->num].notSolidBrushModel )
		{
			if ( customEntityState[ref->num].clientMask[clientNum >> 5] & (1 << (clientNum & 0x1F)) )
				ent->r.contents = CONTENTS_NONE;
		}
	}
}

void RestoreBrushModelContents()
{
	for ( int i = 0; i < savedBrushModelContentsCount; i++ )
	{
		savedBrushModelContents_t *ref = &savedBrushModelContents[i];
		gentity_t *ent = &g_entities[ref->num];

		ent->r.contents = ref->contents;
	}
}

void custom_Pmove(pmove_t *pm)
{
	/* New code start: (not)SolidForPlayer */
	int id = pm->ps->clientNum;
	qboolean updateBrushModelContents = g_brushModelCollisionTweaks->current.boolean;

	if ( updateBrushModelContents )
		SetupBrushModelContents(id);
	/* New code end */

	hook_pmove->unhook();
	void (*Pmove)(pmove_t *pm);
	*(int *)&Pmove = hook_pmove->from;
	Pmove(pm);
	hook_pmove->hook();

	// New: (not)SolidForPlayer
	if ( updateBrushModelContents )
		RestoreBrushModelContents();
}

void hook_SetExpFog_density_typo(const char *string)
{
	// New: Exchanged "distance" with "density"
	Scr_Error("setExpFog: density must be greater than 0 and less than 1");
}

class cCallOfDuty2Pro
{
public:
	cCallOfDuty2Pro()
	{
		// Do not inherit lib of parent
		unsetenv("LD_PRELOAD");

		// Crash handlers for debugging
		signal(SIGSEGV, ServerCrash);
		signal(SIGABRT, ServerCrash);

		// Otherwise the printf()'s are printed at crash/end on older os/compiler versions
		setbuf(stdout, NULL);

		printf("> [LIBCOD] Compiled for Call of Duty 2\n");
		printf("> [LIBCOD] Compiled %s %s using GCC %s\n", __DATE__, __TIME__, __VERSION__);

		// Allow to write in executable memory
		mprotect((void *)0x08048000, 0x135000, PROT_READ | PROT_WRITE | PROT_EXEC);

		// Begin of hooking block
		cracking_hook_call(0x080622F9, (int)common_init_complete_print);
		cracking_hook_call(0x08090BA0, (int)hook_ClientCommand);
		cracking_hook_call(0x0808DB12, (int)hook_AuthorizeState);
		cracking_hook_call(0x0808BDC8, (int)hook_findMap);
		cracking_hook_call(0x08070BE7, (int)Scr_GetCustomFunction);
		cracking_hook_call(0x08070E0B, (int)Scr_GetCustomMethod);
		cracking_hook_call(0x08082346, (int)hook_RuntimeError_in_VM_Execute);
		cracking_hook_call(0x0808FD52, (int)hook_bad_printf);
		cracking_hook_call(0x080EBC58, (int)hook_findWeaponIndex);
		cracking_hook_call(0x08062644, (int)hitch_warning_print);
		cracking_hook_call(0x080AD1FE, (int)hook_Com_MakeSoundAliasesPermanent);
		cracking_hook_call(0x0811599A, (int)hook_SetExpFog_density_typo);

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

		hook_fire_grenade = new cHook(0x0810E68E, (int)custom_fire_grenade);
		hook_fire_grenade->hook();
		hook_touch_item_auto = new cHook(0x08105C80, int(custom_Touch_Item_Auto));
		hook_touch_item_auto->hook();
		hook_g_tempentity = new cHook(0x0811EFC4, (int)custom_G_TempEntity);
		hook_g_tempentity->hook();
		hook_gscr_loadconsts = new cHook(0x081224F8, (int)custom_GScr_LoadConsts);
		hook_gscr_loadconsts->hook();
		hook_sv_masterheartbeat = new cHook(0x08096ED6, (int)custom_SV_MasterHeartbeat);
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
		hook_playercmd_cloneplayer = new cHook(0x080FCC76, (int)custom_PlayerCmd_ClonePlayer);
		hook_playercmd_cloneplayer->hook();
		hook_pm_beginweaponchange = new cHook(0x080EDC30, (int)custom_PM_BeginWeaponChange);
		hook_pm_beginweaponchange->hook();
		hook_com_initdvars = new cHook(0x08061D90, (int)custom_Com_InitDvars);
		hook_com_initdvars->hook();
		hook_sv_verifyiwds_f = new cHook(0x08090534, int(custom_SV_VerifyIwds_f));
		hook_sv_verifyiwds_f->hook();
		hook_g_freeentity = new cHook(0x0811EE50, int(custom_G_FreeEntity));
		hook_g_freeentity->hook();
		hook_g_initgentity = new cHook(0x0811E85C, int(custom_G_InitGentity));
		hook_g_initgentity->hook();
		hook_scr_execentthread = new cHook(0x0811B284, int(custom_Scr_ExecEntThread));
		hook_scr_execentthread->hook();
		hook_scr_execthread = new cHook(0x08083FD6, int(custom_Scr_ExecThread));
		hook_scr_execthread->hook();
		hook_sv_freeconfigstrings = new cHook(0x080932B6, int(custom_SV_FreeConfigstrings));
		hook_sv_freeconfigstrings->hook();
		hook_sys_quit = new cHook(0x080D3A7A, int(custom_Sys_Quit));
		hook_sys_quit->hook();
		hook_fs_registerdvars = new cHook(0x080A2C3C, (int)custom_FS_RegisterDvars);
		hook_fs_registerdvars->hook();
		hook_pmove = new cHook(0x080E9464, (int)custom_Pmove);
		hook_pmove->hook();

		#if COMPILE_PLAYER == 1
		hook_play_movement = new cHook(0x08090DAC, (int)custom_SV_ClientThink);
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
		cracking_hook_function(0x080783FA, (int)custom_Scr_PrintPrevCodePos);
		cracking_hook_function(0x0809B016, (int)custom_SV_ArchiveSnapshot);
		cracking_hook_function(0x0809A408, (int)custom_SV_BuildClientSnapshot);
		cracking_hook_function(0x0811B770, (int)custom_G_SpawnEntitiesFromString);
		cracking_hook_function(0x080584F0, (int)custom_CM_IsBadStaticModel);
		cracking_hook_function(0x08113818, (int)custom_Scr_BulletTrace);
		cracking_hook_function(0x08113A4A, (int)custom_Scr_BulletTracePassed);
		cracking_hook_function(0x08113B0C, (int)custom_Scr_SightTracePassed);
		cracking_hook_function(0x08117022, (int)custom_GScr_KickPlayer);
		cracking_hook_function(0x08113128, (int)custom_GScr_Obituary);
		cracking_hook_function(0x081124F6, (int)custom_GScr_SetHintString);
		cracking_hook_function(0x08092302, (int)custom_SV_MapExists);
		cracking_hook_function(0x08109CE0, (int)custom_G_UpdateObjectiveToClients);
		cracking_hook_function(0x08120A70, (int)custom_FireWeaponMelee);
		cracking_hook_function(0x08120484, (int)custom_Bullet_Fire);
		cracking_hook_function(0x0811FE90, (int)custom_Bullet_Fire_Extended);
		cracking_hook_function(0x08121BC6, (int)custom_Player_UpdateCursorHints);
		cracking_hook_function(0x08060C20, (int)custom_Com_PrintMessage);
		cracking_hook_function(0x08109F5C, (int)custom_G_RunFrameForEntity);
		cracking_hook_function(0x08090BAC, (int)custom_SV_ClientCommand);
		cracking_hook_function(0x080F6E9E, (int)custom_StuckInClient);
		cracking_hook_function(0x080F5682, (int)custom_G_SetClientContents);
		cracking_hook_function(0x0809CD64, (int)custom_SV_ClipMoveToEntity);
		cracking_hook_function(0x08101B40, (int)custom_G_DamageClient);
		cracking_hook_function(0x0811CEA8, (int)custom_G_CheckHitTriggerDamage);
		cracking_hook_function(0x0811D096, (int)custom_G_GrenadeTouchTriggerDamage);
		cracking_hook_function(0x0809C63A, (int)custom_SV_LinkEntity);
		cracking_hook_function(0x0805E986, (int)custom_CM_AreaEntities);
		cracking_hook_function(0x080FD518, (int)custom_PlayerCmd_DeactivateReverb);
		cracking_hook_function(0x080FD7C0, (int)custom_PlayerCmd_DeactivateChannelVolumes);
		cracking_hook_function(0x080FB170, (int)custom_PlayerCmd_finishPlayerDamage);
		cracking_hook_function(0x080FBBB4, (int)custom_PlayerCmd_Suicide);
		cracking_hook_function(0x08100E54, (int)custom_Cmd_PrintEntities_f);
		cracking_hook_function(0x080ff4ac, (int)custom_Cmd_FollowCycle_f);
		cracking_hook_function(0x08113076, (int)custom_GScr_LogPrint);
		cracking_hook_function(0x080960E2, (int)custom_SV_PacketEvent);
		cracking_hook_function(0x08094530, (int)custom_SV_CanReplaceServerCommand);
		cracking_hook_function(0x0809114E, (int)custom_SV_ExecuteClientMessage);
		cracking_hook_function(0x0811D300, (int)custom_G_FindConfigstringIndex);
		cracking_hook_function(0x0811D49C, (int)custom_G_ModelIndex);
		cracking_hook_function(0x08092456, (int)custom_SV_GetClientPing);
		cracking_hook_function(0x08093520, (int)custom_SV_SpawnServer);
		cracking_hook_function(0x080F5C34, (int)custom_G_AddPlayerMantleBlockage);
		cracking_hook_function(0x080F82B6, (int)custom_ClientScr_SetHeadIconTeam);
		cracking_hook_function(0x080F8388, (int)custom_ClientScr_GetHeadIconTeam);
		cracking_hook_function(0x0810A982, (int)custom_Fire_Lead);
		cracking_hook_function(0x08120870, (int)custom_FireWeaponAntiLag);
		cracking_hook_function(0x080F8E7A, (int)custom_ClientConnect);
		cracking_hook_function(0x080F8916, (int)custom_G_GetPlayerViewOrigin);
		cracking_hook_function(0x08094C84, (int)custom_SVC_Status);
		cracking_hook_function(0x0810B9A4, (int)custom_G_ClientStopUsingTurret);
		cracking_hook_function(0x0810175A, (int)custom_player_die);
		cracking_hook_function(0x0808E2AA, (int)custom_SV_DirectConnect);
		cracking_hook_function(0x0806BA4C, (int)custom_Netchan_TransmitNextFragment);
		cracking_hook_function(0x0808F628, (int)custom_SV_ClientEnterWorld);
		cracking_hook_function(0x0809537C, (int)custom_SVC_Info);
		cracking_hook_function(0x080B4ADA, (int)custom_Dvar_SetFromStringFromSource);
		cracking_hook_function(0x08096E0C, (int)custom_SV_MasterAddress);
		cracking_hook_function(0x080FDF08, (int)custom_DeathmatchScoreboardMessage);
		cracking_hook_function(0x0811220C, (int)custom_ScrCmd_SetContents);
		cracking_hook_function(0x0808C706, (int)custom_SV_Status_f);
		cracking_hook_function(0x0811037A, (int)custom_GScr_LoadLevelScript);
		cracking_hook_function(0x080F8C5E, (int)custom_ClientUserinfoChanged);
		cracking_hook_function(0x080D67DA, (int)custom_Sys_InitializeCriticalSections);
		cracking_hook_function(0x080D6842, (int)custom_Sys_EnterCriticalSection);
		cracking_hook_function(0x080D6864, (int)custom_Sys_LeaveCriticalSection);

		#if COMPILE_JUMP == 1
		cracking_hook_function(0x080DC8CA, (int)Jump_ReduceFriction);
		cracking_hook_function(0x080DC95C, (int)Jump_ClampVelocity);
		cracking_hook_function(0x080DC7C8, (int)Jump_IsPlayerAboveMax);
		cracking_hook_function(0x080DC73A, (int)Jump_GetStepHeight);
		cracking_hook_function(0x080DCCA8, (int)Jump_Check);
		cracking_hook_function(0x080DC818, (int)Jump_ApplySlowdown);
		cracking_hook_function(0x080DC60C, (int)Jump_RegisterDvars);
		#endif

		#if COMPILE_BOTS == 1
		cracking_hook_function(0x0809676C, (int)custom_SV_BotUserMove);
		#endif

		cracking_write_hex(0x0818815C, (char *)"00"); // Removes debug mode restriction from getentbynum
		cracking_write_hex(0x0815B2A0, (char *)"00"); // Disables warning: "Non-localized %s string is not allowed to have letters in it. Must be changed over to a localized string: \"%s\"\n"
		// End of hooking block

		gsc_weapons_init();
		printf("> [LIBCOD] Finished loading extension\n");
	}

	~cCallOfDuty2Pro()
	{
		gsc_weapons_free();
		printf("> [LIBCOD] Unloaded extension\n");
	}
};

cCallOfDuty2Pro *pro;

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