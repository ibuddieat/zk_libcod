New script code methods:
  * `<bot> setWalkValues(<forward_count>, <right_count>)`
  * `<entity> getTagAngles(<tag name>)`
  * `<entity> getTagOrigin(<tag name>)`
  * `<entity> hasTag(<tag name>)`
  * `<entity> isLinkedTo()`
  * `<entity> spawnGrenade(<weapon>, <origin>, [<direction>], [<velocity>], [<fuseTime>])`
  * `<player> earthquakeForPlayer(<scale>, <duration>, <source>, <radius>)`
  * `<player> getClientHudElemCount()`
  * `<player> getEnterTime()`
  * `<player> getGroundEntity()`
  * `<player> getInactivityTime()`
  * `<player> getPlayerstateFlags()`
  * `<player> getRemainingSoundFileDuration()`
  * `<player> getViewOrigin()`
  * `<player> isPlayingSoundFile()`
  * `<player> muteClient(<player>)`
  * `<player> noclip("on|off|toggle")`
  * `<player> objective_player_add(<objective_number>, <state>, <position>, <iconshader>)`
  * `<player> objective_player_delete(<objective_number>)`
  * `<player> objective_player_icon(<objective_number>, <icon>)`
  * `<player> objective_player_position(<objective_number>, <position>)`
  * `<player> objective_player_state(<objective_number>, <state>)`
  * `<player> playFxForPlayer(<effect id>, <position of effect>, [<forward vector>], [<up vector>])`
  * `<player> playFxOnTagForPlayer(<effect id>, <entity>, <tag name>)`
  * `<player> playSoundFile(<sound index>, [<offset>], [<source player>])`
  * `<player> runScriptAnimation(<scriptAnimEventType>, <isContinue>, <force>)`
  * `<player> setEarthquakes("on|off|toggle")`
  * `<player> setFireRangeScale(<scale>)`
  * `<player> setFireThroughWalls(<enabled>)`
  * `<player> setMeleeHeightScale(<scale>)`
  * `<player> setMeleeRangeScale(<scale>)`
  * `<player> setMeleeWidthScale(<scale>)`
  * `<player> silent("on|off|toggle")`
  * `<player> stopSoundFile()`
  * `<player> stopUseTurret()`
  * `<player> unmuteClient(<player>)`
  * `<turret> getTurretOwner()`
  * `<weapon> getWeaponItemAmmo()`
  * `<weapon> setWeaponItemAmmo(<value>)`

New script code functions:
  * `getWeaponFuseTime(<weapon name>)`
  * `setWeaponFuseTime(<weapon name>, <time in ms>)`
  * `getEntityCount()`
  * `getSoundFileDuration(<file path>)`
  * `loadSoundFile(<file path>, <callback function>, [<volume>], [<sound index>])`
  * `logPrintConsole(<message>)`
  * `resetTestClientNaming()`
  * `setNextTestClientName(<name>)`