#include "dvar.hpp"

// Stock dvars
dvar_t *bg_bobMax;
dvar_t *bg_fallDamageMaxHeight;
dvar_t *bg_fallDamageMinHeight;
dvar_t *cl_allowDownload;
dvar_t *cl_paused;
dvar_t *com_dedicated;
dvar_t *com_logfile;
dvar_t *com_sv_running;
dvar_t *com_timescale;
dvar_t *developer;
dvar_t *fs_debug;
dvar_t *fs_game;
dvar_t *fs_homepath;
dvar_t *g_antilag;
dvar_t *g_banIPs;
dvar_t *g_knockback;
dvar_t *g_mantleBlockTimeBuffer;
dvar_t *g_maxDroppedWeapons;
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
dvar_t *sv_debugReliableCmds;
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
dvar_t *sv_showAverageBPS;
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
dvar_t *fs_gametypes;
dvar_t *fs_library;
dvar_t *fs_mapScriptDirectories;
dvar_t *fs_replaceStockMaps;
dvar_t *g_brushModelCollisionTweaks;
dvar_t *g_bulletDrop;
dvar_t *g_bulletDropMaxTime;
dvar_t *g_corpseHit;
dvar_t *g_debugCallbacks;
dvar_t *g_debugEvents;
dvar_t *g_debugStaticModels;
dvar_t *g_droppedWeaponsNeglectBots;
dvar_t *g_forceRate;
dvar_t *g_forceSnaps;
dvar_t *g_logPickup;
dvar_t *g_mantleBlockEnable;
dvar_t *g_noMoverBlockage;
dvar_t *g_playerCollision;
dvar_t *g_playerCollisionEjectDamageAllowed;
dvar_t *g_playerCollisionEjectDuration;
dvar_t *g_playerEject;
dvar_t *g_pointTraceMovement;
dvar_t *g_reservedModels;
dvar_t *g_resetSlide;
dvar_t *g_safePrecache;
dvar_t *g_sendEmtpyOffhandEvents;
dvar_t *g_spawnMapTurrets;
dvar_t *g_spawnMapWeapons;
dvar_t *g_spectateBots;
dvar_t *g_triggerMode;
dvar_t *g_turretMissingTagTerminalError;
dvar_t *jump_bounceEnable;
dvar_t *jump_carryMoverVelocity;
dvar_t *libcod;
dvar_t *loc_loadLocalizedMods;
dvar_t *logErrors;
dvar_t *logfileName;
dvar_t *logfileRotate;
dvar_t *logTimestamps;
dvar_t *net_noFragmentationDelay;
dvar_t *scr_turretDamageName;
dvar_t *sv_allowRcon;
dvar_t *sv_authorizePort;
dvar_t *sv_authorizeServer;
dvar_t *sv_authorizeTimeout;
dvar_t *sv_autoAddSnapshotEntities;
dvar_t *sv_botKickMessages;
dvar_t *sv_botReconnectMode;
dvar_t *sv_botUseTriggerUse;
dvar_t *sv_cracked;
dvar_t *sv_disconnectMessages;
dvar_t *sv_downloadMessage;
dvar_t *sv_downloadMessageAtMap;
dvar_t *sv_downloadMessageForLegacyClients;
dvar_t *sv_downloadNotifications;
dvar_t *sv_downloadRetransmitTimeout;
dvar_t *sv_fastDownload;
dvar_t *sv_fastDownloadSpeed;
dvar_t *sv_genericServerErrorMessage;
dvar_t *sv_isLookingAtOnDemand;
dvar_t *sv_kickGamestateLimitedClients;
dvar_t *sv_kickMessages;
dvar_t *sv_limitLocalRcon;
dvar_t *sv_logHeartbeats;
dvar_t *sv_logRcon;
dvar_t *sv_masterPort;
dvar_t *sv_masterServer;
dvar_t *sv_maxSnapshotEntities;
dvar_t *sv_minimizeSysteminfo;
dvar_t *sv_noauthorize;
dvar_t *sv_reservedConfigstringBufferSize;
dvar_t *sv_timeoutMessages;
dvar_t *sv_updateCursorHints;
dvar_t *sv_verifyIwds;
dvar_t *sv_version;
dvar_t *sv_wwwDlDisconnectedMessages;

// Additional dvar settings that cannot be represented with the 16 bits
// of dvar flags available by default
unsigned int customDvarSettingsCount = 0;
customDvarSettings_t customDvarSettings[MAX_DVARS];

extern cHook *hook_Com_InitDvars;
void custom_Com_InitDvars(void)
{
	// Register custom dvars required early on server start
	libcod = Dvar_RegisterBool("libcod", qtrue, DVAR_ROM);
	logfileName = Dvar_RegisterString("logfileName", "console_mp_server.log", DVAR_ARCHIVE);
	logfileRotate = Dvar_RegisterInt("logfileRotate", 0, 0, 1000, DVAR_ARCHIVE);
	logTimestamps = Dvar_RegisterBool("logTimestamps", qfalse, DVAR_ARCHIVE);
	sv_autoAddSnapshotEntities = Dvar_RegisterBool("sv_autoAddSnapshotEntities", qtrue, DVAR_ARCHIVE | DVAR_LATCH);
	sv_reservedConfigstringBufferSize = Dvar_RegisterInt("sv_reservedConfigstringBufferSize", 0, 0, 8192, DVAR_ARCHIVE);
	sv_authorizePort = Dvar_RegisterInt("sv_authorizePort", 20700, 0, 65535, DVAR_ARCHIVE);
	sv_authorizeServer = Dvar_RegisterString("sv_authorizeServer", "cod2master.activision.com", DVAR_ARCHIVE);
	sv_authorizeTimeout = Dvar_RegisterInt("sv_authorizeTimeout", 3000, 0, 1200000, DVAR_ARCHIVE);
	sv_masterPort = Dvar_RegisterInt("sv_masterPort", 20710, 0, 65535, DVAR_ARCHIVE);
	sv_masterServer = Dvar_RegisterString("sv_masterServer", "cod2master.activision.com", DVAR_ARCHIVE);
	sv_version = Dvar_RegisterString("sv_version", "1.3", DVAR_ARCHIVE | DVAR_LATCH);

	/* Register stock dvars here with different settings, scheme:
	dvar_t *dvar = Dvar_Register<Type>(var_name, default value, [min. value, max. value,] flags); */

	// We do not allow protocol 119 as max. value here as it does not have a
	// unique shortVersion. It is supported via proxy only, see
	// sv_proxyEnable_1_3_119 dvar
	Dvar_RegisterInt("protocol", 118, 115, 118, DVAR_INTERNAL | DVAR_ROM | DVAR_SERVERINFO);

	hook_Com_InitDvars->unhook();
	void (*Com_InitDvars)(void);
	*(int *)&Com_InitDvars = hook_Com_InitDvars->from;
	Com_InitDvars();
	hook_Com_InitDvars->hook();

	// Get references to early loaded stock dvars
	cl_paused = Dvar_FindVar("cl_paused");
	com_dedicated = Dvar_FindVar("dedicated");
	com_logfile = Dvar_FindVar("logfile");
	com_sv_running = Dvar_FindVar("sv_running");
}

void hook_Com_Printf_in_Com_Init_Try_Block_Function(const char *format, ...)
{
	// Here we are after executing Com_InitDvars() and SV_Init() where a big
	// chunk of dvars are defined. However, there is still some that are
	// defined later, see custom_G_ProcessIPBans
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
	sv_debugReliableCmds = Dvar_FindVar("sv_debugReliableCmds");
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
	sv_showAverageBPS = Dvar_FindVar("sv_showAverageBPS");
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
	fs_gametypes = Dvar_RegisterString("fs_gametypes", "", DVAR_ARCHIVE);
	fs_library = Dvar_RegisterString("fs_library", "", DVAR_ARCHIVE);
	fs_mapScriptDirectories = Dvar_RegisterInt("fs_mapScriptDirectories", 0, 0, 2, DVAR_ARCHIVE);
	fs_replaceStockMaps = Dvar_RegisterBool("fs_replaceStockMaps", qfalse, DVAR_ARCHIVE);
	g_brushModelCollisionTweaks = Dvar_RegisterBool("g_brushModelCollisionTweaks", qfalse, DVAR_ARCHIVE);
	g_bulletDrop = Dvar_RegisterBool("g_bulletDrop", qfalse, DVAR_ARCHIVE);
	g_bulletDropMaxTime = Dvar_RegisterInt("g_bulletDropMaxTime", 10000, 50, 60000, DVAR_ARCHIVE);
	g_corpseHit = Dvar_RegisterBool("g_corpseHit", qtrue, DVAR_ARCHIVE);
	g_debugCallbacks = Dvar_RegisterBool("g_debugCallbacks", qfalse, DVAR_ARCHIVE);
	g_debugEvents = Dvar_RegisterBool("g_debugEvents", qfalse, DVAR_ARCHIVE);
	g_debugStaticModels = Dvar_RegisterBool("g_debugStaticModels", qfalse, DVAR_ARCHIVE);
	g_droppedWeaponsNeglectBots = Dvar_RegisterBool("g_droppedWeaponsNeglectBots", qfalse, DVAR_ARCHIVE);
	g_forceRate = Dvar_RegisterInt("g_forceRate", 0, 0, 25000, DVAR_ARCHIVE);
	g_forceSnaps = Dvar_RegisterInt("g_forceSnaps", 0, 0, 30, DVAR_ARCHIVE);
	g_logPickup = Dvar_RegisterBool("g_logPickup", qtrue, DVAR_ARCHIVE);
	g_mantleBlockEnable = Dvar_RegisterBool("g_mantleBlockEnable", qtrue, DVAR_ARCHIVE);
	g_noMoverBlockage = Dvar_RegisterBool("g_noMoverBlockage", qfalse, DVAR_ARCHIVE);
	g_playerCollision = Dvar_RegisterBool("g_playerCollision", qtrue, DVAR_ARCHIVE);
	g_playerCollisionEjectDamageAllowed = Dvar_RegisterBool("g_playerCollisionEjectDamageAllowed", qfalse, DVAR_ARCHIVE);
	g_playerCollisionEjectDuration = Dvar_RegisterInt("g_playerCollisionEjectDuration", 300, 50, 1000, DVAR_ARCHIVE);
	g_playerEject = Dvar_RegisterBool("g_playerEject", qtrue, DVAR_ARCHIVE);
	g_pointTraceMovement = Dvar_RegisterBool("g_pointTraceMovement", qfalse, DVAR_ARCHIVE);
	g_resetSlide = Dvar_RegisterBool("g_resetSlide", qfalse, DVAR_ARCHIVE);
	g_sendEmtpyOffhandEvents = Dvar_RegisterBool("g_sendEmtpyOffhandEvents", qtrue, DVAR_ARCHIVE);
	g_spawnMapTurrets = Dvar_RegisterBool("g_spawnMapTurrets", qtrue, DVAR_ARCHIVE);
	g_spawnMapWeapons = Dvar_RegisterBool("g_spawnMapWeapons", qtrue, DVAR_ARCHIVE);
	g_triggerMode = Dvar_RegisterInt("g_triggerMode", 1, 0, 2, DVAR_ARCHIVE);
	g_turretMissingTagTerminalError = Dvar_RegisterBool("g_turretMissingTagTerminalError", qtrue, DVAR_ARCHIVE);
	g_spectateBots = Dvar_RegisterBool("g_spectateBots", qtrue, DVAR_ARCHIVE);
	loc_loadLocalizedMods = Dvar_RegisterBool("loc_loadLocalizedMods", qfalse, DVAR_ARCHIVE);
	logErrors = Dvar_RegisterBool("logErrors", qfalse, DVAR_ARCHIVE);
	net_noFragmentationDelay = Dvar_RegisterBool("net_noFragmentationDelay", qfalse, DVAR_ARCHIVE);
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
	sv_downloadNotifications = Dvar_RegisterBool("sv_downloadNotifications", qfalse, DVAR_ARCHIVE);
	sv_downloadRetransmitTimeout = Dvar_RegisterInt("sv_downloadRetransmitTimeout", 1000, 250, 10000, DVAR_ARCHIVE);
	sv_fastDownload = Dvar_RegisterBool("sv_fastDownload", qfalse, DVAR_ARCHIVE);
	sv_fastDownloadSpeed = Dvar_RegisterInt("sv_fastDownloadSpeed", MAX_DOWNLOAD_WINDOW, 1, MAX_DOWNLOAD_WINDOW, DVAR_ARCHIVE);
	sv_genericServerErrorMessage = Dvar_RegisterBool("sv_genericServerErrorMessage", qtrue, DVAR_ARCHIVE);
	sv_isLookingAtOnDemand = Dvar_RegisterBool("sv_isLookingAtOnDemand", qfalse, DVAR_ARCHIVE);
	sv_kickGamestateLimitedClients = Dvar_RegisterBool("sv_kickGamestateLimitedClients", qtrue, DVAR_ARCHIVE);
	sv_kickMessages = Dvar_RegisterBool("sv_kickMessages", qtrue, DVAR_ARCHIVE);
	sv_limitLocalRcon = Dvar_RegisterBool("sv_limitLocalRcon", qtrue, DVAR_ARCHIVE);
	sv_logHeartbeats = Dvar_RegisterBool("sv_logHeartbeats", qtrue, DVAR_ARCHIVE);
	sv_logRcon = Dvar_RegisterBool("sv_logRcon", qtrue, DVAR_ARCHIVE);
	sv_maxSnapshotEntities = Dvar_RegisterInt("sv_maxSnapshotEntities", 1024, 64, 1024, DVAR_ARCHIVE);
	sv_noauthorize = Dvar_RegisterBool("sv_noauthorize", qfalse, DVAR_ARCHIVE);
	sv_timeoutMessages = Dvar_RegisterBool("sv_timeoutMessages", qtrue, DVAR_ARCHIVE);
	sv_updateCursorHints = Dvar_RegisterBool("sv_updateCursorHints", qtrue, DVAR_ARCHIVE);
	sv_verifyIwds = Dvar_RegisterBool("sv_verifyIwds", qtrue, DVAR_ARCHIVE);
	sv_wwwDlDisconnectedMessages = Dvar_RegisterBool("sv_wwwDlDisconnectedMessages", qtrue, DVAR_ARCHIVE);

	/* Register (thus override) dvars that would otherwise be defined later in
	 G_RegisterDvars, example:
	g_gravity = Dvar_RegisterFloat("g_gravity", 800.0, 1.0, 3.402823e+38, DVAR_INTERNAL);
	*/
}

extern cHook *hook_FS_RegisterDvars;
qboolean custom_FS_RegisterDvars(void)
{
	int ret;

	hook_FS_RegisterDvars->unhook();
	qboolean (*FS_RegisterDvars)(void);
	*(int *)&FS_RegisterDvars = hook_FS_RegisterDvars->from;
	ret = FS_RegisterDvars();
	hook_FS_RegisterDvars->hook();

	// Get references to file system dvars
	fs_debug = Dvar_FindVar("fs_debug");
	fs_game = Dvar_FindVar("fs_game");
	fs_homepath = Dvar_FindVar("fs_homepath");

	return ret;
}

extern cHook *hook_G_ProcessIPBans;
void custom_G_ProcessIPBans(void)
{
	// This is right after G_RegisterDvars() and BG_RegisterDvars(), giving us
	// access to variables that are not yet defined at
	// hook_Com_Printf_in_Com_Init_Try_Block_Function
	bg_bobMax = Dvar_FindVar("bg_bobMax");
	g_antilag = Dvar_FindVar("g_antilag");
	g_banIPs = Dvar_FindVar("g_banIPs");
	g_knockback = Dvar_FindVar("g_knockback");
	g_mantleBlockTimeBuffer = Dvar_FindVar("g_mantleBlockTimeBuffer");
	g_maxDroppedWeapons = Dvar_FindVar("g_maxDroppedWeapons");
	g_password = Dvar_FindVar("g_password");
	g_playerCollisionEjectSpeed = Dvar_FindVar("g_playerCollisionEjectSpeed");
	g_voiceChatTalkingDuration = Dvar_FindVar("g_voiceChatTalkingDuration");
	player_dmgtimer_maxTime = Dvar_FindVar("player_dmgtimer_maxTime");
	player_dmgtimer_timePerPoint = Dvar_FindVar("player_dmgtimer_timePerPoint");
	player_meleeHeight = Dvar_FindVar("player_meleeHeight");
	player_meleeRange = Dvar_FindVar("player_meleeRange");
	player_meleeWidth = Dvar_FindVar("player_meleeWidth");

	hook_G_ProcessIPBans->unhook();
	void (*G_ProcessIPBans)(void);
	*(int *)&G_ProcessIPBans = hook_G_ProcessIPBans->from;
	G_ProcessIPBans();
	hook_G_ProcessIPBans->hook();
}

extern cHook *hook_Dvar_AddCommands;
void custom_Dvar_AddCommands(void)
{
	hook_Dvar_AddCommands->unhook();
	void (*Dvar_AddCommands)(void);
	*(int *)&Dvar_AddCommands = hook_Dvar_AddCommands->from;
	Dvar_AddCommands();
	hook_Dvar_AddCommands->hook();

	// Register custom dvar functions accessible from console, config files,
	// and the executeCommand script function
	Cmd_AddCommand("protect", Dvar_Protect_f);
}

void Dvar_Protect_f(void)
{
	if ( Cmd_Argc() < 3 )
	{
		Com_Printf("USAGE: protect <variable> <policy: 'r', 'w', or 'rw'>\n");
		return;
	}

	/*
	The dvar security policies are enforced at these commands or functions:
	  - Script SetDvar
	  - Script GetDvar
	  - Script MakeDvarServerInfo
	  - dvardump
	  - dvarlist
	  - protect
	  - reset
	  - set
	  - seta
	  - setfromdvar
	  - sets
	  - setu
	  - toggle
	  - togglep
	  - writeconfig
	  - writedefaults
	This also applies for dvar commands (commands that start with the name of
	the dvar instead of set/seta etc.) and commands issued through the script
	executeCommand function, or commands executed from a .cfg file. Here's some
	call stacks on protect invocation, for reference:
	  - Console: direct call to protect
		0x8060928 Cmd_ExecuteString
		0x805fff9 Cbuf_ExecuteInternal
		0x806000b Cbuf_Execute
		0x80627a4 Com_Frame_Try_Block_Function
		0x806281f Com_Frame
	  - Startup auto-exec: protect in config_mp_server.cfg
		0x8060928 Cmd_ExecuteString
		0x805fff9 Cbuf_ExecuteInternal
		0x806000b Cbuf_Execute
		0x8062027 Com_RunAutoExec
		0x8062090 Com_ExecStartupConfigs
	  - Script executeCommand: direct call to protect
		0x8060928 Cmd_ExecuteString
		0xd4d5ec60 gsc_utils_executecommand
		0x8080ce8 VM_ExecuteInternal
		0x8083db8 VM_Resume
		0x80842e9 VM_SetTime
	  - Script executeCommand: protect in protect.cfg
		0x8060928 Cmd_ExecuteString
		0x805fff9 Cbuf_ExecuteInternal
		0x806000b Cbuf_Execute
		0x80627a4 Com_Frame_Try_Block_Function
		0x806281f Com_Frame
	*/

	dvar_t *dvar = Dvar_FindVar(Cmd_Argv(1));
	if ( dvar )
	{
		unsigned int i;
		customDvarSettings_t *setting = NULL;

		for ( i = 0; i < customDvarSettingsCount; ++i )
		{
			if ( customDvarSettings[i].dvar == dvar )
			{
				setting = &customDvarSettings[i];
				break;
			}
		}
		if ( !setting )
		{
			setting = &customDvarSettings[customDvarSettingsCount];
			setting->dvar = dvar;
			setting->flags = DVAR_SCRIPT_DEFAULT;
			customDvarSettingsCount++;
		}
		else
		{
			// Generally, once set, do not allow a policy to be downgraded from
			// anywhere. Because of the executeCommand script function, we
			// cannot reliably differentiate between calls to protect from
			// console or script code since the call stack looks the same when
			// issuing a config exec command like executeCommand("exec protect.cfg")
			if ( setting->flags != DVAR_SCRIPT_DEFAULT )
			{
				Com_Printf("WARNING: Variable '%s' already has a protection policy\n", Cmd_Argv(1));
				return;
			}
		}

		if ( !strlen(Cmd_Argv(2)) )
		{
			// No read-write protection, but protection from policy override
			setting->flags = DVAR_SCRIPT_DEFAULT;
		}
		else if ( !strcmp(Cmd_Argv(2), "r") )
		{
			// Read protection
			setting->flags |= DVAR_SCRIPT_NOREAD;
		}
		else if ( !strcmp(Cmd_Argv(2), "w") )
		{
			// Write protection
			setting->flags |= DVAR_SCRIPT_NOWRITE;
		}
		else if ( !strcmp(Cmd_Argv(2), "rw") || !strcmp(Cmd_Argv(2), "wr") )
		{
			// Read-write protection
			setting->flags |= ( DVAR_SCRIPT_NOREAD | DVAR_SCRIPT_NOWRITE );
		}
		else
		{
			Com_Printf("WARNING: Unknown protection policy '%s' for variable '%s'\n", Cmd_Argv(2), Cmd_Argv(1));
		}
	}
	else
	{
		Com_Printf("WARNING: Cannot protect not yet defined variable '%s'\n", Cmd_Argv(1));
	}
}

qboolean Dvar_IsReadProtected(dvar_t *dvar)
{
	if ( !dvar )
		return qfalse;

	for ( unsigned int i = 0; i < customDvarSettingsCount; ++i )
	{
		if ( customDvarSettings[i].dvar == dvar )
		{
			if ( customDvarSettings[i].flags & DVAR_SCRIPT_NOREAD )
				return qtrue;
			else
				return qfalse;
		}
	}

	return qfalse;
}

qboolean Dvar_IsWriteProtected(dvar_t *dvar)
{
	if ( !dvar )
		return qfalse;

	for ( unsigned int i = 0; i < customDvarSettingsCount; ++i )
	{
		if ( customDvarSettings[i].dvar == dvar )
		{
			if ( customDvarSettings[i].flags & DVAR_SCRIPT_NOWRITE )
				return qtrue;
			else
				return qfalse;
		}
	}

	return qfalse;
}

extern cHook *hook_Dvar_Reset_f;
void custom_Dvar_Reset_f(void)
{
	dvar_t *dvar;

	// New: Dvar access policies
	dvar = Dvar_FindVar(Cmd_Argv(1));
	if ( dvar && Dvar_IsWriteProtected(dvar) )
	{
		Com_Printf("WARNING: Write attempt via reset on protected dvar '%s'\n", Cmd_Argv(1));
		return;
	}

	hook_Dvar_Reset_f->unhook();
	void (*Dvar_Reset_f)(void);
	*(int *)&Dvar_Reset_f = hook_Dvar_Reset_f->from;
	Dvar_Reset_f();
	hook_Dvar_Reset_f->hook();
}

extern cHook *hook_Dvar_Toggle_f;
void custom_Dvar_Toggle_f(void)
{
	dvar_t *dvar;

	// New: Dvar access policies
	dvar = Dvar_FindVar(Cmd_Argv(1));
	if ( dvar && Dvar_IsWriteProtected(dvar) )
	{
		Com_Printf("WARNING: Write attempt via toggle on protected dvar '%s'\n", Cmd_Argv(1));
		return;
	}

	hook_Dvar_Toggle_f->unhook();
	void (*Dvar_Toggle_f)(void);
	*(int *)&Dvar_Toggle_f = hook_Dvar_Toggle_f->from;
	Dvar_Toggle_f();
	hook_Dvar_Toggle_f->hook();
}

extern cHook *hook_Dvar_TogglePrint_f;
void custom_Dvar_TogglePrint_f(void)
{
	dvar_t *dvar;

	// New: Dvar access policies
	dvar = Dvar_FindVar(Cmd_Argv(1));
	if ( dvar && Dvar_IsWriteProtected(dvar) )
	{
		Com_Printf("WARNING: Write attempt via togglep on protected dvar '%s'\n", Cmd_Argv(1));
		return;
	}

	hook_Dvar_TogglePrint_f->unhook();
	void (*Dvar_TogglePrint_f)(void);
	*(int *)&Dvar_TogglePrint_f = hook_Dvar_TogglePrint_f->from;
	Dvar_TogglePrint_f();
	hook_Dvar_TogglePrint_f->hook();
}

void custom_Dvar_SetS_f(void)
{
	dvar_t *dvar;

	if ( Cmd_Argc() < 3 )
	{
		Com_Printf("USAGE: sets <variable> <value>\n");
		return;
	}

	Dvar_Set_f();
	dvar = Dvar_FindVar(Cmd_Argv(1));

	if ( dvar )
	{
		/* New code start: Dvar access policies */
		if ( dvar && Dvar_IsReadProtected(dvar) )
		{
			Com_Printf("WARNING: Read attempt via sets on protected dvar '%s'\n", Cmd_Argv(1));
			return;
		}
		/* New code end */

		Dvar_AddFlags(dvar, DVAR_SERVERINFO);
	}
}

void custom_Dvar_SetU_f(void)
{
	dvar_t *dvar;

	if ( Cmd_Argc() < 3 )
	{
		Com_Printf("USAGE: setu <variable> <value>\n");
		return;
	}

	Dvar_Set_f();
	dvar = Dvar_FindVar(Cmd_Argv(1));

	if ( dvar )
	{
		/* New code start: Dvar access policies */
		if ( dvar && Dvar_IsReadProtected(dvar) )
		{
			Com_Printf("WARNING: Read attempt via setu on protected dvar '%s'\n", Cmd_Argv(1));
			return;
		}
		/* New code end */

		Dvar_AddFlags(dvar, DVAR_USERINFO);
	}
}

void custom_Dvar_SetFromDvar_f(void)
{
	dvar_t *srcDvar;
	dvar_t *dstDvar;

	if ( Cmd_Argc() == 3 )
	{
		srcDvar = Dvar_FindVar(Cmd_Argv(2));

		/* New code start: Dvar access policies */
		if ( srcDvar && Dvar_IsReadProtected(srcDvar) )
		{
			Com_Printf("WARNING: Read attempt via setfromdvar on protected dvar '%s'\n", Cmd_Argv(2));
			return;
		}

		dstDvar = Dvar_FindVar(Cmd_Argv(1));
		if ( dstDvar && Dvar_IsWriteProtected(dstDvar) )
		{
			Com_Printf("WARNING: Write attempt via setfromdvar on protected dvar '%s'\n", Cmd_Argv(1));
			return;
		}
		/* New code end */

		if ( srcDvar )
			Dvar_SetCommand(Cmd_Argv(1), Dvar_DisplayableValue(srcDvar));
		else
			Com_Printf("dvar '%s' doesn't exist\n", Cmd_Argv(2));
	}
	else
		Com_Printf("USAGE: setfromdvar <dest_dvar> <source_dvar>\n");
}

void custom_GScr_GetDvar(void)
{
	const char *dvarName;
	dvar_t *dvar;

	dvarName = Scr_GetString(0);

	/* New code start: Dvar access policies */
	dvar = Dvar_FindVar(dvarName);
	if ( dvar && Dvar_IsReadProtected(dvar) )
	{
		Com_Printf("WARNING: Read attempt via GetCvar on protected dvar '%s'\n", dvarName);
		return;
	}
	/* New code end */

	Scr_AddString(Dvar_GetVariantString(dvarName));
}

extern cHook *hook_GScr_SetDvar;
void custom_GScr_SetDvar(void)
{
	const char *dvarName;
	dvar_t *dvar;
	
	dvarName = Scr_GetString(0);
	if ( !Dvar_IsValidName(dvarName) )
	{
		Scr_Error(va("Dvar %s has an invalid dvar name", dvarName));
	}

	dvar = Dvar_FindVar(dvarName);
	if ( dvar )
	{
		// New: Dvar access policies
		if ( Dvar_IsWriteProtected(dvar) )
		{
			Com_Printf("WARNING: Write attempt via SetCvar on protected dvar '%s'\n", dvarName);
			return;
		}
	}

	hook_GScr_SetDvar->unhook();
	void (*GScr_SetDvar)(void);
	*(int *)&GScr_SetDvar = hook_GScr_SetDvar->from;
	GScr_SetDvar();
	hook_GScr_SetDvar->hook();
}

extern cHook *hook_GScr_MakeDvarServerInfo;
void custom_GScr_MakeDvarServerInfo(void)
{
	const char *dvarName = Scr_GetString(0);
	dvar_t *dvar = Dvar_FindVar(dvarName);

	if ( dvar )
	{
		// New: Dvar access policies
		if ( Dvar_IsReadProtected(dvar) )
		{
			Com_Printf("WARNING: Read attempt via MakeCvarServerInfo on protected dvar '%s'\n", dvarName);
			return;
		}
	}

	hook_GScr_MakeDvarServerInfo->unhook();
	void (*GScr_MakeDvarServerInfo)(void);
	*(int *)&GScr_MakeDvarServerInfo = hook_GScr_MakeDvarServerInfo->from;
	GScr_MakeDvarServerInfo();
	hook_GScr_MakeDvarServerInfo->hook();
}

extern cHook *hook_Dvar_Command;
qboolean custom_Dvar_Command(void)
{
	const char *dvarName = Cmd_Argv(0);
	dvar_t *dvar = Dvar_FindVar(dvarName);
	qboolean ret;

	if ( dvar )
	{
		// New: Dvar access policies
		if ( Cmd_Argc() == 1 )
		{
			if ( Dvar_IsReadProtected(dvar) )
			{
				Com_Printf("WARNING: Read attempt via dvar command on protected dvar '%s'\n", dvarName);
				return qfalse;
			}
		}
		else
		{
			if ( Dvar_IsWriteProtected(dvar) )
			{
				Com_Printf("WARNING: Write attempt via dvar command on protected dvar '%s'\n", dvarName);
				return qfalse;
			}
		}
	}

	hook_Dvar_Command->unhook();
	qboolean (*Dvar_Command)(void);
	*(int *)&Dvar_Command = hook_Dvar_Command->from;
	ret = Dvar_Command();
	hook_Dvar_Command->hook();

	return ret;
}

extern cHook *hook_Dvar_SetCommand;
void custom_Dvar_SetCommand(const char *dvarName, const char *string)
{
	dvar_t *dvar;

	// New: Dvar access policies
	dvar = Dvar_FindVar(dvarName);
	if ( dvar && Dvar_IsWriteProtected(dvar) )
	{
		Com_Printf("WARNING: Write attempt via %s on protected dvar '%s'\n", Cmd_Argv(0), dvarName);
		return;
	}

	hook_Dvar_SetCommand->unhook();
	void (*Dvar_SetCommand)(const char *dvarName, const char *string);
	*(int *)&Dvar_SetCommand = hook_Dvar_SetCommand->from;
	Dvar_SetCommand(dvarName, string);
	hook_Dvar_SetCommand->hook();
}

void custom_Dvar_List_f(void)
{
	const char *match;
	dvar_t *dvar;

	if ( Cmd_Argc() <= 1 )
		match = NULL;
	else
		match = Cmd_Argv(1);

	for ( dvar = sortedDvars; dvar; dvar = dvar->next )
	{
		if ( match && !Com_Filter(match, dvar->name, qfalse) )
			continue;

		if ( dvar->flags & ( DVAR_SERVERINFO | DVAR_SERVERINFO_NOUPDATE ) )
			Com_Printf("S");
		else
			Com_Printf(" ");

		if ( dvar->flags & DVAR_USERINFO )
			Com_Printf("U");
		else
			Com_Printf(" ");

		if ( dvar->flags & DVAR_ROM )
			Com_Printf("R");
		else
			Com_Printf(" ");

		if ( dvar->flags & DVAR_INIT )
			Com_Printf("I");
		else
			Com_Printf(" ");

		if ( dvar->flags & DVAR_ARCHIVE )
			Com_Printf("A");
		else
			Com_Printf(" ");

		if ( dvar->flags & DVAR_LATCH )
			Com_Printf("L");
		else
			Com_Printf(" ");

		if ( dvar->flags & DVAR_CHEAT )
			Com_Printf("C");
		else
			Com_Printf(" ");

		if ( Dvar_IsReadProtected(dvar) ) // New: Dvar access policies
			Com_Printf(" %s (read protected)\n", dvar->name);
		else
			Com_Printf(" %s \"%s\"\n", dvar->name, Dvar_DisplayableValue(dvar));
	}

	Com_Printf("\n%i total dvars\n", dvarCount);
}

void custom_Com_DvarDump(conChannel_t channel)
{
	dvar_t *dvar;
	const char *match;
	int count = 0;
	char summary[8192];

	if ( Cmd_Argc() <= 1 )
		match = NULL;
	else
		match = Cmd_Argv(1);

	if ( channel != CON_CHANNEL_LOGFILEONLY || ( com_logfile && com_logfile->current.integer ) )
	{
		Com_PrintMessage(channel, "=============================== DVAR DUMP ========================================\n");

		for ( dvar = sortedDvars; dvar; dvar = dvar->next, count++ )
		{
			if ( !match || Com_Filter(match, dvar->name, 0) )
			{
				if ( Dvar_IsReadProtected(dvar) ) // New: Dvar access policies
				{
					Com_sprintf(summary, sizeof(summary), "      %s (read protected)\n", dvar->name);
				}
				else
				{
					if ( Dvar_HasLatchedValue(dvar) )
						Com_sprintf(summary, sizeof(summary), "      %s \"%s\" -- latched \"%s\"\n", dvar->name,
									Dvar_DisplayableValue(dvar), Dvar_DisplayableLatchedValue(dvar));
					else
						Com_sprintf(summary, sizeof(summary), "      %s \"%s\"\n", dvar->name, Dvar_DisplayableValue(dvar));
				}

				Com_PrintMessage(channel, summary);
			}
		}

		Com_sprintf(summary, sizeof(summary), "\n%i total dvars\n%i dvar indexes\n", count, dvarCount);
		Com_PrintMessage(channel, summary);
		Com_PrintMessage(channel, "=============================== END DVAR DUMP =====================================\n");
	}
}

void custom_Dvar_WriteDefaults(fileHandle_t f)
{
	dvar_t *dvar;

	for ( dvar = sortedDvars; dvar; dvar = dvar->next )
	{
		if ( I_stricmp(dvar->name, "cl_cdkey") )
		{
			if ( !( dvar->flags & ( DVAR_ROM | DVAR_CHEAT | DVAR_EXTERNAL ) ) &&
			     !Dvar_IsReadProtected(dvar) ) // New: Dvar access policies
				FS_Printf(f, "set %s \"%s\"\n", dvar->name, Dvar_DisplayableResetValue(dvar));
		}
	}
}

void custom_Dvar_WriteVariables(fileHandle_t f)
{
	dvar_t *dvar;

	for ( dvar = sortedDvars; dvar; dvar = dvar->next )
	{
		if ( I_stricmp(dvar->name, "cl_cdkey") )
		{
			if ( !( ( dvar->flags ^ DVAR_ARCHIVE ) & DVAR_ARCHIVE ) &&
			     !Dvar_IsReadProtected(dvar) ) // New: Dvar access policies
				FS_Printf(f, "seta %s \"%s\"\n", dvar->name, Dvar_DisplayableLatchedValue(dvar));
		}
	}
}

void custom_Dvar_SetFromStringFromSource(dvar_t *dvar, const char *string, DvarSetSource source)
{
	char buf[MAX_STRINGLENGTH];
	DvarValue newValue;

	Sys_EnterCriticalSection(CRITSECT_DVAR);

	/* New code start: sv_version dvar value sanitization */
	if ( sv_version && dvar == sv_version )
		string = GetShortVersionFromProtocol(GetProtocolFromShortVersion(string));
	/* New code end */

	I_strncpyz(buf, string, sizeof(buf));
	Dvar_StringToValue(&newValue, dvar->type, dvar->domain, buf);
	if ( dvar->type == DVAR_TYPE_ENUM && newValue.integer == DVAR_INVALID_ENUM_INDEX )
	{
		Com_Printf("'%s' is not a valid value for dvar '%s'\n", buf, dvar->name);
		Dvar_PrintDomain(dvar->type, dvar->domain);
		newValue = dvar->reset;
	}
	Dvar_SetVariant(dvar, newValue, source);

	Sys_LeaveCriticalSection(CRITSECT_DVAR);
}