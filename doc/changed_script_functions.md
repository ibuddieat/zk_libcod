# Changed with or since release v12.0:

Script code functions:
  * Renamed `exponent()` to `pow()`

# Changed with or since release v11.0:

Script code methods:
  * Aligned return values of `getStance()` with those expected by `setStance()`
  * Replaced `silent()` by `enableSilent()` and `disableSilent()`
  * Replaced `setEarthquakes()` by `enableEarthquakes()` and `disableEarthquakes()`
  * Reverted merge of `enableItemPickup()` and `disableItemPickup()` into `itemPickup()`
  * Renamed `adsAim()` to `setAim()`
  * Renamed `throwNade()` to `throwGrenade()`
  * Renamed `clientCommand()` to `processClientCommand()`
  * Renamed `clientUserinfoChanged()` to `processClientUserinfoChange()`
  * Renamed `printOutOfBand()` to `connectionlessPacketToClient()`
  * Renamed `connectionlesspacket()` to `connectionlessPacketToServer()`
  * Renamed `get_userinfo()` to `getUserinfo()`
  * Renamed `set_userinfo()` to `setUserinfo()`
  * Renamed `setg_speed()` to `setSpeed()`
  * Renamed `getg_speed()` to `getSpeed()`
  * Renamed `setg_gravity()` to `setGravity()`
  * Renamed `getg_gravity()` to `getGravity()`
  * Renamed `play_anim()` to `setAnimation()`
  * Renamed `runScriptAnimation()` to `playScriptAnimation()`
  * Renamed `setjump_height()` to `setJumpHeight()`
  * Renamed `setjump_slowdownenable()` to `setJumpSlowdownEnable()`

Script code functions:
  * Merged `kick2()` into `kick()` (same usage)
  * Renamed `exec_async_create()` to `execute_async_create()`
  * Renamed `exec_async_create_nosave()` to `execute_async_create_nosave()`
  * Renamed `exec_async_checkdone()` to `execute_async_checkdone()`
  * Renamed `remoteCommand()` to `processRemoteCommand()`
  * Renamed `cmd_executestring()` to `executeCommand()`
  * Renamed `sendGameServerCommand()` to `sendCommandToClient()`
  * Renamed `fs_loaddir()` to `loadDir()`
  * Renamed `g_findConfigStringIndexOriginal()` to `findConfigStringIndexOriginal()`
  * Renamed `g_findConfigStringIndex()` to `findConfigStringIndex()`
  * `bulletTracePassed(<start>, <end>, <hit characters>, <ignore entity>, [<content mask>])` got one more parameter for custom content masks

# Changed since release v10.0:

Script code methods:
  * `<entity> setHintString(<string>)` now also supports trigger_radius entities
  * `<player> disableItemPickup()` and `<player> enableItemPickup()` were merged into `<player> itemPickup(<enabled>)`
  * `<player> finishPlayerDamage()` got one more parameter to toggle bullet impact events
  * `<player> get_userinfo()` now returns strings only, instead of string or undefined
  * `<player> getClientState()` was renamed to `<player> getClientConnectState()`

Script code functions:
  * `bulletTrace(<start>, <end>, <hit characters>, <ignore entity>, [<content mask>])` got one more parameter for custom content masks and now also returns surface flags
  * `getEntByNum(<entity id>)` can be used without developer mode
  * `getType(<parameter>)` now correctly resolves object types (e.g., struct, array, entity)
  * `obituary(<victim>, <attacker>, <weapon>, <meansOfDeath>, [<team>], [<origin>], [<max. distance>])` got three more parameters
  * `sightTracePassed(<start>, <end>, <hit characters>, <ignore entity>, [<content mask>])` got one more parameter for custom content masks