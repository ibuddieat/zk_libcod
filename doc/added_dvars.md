Added dvars:
  * `g_corpseHit`
    * Type: Boolean
    * Default: True
    * Effect: Toggle bullet hit events (sound + effect) for corpses during death animation.
  * `g_debugCallbacks`
    * Type: Boolean
    * Default: False
    * Effect: Toggle debug logging of callback function assignments on server start and callback function calls at runtime.
  * `g_debugEvents`
    * Type: Boolean
    * Default: False
    * Effect: Toggle debug logging of events such as player footsteps, spawn of temporary entities etc.
  * `g_debugStaticModels`
    * Type: Boolean
    * Default: False
    * Effect: Toggle logging of info about static models on map load.
  * `g_dumpVoiceData`
    * Type: Boolean
    * Default: False
    * Effect: Toggle storage of voice data into voiceData.spx, similar to voice.wav for clients if winvoice_save_voice is set to "1". Data is speex-encoded.
  * `g_logPickup`
    * Type: Boolean
    * Default: True
    * Effect: Toggle game logging of item pickup actions.
  * `g_resetSlide`
    * Type: Boolean
    * Default: False
    * Effect: Potential fix for slide-like player movement after receiving damage from falling not being reset.
  * `g_safePrecache`
    * Type: Boolean
    * Default: False
    * Effect: If true, exceeding the limits of loadFx or precacheModel will generate console log warnings instead of stopping the server. Excess effects will be replaced by (= displayed as) `fx/misc/missing_fx.efx` and excess models as `xmodel/default_static_model`. These default assets are loaded automatically (and therefore occupy one config string each) if the dvar is set to true.
  * `g_spawnMapTurrets`
    * Type: Boolean
    * Default: True
    * Effect: If false, turrets loaded from map files are neither precached nor spawned. Instead, the list of loaded turrets is sent to the `CodeCallback_MapTurretsLoad` script callback function.
  * `g_spawnMapWeapons`
    * Type: Boolean
    * Default: True
    * Effect: If false, weapons loaded from map files are neither precached nor spawned. Instead, the list of loaded weapons is sent to the `CodeCallback_MapWeaponsLoad` script callback function.
  * `g_triggerMode`
    * Type: Integer
    * Default: 1
    * Min. Value: 0
    * Max. Value: 2
    * Effect: Toggle experimental fix for broken map damage and use-touch triggers. Possible values: `0`: Triggers disabled. `1`: Stock behavior, fix disabled. `2`: Fix enabled.
  * `logErrors`
    * Type: Boolean
    * Default: False
    * Effect: If true, log (non-terminal) script runtime errors as if developer mode was enabled.
  * `logfileName`
    * Type: String
    * Default: "console_mp_server.log"
    * Effect: Controls the console log file name. Should be set before the stock "logfile" dvar is set to a value greater than zero.
  * `logfileRotate`
    * Type: Integer
    * Default: 0
    * Min. Value: 0
    * Max. Value: 1000
    * Effect: Defines how many console log files are preserved on a server restart. Naming scheme: &lt;logfileName dvar value&gt;.&lt;incrementing integer&gt;
  * `logTimestamps`
    * Type: Boolean
    * Default: False
    * Effect: If true, prepend a timestamp (example: "Mon Apr  4 13:37:00 2023 ") to each console log entry.
  * `scr_turretDamageName`
    * Type: Boolean
    * Default: False
    * Effect: If true, turret weapon names are passed to Callback_PlayerDamage when using a turret, instead of the player's current weapon slot's weapon. Also fixes the game issue where grenade (and other) kills are shown as turret kills while the player is using a turret.
  * `sv_disconnectMessages`
    * Type: Boolean
    * Default: True
    * Effect: Toggle output of "Game session no longer available" game messages.
  * `sv_botKickMessages`
    * Type: Boolean
    * Default: True
    * Effect: Toggle output of bot disconnect game messages.
  * `sv_botReconnectMode`
    * Type: Integer
    * Default: 0
    * Min. Value: 0
    * Max. Value: 2
    * Effect: 0: Stock behavior, bots stay in connecting state after map switch and get kicked after sv_timeout. 1: Bots get kicked right on map switch. 2: Bots properly reconnect on map switch, using the same slot each.
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
  * `sv_minimizeSysteminfo`
    * Type: Integer
    * Default: 0
    * Min. Value: 0
    * Max. Value: 3
    * Effect:
      * If set to 1, removes some dvars from systeminfo config string so that more .iwd files can be used in a mod without having clients running into "iwd sum/name mismatch errors". Instead, the cod info flag will be set on them so that they are still synchronized with the clients on connect. However, further changes during runtime are not synchronized until gamestate is resent (e.g., during a map switch). Therefore, when using this functionality, it is advised to synchronize these dvars with the clients via script whenever their value changes. Dvars that still stay in the systeminfo config string, unless configured otherwise, are:
      	* cl_allowDownload
      	* cl_wwwDownload
      	* com_hunkMegs
      	* fs_game
      	* sv_serverid
      	* and the jump_... dvars if jump.cpp is compiled.
    	* If set to 2 or greater, also overrides jump-related dvars set in jump.cpp.
    	* If set to 3, also overrides download-related dvars set in libcod.cpp. Note that sending these dvars this way is too late to auto-enable downloading right on a client's first connect. Instead, a reconnect is necessary for the client then, if downloading wasn't enabled already in the first place.
  * `sv_timeoutMessages`
    * Type: Boolean
    * Default: True
    * Effect: Toggle output of player timeout game messages.
  * `sv_verifyIwds`
    * Type: Boolean
    * Default: True
    * Effect: Toggle IWD file verification triggered by cp client commands.
  * `sv_wwwDlDisconnectedMessages`
    * Type: Integer
    * Default: 1
    * Min. Value: 0
    * Max. Value: 2
    * Effect: Toggle output of download-disconnected game messages. A value of `2` prints the relative path to the file requested by the connecting player.

Note: All default values of the dvars listed above are defined to preserve the stock behavior of the game.