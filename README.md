# libcod

**This extension ...**
- provides interoperability between the Call of Duty&reg; 2 server and other software components and/or advanced game modifications (so-called "mods")
- intends to improve the overall game experience and security as the extended software (Call of Duty&reg; 2) is not maintained anymore since more than a decade
- has its focus on the Linux variant of Call of Duty&reg; 2 Multiplayer in version 1.3, other versions are not fully supported (unless the respective offsets/structs are added)
- was implemented for non-profit and educational purposes

**List of high-level changes provided by this repository**
<br>(in comparison to its parent repository, see the initial fork path below)
- [Added cvars](doc/added_cvars.md)
- [Added script callback functions](doc/added_script_callback_functions.md)
- [Added script code functions](doc/added_script_functions.md)
- [Changed script code functions](doc/changed_script_functions.md)
- Patched `Com_PrintMessage` so that strings that begin with a caret are printed properly
- Patched the `clonePlayer` script function to not crash the server if called on a player without model
- Added compile flag `ENABLE_UNSAFE` to toggle the availability of potentially unsafe script functions that would make it easy to harm the server with malicious map scripts
- Added/updated some missing/unknown declarations in [declarations.hpp](code/declarations.hpp)
- Fixed some minor bugs
- Tried to align some coding styles
- Added documentation for script callback functions (stock and libcod)
  + For details, see the updated [Script Docs](https://www.ibuddie.at/libcod/index.html)

**Supported/Tested GCC+OS**
- gcc (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0
- gcc (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0 (with Speex)

**Build requirements**
- gcc and g++ (with multilib on 64-bit x86 operating systems)
- libstdc++5
- MySQL client (if required by functionality)
- Speex 1.1.9 (can be disabled in [config.hpp](code/config.hpp))

**Base requirements installation (for 64-bit Ubuntu 18.04.5 LTS)**
```
sudo dpkg --add-architecture i386
sudo apt update
sudo apt install gcc-multilib g++-multilib libstdc++5:i386
```

**MySQL requirements installation (for 64-bit Ubuntu 18.04.5 LTS)**
```
sudo apt install libmysqlclient-dev:i386
```

**Custom sound file docs (for 64-bit Ubuntu 18.04.5 LTS)**
- [Speex installation](doc/install_speex.md)
- [Audio file conversion](doc/convert_audio_files.md)
- Note: This feature increases RAM usage by about 500 MB. Disable `COMPILE_CUSTOM_VOICE` in [config.hpp](code/config.hpp) to avoid this

**Creating the binary (written to `./code/bin`)**
```
cd code
./doit.sh

# Or without Speex:
./doit.sh nospeex
```

**Initial fork path**
<br>A big thank you to everyone involved in the development of the following projects:
<pre>
  Code:
https://github.com/ibuddieat/zk_libcod (this repository)
├─> <a href="https://github.com/damage99/libcod/tree/d60a4b6ce9ba7225d2a84fe68b233cd3f6aae001">damage99/libcod</a>
│   └─> <a href="https://github.com/voron00/libcod">voron00/libcod</a>
│       └─> <a href="https://github.com/M-itch/libcod">M-itch/libcod</a>
│           └─> <a href="https://github.com/kungfooman/libcod">kungfooman/libcod</a> (original libcod implementation)
│
│ Script reference documentation:
└─> <a href="https://github.com/M-itch/codscriptdoc/tree/7438b23a810b905d2c0f2c8d79cbb1d7948b0482">M-itch/codscriptdoc</a>
</pre>

**Historical background**

Back in late 2019, we at \*.ZK.\* (a gaming community that shaped itself within the zombie server scene, starting in 2010) ran into some rcon-related limitations with the libcod implementations available at that time. Therefore we decided to take a closer look at the inner workings of this extension (specifically, damage99's version listed above).
Another limitation that bothered us was that, after a few months, we intended to switch our servers to another fs_game folder, but then were about to lose client-sided access to player-account-related configuration data (i.e., a config file containing the logon token for authentication purposes against our servers), due to restricted directory traversal abilities on the client side (which are, after all, a good thing). When looking for a solution we even considered to implement a hard-coded exploit that would move said config during the account migration process. However, we eventually found and used another approach, implemented in pure game code (gsc + menu files) so that it would work without libcod as well.
We then started to add more and more functions to our fork, and especially since late 2021 we're adding (and usually also documenting) whatever feels necessary to help us implementing new features and/or mods.

**Other important references**
- \*.ZK.\* Zombie Knights (https://discord.gg/f9uS49Hhcs)
- Killtube~ (https://killtube.org/)
- Enemy-Territory (https://github.com/id-Software/Enemy-Territory)
- CoD4x Server (https://github.com/callofduty4x/CoD4x_Server)
- Ghidra (https://github.com/NationalSecurityAgency/ghidra)
- The Speex Project (https://www.speex.org/)

**Further notes**
- Release notes before version 10.0 were aligned with other (private) \*.ZK.\* repositories, therefore these notes do not necessarily reflect what was new in *this* repository.
- Use this extension at your own risk (see [LICENSE](LICENSE.md)). Some features are considered highly experimental and might not be stable enough for productive use.
