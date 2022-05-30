#include "bsp.hpp"

#if COMPILE_BSP == 1

static fileHandle_t f;

void CM_DebugViewBasicDetails()
{
	FS_Printf( f, "%s:\n\n", cm.name);
	FS_Printf( f, "StaticModels: %d\n", cm.numStaticModels);
	FS_Printf( f, "Materials: %d\n", cm.numMaterials);
	FS_Printf( f, "BrushSides: %d\n", cm.numBrushSides);
	FS_Printf( f, "Nodes: %d\n", cm.numNodes);
	FS_Printf( f, "Leafs: %d\n", cm.numLeafs);
	FS_Printf( f, "LeafbrushNodes: %d\n", cm.leafbrushNodesCount);
	FS_Printf( f, "LeafBrushes: %d\n", cm.numLeafBrushes);
	FS_Printf( f, "LeafSurfaces: %d\n", cm.numLeafSurfaces);
	FS_Printf( f, "Verticles: %d\n", cm.vertCount);
	FS_Printf( f, "Edges: %d\n", cm.edgeCount);
	FS_Printf( f, "TriIndices: %d\n", cm.triCount);
	FS_Printf( f, "CollisionBorders: %d\n", cm.borderCount);
	FS_Printf( f, "CollisionPartitions: %d\n", cm.partitionCount);
	FS_Printf( f, "CollisionAabbTrees: %d\n", cm.aabbTreeCount);
	FS_Printf( f, "SubModels: %d\n", cm.numSubModels);
	FS_Printf( f, "Brushes: %d\n", cm.numBrushes);
	FS_Printf( f, "Clusters: %d\n", cm.numClusters);
	FS_Printf( f, "dynEnts0: %d\n", cm.dynEntCount[0]);
	FS_Printf( f, "dynEnt1: %d\n", cm.dynEntCount[1]);
	FS_Printf( f, "\n");
}

void CM_DebugPrintMaterialInfo()
{
	unsigned int i;
	u_int32_t content = 0;
	u_int32_t surface = 0;

	FS_Printf( f, "--------------- MaterialInfo --------------------\n");

	for(i = 0; i < cm.numMaterials; ++i)
	{
		FS_Printf( f, "%s surf: 0x%08x cont: 0x%08x\n", cm.materials[i].material, cm.materials[i].surfaceFlags, cm.materials[i].contentFlags);
		surface |= cm.materials[i].surfaceFlags;
		content |= cm.materials[i].contentFlags;
	}
	FS_Printf( f, "\n");
	FS_Printf( f, "used surface flags: 0x%08x\n", surface);
	FS_Printf( f, "used content flags: 0x%08x\n", content);
	FS_Printf( f, "\n");
	FS_Printf( f, "-------------------------------------------------\n");
}

void CM_DebugCLeafBrushNodes()
{
	unsigned int i;
	FS_Printf( f, "--------------- CLeafBrushNodes ------------------\n");

	u_int32_t content = 0;
	for(i = 0; i < cm.leafbrushNodesCount; ++i)
	{
		FS_Printf( f, "Count: %d cont: 0x%08x\n", cm.leafbrushNodes[i].leafBrushCount, cm.leafbrushNodes[i].contents);
		content |= cm.leafbrushNodes[i].contents;
	}
	FS_Printf( f, "\n");
	FS_Printf( f, "used content flags: 0x%08x\n", content);
	FS_Printf( f, "\n");
	FS_Printf( f, "-------------------------------------------------\n");
}

void CM_DebugCBrush()
{
	unsigned int i;
	FS_Printf( f, "-------------------- CBrush ----------------------\n");
	u_int32_t content = 0;
	for(i = 0; i < cm.numBrushes; ++i)
	{
		FS_Printf( f, "cont: 0x%08x\n", cm.brushes[i].contents);
		content |= cm.brushes[i].contents;
	}
	FS_Printf( f, "\n");
	FS_Printf( f, "used content flags: 0x%08x\n", content);
	FS_Printf( f, "\n");
	FS_Printf( f, "-------------------------------------------------\n");
}

void CM_DebugCLeaf()
{
	unsigned int i;
	FS_Printf( f, "-------------------- cLeaf ----------------------\n");
	u_int32_t brushcontent = 0;
	u_int32_t terriancontent = 0;

	for(i = 0; i < cm.numLeafs; ++i)
	{
		FS_Printf( f, "brushContents: 0x%08x  terrainContents: 0x%08x\n", cm.leafs[i].brushContents, cm.leafs[i].terrainContents);
		brushcontent |= cm.leafs[i].brushContents;
		terriancontent |= cm.leafs[i].terrainContents;
	}
	FS_Printf( f, "\n");
	FS_Printf( f, "used content flags: 0x%08x  0x%08x\n", brushcontent, terriancontent);
	FS_Printf( f, "\n");
	FS_Printf( f, "-----------------------------------------------\n");
}

void CM_DebugSubModels()
{
	unsigned int i;
	FS_Printf( f, "------------- SubModels.cLeaf -----------------\n");
	u_int32_t brushcontent = 0;

	for(i = 0; i < cm.numSubModels; ++i)
	{
		FS_Printf( f, "brushContents: 0x%08x\n", cm.cmodels[i].leaf.brushContents);
		brushcontent |= cm.cmodels[i].leaf.brushContents;
	}
	FS_Printf( f, "\n");
	FS_Printf( f, "used content flags: 0x%08x\n", brushcontent);
	FS_Printf( f, "\n");
	FS_Printf( f, "-----------------------------------------------\n");
}

void CM_WalkAABB_Trees()
{
	int i;
	FS_Printf( f, "---------------- AABB-Trees -------------------\n");
	CollisionAabbTree_t* tr;

	for(i = 0; i < cm.aabbTreeCount; ++i)
	{
		tr = &cm.aabbTrees[i];
		FS_Printf( f, "AABBTree num=%d, Mat: %s, childs=%d, firstChild=%d\n", i, cm.materials[tr->materialIndex].material, tr->childCount, tr->u.firstChildIndex);
	}
	FS_Printf( f, "-----------------------------------------------\n");
}

void CM_DebugDoAll_f()
{
	if (!scrVarPub.developer)
	{
		Com_Printf("CM_DebugDoAll: developer mode needs to be enabled for debugging.\n");
		return;
	}

	f = FS_FOpenFileWrite("cm_debug.log");

	if (f < 1)
	{
		Com_DPrintf("CM_DebugDoAll: Couldn't write file cm_debug.log");
		return;
	}

	CM_DebugViewBasicDetails();
	CM_DebugPrintMaterialInfo();
	CM_DebugCLeafBrushNodes();
	CM_DebugCBrush();
	CM_DebugCLeaf();
	CM_DebugSubModels();
	CM_WalkAABB_Trees();

	FS_FCloseFile(f);

	Com_DPrintf("CM_DebugDoAll: info dumped to cm_debug.log\n");
}

void CM_AddCommand()
{
	Cmd_AddCommand("cm_dumpinfo", CM_DebugDoAll_f);
}

#endif
