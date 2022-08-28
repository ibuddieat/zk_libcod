Reconstructed game functions:
  * `Touch_Item` to gain more control over item pickup actions
  * `SV_DropClient` to filter disconnect messages via various cvars
  * `SV_SendClientGameState` to reset custom player states on connect
  * `BG_AddPredictableEventToPlayerstate` et sequentes to be able to filter events
  * `G_AddEvent`
  * `MSG_WriteDeltaPlayerstate` et sequentes to be able to alter player/entity attributes
  * `MSG_WriteDeltaStruct`
  * `MSG_WriteDeltaField`
  * `MSG_WriteDeltaClient`
  * `MSG_WriteDeltaEntity`
  * `MSG_WriteDeltaArchivedEntity`
  * `SV_EmitPacketEntities`
  * `SV_EmitPacketClients`
  * `SV_ArchiveSnapshot`
  * `SV_BuildClientSnapshot`
  * `SV_WriteSnapshotToClient`
  * `Com_Error` et sequentes to be able to pass errors to script code
  * `Scr_ErrorInternal`
  * `RuntimeError_Debug`
  * `RuntimeError`
  * `CM_IsBadStaticModel` for `g_debugStaticModels` (this function name is a guess)
  * `G_SpawnEntitiesFromString` et sequentes to provide map weapon/turret info via new script callback functions
  * `G_SetEntityPlacement` (this function name is a guess)
  * `G_CallSpawn`
  * `SV_WWWRedirectClient` for `sv_wwwDlDisconnectedMessages`
  * `SV_MapExists` for manymaps support
  * `G_UpdateObjectives` for per-player objectives (this function name is a guess)
  * `FireWeaponMelee` for per-player melee values

Note: Usually they can be found with the `custom_` prefix in [libcod.cpp](../code/libcod.cpp). Also, this list does not include simple hooks that call the original game function.