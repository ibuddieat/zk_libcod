# Weapons

- `slotId = player getCurrentSlotId()` - 0=none, 1=primary, 2=primaryb
- `weaponId = player getCurrentWeaponId()`
- `weaponId = player getWeaponIdInSlotId(slotId)`
- `slotId = player getSlotIdClipAmmo(slotId)`
- `ammo = player getSlotIdAmmo(slotId)`
- `clip = player getWeaponIdClipAmmo(weaponId)`
- `ammo = player getWeaponIdAmmo(weaponId)`
- `slotId = player setCurrentSlotId(slotId)`
- `isSuccess = player setCurrentWeaponId(weaponId)`
- `isSuccess = player setWeaponIdInSlotId(slotId, weaponId)`
- `isSuccess = player setSlotIdClipAmmo(slotId, clipAmmo)`
- `isSuccess = player setSlotIdAmmo(slotId, ammo)`
- `isSuccess = player setWeaponIdClipAmmo(weaponId, clipAmmo)`
- `isSuccess = player setWeaponIdAmmo(weaponId, ammo)`
- `isSuccess = player canReloadSlotId(slotId, [reloadThreshold])`
- `weaponId = weaponNameToId(weaponName)`


# Graph

- `graphId = graphCreate([persist], [nodeCapacity])` - nodeCapacity is NOT a hard limit
- `isSuccess = graphRemove(graphId)`
- `isSuccess = graphRemoveAll()`
- `nodeId = graphAddNode(graphId, origin, [type], [nodeId])`
- `properties = graphGetNodeProperties(graphId, nodeId)`
- `isSuccess = graphRemoveNode(graphId, nodeId)`
- `isSuccess = graphAddEdge(graphId, fromNodeId, toNodeId, [type], [cost])`
- `properties = graphGetEdgeProperties(graphId, fromNodeId, toNodeId)`
- `isSuccess = graphRemoveEdge(graphId, fromNodeId, toNodeId)`
- `path = graphFindPath(graphId, startNodeId, endNodeId, [skipNodeIds], [skipNodeTypes], [skipEdgeTypes])`
- `isSuccess = graphPrecomputePathsToNode(graphId, nodeId, [skipNodeTypes], [skipEdgeTypes])`
- `nodeId = graphFindClosestNode(graphId, origin)`
- `edgeInfo = graphFindClosestEdge(graphId, origin)`
- Fixed graph node type initialization (affects `skipNodeTypes` filtering in path finding).
