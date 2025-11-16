# libcod2

**This extension ...**
- provides interoperability between the Call of Duty&reg; 2 server and other software components and/or advanced game modifications (so-called "mods"),
- intends to improve the overall game experience and security as the extended software (Call of Duty&reg; 2) is not maintained anymore since more than a decade,
- has its focus on the Linux variant of Call of Duty&reg; 2 Multiplayer and can be used with the original game binary,
- and was implemented for non-profit and educational purposes.

It requires the [server binary](cod2_lnxded) and .iwd files for game version 1.3 to be used as basis and **supports all four official game protocol versions** (check out the `sv_version` and proxy dvars), including the ability to let players from all those versions to connect to the same server. All features in this extension work with the original (stock) client executables.

## ⭐ What's new?

The following list documents changes in comparison to [earlier libcod versions](doc/history.md):

- 🔢 [Added dvars](doc/added_dvars.md) (see example [libcod.cfg](doc/libcod.cfg))
- 🔣 [Added script code functions](doc/added_script_functions.md)
- 🏗 [Changed script code functions](doc/changed_script_functions.md)
- 🚨 [Improved server security](doc/security_features.md)
- 📑 [Improved script documentation](https://www.ibuddie.at/libcod/index.html)
- ⚙ [Other functional adaptions](doc/other_functional_adaptions.md)

> [!IMPORTANT]
> The script API might change between major releases, as documented [here](doc/changed_script_functions.md).

> [!WARNING]
> Use this extension at your own risk (see [LICENSE](LICENSE.md)). New features are considered highly experimental and might not be stable enough for productive use.

## 💡 Knowledge base
- [Build and use](doc/building.md)
- [Known issues and debugging](doc/debugging.md)
- [Tuning and performance](doc/tuning.md)

### 🤝 Acknowledgements
- .BadBoy for feedback, testing and more ideas
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
- 👾 Discord: ibuddie
- 📧 E-Mail: [ibuddie.at@gmail.com](mailto:ibuddie.at@gmail.com)
- ✍️ Want to contribute? Let's get in touch via one of the platforms listed above, or just submit a pull request.
- 🪙 Want to donate? PayPal me at the email address above, or send some spare Satoshis to:

bc1qm527saeznwqjsz2y0nsg96744xxytyrshkwhjd

<img width="256" height="256" alt="grafik" src="https://github.com/user-attachments/assets/da079a6b-f17d-4a1e-812f-552506bb23de" />
