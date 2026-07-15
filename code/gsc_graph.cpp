#include "gsc_graph.hpp"

#if COMPILE_GRAPH == 1

#include <vector>
#include <math.h>

extern dvar_t *sv_graphMaxNodes;
extern dvar_t *sv_graphMaxGraphs;
extern dvar_t *g_debugGraph;
extern dvar_t *sv_graphAstarMaxIterations;
extern dvar_t *sv_graphAstarWeight;

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

struct GraphHeapEntry
{
	float f;
	unsigned int node;
};

/*
 * Search scratch is kept per graph and never cleared: a slot is only valid
 * when its stamp equals the current search id, so starting a new search is an
 * O(1) counter bump and the hot loop never allocates after warmup.
 */
struct GraphScratch
{
	std::vector<float> gScore;
	std::vector<float> fScore;
	std::vector<int> parent;
	std::vector<unsigned int> stamp;
	std::vector<unsigned int> closedStamp;
	std::vector<GraphHeapEntry> heap;
	unsigned int searchId;
};

struct GraphStats
{
	unsigned int searches;
	unsigned int partials;
	unsigned int expansionsTotal;
	unsigned int expansionsMax;
	unsigned int usMin;
	unsigned int usMax;
	unsigned int usTotal;
};

struct AStarGraph
{
	int id;
	bool persist;
	unsigned int edgeCount;
	std::vector<GraphNode> nodes;
	GraphScratch scratch;
	GraphStats stats;
};

// All graph state lives on the main server thread - no locking, by design.
static std::vector<AStarGraph> graphs;

static void graphDebugPrint(const char *format, ...)
{
	char buffer[1024];
	va_list args;

	if ( !g_debugGraph->current.boolean )
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

static unsigned int graphMicroseconds()
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return (unsigned int)(tv.tv_sec * 1000000 + tv.tv_usec);
}

static void heapSiftUp(std::vector<GraphHeapEntry> &heap, size_t i)
{
	while ( i > 0 )
	{
		size_t up = (i - 1) / 2;

		if ( heap[up].f <= heap[i].f )
			break;

		GraphHeapEntry tmp = heap[up];
		heap[up] = heap[i];
		heap[i] = tmp;
		i = up;
	}
}

static void heapSiftDown(std::vector<GraphHeapEntry> &heap, size_t i)
{
	size_t size = heap.size();

	for ( ;; )
	{
		size_t left = i * 2 + 1;
		size_t right = left + 1;
		size_t smallest = i;

		if ( left < size && heap[left].f < heap[smallest].f )
			smallest = left;
		if ( right < size && heap[right].f < heap[smallest].f )
			smallest = right;
		if ( smallest == i )
			break;

		GraphHeapEntry tmp = heap[smallest];
		heap[smallest] = heap[i];
		heap[i] = tmp;
		i = smallest;
	}
}

static void heapPush(std::vector<GraphHeapEntry> &heap, float f, unsigned int node)
{
	GraphHeapEntry entry;

	entry.f = f;
	entry.node = node;
	heap.push_back(entry);
	heapSiftUp(heap, heap.size() - 1);
}

static GraphHeapEntry heapPop(std::vector<GraphHeapEntry> &heap)
{
	GraphHeapEntry top = heap[0];

	heap[0] = heap[heap.size() - 1];
	heap.pop_back();

	if ( !heap.empty() )
		heapSiftDown(heap, 0);

	return top;
}

/*
 * Prepare the per-graph scratch for a new search: size the arrays once,
 * invalidate everything else by bumping the search id.
 */
static void scratchBegin(AStarGraph *graph)
{
	GraphScratch &s = graph->scratch;
	size_t need = graph->nodes.size();

	if ( s.gScore.size() < need )
	{
		s.gScore.resize(need);
		s.fScore.resize(need);
		s.parent.resize(need);
		s.stamp.resize(need, 0);
		s.closedStamp.resize(need, 0);
	}

	s.heap.clear();
	s.searchId++;

	// On wraparound, hard-reset the stamps so stale slots cannot alias
	if ( s.searchId == 0 )
	{
		for ( size_t i = 0; i < s.stamp.size(); i++ )
		{
			s.stamp[i] = 0;
			s.closedStamp[i] = 0;
		}
		s.searchId = 1;
	}
}

/*
 * Walk a GSC array parameter of node ids and mark each as closed for this
 * search. Returns qfalse only on a malformed (non-array) parameter.
 */
static qboolean graphMarkSkippedNodes(AStarGraph *graph, int param)
{
	unsigned int objectId;
	unsigned int it;
	unsigned int guard = 0;

	if ( stackGetParamType(param) == VAR_UNDEFINED )
		return qtrue;

	if ( !stackGetParamObject(param, &objectId) )
		return qfalse;

	it = objectId;

	while ( guard < SL_MAX_STRING_INDEX )
	{
		it = FindNextSibling(it);
		if ( it == 0 )
			break;

		VariableValueInternal *entry = &scrVarGlob[it];

		if ( (entry->w.type & VAR_MASK) == VAR_INTEGER )
		{
			int nodeId = entry->u.u.intValue;

			if ( nodeId >= 0 && nodeId < (int)graph->nodes.size() )
				graph->scratch.closedStamp[nodeId] = graph->scratch.searchId;
		}

		guard++;
	}

	return qtrue;
}

/*
 * Walk parent links back from a node and push the path (start..node) as a GSC
 * array of node ids. Returns the path length.
 */
static unsigned int graphPushPath(AStarGraph *graph, unsigned int node, unsigned int start)
{
	GraphScratch &s = graph->scratch;
	unsigned int cur = node;
	unsigned int count = 0;
	unsigned int safety = (unsigned int)graph->nodes.size() + 1;

	// Reuse the heap buffer as reversal storage - the search is done with it
	s.heap.clear();

	while ( count < safety )
	{
		GraphHeapEntry entry;

		entry.f = 0;
		entry.node = cur;
		s.heap.push_back(entry);
		count++;

		if ( cur == start )
			break;

		cur = (unsigned int)s.parent[cur];
	}

	stackPushArray();

	for ( unsigned int i = 0; i < count; i++ )
	{
		stackPushInt((int)s.heap[count - 1 - i].node);
		stackPushArrayLast();
	}

	return count;
}

/*
 * graphFindPath(<graph id>, <start node>, <goal node>, [skip nodes array],
 *               [skip node types mask], [skip edge types mask], [max expansions])
 *   -> array of node ids from start to goal
 * A budget-capped search returns a best-effort partial path instead: the last
 * element not being the goal is the caller's signal. Invalid arguments return
 * undefined. skip masks are AND-tested against node/edge type bits.
 */
void gsc_graph_find_path()
{
	int id;
	int start;
	int goal;
	int skipNodeTypes = 0;
	int skipEdgeTypes = 0;
	int maxExpansions = 0;
	AStarGraph *graph;
	float weight;
	unsigned int expansions = 0;
	unsigned int bestNode;
	float bestH;
	unsigned int started;
	int numParam;

	if ( !stackGetParams("iii", &id, &start, &goal) )
	{
		stackError("gsc_graph_find_path() one or more arguments are undefined or have a wrong type");
		stackPushUndefined();
		return;
	}

	numParam = Scr_GetNumParam();

	if ( numParam >= 5 && stackGetParamType(4) != VAR_UNDEFINED && !stackGetParamInt(4, &skipNodeTypes) )
	{
		stackError("gsc_graph_find_path() skip node types argument has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( numParam >= 6 && stackGetParamType(5) != VAR_UNDEFINED && !stackGetParamInt(5, &skipEdgeTypes) )
	{
		stackError("gsc_graph_find_path() skip edge types argument has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( numParam >= 7 && stackGetParamType(6) != VAR_UNDEFINED && !stackGetParamInt(6, &maxExpansions) )
	{
		stackError("gsc_graph_find_path() max expansions argument has a wrong type");
		stackPushUndefined();
		return;
	}

	graph = graphById(id);

	if ( !graph )
	{
		stackError("gsc_graph_find_path() graph %i does not exist", id);
		stackPushUndefined();
		return;
	}

	if ( start < 0 || start >= (int)graph->nodes.size() || goal < 0 || goal >= (int)graph->nodes.size() )
	{
		stackError("gsc_graph_find_path() node id is out of range (graph %i has %i nodes)", id, (int)graph->nodes.size());
		stackPushUndefined();
		return;
	}

	started = graphMicroseconds();
	scratchBegin(graph);

	if ( numParam >= 4 && !graphMarkSkippedNodes(graph, 3) )
	{
		stackError("gsc_graph_find_path() skip nodes argument is not an array");
		stackPushUndefined();
		return;
	}

	GraphScratch &s = graph->scratch;

	// The server-wide dvar is a ceiling; a per-call budget tightens it further
	if ( sv_graphAstarMaxIterations->current.integer > 0 )
	{
		if ( maxExpansions <= 0 || maxExpansions > sv_graphAstarMaxIterations->current.integer )
			maxExpansions = sv_graphAstarMaxIterations->current.integer;
	}

	weight = sv_graphAstarWeight->current.decimal;
	if ( weight < 1.0f )
		weight = 1.0f;

	if ( start == goal )
	{
		stackPushArray();
		stackPushInt(start);
		stackPushArrayLast();
		return;
	}

	bestNode = start;
	bestH = edgeDistance(graph->nodes[start].origin, graph->nodes[goal].origin);

	s.gScore[start] = 0;
	s.fScore[start] = bestH * weight;
	s.parent[start] = start;
	s.stamp[start] = s.searchId;

	heapPush(s.heap, s.fScore[start], (unsigned int)start);

	while ( !s.heap.empty() )
	{
		GraphHeapEntry top = heapPop(s.heap);
		unsigned int cur = top.node;

		// Lazy deletion: a stale heap entry no longer matches the node's best f
		if ( top.f != s.fScore[cur] || s.closedStamp[cur] == s.searchId )
			continue;

		if ( cur == (unsigned int)goal )
		{
			unsigned int len = graphPushPath(graph, cur, (unsigned int)start);

			graphDebugPrint("findPath g%i %i -> %i: reached, len %u, cost %.0f, %u expansions", id, start, goal, len, s.gScore[cur], expansions);
			goto done;
		}

		s.closedStamp[cur] = s.searchId;
		expansions++;

		if ( maxExpansions > 0 && (int)expansions >= maxExpansions )
			break;

		GraphNode &node = graph->nodes[cur];

		for ( size_t i = 0; i < node.edges.size(); i++ )
		{
			GraphEdge &edge = node.edges[i];
			unsigned int next = edge.to;

			if ( skipEdgeTypes && (edge.type & skipEdgeTypes) )
				continue;
			if ( skipNodeTypes && (graph->nodes[next].type & skipNodeTypes) )
				continue;
			if ( s.closedStamp[next] == s.searchId )
				continue;

			float tentative = s.gScore[cur] + edge.cost;

			if ( s.stamp[next] != s.searchId || tentative < s.gScore[next] )
			{
				float h = edgeDistance(graph->nodes[next].origin, graph->nodes[goal].origin);

				s.gScore[next] = tentative;
				s.fScore[next] = tentative + h * weight;
				s.parent[next] = (int)cur;
				s.stamp[next] = s.searchId;

				if ( h < bestH )
				{
					bestH = h;
					bestNode = next;
				}

				heapPush(s.heap, s.fScore[next], next);
			}
		}
	}

	/*
	 * Budget hit or frontier exhausted: return the closest we got (the caller
	 * detects a partial path by its last element not being the goal)
	 */
	graph->stats.partials++;
	{
		unsigned int len = graphPushPath(graph, bestNode, (unsigned int)start);

		graphDebugPrint("findPath g%i %i -> %i: PARTIAL to %u, len %u, %u expansions", id, start, goal, bestNode, len, expansions);
	}

done:
	{
		unsigned int elapsed = graphMicroseconds() - started;

		graph->stats.searches++;
		graph->stats.expansionsTotal += expansions;
		if ( expansions > graph->stats.expansionsMax )
			graph->stats.expansionsMax = expansions;
		if ( graph->stats.searches == 1 || elapsed < graph->stats.usMin )
			graph->stats.usMin = elapsed;
		if ( elapsed > graph->stats.usMax )
			graph->stats.usMax = elapsed;
		graph->stats.usTotal += elapsed;
	}
}

/*
 * graphFindClosestNode(<graph id>, <origin>, [content mask]) -> node id
 * With a content mask the closest node with a passing locational trace wins
 * (line of sight for MASK_SHOT-style masks) - much more expensive, and returns
 * undefined when no node passes.
 */
void gsc_graph_find_closest_node()
{
	int id;
	vec3_t origin;
	int contentMask = 0;
	AStarGraph *graph;
	int best = -1;
	float bestD = 0;

	if ( !stackGetParams("iv", &id, origin) )
	{
		stackError("gsc_graph_find_closest_node() one or more arguments are undefined or have a wrong type");
		stackPushUndefined();
		return;
	}

	if ( Scr_GetNumParam() >= 3 && !stackGetParamInt(2, &contentMask) )
	{
		stackError("gsc_graph_find_closest_node() content mask argument has a wrong type");
		stackPushUndefined();
		return;
	}

	graph = graphById(id);

	if ( !graph )
	{
		stackError("gsc_graph_find_closest_node() graph %i does not exist", id);
		stackPushUndefined();
		return;
	}

	for ( size_t i = 0; i < graph->nodes.size(); i++ )
	{
		float dx = graph->nodes[i].origin[0] - origin[0];
		float dy = graph->nodes[i].origin[1] - origin[1];
		float dz = graph->nodes[i].origin[2] - origin[2];
		float d = dx * dx + dy * dy + dz * dz;

		if ( best >= 0 && d >= bestD )
			continue;

		if ( contentMask && !G_LocationalTracePassed(origin, graph->nodes[i].origin, ENTITYNUM_NONE, contentMask) )
			continue;

		best = (int)i;
		bestD = d;
	}

	if ( best < 0 )
	{
		stackPushUndefined();
		return;
	}

	stackPushInt(best);
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

	if ( (int)graphs.size() >= sv_graphMaxGraphs->current.integer )
	{
		stackError("gsc_graph_create() graph limit reached (sv_graphMaxGraphs is %i)", sv_graphMaxGraphs->current.integer);
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
	graph.scratch.searchId = 0;
	memset(&graph.stats, 0, sizeof(graph.stats));

	if ( reserve > 0 && reserve <= sv_graphMaxNodes->current.integer )
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

/*
 * graphAddNode(<graph id>, <origin>, [type], [node id]) -> node id
 * Node ids are the vector index and must be added sequentially (0, 1, 2, ...),
 * so a script array index and its native node id are always the same value.
 */
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

	if ( (int)graph->nodes.size() >= sv_graphMaxNodes->current.integer )
	{
		stackError("gsc_graph_add_node() node limit reached (sv_graphMaxNodes is %i)", sv_graphMaxNodes->current.integer);
		stackPushUndefined();
		return;
	}

	VectorCopy(origin, node.origin);
	node.type = type;

	graph->nodes.push_back(node);

	stackPushInt((int)graph->nodes.size() - 1);
}

/*
 * graphAddEdge(<graph id>, <from node>, <to node>, [type], [cost]) -> true/false
 * Edges are directed; add both directions for a two-way link. Cost defaults to
 * the distance between the node origins. Adding a duplicate returns false.
 */
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

/*
 * graphGetStats(<graph id>, [reset]) -> int array:
 *   [0] nodes  [1] edges  [2] searches  [3] partials  [4] expansionsTotal
 *   [5] expansionsMax  [6] usMin  [7] usAvg  [8] usMax
 * Counters accumulate per graph since creation (or the last reset); pass a
 * truthy second argument to reset them after reading.
 */
void gsc_graph_get_stats()
{
	int id;
	int reset = 0;
	AStarGraph *graph;
	unsigned int usAvg = 0;

	if ( !stackGetParams("i", &id) )
	{
		stackError("gsc_graph_get_stats() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( Scr_GetNumParam() >= 2 && !stackGetParamInt(1, &reset) )
	{
		stackError("gsc_graph_get_stats() reset argument has a wrong type");
		stackPushUndefined();
		return;
	}

	graph = graphById(id);

	if ( !graph )
	{
		stackError("gsc_graph_get_stats() graph %i does not exist", id);
		stackPushUndefined();
		return;
	}

	if ( graph->stats.searches )
		usAvg = graph->stats.usTotal / graph->stats.searches;

	stackPushArray();
	stackPushInt((int)graph->nodes.size());
	stackPushArrayLast();
	stackPushInt((int)graph->edgeCount);
	stackPushArrayLast();
	stackPushInt((int)graph->stats.searches);
	stackPushArrayLast();
	stackPushInt((int)graph->stats.partials);
	stackPushArrayLast();
	stackPushInt((int)graph->stats.expansionsTotal);
	stackPushArrayLast();
	stackPushInt((int)graph->stats.expansionsMax);
	stackPushArrayLast();
	stackPushInt((int)graph->stats.usMin);
	stackPushArrayLast();
	stackPushInt((int)usAvg);
	stackPushArrayLast();
	stackPushInt((int)graph->stats.usMax);
	stackPushArrayLast();

	if ( reset )
		memset(&graph->stats, 0, sizeof(graph->stats));
}

/*
 * Erases non-persistent graphs on map change / map_restart. Wired from
 * custom_SV_SpawnServer so per-map graphs never leak across loads.
 */
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
