# ⚙ Other functional improvements & adaptions

> [!NOTE]
> * Changes to existing script code functions or methods are listed [here](changed_script_functions.md)
> * PunkBuster functionality has not been removed, but it is deprecated and no longer being maintained
> * Some of the changes listed here need to be enabled or configured with [newly introduced dvars](added_dvars.md)

- Multi-version support from 1.0 to 1.3, including 1.3 via Game Pass
- Added bullet drop mechanics (see `g_bulletDrop` and `g_bulletDropMaxTime` dvars)
- Added realtime sound streaming through voice chat (see `loadSoundFile` script function)
- Added advanced entity snapshot control (see `sv_autoAddSnapshotEntities` and `sv_maxSnapshotEntities` dvars)
- Added network code improvements to help older protocol versions with low gamestate and snapshot sizes
- Faster direct file download for game version 1.0 (see `sv_fastDownload` dvar)
- Increased max. direct file download size from 64 MB to 512 MB
- Increased level script path length limit from 64 characters to 128
- Added multi-part map support for manymaps
- Other manymaps improvements, like automatic cleanup of symbolic links and main folder support
- Added support for map commands in custom rcon script code callback
- Added ability to show head icons to both teams via `self.headiconteam = "axis_allies";`
- Color codes at the beginning of console log entries are no longer stripped
- Changed formatting of port numbers to show positive numbers only
- Added a crash logger that saves debugging information (locally)
- Added several other fixes and NULL-pointer checks
- The server now halts if the console log file (if enabled) cannot be created/opened for writing
- Excluded client userinfo updates from flood protection so that input is not lost
- Excluded clients with wrong password from scoreboard
- Disabled broken HTTP download for protocol version 119 (1.3 via Game Pass)
- Effect entities from weapons with a `projExplosionEffect` are now properly cleaned up after their effect duration
- Added a plugin-like [extra interface](../code/gsc_extra.hpp) with frequently used hooks, for libcod developers