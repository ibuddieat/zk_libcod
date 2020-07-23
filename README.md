# libcod for \*.ZK.\*

This extension has its focus on Call of Duty&reg; 2 in version 1.3. Other versions are not supported.

Fork path:
```
https://github.com/ibuddieat/zk_libcod (this repository)
└─> https://github.com/damage99/libcod
    └─> https://github.com/voron00/libcod
        └─> https://github.com/M-itch/libcod
            └─> https://github.com/kungfooman/libcod (original libcod implementation)
```

Changes provided by this repository:
- Added `sv_limitLocalRcon` cvar to whitelist internal IPs at rcon rate limiting
- Added `SV_SendClientGameState` function for miscellaneous game engine tests
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
