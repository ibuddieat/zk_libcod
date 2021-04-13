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
- Added script code functions:
  * `<player> noclip(<value>);`
  * `<player> getinactivitytime();`
- Removed `con_coloredPrints` as it may break incoming rcon commands (-> issues with BigBrotherBot)
- Disabled several functions that would make it easy to harm the server with malicious map scripts

Build requirements:
```
g++ (multilib for x64)
MySQL (optional)
```

Creating the binary (written to `./bin`):
```
./doit.sh cod2_1_3
```
