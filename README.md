# libcod

This extension ...
- provides interoperability between the Call of Duty&reg; 2 server and other software components and/or advanced game modifications (so-called "mods")
- intends to improve the overall game experience and security as the extended software (Call of Duty&reg; 2) is not maintained anymore since more than a decade
- has its focus on Call of Duty&reg; 2 Multiplayer in version 1.3, other versions are not fully supported (unless the respective offsets/structs are added)
- was implemented for non-profit and educational purposes

List of high-level changes provided by this repository:
- [Added cvars](doc/added_cvars.md)
- [Added script callback functions](doc/added_script_callback_functions.md)
- [Added script code functions](doc/added_script_functions.md)
- Changed script code functions:
  * `<player> get_userinfo()` now returns strings only, instead of string or undefined
  * `obituary(<victim>, <attacker>, <weapon>, <meansOfDeath>, [<team>], [<origin>], [<max. distance>])`
- [Reconstructed some game functions](doc/reconstructed_functions.md) to be able to implement the extensions mentioned above
- Patched `Com_PrintMessage` so that strings that begin with a caret are printed properly
- Added compile flag `ENABLE_UNSAFE` to toggle the availability of potentially unsafe script functions that would make it easy to harm the server with malicious map scripts
- Added/updated some missing/unknown declarations in [declarations.hpp](code/declarations.hpp)
- Fixed some minor bugs
- Tried to align some coding styles
- Added documentation for script callback functions (stock and libcod)

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

Creating the binary (written to `./code/bin`):
```
cd code
./doit.sh
```

Initial fork path:

<pre>
  Code:
https://github.com/ibuddieat/zk_libcod (this repository)
├─> <a href="https://github.com/damage99/libcod/tree/d60a4b6ce9ba7225d2a84fe68b233cd3f6aae001">damage99/libcod</a>
│    └─> <a href="https://github.com/voron00/libcod">voron00/libcod</a>
│       └─> <a href="https://github.com/M-itch/libcod">M-itch/libcod</a>
│           └─> <a href="https://github.com/kungfooman/libcod">kungfooman/libcod</a> (original libcod implementation)
│
│ Script reference documentation:
└─> <a href="https://github.com/M-itch/codscriptdoc/tree/7438b23a810b905d2c0f2c8d79cbb1d7948b0482">M-itch/codscriptdoc</a>
</pre>

Other important references:

- Killtube~ (https://killtube.org/)
- Enemy-Territory (https://github.com/id-Software/Enemy-Territory)
- CoD4x Server (https://github.com/callofduty4x/CoD4x_Server)
- Ghidra (https://github.com/NationalSecurityAgency/ghidra)
- \*.ZK.\* Zombie Knights (https://discord.gg/8g9uWW5)