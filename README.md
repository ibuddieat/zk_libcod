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
- Added `sv_limitLocalRcon` cvar to whitelist internal IPs at rcon rate limiting
- Added `custom_SV_DropClient` function to disable bot disconnect messages
- Added `custom_SV_SendClientGameState` function for miscellaneous game engine tests
- Added `g_debugEvents` cvar to log events such as player footsteps, temporary entities etc.
- Added `custom_BG_AddPredictableEventToPlayerstate` function to be able to filter aforementioned events
- Added `custom_G_AddEvent` function to be able to filter aforementioned events
- Added `custom_G_TempEntity` function to be able to filter aforementioned events
- Added `custom_MSG_WriteDeltaPlayerstate` function
- Added script code functions:
  * `<player> noclip("on|off|toggle")`
  * `<player> getinactivitytime()`
  * `getweaponfusetime(<weapon name>)`
  * `setweaponfusetime(<weapon name>, <time in ms>)`
  * `getentitycount()`
  * `setnexttestclientname()`
  * `resettestclientnaming()`
- Removed `con_coloredPrints` cvar as it may break incoming rcon commands (-> issues with BigBrotherBot)
- Disabled several functions that would make it easy to harm the server with malicious map scripts

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
