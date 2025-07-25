> [!NOTE]
> * Unless stated otherwise, all default values are defined to preserve the stock behavior of the game.
> * Text in square brackets like `[dvar]` refers to the value of the respective dvar.
> * Stock script functions/methods start in uppercase here, whereas libcod script functions/methods start in lowercase.

🔢 Added dvars:

  * `fs_gametypes`
    * Type: String
    * Default: ""
    * Effect: Defines the relative path to where the server should look for gametype .gsc files containing a `main` function. If set to an empty string (as by default), the server uses `maps/mp/gametypes`.
  * `fs_mapScriptDirectories`
    * Type: Integer
    * Default: 0
    * Min. Value: 0
    * Max. Value: 2
    * Effect: `0`: The map script's `main` function is searched in `maps\mp\[mapname].gsc`. `1`: The map script's `main` function is searched in `maps\mp\[mapname]\[mapname].gsc` instead. `2`: The map script's `main` function is first searched in `maps\mp\[mapname]\[mapname].gsc` and reverts to stock behavior if not found.
  * `fs_replaceStockMaps`
    * Type: Boolean
    * Default: False
    * Effect: If true, manymaps supports to load custom stock map .iwd files so that mp_rhine.iwd and mp_harbor.iwd could be loaded by players on game version 1.0, if these .iwd files are stored in the manymaps library. Related dvar: `fs_library`.
  * `g_brushModelCollisionTweaks`
    * Type: Boolean
    * Default: False
    * Effect: If false, the `notSolidForPlayer` and `solidForPlayer` script methods have no effect.
  * `g_bulletDrop`
    * Type: Boolean
    * Default: False
    * Effect: Toggle bullet gravity system. Some new script methods then allow to enable and use it on a per-player basis, with custom bullet velocity, drag and models.
  * `g_bulletDropMaxTime`
    * Type: Integer
    * Default: 10000
    * Min. Value: 50
    * Max. Value: 60000
    * Effect: Defines the time in milliseconds how long gravity-enabled bullets are allowed to travel.
  * `g_corpseHit`
    * Type: Boolean
    * Default: True
    * Effect: Toggle bullet hit events (sound + effect) for corpses during death animation.
  * `g_debugCallbacks`
    * Type: Boolean
    * Default: False
    * Effect: Toggle debug logging of script callback function assignments on server start and script callback function calls at runtime.
  * `g_debugEvents`
    * Type: Boolean
    * Default: False
    * Effect: Toggle debug logging of events such as player footsteps, spawn of temporary entities etc.
  * `g_debugStaticModels`
    * Type: Boolean
    * Default: False
    * Effect: Toggle logging of info about static models on map load.
  * `g_droppedWeaponsNeglectBots`
    * Type: Boolean
    * Default: False
    * Effect: If enabled, bots are excluded from dropped weapon removal distance calculations while human players are active.
  * `g_forceRate`
    * Type: Integer
    * Default: 0
    * Min. Value: 0 (disabled)
    * Min. Effective Value: 1000
    * Max. Value: 25000
    * Effect: Forces every client's rate. Does not update the rate dvar value on the client side.
  * `g_forceSnaps`
    * Type: Integer
    * Default: 0
    * Min. Value: 0 (disabled)
    * Min. Effective Value: 1
    * Max. Value: 30
    * Effect: Forces every client's snaps. Does not update the snaps dvar value on the client side. 
  * `g_logPickup`
    * Type: Boolean
    * Default: True
    * Effect: Toggle game logging of item pickup actions.
  * `g_noMoverBlockage`
    * Type: Boolean
    * Default: False
    * Effect: Defines whether mover entities (e.g., elevators) can be blocked by other entities. Related script callback function: `CodeCallback_MoverBlockage`.
  * `g_playerCollisionEjectDamageAllowed`
    * Type: Boolean
    * Default: False
    * Effect: By default, players that are stuck within each other cannot be damaged by bullets while being ejected (coming unstuck). With this dvar, damage can be enabled.
  * `g_playerCollisionEjectDuration`
    * Type: Integer
    * Default: 300
    * Min. Value: 50
    * Max. Value: 1000
    * Effect: Defines the time in milliseconds how long players that are too close to each other will be moved away from each other with the speed defined via the `g_playerCollisionEjectSpeed` dvar.
  * `g_reservedModels`
    * Type: Integer
    * Default: 0
    * Min. Value: 0
    * Max. Value: 255
    * Effect: Set the number of models that cannot be used by the loaded map, thus reserves a specific number of slots to be loadable for precached weapons (the necessary world models) and regular precached models. Recommended to be used only together with the `g_safePrecache` dvar enabled. The slots are reserved for precaching within the following script functions: Map `main()`, gametype `main()`, and `CodeCallback_StartGameType`.
  * `g_resetSlide`
    * Type: Boolean
    * Default: False
    * Effect: Fix for slide-like player movement after receiving damage from falling or grenades not being reset.
  * `g_safePrecache`
    * Type: Boolean
    * Default: False
    * Effect: If true, exceeding the limits of `LoadFx` or `PrecacheModel` will generate console log warnings instead of stopping the server. Excess effects will be replaced by (= displayed as) `fx/misc/missing_fx.efx` and excess models as `xmodel/default_static_model`. These default assets are loaded automatically (and therefore occupy one config string each) if the dvar is set to true. Map models are loaded first, but can be limited via the `g_reservedModels` dvar.
  * `g_sendEmtpyOffhandEvents`
    * Type: Boolean
    * Default: True
    * Effect: If disabled, players can no longer suppress other player events (e.g., firing or footstep sounds) by holding a grenade button.
  * `g_spawnMapTurrets`
    * Type: Boolean
    * Default: True
    * Effect: If false, turrets loaded from map files are neither precached nor spawned. Instead, the list of turrets to load is sent to the `CodeCallback_MapTurretsLoad` script callback function.
  * `g_spawnMapWeapons`
    * Type: Boolean
    * Default: True
    * Effect: If false, weapons loaded from map files are neither precached nor spawned. Instead, the list of weapons to load is sent to the `CodeCallback_MapWeaponsLoad` script callback function.
  * `g_spectateBots`
    * Type: Boolean
    * Default: True
    * Effect: Toggle the ability to spectate bots. Alternatively or in addition, the `setAllowSpectators` script method can be used on a per-player basis.
  * `g_triggerMode`
    * Type: Integer
    * Default: 1
    * Min. Value: 0
    * Max. Value: 2
    * Effect: Toggle experimental fix for broken map damage triggers. Possible values: `0`: Triggers disabled. `1`: Stock behavior, fix disabled. `2`: Fix enabled.
  * `g_turretMissingTagTerminalError`
    * Type: Boolean
    * Default: True
    * Effect: If false, the server will not halt when the `tag_player` tag cannot be found on a turret, but instead yield a script error to `CodeCallback_Error`, if that script callback function is enabled. Such error can occur in killcam-related edge cases, e.g., when a turret is removed while the player is alive and in killcam.
  * `jump_bounceEnable`
    * Type: Boolean
    * Default: False
    * Effect: If true, enables bounce jumping like it is possible in Call of Duty&reg; 4.
  * `jump_carryMoverVelocity`
    * Type: Boolean
    * Default: False
    * Effect: If true, the velocity of mover entities (e.g., elevators) is applied to players jumping off such entity.
  * `libcod`
    * Type: Boolean
    * Default: True
    * Effect: Allows scripts to determine whether libcod is present or not. Read only.
  * `loc_loadLocalizedMods`
    * Type: Boolean
    * Default: False
    * Effect: If true, localized .iwd files (such that follow the "localized_&lt;language&gt;_&lt;text&gt;.iwd" pattern) in the `fs_game` folder (unless it is "main") will be downloaded by clients too.
  * `logErrors`
    * Type: Boolean
    * Default: False
    * Effect: If true, log (non-terminal) script runtime errors as if developer mode was enabled.
  * `logfileName`
    * Type: String
    * Default: "console_mp_server.log"
    * Effect: Controls the console log file name. Should be set before the stock `logfile` dvar is set to a value greater than zero.
  * `logfileRotate`
    * Type: Integer
    * Default: 0
    * Min. Value: 0
    * Max. Value: 1000
    * Effect: Defines how many console log files are preserved on a server restart. Naming scheme: `[logfileName].#` where `#` is an incrementing integer.
  * `logTimestamps`
    * Type: Boolean
    * Default: False
    * Effect: If true, prepend a timestamp (example: "Mon Apr  4 13:37:00 2023 ") to each console log entry.
  * `net_noFragmentationDelay`
    * Type: Boolean
    * Default: False
    * Effect: If true, the server will send out all pending network fragments instead of a single fragment per server frame. This can improve lag in busy game conditions (i.e., many players and many entities updated at a time), while also increasing server load (higher network I/O).
  * `scr_turretDamageName`
    * Type: Boolean
    * Default: False
    * Effect: If true, turret weapon names are passed to `Callback_PlayerDamage` when using a turret, instead of the player's current weapon slot's weapon. Also fixes the game issue where grenade (and other) kills are shown as turret kills while the player is using a turret.
  * `sv_autoAddSnapshotEntities`
    * Type: Boolean
    * Default: True
    * Effect: If set to false, it is up to the script code (instead of the game engine itself) to populate server snapshots with entities to use and draw on the client side. Requires archived server snapshots to be disabled. Related script methods: `SetArchive`, `addEntToSnapshots`, `removeEntFromSnapshots` and `getNumberOfEntsInSnapshot`.
  * `sv_authorizePort`
    * Type: Integer
    * Default: 20700
    * Min. Value: 0
    * Max. Value: 65535
    * Effect: Port to use for communication to the authorize server (for player GUID calculation).
  * `sv_authorizeServer`
    * Type: String
    * Default: "cod2master.activision.com"
    * Effect: Hostname of the authorize server (for player GUID calculation) to communicate to.
  * `sv_authorizeTimeout`
    * Type: Integer
    * Default: 3000
    * Min. Value: 0
    * Max. Value: 1200000
    * Effect: The original authorize server timeout is 20 minutes. We default it to a few seconds to not have the game go dead once the authorize server goes offline (again). Setting this timeout too low will make players with a valid CD key connect with a zero GUID too. The timeout is defined in milliseconds.
  * `sv_botKickMessages`
    * Type: Boolean
    * Default: True
    * Effect: Toggle output of bot disconnect game messages.
  * `sv_botReconnectMode`
    * Type: Integer
    * Default: 0
    * Min. Value: 0
    * Max. Value: 2
    * Effect: 0: Stock behavior, bots stay in connecting state after map switch and get kicked after `[sv_timeout]` seconds. 1: Bots get kicked right on map switch. 2: Bots properly reconnect on map switch, using the same slot each.
  * `sv_botUseTriggerUse`
    * Type: Boolean
    * Default: False
    * Effect: Toggle whether bots should be notified when triggers of type trigger_use or trigger_use_touch are nearby (i.e., when the hint string appears). Once available, the bot entity will be notified with a "bot_trigger" notify where the trigger entity is the first and only argument.
  * `sv_disconnectMessages`
    * Type: Boolean
    * Default: True
    * Effect: Toggle output of "Game session no longer available" game messages.
  * `sv_downloadMessageAtMap`
    * Type: Boolean
    * Default: True
    * Effect: Define whether the `sv_downloadMessage` dvar should also apply for map file downloads, i.e., such that contain `mp_` or `empty` in the file path.
  * `sv_downloadMessageForLegacyClients`
    * Type: String
    * Default: ""
    * Effect: If set to a non-empty string, players on game version 1.0 and 1.3 (protocol 119 only) that attempt to download a file on connect are disconnected from the server, with the specified message displayed after disconnect. Related dvars: `sv_downloadMessage`, `sv_downloadMessageAtMap`.
  * `sv_downloadNotifications`
    * Type: Boolean
    * Default: False
    * Effect: If enabled, prints the relative path to the file requested by the connecting player ingame. Applied for both direct and HTTP-based downloads.
  * `sv_downloadRetransmitTimeout`
    * Type: Integer
    * Default: 1000
    * Min. Value: 250
    * Max. Value: 10000
    * Effect: Controls the time in milliseconds after which the server will retransmit direct file download blocks, if a client did not acknowledge the previously sent blocks in the specified time.
  * `sv_fastDownload`
    * Type: Boolean
    * Default: False
    * Effect: Enable this for higher direct download speeds. Does not affect HTTP-based downloads. Currently considered experimental. May cause clients with an unstable internet connection to get stuck in the download process without an error message on the client side, requiring a manual reconnect.
  * `sv_fastDownloadSpeed`
    * Type: Integer
    * Default: 8
    * Min. Value: 1
    * Max. Value: 8
    * Effect: Controls the direct download speed when the `sv_fastDownload` dvar is enabled.
  * `sv_isLookingAtOnDemand`
    * Type: Boolean
    * Default: False
    * Effect: Enable this if you expect only a few calls (less than one per player per server frame) to the stock `IsLookingAt` script method during runtime, to reduce server load.
  * `sv_kickGamestateLimitedClients`
    * Type: Boolean
    * Default: True
    * Effect: Kick clients that do not support enough gamestate data for the current mod to function properly (usually only affecting players on game versions 1.0 or 1.2). If set to false, the players can connect, but will most probably have some shaders displayed incorrectly (in an optimal case).
  * `sv_kickMessages`
    * Type: Boolean
    * Default: True
    * Effect: Generally toggle output of player kick game messages. If false, this overrides enabled `sv_botKickMessages`.
  * `sv_limitLocalRcon`
    * Type: Boolean
    * Default: True
    * Effect: Toggle whitelisting of internal IP address ranges at rcon rate limiter.
  * `sv_logHeartbeats`
    * Type: Boolean
    * Default: True
    * Effect: Toggle logging of heartbeats that are sent to the master server.
  * `sv_logRcon`
    * Type: Boolean
    * Default: True
    * Effect: Toggle logging of successful rcon commands.
  * `sv_masterPort`
    * Type: Integer
    * Default: 20710
    * Min. Value: 0
    * Max. Value: 65535
    * Effect: Port to use for communication to the master server (for the ingame server browser).
  * `sv_masterServer`
    * Type: String
    * Default: "cod2master.activision.com"
    * Effect: Hostname of the master server (for the ingame server browser) to communicate to.
  * `sv_maxSnapshotEntities`
    * Type: Integer
    * Default: 1024
    * Min. Value: 64
    * Max. Value: 1024
    * Effect: Controls the max. number of entities added to snapshots. Can be used to avoid Netchan_Transmit overflow errors if legacy client support is enabled (e.g., with a setting of 256), or to avoid entity corruption issues on the client side.
  * `sv_minimizeSysteminfo`
    * Type: Integer
    * Default: 0
    * Min. Value: 0
    * Max. Value: 3
    * Effect:
      * If set to 1, removes some dvars from the systeminfo config string so that more .iwd files can be used in a mod without having clients running into "iwd sum/name mismatch errors". Instead, the cod info flag will be set on them so that they are still synchronized with the clients on connect. However, further changes during runtime are not synchronized until gamestate is resent (e.g., during a map switch). Therefore, when using this functionality, it is advised to synchronize these dvars with the clients via script whenever their value changes. Dvars that still stay in the systeminfo config string, unless configured otherwise, are:
        * cl_allowDownload
        * cl_wwwDownload
        * fs_game
        * sv_serverid
        * and the jump_... dvars if jump.cpp is compiled.
      * If set to 2 or greater, also overrides jump-related dvars set in jump.cpp.
      * If set to 3, also overrides download-related dvars set in libcod.cpp. Note that sending these dvars this way is too late to auto-enable downloading right on a client's first connect. Instead, a reconnect is necessary for the client then, if downloading was not enabled already in the first place.
  * `sv_proxiesVisibleForTrackers`
    * Type: Boolean
    * Default: False
    * Effect: Toggle whether game info requests coming from known trackers should be answered by proxied ports (see the sv_proxyEnable dvars).
  * `sv_proxyAddress_1_0`
    * Type: String
    * Default: "0.0.0.0:28960"
    * Effect: Hostname and port the proxy socket for game version 1.0 should listen to (see `sv_proxyEnable_1_0` dvar).
  * `sv_proxyAddress_1_2`
    * Type: String
    * Default: "0.0.0.0:28961"
    * Effect: Hostname and port the proxy socket for game version 1.2 should listen to (see `sv_proxyEnable_1_2` dvar).
  * `sv_proxyAddress_1_3`
    * Type: String
    * Default: "0.0.0.0:28962"
    * Effect: Hostname and port the proxy socket for game version 1.3 should listen to (see `sv_proxyEnable_1_3` dvar).
  * `sv_proxyAddress_1_3_119`
    * Type: String
    * Default: "0.0.0.0:28963"
    * Effect: Hostname and port the proxy socket for game version 1.3 (protocol 119) should listen to (see `sv_proxyEnable_1_3_119` dvar).
  * `sv_proxyEnable_1_0`
    * Type: Boolean
    * Default: False
    * Effect: Allows to open another (proxy) port that is advertised to the master server to show up as game version 1.0 in the ingame server browser. Has no effect if sv_version is already set to "1.0". If enabled, the proxy is started on map (re)load, and shuts down (only) on server quit. Rcon commands are ignored on proxy ports. Clients connecting via the proxy always have a zero GUID, and player ping restrictions are not enforced on that port.
  * `sv_proxyEnable_1_2`
    * Type: Boolean
    * Default: False
    * Effect: Allows to open another (proxy) port that is advertised to the master server to show up as game version 1.2 in the ingame server browser. Has no effect if sv_version is already set to "1.2". If enabled, the proxy is started on map (re)load, and shuts down (only) on server quit. Rcon commands are ignored on proxy ports. Clients connecting via the proxy always have a zero GUID, and player ping restrictions are not enforced on that port.
  * `sv_proxyEnable_1_3`
    * Type: Boolean
    * Default: False
    * Effect: Allows to open another (proxy) port that is advertised to the master server to show up as game version 1.3 in the ingame server browser. Has no effect if sv_version is already set to "1.3". If enabled, the proxy is started on map (re)load, and shuts down (only) on server quit. Rcon commands are ignored on proxy ports. Clients connecting via the proxy always have a zero GUID, and player ping restrictions are not enforced on that port.
  * `sv_proxyEnable_1_3_119`
    * Type: Boolean
    * Default: False
    * Effect: Allows to open another (proxy) port that is advertised to the master server to show up as game version 1.3 (protocol 119) in the ingame server browser. If enabled, the proxy is started on map (re)load, and shuts down (only) on server quit. Rcon commands are ignored on proxy ports. Clients connecting via the proxy always have a zero GUID, and player ping restrictions are not enforced on that port.
  * `sv_proxyForwardAddress_1_0`
    * Type: String
    * Default: "`[net_ip]:[net_port]`"
    * Effect: Destination hostname and port the proxy socket for game version 1.0 should forward packets to (see `sv_proxyAddress_1_0` dvar).
  * `sv_proxyForwardAddress_1_2`
    * Type: String
    * Default: "`[net_ip]:[net_port]`"
    * Effect: Destination hostname and port the proxy socket for game version 1.2 should forward packets to (see `sv_proxyAddress_1_2` dvar).
  * `sv_proxyForwardAddress_1_3`
    * Type: String
    * Default: "`[net_ip]:[net_port]`"
    * Effect: Destination hostname and port the proxy socket for game version 1.3 should forward packets to (see `sv_proxyAddress_1_3` dvar).
  * `sv_proxyForwardAddress_1_3_119`
    * Type: String
    * Default: "`[net_ip]:[net_port]`"
    * Effect: Destination hostname and port the proxy socket for game version 1.3 (protocol 119) should forward packets to (see `sv_proxyAddress_1_3_119` dvar).
  * `sv_reservedConfigstringBufferSize`
    * Type: Integer
    * Default: 0
    * Min. Value: 0
    * Max. Value: 8192
    * Effect: For map-mod combinations that might exceed the available gamestate data on older game versions (1.0 and 1.2), reserve some buffer for configstrings that are allocated at runtime (e.g., those populated by `PlayFxOnTag` or when playing sounds). Its value needs to be set higher in case clients with older game versions tend to run into "MAX_GAMESTATE_CHARS exceeded" errors.
  * `sv_timeoutMessages`
    * Type: Boolean
    * Default: True
    * Effect: Toggle output of player timeout game messages.
  * `sv_updateCursorHints`
    * Type: Boolean
    * Default: True
    * Effect: Disable this on maps without activatable entities (e.g., triggers, turrets, dropped weapons), to reduce server load.
  * `sv_version`
    * Type: String
    * Default: "1.3"
    * Effect: Controls the server's game version as announced to the master server and returned in server info and status requests. Also, when using version 1.0, the maps `mp_rhine` and `mp_harbor` are not available via manymaps unless added as standalone maps (like other custom maps). Requires a map reload after being changed. Recommended to be kept on "1.3" if the sv_proxy dvars are used to start proxies.
  * `sv_wwwDlDisconnectedMessages`
    * Type: Boolean
    * Default: True
    * Effect: Toggle output of download-disconnected game messages.

Other dvars (e.g., from legacy versions of libcod or other repositories):

  * `con_coloredPrints`
    * Type: Boolean
    * Default: False
    * Effect: If true, strings containing color codes (e.g., ^1 for red) are drawn in color in server console.
  * `fs_callbacks`
    * Type: String
    * Default: ""
    * Effect: Defines the relative path to the .gsc file (without file suffix) containing script callback methods/functions. If set to an empty string (as by default), the server uses `maps/mp/gametypes/_callbacksetup`.
  * `fs_library`
    * Type: String
    * Default: ""
    * Effect: Defines the relative path to the manymaps library - a folder (relative to `[fs_homepath]/[fs_game]`), that is supposed to contain map .iwd files. If set to an empty string (as by default), the server uses `[fs_homepath]/[fs_game]/Library`. This is an optional feature, the library folder does not have to exist for a server to operate correctly.
  * `g_mantleBlockEnable`
    * Type: Boolean
    * Default: True
    * Effect: If false, the invisible barrier that is created while a player is mantling/climbing over an obstacle, is not created.
  * `g_playerCollision`
    * Type: Boolean
    * Default: True
    * Effect: If false, player contents are no longer updated on each server frame, therefore allowing custom entity contents to be set after player spawn without having them overriden. Applies globally for all players on the server. Alternatively, the `overrideContents` and `setCollisionTeam` script methods can be used on a per-player or per-team basis, respectively.
  * `g_playerEject`
    * Type: Boolean
    * Default: True
    * Effect: If false, players that are close to each other (potentially causing hitbox overlaps) are no longer automatically moved away from each other.
  * `sv_allowRcon`
    * Type: Boolean
    * Default: True
    * Effect: If false, rcon commands are ignored.
  * `sv_cracked`
    * Type: Boolean
    * Default: False
    * Effect: If true, players without a valid CD key are allowed to join the server (with a zero GUID).
  * `sv_downloadMessage`
    * Type: String
    * Default: ""
    * Effect: If set to a non-empty string, players that attempt to download a file on connect are disconnected from the server, with the specified message displayed after disconnect. Applies to all game versions. Related dvar: `sv_downloadMessageAtMap`.
  * `sv_noauthorize`
    * Type: Boolean
    * Default: False
    * Effect: If true, the server will not go through the authorization process for connecting players, thus yielding a zero GUID for those players. Recommended to be enabled in case the master server returns incorrect GUIDs.
  * `sv_verifyIwds`
    * Type: Boolean
    * Default: True
    * Effect: Toggle .iwd file verification triggered by cp client commands.