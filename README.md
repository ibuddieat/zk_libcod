# libcod for \*.ZK.\* Zombie Knights

This extension has its focus on Call of Duty&reg; 2 in version 1.3. Other versions are not supported.

Initial fork path:

<pre>
https://github.com/ibuddieat/zk_libcod (this repository)
└─> <a href="https://github.com/damage99/libcod">damage99/libcod</a>
    └─> <a href="https://github.com/voron00/libcod">voron00/libcod</a>
        └─> <a href="https://github.com/M-itch/libcod">M-itch/libcod</a>
            └─> <a href="https://github.com/kungfooman/libcod">kungfooman/libcod</a> (original libcod implementation)
</pre>

Changes provided by this repository:
- Added cvars:
  * `sv_limitLocalRcon` to whitelist internal IPs at rcon rate limiting
  * `sv_logRcon` to disable (successful) rcon command logging
  * `sv_logHeartbeat` to disable logging of heartbeats that are sent to the master server
  * `g_debugEvents` to log events such as player footsteps, temporary entities etc.
  * `g_logPickup` to control logging of item pickup actions
  * `g_notifyPickup` to define whether to use the stock pickup logic or custom notify events
- Removed libcod cvars:
  * `con_coloredPrints` as it may break incoming rcon commands, thus causing issues with BigBrotherBot
- Reconstructed functions:
  * `custom_Touch_Item` to gain more control over item pickup actions
  * `custom_SV_DropClient` to disable bot disconnect messages
  * `custom_SV_SendClientGameState` for miscellaneous game engine tests
  * `custom_BG_AddPredictableEventToPlayerstate` to be able to filter events
  * `custom_G_AddEvent` to be able to filter events
  * `custom_G_TempEntity` to be able to filter events
  * `custom_MSG_WriteDeltaPlayerstate`
  * `custom_MSG_WriteDeltaStruct`
- Added script code functions:
  * `<player> noclip("on|off|toggle")`
  * `<player> getinactivitytime()`
  * `getweaponfusetime(<weapon name>)`
  * `setweaponfusetime(<weapon name>, <time in ms>)`
  * `getentitycount()`
  * `setnexttestclientname(<name>)`
  * `resettestclientnaming()`
  * `logprintconsole(<message>)`
- Disabled several libcod functions that would make it easy to harm the server with malicious map scripts
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
./doit.sh cod2_1_3
```
