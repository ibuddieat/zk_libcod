New script code methods:
  * `<bot> setWalkValues(<forward_count>, <right_count>)`
  * `<entity> addEntityVelocity(<velocity>)`
  * `<entity> addGrenadeFuseTime(<time>)`
  * `<entity> disableBounce()`
  * `<entity> disableGravity()`
  * `<entity> disableTalkerIcon(<player>)`
  * `<entity> enableBounce([<parallelBounce>], [<perpendicularBounce>])`
  * `<entity> enableGravity([<model collide>])`
  * `<entity> enableTalkerIcon(<player>)`
  * `<entity> getClipmask()`
  * `<entity> getContents()`
  * `<entity> getEntityVelocity()`
  * `<entity> getTagAngles(<tag name>)`
  * `<entity> getTagOrigin(<tag name>)`
  * `<entity> getVmax()`
  * `<entity> getVmin()`
  * `<entity> hasTag(<tag name>)`
  * `<entity> isLinkedTo()`
  * `<entity> setClipmask(<clip mask>)`
  * `<entity> setEntityVelocity(<velocity>)`
  * `<entity> setLight(<red>, <green>, <blue>, <intensity>)`
  * `<entity> spawnGrenade(<weapon>, <origin>, [<direction>], [<velocity>], [<fuseTime>])`
  * `<player> disableBulletImpacts()`
  * `<player> earthquakeForPlayer(<scale>, <duration>, <source>, <radius>)`
  * `<player> enableBulletImpacts()`
  * `<player> getClientHudElemCount()`
  * `<player> getCollisionTeam()`
  * `<player> getEnterTime()`
  * `<player> getGroundEntity()`
  * `<player> getInactivityTime()`
  * `<player> getPlayerstateFlags()`
  * `<player> getRemainingSoundFileDuration()`
  * `<player> getServerCommandQueueSize()`
  * `<player> getViewOrigin()`
  * `<player> isPlayingSoundFile()`
  * `<player> muteClient(<player id>)`
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
  * `<player> setCollisionTeam(<team>)`
  * `<player> setEarthquakes("on|off|toggle")`
  * `<player> setFireRangeScale(<scale>)`
  * `<player> setFireThroughWalls(<enabled>)`
  * `<player> setMeleeHeightScale(<scale>)`
  * `<player> setMeleeRangeScale(<scale>)`
  * `<player> setMeleeWidthScale(<scale>)`
  * `<player> setPing(<value>)`
  * `<player> silent("on|off|toggle")`
  * `<player> stopSoundFile()`
  * `<player> stopUseTurret()`
  * `<player> unmuteClient(<player id>)`
  * `<turret> getTurretOwner()`
  * `<weapon> getWeaponItemAmmo()`
  * `<weapon> setWeaponItemAmmo(<value>)`

New script code functions:
  * `chr(<number>)`
  * `getWeaponFuseTime(<weapon name>)`
  * `setWeaponFuseTime(<weapon name>, <time in ms>)`
  * `fromHex(<hex string>)`
  * `getEntityCount([<in use>])`
  * `getSoundAliasesFromFile(<filename>)`
  * `getSoundFileDuration(<file path>)`
  * `loadSoundFile(<file path>, <callback function>, [<volume>], [<sound index>])`
  * `loadSpeexFile(<file path>, [<sound index>])`
  * `logPrintConsole(<message>)`
  * `ord(<string character>)`
  * `resetTestClientNaming()`
  * `saveSpeexFile(<sound index>, <file path>)`
  * `setNextTestClientName(<name>)`
  * `soundDuration(<aliasname>)`
  * `toHex(<number>)`