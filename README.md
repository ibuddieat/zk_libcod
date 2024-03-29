# libcod

**This extension ...**
- provides interoperability between the Call of Duty&reg; 2 server and other software components and/or advanced game modifications (so-called "mods"),
- intends to improve the overall game experience and security as the extended software (Call of Duty&reg; 2) is not maintained anymore since more than a decade,
- has its focus on the Linux variant of Call of Duty&reg; 2 Multiplayer in version 1.3, other versions are not fully supported (unless the respective offsets/structs are added),
- and was implemented for non-profit and educational purposes.

## ⭐ What's new?
> [!NOTE]
> These are changes in comparison to the [parent repository](doc/history.md).
- 🔢 [Added dvars](doc/added_dvars.md)
- 🔀 [Added script callback functions](doc/added_script_callback_functions.md)
- 🔣 [Added script code functions](doc/added_script_functions.md)
- 🏗 [Changed script code functions](doc/changed_script_functions.md)
- 🚨 [Improved server security](doc/security_features.md)
- ⚙ Other functional adaptions
  - Added support for map commands in custom rcon callback
  - Allow to show head icons to both teams (via `axis_allies`)
  - Excluded clients with wrong password from scoreboard
  - Patched `Com_PrintMessage` so that strings that begin with a caret are printed properly
- 🗒 Improvements in documentation and code
  - Added a crash logger that saves debugging information (locally)
  - Added documentation for all new functions and some where documentation was missing
  - Added documentation for script callback functions (stock and libcod)
    - See the updated [script docs](https://www.ibuddie.at/libcod/index.html)
  - Fixed some minor bugs
  - Tried to align some coding styles 💩

> [!IMPORTANT]
> - Use this extension at your own risk (see [LICENSE](LICENSE.md)). Some features are considered highly experimental and might not be stable enough for productive use.
> - The script API might change between major releases, documented [here](doc/changed_script_functions.md).

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