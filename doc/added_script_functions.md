New script code methods:
  * `<bot> setWalkValues(<forward_count>, <right_count>)`
  * `<entity> addEntityVelocity(<velocity>)`
  * `<entity> addGrenadeFuseTime(<time>)`
  * `<entity> disableBounce()`
  * `<entity> disableGravity()`
  * `<entity> enableBounce([<parallelBounce>], [<perpendicularBounce>])`
  * `<entity> enableGravity([<model collide>])`
  * `<entity> getClipmask()`
  * `<entity> getContents()`
  * `<entity> getEntityVelocity()`
  * `<entity> getGrenadeFuseTime()`
  * `<entity> getMaxEntityVelocity()`
  * `<entity> getTagAngles(<tag name>)`
  * `<entity> getTagOrigin(<tag name>)`
  * `<entity> getVmax()`
  * `<entity> getVmin()`
  * `<entity> hasTag(<tag name>)`
  * `<entity> isGravityEnabled()`
  * `<entity> isLinkedTo()`
  * `<entity> isTurret()`
  * `<entity> notSolidForPlayer(<player>)`
  * `<entity> setClipmask(<clip mask>)`
  * `<entity> setEntityVelocity(<velocity>)`
  * `<entity> setMaxEntityVelocity(<velocity>)`
  * `<entity> setLight(<red>, <green>, <blue>, <intensity>)`
  * `<entity> solidForPlayer(<player>)`
  * `<entity> spawnGrenade(<weapon>, <origin>, [<direction>], [<velocity>], [<fuseTime>])`
  * `<player> isAllowingSpectators()`
  * `<player> canUseTurret(<turret>)`
  * `<player> disableBulletDrop()`
  * `<player> disableBulletImpacts()`
  * `<player> disableEarthquakes()`
  * `<player> disableSilent()`
  * `<player> disableTalkerIcon(<player>)`
  * `<player> earthquakeForPlayer(<scale>, <duration>, <source>, <radius>)`
  * `<player> enableBulletDrop()`
  * `<player> enableBulletImpacts()`
  * `<player> enableEarthquakes()`
  * `<player> enableSilent()`
  * `<player> enableTalkerIcon(<player>)`
  * `<player> forceShot([<include client>])`
  * `<player> getClientHudElemCount()`
  * `<player> getCollisionTeam()`
  * `<player> getCurrentWeaponAmmo()`
  * `<player> getCurrentWeaponClipAmmo()`
  * `<player> getEnterTime()`
  * `<player> getGroundEntity()`
  * `<player> getInactivityTime()`
  * `<player> getPlayerstateFlags()`
  * `<player> getProtocol()`
  * `<player> getProtocolString()`
  * `<player> getRemainingSoundFileDuration()`
  * `<player> getServerCommandQueueSize()`
  * `<player> getViewOrigin()`
  * `<player> isHiddenFromScoreboard()`
  * `<player> isPlayingSoundFile()`
  * `<player> isReloading()`
  * `<player> isThrowingGrenade()`
  * `<player> isUsingTurret()`
  * `<player> isUsingBinoculars()`
  * `<player> muteClient(<player id>)`
  * `<player> noClip("on|off|toggle")`
  * `<player> objective_player_add(<objective_number>, <state>, <position>, <iconshader>)`
  * `<player> objective_player_delete(<objective_number>)`
  * `<player> objective_player_icon(<objective_number>, <icon>)`
  * `<player> objective_player_position(<objective_number>, <position>)`
  * `<player> objective_player_state(<objective_number>, <state>)`
  * `<player> overrideContents(<contents>)`
  * `<player> playFxForPlayer(<effect id>, <position of effect>, [<forward vector>], [<up vector>])`
  * `<player> playFxOnTagForPlayer(<effect id>, <entity>, <tag name>)`
  * `<player> playSoundFile(<sound index>, [<offset>], [<source player>])`
  * `<player> playScriptAnimation(<scriptAnimEventType>, <isContinue>, <force>)`
  * `<player> processSuicide()`
  * `<player> setAllowSpectators(<allow>)`
  * `<player> setBulletDrag(<value>)`
  * `<player> setBulletMask(<mask>)`
  * `<player> setBulletModel(<model>, [<time>])`
  * `<player> setBulletVelocity(<value>)`
  * `<player> setCollisionTeam(<team>)`
  * `<player> setConfigString(<string>, <index>)`
  * `<player> setCurrentWeaponAmmo(<value>)`
  * `<player> setCurrentWeaponClipAmmo(<value>)`
  * `<player> setFireRangeScale(<scale>)`
  * `<player> setFireThroughWalls(<enabled>)`
  * `<player> setHiddenFromScoreboard(<enabled>)`
  * `<player> setMeleeHeightScale(<scale>)`
  * `<player> setMeleeRangeScale(<scale>)`
  * `<player> setMeleeWidthScale(<scale>)`
  * `<player> setOriginAndAngles(<origin>, <angles>)`
  * `<player> setTurretSpreadScale(<scale>)`
  * `<player> setWeaponSpreadScale(<scale>)`
  * `<player> setPing(<ingame value>, [<status value>])`
  * `<player> stopSoundFile()`
  * `<player> stopUseTurret()`
  * `<player> unmuteClient(<player id>)`
  * `<player> useEntity(<entity>)`
  * `<player> useTurret(<turret>)`
  * `<turret> getTurretOwner()`
  * `<weapon> getWeaponItemAmmo()`
  * `<weapon> setWeaponItemAmmo(<value>)`

New script code functions:
  * `chr(<number>)`
  * `getCallStack()`
  * `getSoundDuration(<aliasname>)`
  * `getSoundInfo(<aliasname>)`
  * `getWeaponFuseTime(<weapon name>)`
  * `setWeaponFuseTime(<weapon name>, <time in ms>)`
  * `fromHex(<hex string>)`
  * `getEntityCount([<in use>])`
  * `getMicroseconds()`
  * `getMilliseconds()`
  * `getSoundAliasesFromFile(<filename>)`
  * `getSoundFileDuration(<file path>)`
  * `isSemiAutoWeapon(<weapon name>)`
  * `loadSoundFile(<file path>, <callback function>, [<volume>], [<sound index>])`
  * `loadSpeexFile(<file path>, [<sound index>])`
  * `logPrintConsole(<message>)`
  * `makeClientLocalizedString(<string>)`
  * `makeString(<localized string>)`
  * `ord(<string character>)`
  * `resetTestClientNaming()`
  * `roundTo(<value>, <precision>)`
  * `saveSpeexFile(<sound index>, <file path>)`
  * `setNextTestClientName(<name>)`
  * `toHex(<number>)`