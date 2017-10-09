The Call of Duty extension *libcod* is adding new server-side functions to:

 - Call Of Duty 2 1.0
 - Call Of Duty 2 1.2
 - Call Of Duty 2 1.3

Requirements:
```
Perl
g++ (multilib for x64)
MySQL (optional)
```

Working with the source / Compiling:
```
./doit.sh cod2_1_0
./doit.sh cod2_1_2
./doit.sh cod2_1_3
```

Starting the server:
```
export LD_PRELOAD="$HOME/cod2_1_0/libcod2_1_0.so"
./cod2_lnxded +set fs_game ... +set dedicated 2 ...
```

Little overview of added functions:
- MySQL
- Linux shell command execution (adds tons of possibilites like executing external scripts, commands and pass their output to GSC)
- setVelocity, getVelocity, addVelocity (needed for the mods: portal, surf)
- keyPressed-functions for left, right, forward, backward, leanleft, leanright, jump etc., (needed for: surf, doublejump made by IzNoGod)
- setAlive-function (needed for: zombots, so xmodels are damagable without damage-trigger... zombots in stockmaps)
- disableGlobalPlayerCollision() also disableGlobalPlayerEject()
- native bot functions (bots can move, shoot, jump, melee, ads, set stance)
- weapon-functions (get/set weapon damage, firetime, maxammo without editing their files)

Engine fixes:
- Faster download for CoD2 1.0
- Directory traversal fix
- Weapon struct segfault fix
- Rate Limiter
	
Community / Help: http://killtube.org/forum.php

CoD1 users:
Libcod support for CoD1 was very basic and without almost all functions and it's been dropped.
Use php's CoDExtended: https://github.com/riicchhaarrd/CoDExtended

CoD4 users: use CoD4x: https://github.com/callofduty4x/CoD4x_Server