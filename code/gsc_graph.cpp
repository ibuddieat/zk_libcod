#include "gsc_graph.hpp"

#if COMPILE_GRAPH == 1

#include <vector>
#include <math.h>

extern dvar_t *graph_max_nodes;
extern dvar_t *graph_max_graphs;
extern dvar_t *graph_debug;

struct GraphEdge
{
	unsigned int to;
	int type;
	float cost;
};

struct GraphNode
{
	vec3_t origin;
	int type;
	std::vector<GraphEdge> edges;
};

struct AStarGraph
{
	int id;
	bool persist;
	unsigned int edgeCount;
	std::vector<GraphNode> nodes;
};

// All graph state lives on the main server thread - no locking, by design.
static std::vector<AStarGraph> graphs;

static void graphDebugPrint(const char *format, ...)
{
	char buffer[1024];
	va_list args;

	if ( !graph_debug->current.boolean )
		return;

	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	Com_Printf("[GRAPH] %s\n", buffer);
}

static AStarGraph *graphById(int id)
{
	for ( size_t i = 0; i < graphs.size(); i++ )
	{
		if ( graphs[i].id == id )
			return &graphs[i];
	}

	return NULL;
}

static float edgeDistance(const vec3_t a, const vec3_t b)
{
	float dx = b[0] - a[0];
	float dy = b[1] - a[1];
	float dz = b[2] - a[2];

	return sqrtf(dx * dx + dy * dy + dz * dz);
}

// graphCreate([persist], [reserve]) -> graph id
void gsc_graph_create()
{
	int persist = 0;
	int reserve = 0;
	int id;
	AStarGraph graph;

	if ( Scr_GetNumParam() >= 1 && !stackGetParamInt(0, &persist) )
	{
		stackError("gsc_graph_create() persist argument has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( Scr_GetNumParam() >= 2 && !stackGetParamInt(1, &reserve) )
	{
		stackError("gsc_graph_create() reserve argument has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( (int)graphs.size() >= graph_max_graphs->current.integer )
	{
		stackError("gsc_graph_create() graph limit reached (graph_max_graphs is %i)", graph_max_graphs->current.integer);
		stackPushUndefined();
		return;
	}

	// Lowest free id so ids stay small and stable across create/remove churn
	id = 0;
	while ( graphById(id) )
		id++;

	graph.id = id;
	graph.persist = persist != 0;
	graph.edgeCount = 0;

	if ( reserve > 0 && reserve <= graph_max_nodes->current.integer )
		graph.nodes.reserve(reserve);

	graphs.push_back(graph);
	graphDebugPrint("created graph %i (persist %i, reserve %i)", id, persist, reserve);

	stackPushInt(id);
}

// graphRemove(<graph id>) -> true
void gsc_graph_remove()
{
	int id;

	if ( !stackGetParams("i", &id) )
	{
		stackError("gsc_graph_remove() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	for ( size_t i = 0; i < graphs.size(); i++ )
	{
		if ( graphs[i].id == id )
		{
			graphs.erase(graphs.begin() + i);
			graphDebugPrint("removed graph %i", id);
			stackPushBool(qtrue);
			return;
		}
	}

	stackError("gsc_graph_remove() graph %i does not exist", id);
	stackPushUndefined();
}

// graphRemoveAll() -> true
void gsc_graph_removeall()
{
	graphDebugPrint("removing all %i graphs", (int)graphs.size());
	graphs.clear();
	stackPushBool(qtrue);
}

// graphAddNode(<graph id>, <origin>, [type], [node id]) -> node id
// Node ids are the vector index and must be added sequentially (0, 1, 2, ...),
// so a script array index and its native node id are always the same value.
void gsc_graph_add_node()
{
	int id;
	vec3_t origin;
	int type = 0;
	int nodeId = -1;
	AStarGraph *graph;
	GraphNode node;

	if ( !stackGetParams("iv", &id, origin) )
	{
		stackError("gsc_graph_add_node() one or more arguments are undefined or have a wrong type");
		stackPushUndefined();
		return;
	}

	if ( Scr_GetNumParam() >= 3 && !stackGetParamInt(2, &type) )
	{
		stackError("gsc_graph_add_node() type argument has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( Scr_GetNumParam() >= 4 && !stackGetParamInt(3, &nodeId) )
	{
		stackError("gsc_graph_add_node() node id argument has a wrong type");
		stackPushUndefined();
		return;
	}

	graph = graphById(id);

	if ( !graph )
	{
		stackError("gsc_graph_add_node() graph %i does not exist", id);
		stackPushUndefined();
		return;
	}

	if ( nodeId >= 0 && nodeId != (int)graph->nodes.size() )
	{
		stackError("gsc_graph_add_node() node id %i is out of sequence (next is %i)", nodeId, (int)graph->nodes.size());
		stackPushUndefined();
		return;
	}

	if ( (int)graph->nodes.size() >= graph_max_nodes->current.integer )
	{
		stackError("gsc_graph_add_node() node limit reached (graph_max_nodes is %i)", graph_max_nodes->current.integer);
		stackPushUndefined();
		return;
	}

	VectorCopy(origin, node.origin);
	node.type = type;

	graph->nodes.push_back(node);

	stackPushInt((int)graph->nodes.size() - 1);
}

// graphAddEdge(<graph id>, <from node>, <to node>, [type], [cost]) -> true/false
// Edges are directed; add both directions for a two-way link. Cost defaults to
// the distance between the node origins. Adding a duplicate returns false.
void gsc_graph_add_edge()
{
	int id;
	int from;
	int to;
	int type = 0;
	float cost = -1;
	AStarGraph *graph;
	GraphEdge edge;

	if ( !stackGetParams("iii", &id, &from, &to) )
	{
		stackError("gsc_graph_add_edge() one or more arguments are undefined or have a wrong type");
		stackPushUndefined();
		return;
	}

	if ( Scr_GetNumParam() >= 4 && !stackGetParamInt(3, &type) )
	{
		stackError("gsc_graph_add_edge() type argument has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( Scr_GetNumParam() >= 5 && !stackGetParamFloat(4, &cost) )
	{
		stackError("gsc_graph_add_edge() cost argument has a wrong type");
		stackPushUndefined();
		return;
	}

	graph = graphById(id);

	if ( !graph )
	{
		stackError("gsc_graph_add_edge() graph %i does not exist", id);
		stackPushUndefined();
		return;
	}

	if ( from < 0 || from >= (int)graph->nodes.size() || to < 0 || to >= (int)graph->nodes.size() )
	{
		stackError("gsc_graph_add_edge() node id is out of range (graph %i has %i nodes)", id, (int)graph->nodes.size());
		stackPushUndefined();
		return;
	}

	if ( from == to )
	{
		stackError("gsc_graph_add_edge() cannot add an edge from node %i to itself", from);
		stackPushUndefined();
		return;
	}

	for ( size_t i = 0; i < graph->nodes[from].edges.size(); i++ )
	{
		if ( graph->nodes[from].edges[i].to == (unsigned int)to )
		{
			graphDebugPrint("duplicate edge %i -> %i on graph %i ignored", from, to, id);
			stackPushBool(qfalse);
			return;
		}
	}

	edge.to = to;
	edge.type = type;

	if ( cost >= 0 )
		edge.cost = cost;
	else
		edge.cost = edgeDistance(graph->nodes[from].origin, graph->nodes[to].origin);

	graph->nodes[from].edges.push_back(edge);
	graph->edgeCount++;

	stackPushBool(qtrue);
}

// graphGetNodeOrigin(<graph id>, <node id>) -> origin vector
void gsc_graph_get_node_origin()
{
	int id;
	int nodeId;
	AStarGraph *graph;

	if ( !stackGetParams("ii", &id, &nodeId) )
	{
		stackError("gsc_graph_get_node_origin() one or more arguments are undefined or have a wrong type");
		stackPushUndefined();
		return;
	}

	graph = graphById(id);

	if ( !graph )
	{
		stackError("gsc_graph_get_node_origin() graph %i does not exist", id);
		stackPushUndefined();
		return;
	}

	if ( nodeId < 0 || nodeId >= (int)graph->nodes.size() )
	{
		stackError("gsc_graph_get_node_origin() node id %i is out of range (graph %i has %i nodes)", nodeId, id, (int)graph->nodes.size());
		stackPushUndefined();
		return;
	}

	stackPushVector(graph->nodes[nodeId].origin);
}

// graphGetNodeCount(<graph id>) -> node count
void gsc_graph_get_node_count()
{
	int id;
	AStarGraph *graph;

	if ( !stackGetParams("i", &id) )
	{
		stackError("gsc_graph_get_node_count() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	graph = graphById(id);

	if ( !graph )
	{
		stackError("gsc_graph_get_node_count() graph %i does not exist", id);
		stackPushUndefined();
		return;
	}

	stackPushInt((int)graph->nodes.size());
}

// graphGetEdgeCount(<graph id>) -> edge count
void gsc_graph_get_edge_count()
{
	int id;
	AStarGraph *graph;

	if ( !stackGetParams("i", &id) )
	{
		stackError("gsc_graph_get_edge_count() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	graph = graphById(id);

	if ( !graph )
	{
		stackError("gsc_graph_get_edge_count() graph %i does not exist", id);
		stackPushUndefined();
		return;
	}

	stackPushInt((int)graph->edgeCount);
}

// Erases non-persistent graphs on map change / map_restart. Wired from
// custom_SV_SpawnServer so per-map graphs never leak across loads.
void gsc_graph_cleanup_on_spawn_server(void)
{
	size_t i = 0;
	int removed = 0;

	while ( i < graphs.size() )
	{
		if ( !graphs[i].persist )
		{
			graphs.erase(graphs.begin() + i);
			removed++;
		}
		else
		{
			i++;
		}
	}

	if ( removed )
		graphDebugPrint("map change: removed %i non-persistent graphs, %i kept", removed, (int)graphs.size());
}

#endif
