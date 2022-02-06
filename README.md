# libcod for \*.ZK.\* Zombie Knights

Initial fork path:

<pre>
https://github.com/ibuddieat/zk_libcod (this repository)
└─> <a href="https://github.com/damage99/libcod">damage99/libcod</a>
    └─> <a href="https://github.com/voron00/libcod">voron00/libcod</a>
        └─> <a href="https://github.com/M-itch/libcod">M-itch/libcod</a>
            └─> <a href="https://github.com/kungfooman/libcod">kungfooman/libcod</a> (original libcod implementation)
</pre>

This extension ...
- provides interoperability between the Call of Duty&reg; 2 server and other software components and/or advanced game modifications (so-called "mods")
- intends to improve the overall game experience and security as the extended software (Call of Duty&reg; 2) is not maintained anymore since more than a decade
- has its focus on Call of Duty&reg; 2 in version 1.3, other versions are not fully supported (unless the respective offsets are added)
- is maintained for non-profit and educational purposes

List of high-level changes provided by this repository:
- Added cvars:
  * `sv_limitLocalRcon` to whitelist internal IPs at rcon rate limiting
  * `sv_logRcon` to disable (successful) rcon command logging
  * `sv_logHeartbeat` to disable logging of heartbeats that are sent to the master server
  * `g_debugEvents` to log events such as player footsteps, temporary entities etc.
  * `g_logPickup` to control logging of item pickup actions
  * `g_notifyPickup` to define whether to use the stock pickup logic or custom notify events
  * `g_debugStaticModels` to print info about static models on map load
- Added script code functions:
  * `<player> getinactivitytime()`
  * `<player> noclip("on|off|toggle")`
  * `<player> playfxforplayer(<effect id>, <position of effect>, [<forward vector>], [<up vector>])`
  * `<player> setearthquakes("on|off|toggle")`
  * `getweaponfusetime(<weapon name>)`
  * `setweaponfusetime(<weapon name>, <time in ms>)`
  * `getentitycount()`
  * `setnexttestclientname(<name>)`
  * `resettestclientnaming()`
  * `logprintconsole(<message>)`
- Added script callback functions:
  * `CodeCallback_Error`
- Removed libcod cvars:
  * `con_coloredPrints` as it may break incoming rcon commands, thus causing issues with BigBrotherBot
- Reconstructed functions (see functions with `custom_` prefix in `libcod.cpp`):
  * `Touch_Item` to gain more control over item pickup actions
  * `SV_DropClient` to disable bot disconnect messages
  * `SV_SendClientGameState` for miscellaneous game engine tests (e.g., connect configstrings)
  * `BG_AddPredictableEventToPlayerstate` et sequentes to be able to filter events
  * `G_AddEvent`
  * `MSG_WriteDeltaPlayerstate` et sequentes to be able to filter player/entity attributes
  * `MSG_WriteDeltaStruct`
  * `MSG_WriteDeltaClient`
  * `MSG_WriteDeltaEntity`
  * `MSG_WriteDeltaArchivedEntity`
  * `SV_EmitPacketEntities`
  * `SV_EmitPacketClients`
  * `SV_ArchiveSnapshot`
  * `SV_BuildClientSnapshot`
  * `SV_WriteSnapshotToClient`
  * `Com_Error` et sequentes to be able to pass errors to gsc code
  * `Scr_Error`
  * `RuntimeError_Debug`
  * `RuntimeError`
  * `CM_IsBadStaticModel` for `g_debugStaticModels` (this function name is a guess)
- Commented out several libcod functions that would make it easy to harm the server with malicious map scripts, thus currently breaking manymaps support
- Added/updated some missing/unknown declarations

Build requirements:
- gcc and g++ (with multilib on 64-bit x86 operating systems)
- libstdc++5
- MySQL client (if required by functionality)

Requirements installation (for Ubuntu 18.04.5 LTS):
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