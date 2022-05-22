# libcod for \*.ZK.\* Zombie Knights

This extension ...
- provides interoperability between the Call of Duty&reg; 2 server and other software components and/or advanced game modifications (so-called "mods")
- intends to improve the overall game experience and security as the extended software (Call of Duty&reg; 2) is not maintained anymore since more than a decade
- has its focus on Call of Duty&reg; 2 in version 1.3, other versions are not fully supported (unless the respective offsets are added)
- is maintained for non-profit and educational purposes

List of high-level changes provided by this repository:
- Added cvars:
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
- Added script code functions:
  * `<entity> getTagAngles(<tag name>)`
  * `<entity> getTagOrigin(<tag name>)`
  * `<entity> hasTag(<tag name>)`
  * `<entity> isLinkedTo()`
  * `<player> getClientHudElemCount()`
  * `<player> getEnterTime()`
  * `<player> getGroundEntity()`
  * `<player> getInactivityTime()`
  * `<player> noclip("on|off|toggle")`
  * `<player> playFxForPlayer(<effect id>, <position of effect>, [<forward vector>], [<up vector>])`
  * `<player> playFxOnTagForPlayer(<effect id>, <entity>, <tag name>)`
  * `<player> runScriptAnimation(<scriptAnimEventType>, <isContinue>, <force>)`
  * `<player> setEarthquakes("on|off|toggle")`
  * `<player> silent("on|off|toggle")`
  * `<weapon> getWeaponItemAmmo()`
  * `<weapon> setWeaponItemAmmo(<value>)`
  * `getWeaponFuseTime(<weapon name>)`
  * `setWeaponFuseTime(<weapon name>, <time in ms>)`
  * `getEntityCount()`
  * `setNextTestClientName(<name>)`
  * `resetTestClientNaming()`
  * `logPrintConsole(<message>)`
- Changed script code functions:
  * `<player> get_userinfo()` now returns strings only, instead of string or undefined
  * `obituary(<victim>, <attacker>, <weapon>, <meansOfDeath>, [<team>], [<origin>], [<max. distance>])`
- Added script callback functions:
  * `CodeCallback_MapWeapons`
  * `CodeCallback_Error`
  * `CodeCallback_ReloadButton`
  * `CodeCallback_LeanLeftButton`
  * `CodeCallback_LeanRightButton`
  * `CodeCallback_ProneButton`
  * `CodeCallback_CrouchButton`
  * `CodeCallback_JumpButton`
  * `CodeCallback_AdsButton`
  * `CodeCallback_MeleeBreathButton`
  * `CodeCallback_HoldBreathButton`
  * `CodeCallback_FragButton`
  * `CodeCallback_SmokeButton`
- Reconstructed functions (see functions with `custom_` prefix in `libcod.cpp`):
  * `Touch_Item` to gain more control over item pickup actions
  * `SV_DropClient` to filter disconnect messages via various cvars
  * `SV_SendClientGameState` for miscellaneous game engine tests (e.g., connect configstrings)
  * `BG_AddPredictableEventToPlayerstate` et sequentes to be able to filter events
  * `G_AddEvent`
  * `MSG_WriteDeltaPlayerstate` et sequentes to be able to filter player/entity attributes
  * `MSG_WriteDeltaStruct`
  * `MSG_WriteDeltaField`
  * `MSG_WriteDeltaClient`
  * `MSG_WriteDeltaEntity`
  * `MSG_WriteDeltaArchivedEntity`
  * `SV_EmitPacketEntities`
  * `SV_EmitPacketClients`
  * `SV_ArchiveSnapshot`
  * `SV_BuildClientSnapshot`
  * `SV_WriteSnapshotToClient`
  * `Com_Error` et sequentes to be able to pass errors to gsc code
  * `Scr_ErrorInternal`
  * `RuntimeError_Debug`
  * `RuntimeError`
  * `CM_IsBadStaticModel` for `g_debugStaticModels` (this function name is a guess)
  * `G_SpawnEntitiesFromString` et sequentes to provide map weapon info via CodeCallback_MapWeapons
  * `G_CallSpawn`
  * `SV_WWWRedirectClient` for `sv_wwwDlDisconnectedMessages`
  * `SV_MapExists` for manymaps support
- Patched Com_PrintMessage so that strings that begin with a caret are printed properly
- Added compile flag `ENABLE_UNSAFE` to toggle the availability of potentially unsafe script functions that would make it easy to harm the server with malicious map scripts
- Added/updated some missing/unknown declarations
- Fixed some minor bugs
- Tried to align some coding styles

Build requirements:
- gcc and g++ (with multilib on 64-bit x86 operating systems)
- libstdc++5
- MySQL client (if required by functionality)

Requirements installation (tested on Ubuntu 18.04.5 LTS):
```
dpkg --add-architecture i386
apt update
apt install gcc-multilib
apt install g++-multilib
apt install libstdc++5:i386
apt install libmysqlclient-dev:i386
```

Creating the binary (written to `./bin`):
```
./doit.sh
```

Initial fork path:

<pre>
https://github.com/ibuddieat/zk_libcod (this repository)
└─> <a href="https://github.com/damage99/libcod">damage99/libcod</a>
    └─> <a href="https://github.com/voron00/libcod">voron00/libcod</a>
        └─> <a href="https://github.com/M-itch/libcod">M-itch/libcod</a>
            └─> <a href="https://github.com/kungfooman/libcod">kungfooman/libcod</a> (original libcod implementation)
</pre>

Other important references:

- Killtube~ (https://killtube.org/)
- Enemy-Territory (https://github.com/id-Software/Enemy-Territory)
- CoD4x Server (https://github.com/callofduty4x/CoD4x_Server)
- Ghidra (https://github.com/NationalSecurityAgency/ghidra)