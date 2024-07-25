## Known issues
- Known issues that can lead to a server crash:
  - Using `wait` functions within callback functions where this is advised against, see [added script callback functions](doc/added_script_callback_functions.md)
  - Using `executeCommand()` with map commands (e.g., map_rotate)
  - Removing a turret in a script callback function, if the damage comes from said turret
  - Changing the `sv_maxclients` dvar during active gameplay (e.g., by using `SetCvar()` in script code)
  - Kicking players in the connect script callback function, without delay
  - Using the custom notify script callback functions without a `waittillframeend` before calling `Spawn()` on a player
  - Extremely high entity velocity (see `enableGravity()`)
  - [Unconfirmed] Using `Attach()` if a tag/bone exists more than once in the combined models
- Known issues leading to functional problems:
  - Using `player SetModel("");` is not recommended as it can lead to god mode situations, or worse
  - Using non-default team collision settings causes friendly player names (stock hud) to disappear on the affected players. Depending on the used settings, they might reappear during killcam

Most of the issues listed here are problems that are present in the original game engine already, meaning that they were *not* introduced by libcod add-ons.

## Debugging
- To get a debug build version, use `./doit.sh debug` in the build process
- The server attempts to write a crash.log file in the server's working directory on unexpected termination (e.g., on segmentation faults), containing a stack trace
- Running the server within GDB:
  - `gdb -ex r --args env LD_PRELOAD=./bin/libcod2.so ./cod2_lnxded +set fs_game ...` etc.