Added cvars:
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
  * `g_spawnMapTurrets`
    * Type: Boolean
    * Default: True
    * Effect: If false, turrets loaded from map files are neither precached nor spawned. Instead, the list of loaded turrets is sent to the `CodeCallback_MapTurretsLoad` script callback function.
  * `g_spawnMapWeapons`
    * Type: Boolean
    * Default: True
    * Effect: If false, weapons loaded from map files are neither precached nor spawned. Instead, the list of loaded weapons is sent to the `CodeCallback_MapWeaponsLoad` script callback function.
  * `logfileName`
    * Type: String
    * Default: "console_mp_server.log"
    * Effect: Controls the console log file name. Should be set before the stock "logfile" cvar is set to a value greater than zero.
  * `logfileRotate`
    * Type: Integer
    * Default: 0
    * Min. Value: 0
    * Max. Value: 1000
    * Effect: Defines how many console log files are preserved on a server restart. Naming scheme: &lt;logfileName cvar value&gt;.&lt;incrementing integer&gt;
  * `sv_disconnectMessages`
    * Type: Boolean
    * Default: True
    * Effect: Toggle output of "Game session no longer available" game messages.
  * `sv_botKickMessages`
    * Type: Boolean
    * Default: True
    * Effect: Toggle output of bot disconnect game messages.
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
  * `sv_timeoutMessages`
    * Type: Boolean
    * Default: True
    * Effect: Toggle output of player timeout game messages.
  * `sv_wwwDlDisconnectedMessages`
    * Type: Integer
    * Default: 1
    * Min. Value: 0
    * Max. Value: 2
    * Effect: Toggle output of download-disconnected game messages. A value of `2` prints the relative path to the file requested by the connecting player.

Note: All default values of the cvars listed above are defined to preserve the stock behavior of the game.