Added cvars:
  * `g_debugEvents` to log events such as player footsteps, temporary entities etc.
  * `g_debugStaticModels` to print info about static models on map load
  * `g_logPickup` to control logging of item pickup actions
  * `g_notifyPickup` to define whether to use the stock pickup logic or custom notify events
  * `g_spawnMapWeapons` to control precaching & spawning of weapons in map files
  * `sv_disconnectMessages` to disable "Game session no longer available" messages
  * `sv_botKickMessages` to disable bot disconnect messages being shown to other players
  * `sv_kickMessages` to generally disable player kick messages being shown to other players
  * `sv_limitLocalRcon` to whitelist internal IPs at rcon rate limiting
  * `sv_logHeartbeats` to disable logging of heartbeats that are sent to the master server
  * `sv_logRcon` to disable (successful) rcon command logging
  * `sv_timeoutMessages` to disable player timeout messages being shown to other players
  * `sv_wwwDlDisconnectedMessages` to adapt/hide download messages being shown to other players

Note: All default values of the cvars listed above are defined to preserve the stock behavior of the game.