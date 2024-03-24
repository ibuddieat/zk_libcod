## References & initial fork path
A big thank you to everyone involved in the development of the following projects:
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

> [!IMPORTANT]
> - Release notes before version 10.0 were aligned with other (private) \*.ZK.\* repositories, therefore these notes do not necessarily reflect what was new in *this* repository.

## Historical background
Back in late 2019, we at \*.ZK.\* (a gaming community that shaped itself within the zombie server scene, starting in 2010) ran into some rcon-related limitations with the libcod implementations available at that time. Therefore we decided to take a closer look at the inner workings of this extension (specifically, damage99's version listed above).
Another limitation that bothered us was that, after a few months, we intended to switch our servers to another fs_game folder, but then were about to lose client-sided access to player-account-related configuration data (i.e., a config file containing the logon token for authentication purposes against our servers), due to restricted directory traversal abilities on the client side (which are, after all, a good thing). When looking for a solution we even considered to implement a hard-coded exploit that would move said config during the account migration process. However, we eventually found and used another approach, implemented in pure game code (gsc + menu files) so that it would work without libcod as well.
We then started to add more and more functions to our fork, and especially since late 2021 we're adding (and usually also documenting) whatever feels necessary to help us implementing new features and/or mods.