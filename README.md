# ${\textsf{\color{red}lib\color{green}cod\color{blue}2}\ }$

**This extension ...**
- provides interoperability between the Call of Duty&reg; 2 server and other software components and/or advanced game modifications (so-called "mods"),
- intends to improve the overall game experience and security as the extended software (Call of Duty&reg; 2) is not maintained anymore since more than a decade,
- has its focus on the Linux variant of Call of Duty&reg; 2 Multiplayer and can be used with the original game binary,
- and was implemented for non-profit and educational purposes.

## ⭐ What's new?

The extension requires the [server binary for version 1.3](cod2_lnxded) to be used as basis and **supports all three major game versions** (check out the `sv_version` and proxy dvars), including the ability to let players from all those versions to connect to the same server. The following list documents changes in comparison to the [parent repository](doc/history.md):

- 🔢 [Added dvars](doc/added_dvars.md) (see example [libcod.cfg](doc/libcod.cfg))
- 🔀 [Added script callback functions](doc/added_script_callback_functions.md)
- 🔣 [Added script code functions](doc/added_script_functions.md)
- 🏗 [Changed script code functions](doc/changed_script_functions.md)
- 🚨 [Improved server security](doc/security_features.md)
- ⚙ Other functional adaptions
  - Added support for map commands in custom rcon callback
  - Allow to show head icons to both teams (via `axis_allies`)
  - Excluded clients with wrong password from scoreboard
  - Patched `Com_PrintMessage` so that strings that begin with a caret are printed properly in console
- 🗒 Improvements in documentation and code
  - Added a crash logger that saves debugging information (locally)
  - Added documentation for all new functions and some where documentation was missing
  - Added documentation for script callback functions (stock and libcod)
    - See the updated [script docs](https://www.ibuddie.at/libcod/index.html)
  - Fixed some minor bugs
  - Tried to align some coding styles 💩

> [!IMPORTANT]
> The script API might change between major releases, as documented [here](doc/changed_script_functions.md).

## 💡 Knowledge base
- [Build and use](doc/building.md)
- [Known issues and debugging](doc/debugging.md)

### 🤝 Acknowledgements
- IzNoGoD for feedback, testing and further ideas
- Prawy for feedback and discovery of bugs
- Raphael for valid security concerns

### 🔗 Other important references
- [\*.ZK.\* Zombie Knights](https://discord.gg/f9uS49Hhcs)
- [Call of Duty Extended](https://github.com/xtnded/codextended)
- [CoD2 Proxy Server](https://github.com/filthyfreak/CoD2_Proxy_Server)
- [CoD4x Server](https://github.com/callofduty4x/CoD4x_Server)
- [Enemy-Territory](https://github.com/id-Software/Enemy-Territory)
- [Ghidra](https://github.com/NationalSecurityAgency/ghidra)
- [Killtube~](https://killtube.org/)
- [Reverse Engeneering of Call of Duty 2 Server](https://github.com/voron00/CoD2rev_Server)
- [The Speex Project](https://www.speex.org/)

### 💬 Contact & contributing
- Discord: ibuddie
- E-Mail: [ibuddie.at@gmail.com](mailto:ibuddie.at@gmail.com)
- Want to contribute? Let's get in touch via one of the platforms listed above, or just submit a pull request.

> [!WARNING]
> Use this extension at your own risk (see [LICENSE](LICENSE.md)). Some features are considered highly experimental and might not be stable enough for productive use.