New script code methods:
  * `<bot> setWalkValues(<forward_count>, <right_count>)`
  * `<entity> getTagAngles(<tag name>)`
  * `<entity> getTagOrigin(<tag name>)`
  * `<entity> hasTag(<tag name>)`
  * `<entity> isLinkedTo()`
  * `<player> earthquakeForPlayer(<scale>, <duration>, <source>, <radius>)`
  * `<player> getClientHudElemCount()`
  * `<player> getEnterTime()`
  * `<player> getGroundEntity()`
  * `<player> getInactivityTime()`
  * `<player> getPlayerstateFlags()`
  * `<player> muteClient(<id>)`
  * `<player> noclip("on|off|toggle")`
  * `<player> objective_player_add(<objective_number>, <state>, <position>, <iconshader>)`
  * `<player> objective_player_delete(<objective_number>)`
  * `<player> objective_player_icon(<objective_number>, <icon>)`
  * `<player> objective_player_position(<objective_number>, <position>)`
  * `<player> objective_player_state(<objective_number>, <state>)`
  * `<player> playFxForPlayer(<effect id>, <position of effect>, [<forward vector>], [<up vector>])`
  * `<player> playFxOnTagForPlayer(<effect id>, <entity>, <tag name>)`
  * `<player> runScriptAnimation(<scriptAnimEventType>, <isContinue>, <force>)`
  * `<player> setEarthquakes("on|off|toggle")`
  * `<player> silent("on|off|toggle")`
  * `<player> stopUseTurret()`
  * `<player> unmuteClient(<id>)`
  * `<turret> getTurretOwner()`
  * `<weapon> getWeaponItemAmmo()`
  * `<weapon> setWeaponItemAmmo(<value>)`

New script code functions:
  * `getWeaponFuseTime(<weapon name>)`
  * `setWeaponFuseTime(<weapon name>, <time in ms>)`
  * `getEntityCount()`
  * `logPrintConsole(<message>)`
  * `resetTestClientNaming()`
  * `setNextTestClientName(<name>)`