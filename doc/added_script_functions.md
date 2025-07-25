🔣 New script code methods:
  * `<bot> setWalkValues(<forward_count>, <right_count>)`
  * `<bot> throwSmokeGrenade(<bool>)`
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
  * `<entity> hideFromPlayer(<player>)`
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
  * `<player> addEntToSnapshots(<entity>)`
  * `<player> canMantle()`
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
  * `<player> executeClientCommand(<command>)`
  * `<player> forceShot([<include client>])`
  * `<player> getBulletMask()`
  * `<player> getClientHudElemCount()`
  * `<player> getCollisionTeam()`
  * `<player> getCurrentWeaponAmmo()`
  * `<player> getCurrentWeaponClipAmmo()`
  * `<player> getEnterTime()`
  * `<player> getGroundEntity()`
  * `<player> getInactivityTime()`
  * `<player> getNumberOfEntsInSnapshot()`
  * `<player> getPlayerstateFlags()`
  * `<player> getProtocol()`
  * `<player> getProtocolString()`
  * `<player> getRemainingSoundFileDuration()`
  * `<player> getServerCommandQueueSize()`
  * `<player> getViewOrigin()`
  * `<player> isAllowingSpectators()`
  * `<player> isChatting()`
  * `<player> isHiddenFromScoreboard()`
  * `<player> isPlayingSoundFile()`
  * `<player> isReloading()`
  * `<player> isShellShocked()`
  * `<player> isThrowingGrenade()`
  * `<player> isUseTouching()`
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
  * `<player> removeEntFromSnapshots(<entity>)`
  * `<player> setActivateOnUseButtonRelease(<enabled>)`
  * `<player> setAllowSpectators(<allow>)`
  * `<player> setBulletDrag(<value>)`
  * `<player> setBulletMask(<mask>)`
  * `<player> setBulletModel(<model>, [<time>])`
  * `<player> setBulletVelocity(<value>)`
  * `<player> setCollisionTeam(<team>)`
  * `<player> setConfigStringForPlayer(<index>, <string>)`
  * `<player> setCullFogForPlayer(<near distance>, <far distance>, <red>, <green>, <blue>, <transition time>)`
  * `<player> setCurrentWeaponAmmo(<value>)`
  * `<player> setCurrentWeaponClipAmmo(<value>)`
  * `<player> setExpFogForPlayer(<density>, <red>, <green>, <blue>, <transition time>)`
  * `<player> setFireRangeScale(<scale>)`
  * `<player> setFireThroughWalls(<enabled>)`
  * `<player> setHiddenFromScoreboard(<enabled>)`
  * `<player> setMeleeHeightScale(<scale>)`
  * `<player> setMeleeRangeScale(<scale>)`
  * `<player> setMeleeWidthScale(<scale>)`
  * `<player> setOriginAndAngles(<origin>, <angles>)`
  * `<player> setProneStepSize(<size>)`
  * `<player> setStepSize(<size>)`
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

🔣 New script code functions:
  * `chr(<number>)`
  * `getCallStack()`
  * `getConfigString(<index>)`
  * `getMovers()`
  * `getSoundDuration(<aliasname>)`
  * `getSoundInfo(<aliasname>)`
  * `getWeaponFuseTime(<weapon name>)`
  * `setWeaponFuseTime(<weapon name>, <time in ms>)`
  * `fromHex(<hex string>)`
  * `getCvarFlags(<dvar name>)`
  * `getEntityCount([<in use>])`
  * `getMicroseconds()`
  * `getMilliseconds()`
  * `getSavePersist()`
  * `getSoundAliasesFromFile(<filename>)`
  * `getSoundFileDuration(<file path>)`
  * `getWeaponRaiseTime(<weapon name>)`
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
  * `setConfigString(<index>, <string>)`
  * `setConsolePrefix(<prefix>)`
  * `setNextTestClientName(<name>)`
  * `setSavePersist(<save persistent>)`
  * `setWeaponRaiseTime(<weapon name>, <raise time>)`
  * `toHex(<number>)`

🔀 New script code callback functions:
  * `CodeCallback_EntityEvent`
  * `CodeCallback_Error`
  * `CodeCallback_HitchWarning`
  * `CodeCallback_MapTurretsLoad`
  * `CodeCallback_MapWeaponsLoad`
  * `CodeCallback_MoverBlockage`
  * `CodeCallback_Notify`
  * `CodeCallback_NotifyDebug`
  * `CodeCallback_Pickup`
  * `CodeCallback_PlayerEvent`
  * `CodeCallback_Suicide`
  * `CodeCallback_WeaponChange`
  * `CodeCallback_AdsButton`
  * `CodeCallback_CrouchButton`
  * `CodeCallback_FragButton`
  * `CodeCallback_HoldBreathButton`
  * `CodeCallback_LeanLeftButton`
  * `CodeCallback_LeanRightButton`
  * `CodeCallback_MeleeBreathButton`
  * `CodeCallback_ProneButton`
  * `CodeCallback_ReloadButton`
  * `CodeCallback_SmokeButton`
  * `CodeCallback_StandButton`