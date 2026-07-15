#include "gsc_graph.hpp"

#if COMPILE_GRAPH == 1

#include <vector>
#include <math.h>
#include <float.h>
#include <string.h>
#include <stdlib.h>

#define GRAPH_MAX_PATH 64

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
	bool removed;
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

/*
 * A cached single-destination shortest-path tree (reverse Dijkstra from a goal):
 * for every node, the cost to reach the goal and the next hop toward it. Valid
 * for one (goal, skip-mask) combination until any mutation invalidates it.
 */
struct GraphPrecompute
{
	int goal;
	int skipNodeTypes;
	int skipEdgeTypes;
	std::vector<float> costToGoal;
	std::vector<int> nextHop;
};

struct AStarGraph
{
	int id;
	bool persist;
	unsigned int edgeCount;
	std::vector<GraphNode> nodes;
	GraphScratch scratch;
	GraphStats stats;
	std::vector<GraphPrecompute> precomputes;
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

// Drops every cached shortest-path tree - called after any change to the graph
// so a stale precompute can never be consulted.
static void graphInvalidatePrecompute(AStarGraph *graph)
{
	graph->precomputes.clear();
}

// The cached shortest-path tree matching a goal and skip-mask combination, or NULL.
static GraphPrecompute *findPrecompute(AStarGraph *graph, int goal, int skipNodeTypes, int skipEdgeTypes)
{
	for ( size_t i = 0; i < graph->precomputes.size(); i++ )
	{
		GraphPrecompute &pc = graph->precomputes[i];

		if ( pc.goal == goal && pc.skipNodeTypes == skipNodeTypes && pc.skipEdgeTypes == skipEdgeTypes )
			return &graph->precomputes[i];
	}

	return NULL;
}

// Index of the directed from->to edge within node `from`'s edge list, or -1.
static int graphFindEdgeIndex(AStarGraph *graph, int from, int to)
{
	if ( from < 0 || from >= (int)graph->nodes.size() )
		return -1;

	std::vector<GraphEdge> &edges = graph->nodes[from].edges;

	for ( size_t i = 0; i < edges.size(); i++ )
	{
		if ( edges[i].to == (unsigned int)to )
			return (int)i;
	}

	return -1;
}

static float edgeDistance(const vec3_t a, const vec3_t b)
{
	float dx = b[0] - a[0];
	float dy = b[1] - a[1];
	float dz = b[2] - a[2];

	return sqrtf(dx * dx + dy * dy + dz * dz);
}

// Standard bit-serial CRC-32 (poly 0xEDB88320) for the save-file integrity check.
static unsigned int graphCrc32(const unsigned char *data, size_t len)
{
	unsigned int crc = 0xFFFFFFFF;
	size_t i;
	int k;

	for ( i = 0; i < len; i++ )
	{
		crc ^= data[i];
		for ( k = 0; k < 8; k++ )
			crc = (crc >> 1) ^ (0xEDB88320 & (0 - (crc & 1)));
	}

	return ~crc;
}

/* Squared distance from point p to the segment ab, with the projection clamped
 * to the segment ends. */
static float pointSegmentDistSq(const vec3_t p, const vec3_t a, const vec3_t b)
{
	float abx = b[0] - a[0];
	float aby = b[1] - a[1];
	float abz = b[2] - a[2];
	float apx = p[0] - a[0];
	float apy = p[1] - a[1];
	float apz = p[2] - a[2];
	float abLen2 = abx * abx + aby * aby + abz * abz;
	float t = 0;
	float cx;
	float cy;
	float cz;
	float dx;
	float dy;
	float dz;

	if ( abLen2 > 0 )
		t = (apx * abx + apy * aby + apz * abz) / abLen2;

	if ( t < 0 )
		t = 0;
	if ( t > 1 )
		t = 1;

	cx = a[0] + abx * t;
	cy = a[1] + aby * t;
	cz = a[2] + abz * t;

	dx = p[0] - cx;
	dy = p[1] - cy;
	dz = p[2] - cz;

	return dx * dx + dy * dy + dz * dz;
}

static unsigned int graphMicroseconds()
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return (unsigned int)(tv.tv_sec * 1000000 + tv.tv_usec);
}

// Fold one completed search into the per-graph stats (shared by the A* path and
// the precompute fast path so both are measured identically).
static void graphRecordSearch(AStarGraph *graph, unsigned int started, unsigned int expansions)
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

	if ( graph->nodes[start].removed || graph->nodes[goal].removed )
	{
		stackError("gsc_graph_find_path() start or goal node has been removed");
		stackPushUndefined();
		return;
	}

	started = graphMicroseconds();

	// A precomputed shortest-path tree answers instantly when there is no
	// per-call skip-node list and the goal + masks match a cached tree.
	if ( numParam < 4 || stackGetParamType(3) == VAR_UNDEFINED )
	{
		GraphPrecompute *pc = findPrecompute(graph, goal, skipNodeTypes, skipEdgeTypes);

		if ( pc && pc->costToGoal[start] < FLT_MAX )
		{
			unsigned int cur = (unsigned int)start;
			unsigned int guard = 0;
			unsigned int safety = (unsigned int)graph->nodes.size() + 1;

			stackPushArray();
			while ( guard < safety )
			{
				stackPushInt((int)cur);
				stackPushArrayLast();

				if ( cur == (unsigned int)goal )
					break;
				if ( pc->nextHop[cur] < 0 )
					break;

				cur = (unsigned int)pc->nextHop[cur];
				guard++;
			}

			graphDebugPrint("findPath g%i %i -> %i: cached, %u hops", id, start, goal, guard + 1);
			graphRecordSearch(graph, started, 0);
			return;
		}
	}

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
	graphRecordSearch(graph, started, expansions);
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
		if ( graph->nodes[i].removed )
			continue;

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

/*
 * graphFindClosestEdge(<graph id>, <origin>) -> [from, to] of the nearest edge
 * by point-to-segment distance, or undefined when the graph has no edges.
 */
void gsc_graph_find_closest_edge()
{
	int id;
	vec3_t origin;
	AStarGraph *graph;
	int bestFrom = -1;
	int bestTo = -1;
	float bestD = 0;

	if ( !stackGetParams("iv", &id, origin) )
	{
		stackError("gsc_graph_find_closest_edge() one or more arguments are undefined or have a wrong type");
		stackPushUndefined();
		return;
	}

	graph = graphById(id);

	if ( !graph )
	{
		stackError("gsc_graph_find_closest_edge() graph %i does not exist", id);
		stackPushUndefined();
		return;
	}

	for ( size_t i = 0; i < graph->nodes.size(); i++ )
	{
		GraphNode &node = graph->nodes[i];

		for ( size_t e = 0; e < node.edges.size(); e++ )
		{
			unsigned int to = node.edges[e].to;
			float d = pointSegmentDistSq(origin, node.origin, graph->nodes[to].origin);

			if ( bestFrom >= 0 && d >= bestD )
				continue;

			bestFrom = (int)i;
			bestTo = (int)to;
			bestD = d;
		}
	}

	if ( bestFrom < 0 )
	{
		stackPushUndefined();
		return;
	}

	stackPushArray();
	stackPushInt(bestFrom);
	stackPushArrayLast();
	stackPushInt(bestTo);
	stackPushArrayLast();
}

/*
 * graphGetAllNodes(<graph id>, [origin], [max dist sq]) -> array of node ids.
 * With an origin and a squared radius, only nodes within that radius are
 * returned - a cheap spatial filter for viz and "nodes near me" queries.
 */
void gsc_graph_get_all_nodes()
{
	int id;
	vec3_t origin;
	float maxDistSq = -1;
	int haveOrigin = 0;
	AStarGraph *graph;

	if ( !stackGetParams("i", &id) )
	{
		stackError("gsc_graph_get_all_nodes() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( Scr_GetNumParam() >= 2 && stackGetParamType(1) != VAR_UNDEFINED )
	{
		if ( !stackGetParamVector(1, origin) )
		{
			stackError("gsc_graph_get_all_nodes() origin argument has a wrong type");
			stackPushUndefined();
			return;
		}

		haveOrigin = 1;
	}

	if ( Scr_GetNumParam() >= 3 && !stackGetParamFloat(2, &maxDistSq) )
	{
		stackError("gsc_graph_get_all_nodes() max dist sq argument has a wrong type");
		stackPushUndefined();
		return;
	}

	graph = graphById(id);

	if ( !graph )
	{
		stackError("gsc_graph_get_all_nodes() graph %i does not exist", id);
		stackPushUndefined();
		return;
	}

	stackPushArray();

	for ( size_t i = 0; i < graph->nodes.size(); i++ )
	{
		if ( graph->nodes[i].removed )
			continue;

		if ( haveOrigin && maxDistSq >= 0 )
		{
			float dx = graph->nodes[i].origin[0] - origin[0];
			float dy = graph->nodes[i].origin[1] - origin[1];
			float dz = graph->nodes[i].origin[2] - origin[2];

			if ( dx * dx + dy * dy + dz * dz > maxDistSq )
				continue;
		}

		stackPushInt((int)i);
		stackPushArrayLast();
	}
}

/*
 * graphGetAllEdges(<graph id>) -> flat array of directed edges as
 * [from0, to0, from1, to1, ...]. Read two ids per edge; a two-way link shows
 * up as two entries because each direction is stored separately.
 */
void gsc_graph_get_all_edges()
{
	int id;
	AStarGraph *graph;

	if ( !stackGetParams("i", &id) )
	{
		stackError("gsc_graph_get_all_edges() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	graph = graphById(id);

	if ( !graph )
	{
		stackError("gsc_graph_get_all_edges() graph %i does not exist", id);
		stackPushUndefined();
		return;
	}

	stackPushArray();

	for ( size_t i = 0; i < graph->nodes.size(); i++ )
	{
		GraphNode &node = graph->nodes[i];

		for ( size_t e = 0; e < node.edges.size(); e++ )
		{
			stackPushInt((int)i);
			stackPushArrayLast();
			stackPushInt((int)node.edges[e].to);
			stackPushArrayLast();
		}
	}
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
	node.removed = false;

	graph->nodes.push_back(node);
	graphInvalidatePrecompute(graph);

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

	if ( graph->nodes[from].removed || graph->nodes[to].removed )
	{
		stackError("gsc_graph_add_edge() cannot add an edge touching a removed node");
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
	graphInvalidatePrecompute(graph);

	stackPushBool(qtrue);
}

/*
 * graphGetEdgeProperties(<graph id>, <from>, <to>) -> [type, cost], or
 * undefined when there is no such directed edge.
 */
void gsc_graph_get_edge_properties()
{
	int id;
	int from;
	int to;
	AStarGraph *graph;
	int idx;

	if ( !stackGetParams("iii", &id, &from, &to) )
	{
		stackError("gsc_graph_get_edge_properties() one or more arguments are undefined or have a wrong type");
		stackPushUndefined();
		return;
	}

	graph = graphById(id);

	if ( !graph )
	{
		stackError("gsc_graph_get_edge_properties() graph %i does not exist", id);
		stackPushUndefined();
		return;
	}

	idx = graphFindEdgeIndex(graph, from, to);

	if ( idx < 0 )
	{
		stackPushUndefined();
		return;
	}

	GraphEdge &edge = graph->nodes[from].edges[idx];

	stackPushArray();
	stackPushInt(edge.type);
	stackPushArrayLast();
	stackPushFloat(edge.cost);
	stackPushArrayLast();
}

// graphSetEdgeCost(<graph id>, <from>, <to>, <cost>) -> true, or false if the edge is missing
void gsc_graph_set_edge_cost()
{
	int id;
	int from;
	int to;
	float cost;
	AStarGraph *graph;
	int idx;

	if ( !stackGetParams("iii", &id, &from, &to) )
	{
		stackError("gsc_graph_set_edge_cost() one or more arguments are undefined or have a wrong type");
		stackPushUndefined();
		return;
	}

	if ( !stackGetParamFloat(3, &cost) )
	{
		stackError("gsc_graph_set_edge_cost() cost argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	// A* requires non-negative edge costs; a negative one would break the search
	if ( cost < 0 )
	{
		stackError("gsc_graph_set_edge_cost() cost must be non-negative");
		stackPushUndefined();
		return;
	}

	graph = graphById(id);

	if ( !graph )
	{
		stackError("gsc_graph_set_edge_cost() graph %i does not exist", id);
		stackPushUndefined();
		return;
	}

	idx = graphFindEdgeIndex(graph, from, to);

	if ( idx < 0 )
	{
		stackPushBool(qfalse);
		return;
	}

	graph->nodes[from].edges[idx].cost = cost;
	graphInvalidatePrecompute(graph);
	stackPushBool(qtrue);
}

// graphSetEdgeType(<graph id>, <from>, <to>, <type>) -> true, or false if the edge is missing
void gsc_graph_set_edge_type()
{
	int id;
	int from;
	int to;
	int type;
	AStarGraph *graph;
	int idx;

	if ( !stackGetParams("iiii", &id, &from, &to, &type) )
	{
		stackError("gsc_graph_set_edge_type() one or more arguments are undefined or have a wrong type");
		stackPushUndefined();
		return;
	}

	graph = graphById(id);

	if ( !graph )
	{
		stackError("gsc_graph_set_edge_type() graph %i does not exist", id);
		stackPushUndefined();
		return;
	}

	idx = graphFindEdgeIndex(graph, from, to);

	if ( idx < 0 )
	{
		stackPushBool(qfalse);
		return;
	}

	graph->nodes[from].edges[idx].type = type;
	graphInvalidatePrecompute(graph);
	stackPushBool(qtrue);
}

/*
 * graphRemoveEdge(<graph id>, <from>, <to>) -> true, or false when there is no
 * such directed edge. Only this one direction is removed.
 */
void gsc_graph_remove_edge()
{
	int id;
	int from;
	int to;
	AStarGraph *graph;
	int idx;

	if ( !stackGetParams("iii", &id, &from, &to) )
	{
		stackError("gsc_graph_remove_edge() one or more arguments are undefined or have a wrong type");
		stackPushUndefined();
		return;
	}

	graph = graphById(id);

	if ( !graph )
	{
		stackError("gsc_graph_remove_edge() graph %i does not exist", id);
		stackPushUndefined();
		return;
	}

	idx = graphFindEdgeIndex(graph, from, to);

	if ( idx < 0 )
	{
		stackPushBool(qfalse);
		return;
	}

	graph->nodes[from].edges.erase(graph->nodes[from].edges.begin() + idx);
	graph->edgeCount--;
	graphInvalidatePrecompute(graph);
	stackPushBool(qtrue);
}

/*
 * graphGetNodeProperties(<graph id>, <node id>) -> [type, outgoing edge count],
 * or undefined for an invalid node. Use graphGetNodeOrigin for the position.
 */
void gsc_graph_get_node_properties()
{
	int id;
	int nodeId;
	AStarGraph *graph;

	if ( !stackGetParams("ii", &id, &nodeId) )
	{
		stackError("gsc_graph_get_node_properties() one or more arguments are undefined or have a wrong type");
		stackPushUndefined();
		return;
	}

	graph = graphById(id);

	if ( !graph )
	{
		stackError("gsc_graph_get_node_properties() graph %i does not exist", id);
		stackPushUndefined();
		return;
	}

	if ( nodeId < 0 || nodeId >= (int)graph->nodes.size() )
	{
		stackPushUndefined();
		return;
	}

	if ( graph->nodes[nodeId].removed )
	{
		stackPushUndefined();
		return;
	}

	stackPushArray();
	stackPushInt(graph->nodes[nodeId].type);
	stackPushArrayLast();
	stackPushInt((int)graph->nodes[nodeId].edges.size());
	stackPushArrayLast();
}

// graphSetNodeType(<graph id>, <node id>, <type>) -> true, or false for an invalid node
void gsc_graph_set_node_type()
{
	int id;
	int nodeId;
	int type;
	AStarGraph *graph;

	if ( !stackGetParams("iii", &id, &nodeId, &type) )
	{
		stackError("gsc_graph_set_node_type() one or more arguments are undefined or have a wrong type");
		stackPushUndefined();
		return;
	}

	graph = graphById(id);

	if ( !graph )
	{
		stackError("gsc_graph_set_node_type() graph %i does not exist", id);
		stackPushUndefined();
		return;
	}

	if ( nodeId < 0 || nodeId >= (int)graph->nodes.size() )
	{
		stackPushBool(qfalse);
		return;
	}

	graph->nodes[nodeId].type = type;
	graphInvalidatePrecompute(graph);
	stackPushBool(qtrue);
}

/*
 * graphSetNodeOrigin(<graph id>, <node id>, <origin>) -> true, or false for an
 * invalid node. Recomputes the distance cost of every edge touching the node
 * (both directions), so any manually-set costs on those edges are reset.
 */
void gsc_graph_set_node_origin()
{
	int id;
	int nodeId;
	vec3_t origin;
	AStarGraph *graph;

	if ( !stackGetParams("ii", &id, &nodeId) )
	{
		stackError("gsc_graph_set_node_origin() one or more arguments are undefined or have a wrong type");
		stackPushUndefined();
		return;
	}

	if ( !stackGetParamVector(2, origin) )
	{
		stackError("gsc_graph_set_node_origin() origin argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	graph = graphById(id);

	if ( !graph )
	{
		stackError("gsc_graph_set_node_origin() graph %i does not exist", id);
		stackPushUndefined();
		return;
	}

	if ( nodeId < 0 || nodeId >= (int)graph->nodes.size() )
	{
		stackPushBool(qfalse);
		return;
	}

	VectorCopy(origin, graph->nodes[nodeId].origin);

	// Outgoing edges from the moved node
	for ( size_t e = 0; e < graph->nodes[nodeId].edges.size(); e++ )
	{
		GraphEdge &edge = graph->nodes[nodeId].edges[e];
		edge.cost = edgeDistance(graph->nodes[nodeId].origin, graph->nodes[edge.to].origin);
	}

	// Incoming edges from any other node to the moved node
	for ( size_t i = 0; i < graph->nodes.size(); i++ )
	{
		if ( (int)i == nodeId )
			continue;

		for ( size_t e = 0; e < graph->nodes[i].edges.size(); e++ )
		{
			if ( graph->nodes[i].edges[e].to == (unsigned int)nodeId )
				graph->nodes[i].edges[e].cost = edgeDistance(graph->nodes[i].origin, graph->nodes[nodeId].origin);
		}
	}

	graphInvalidatePrecompute(graph);
	stackPushBool(qtrue);
}

/*
 * graphGetNodeIdsAccessibleFrom(<graph id>, <node id>) -> array of every node
 * reachable from it by following edges forward (includes the node itself), or
 * undefined for an invalid node. A breadth-first walk of the component.
 */
void gsc_graph_get_node_ids_accessible_from()
{
	int id;
	int nodeId;
	AStarGraph *graph;
	size_t head;

	if ( !stackGetParams("ii", &id, &nodeId) )
	{
		stackError("gsc_graph_get_node_ids_accessible_from() one or more arguments are undefined or have a wrong type");
		stackPushUndefined();
		return;
	}

	graph = graphById(id);

	if ( !graph )
	{
		stackError("gsc_graph_get_node_ids_accessible_from() graph %i does not exist", id);
		stackPushUndefined();
		return;
	}

	if ( nodeId < 0 || nodeId >= (int)graph->nodes.size() )
	{
		stackPushUndefined();
		return;
	}

	if ( graph->nodes[nodeId].removed )
	{
		stackPushUndefined();
		return;
	}

	std::vector<char> visited(graph->nodes.size(), 0);
	std::vector<unsigned int> queue;

	visited[nodeId] = 1;
	queue.push_back((unsigned int)nodeId);

	stackPushArray();

	head = 0;
	while ( head < queue.size() )
	{
		unsigned int cur = queue[head++];

		stackPushInt((int)cur);
		stackPushArrayLast();

		GraphNode &node = graph->nodes[cur];
		for ( size_t e = 0; e < node.edges.size(); e++ )
		{
			unsigned int to = node.edges[e].to;

			if ( !visited[to] )
			{
				visited[to] = 1;
				queue.push_back(to);
			}
		}
	}
}

/*
 * graphGetNodeIdsAccessibleTo(<graph id>, <node id>) -> array of every node
 * that can reach it by following edges forward (includes the node itself), or
 * undefined for an invalid node. BFS over a temporary reverse adjacency.
 */
void gsc_graph_get_node_ids_accessible_to()
{
	int id;
	int nodeId;
	AStarGraph *graph;
	size_t head;

	if ( !stackGetParams("ii", &id, &nodeId) )
	{
		stackError("gsc_graph_get_node_ids_accessible_to() one or more arguments are undefined or have a wrong type");
		stackPushUndefined();
		return;
	}

	graph = graphById(id);

	if ( !graph )
	{
		stackError("gsc_graph_get_node_ids_accessible_to() graph %i does not exist", id);
		stackPushUndefined();
		return;
	}

	if ( nodeId < 0 || nodeId >= (int)graph->nodes.size() )
	{
		stackPushUndefined();
		return;
	}

	if ( graph->nodes[nodeId].removed )
	{
		stackPushUndefined();
		return;
	}

	// Reverse adjacency: for each node, the list of nodes with an edge into it
	std::vector< std::vector<unsigned int> > incoming(graph->nodes.size());
	for ( size_t i = 0; i < graph->nodes.size(); i++ )
	{
		for ( size_t e = 0; e < graph->nodes[i].edges.size(); e++ )
			incoming[graph->nodes[i].edges[e].to].push_back((unsigned int)i);
	}

	std::vector<char> visited(graph->nodes.size(), 0);
	std::vector<unsigned int> queue;

	visited[nodeId] = 1;
	queue.push_back((unsigned int)nodeId);

	stackPushArray();

	head = 0;
	while ( head < queue.size() )
	{
		unsigned int cur = queue[head++];

		stackPushInt((int)cur);
		stackPushArrayLast();

		std::vector<unsigned int> &in = incoming[cur];
		for ( size_t k = 0; k < in.size(); k++ )
		{
			unsigned int from = in[k];

			if ( !visited[from] )
			{
				visited[from] = 1;
				queue.push_back(from);
			}
		}
	}
}

/*
 * graphRemoveNode(<graph id>, <node id>) -> true, or false for an invalid or
 * already-removed node. A tombstone delete: the slot stays so existing node ids
 * keep their meaning (the id==index contract holds), but the node is flagged
 * removed, its outgoing edges are dropped, and every edge into it is removed.
 * All queries then skip it.
 */
void gsc_graph_remove_node()
{
	int id;
	int nodeId;
	AStarGraph *graph;

	if ( !stackGetParams("ii", &id, &nodeId) )
	{
		stackError("gsc_graph_remove_node() one or more arguments are undefined or have a wrong type");
		stackPushUndefined();
		return;
	}

	graph = graphById(id);

	if ( !graph )
	{
		stackError("gsc_graph_remove_node() graph %i does not exist", id);
		stackPushUndefined();
		return;
	}

	if ( nodeId < 0 || nodeId >= (int)graph->nodes.size() || graph->nodes[nodeId].removed )
	{
		stackPushBool(qfalse);
		return;
	}

	// Drop its outgoing edges
	graph->edgeCount -= (unsigned int)graph->nodes[nodeId].edges.size();
	graph->nodes[nodeId].edges.clear();

	// Drop every edge pointing to it
	for ( size_t i = 0; i < graph->nodes.size(); i++ )
	{
		std::vector<GraphEdge> &edges = graph->nodes[i].edges;

		for ( size_t e = 0; e < edges.size(); )
		{
			if ( edges[e].to == (unsigned int)nodeId )
			{
				edges.erase(edges.begin() + e);
				graph->edgeCount--;
			}
			else
			{
				e++;
			}
		}
	}

	graph->nodes[nodeId].removed = true;
	graphInvalidatePrecompute(graph);
	stackPushBool(qtrue);
}

/*
 * graphPrecomputePathsToNode(<graph id>, <goal>, [skip node types], [skip edge
 * types]) -> true, or false for an invalid/removed goal. Runs one reverse
 * Dijkstra from the goal and caches, for every node, the cost to reach it and
 * the next hop toward it. graphFindPath then answers any start -> this goal
 * (same masks, no per-call skip-node list) by walking the cached hops with no
 * search. Any mutation to the graph invalidates every cache.
 */
void gsc_graph_precompute_paths_to_node()
{
	int id;
	int goal;
	int skipNodeTypes = 0;
	int skipEdgeTypes = 0;
	AStarGraph *graph;
	int numParam;
	size_t n;
	GraphPrecompute *pc;

	if ( !stackGetParams("ii", &id, &goal) )
	{
		stackError("gsc_graph_precompute_paths_to_node() one or more arguments are undefined or have a wrong type");
		stackPushUndefined();
		return;
	}

	numParam = Scr_GetNumParam();

	if ( numParam >= 3 && stackGetParamType(2) != VAR_UNDEFINED && !stackGetParamInt(2, &skipNodeTypes) )
	{
		stackError("gsc_graph_precompute_paths_to_node() skip node types argument has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( numParam >= 4 && stackGetParamType(3) != VAR_UNDEFINED && !stackGetParamInt(3, &skipEdgeTypes) )
	{
		stackError("gsc_graph_precompute_paths_to_node() skip edge types argument has a wrong type");
		stackPushUndefined();
		return;
	}

	graph = graphById(id);

	if ( !graph )
	{
		stackError("gsc_graph_precompute_paths_to_node() graph %i does not exist", id);
		stackPushUndefined();
		return;
	}

	if ( goal < 0 || goal >= (int)graph->nodes.size() || graph->nodes[goal].removed )
	{
		stackPushBool(qfalse);
		return;
	}

	n = graph->nodes.size();

	// Reverse adjacency with costs: for each node, the (predecessor, cost) of
	// every edge pointing into it, honouring the edge-type skip mask. GraphHeapEntry
	// is reused as the pair - f holds the edge cost, node holds the predecessor.
	std::vector< std::vector<GraphHeapEntry> > incoming(n);
	for ( size_t i = 0; i < n; i++ )
	{
		if ( graph->nodes[i].removed )
			continue;

		GraphNode &node = graph->nodes[i];
		for ( size_t e = 0; e < node.edges.size(); e++ )
		{
			GraphHeapEntry ie;

			if ( skipEdgeTypes && (node.edges[e].type & skipEdgeTypes) )
				continue;

			ie.f = node.edges[e].cost;
			ie.node = (unsigned int)i;
			incoming[node.edges[e].to].push_back(ie);
		}
	}

	std::vector<float> dist(n, FLT_MAX);
	std::vector<int> nextHop(n, -1);
	std::vector<char> settled(n, 0);
	std::vector<GraphHeapEntry> heap;

	dist[goal] = 0;
	heapPush(heap, 0, (unsigned int)goal);

	while ( !heap.empty() )
	{
		GraphHeapEntry top = heapPop(heap);
		unsigned int cur = top.node;

		if ( settled[cur] || top.f != dist[cur] )
			continue;

		settled[cur] = 1;

		// A skip-typed node may be a query's start but can never be routed
		// through, so keep its own cost yet do not expand its predecessors.
		if ( skipNodeTypes && (graph->nodes[cur].type & skipNodeTypes) )
			continue;

		std::vector<GraphHeapEntry> &in = incoming[cur];
		for ( size_t k = 0; k < in.size(); k++ )
		{
			unsigned int from = in[k].node;
			float tentative = dist[cur] + in[k].f;

			if ( tentative < dist[from] )
			{
				dist[from] = tentative;
				nextHop[from] = (int)cur;
				heapPush(heap, tentative, from);
			}
		}
	}

	// Store (or refresh) the cache entry for this goal + mask combination
	pc = findPrecompute(graph, goal, skipNodeTypes, skipEdgeTypes);
	if ( !pc )
	{
		if ( graph->precomputes.size() >= 8 )
			graph->precomputes.erase(graph->precomputes.begin());

		GraphPrecompute fresh;
		graph->precomputes.push_back(fresh);
		pc = &graph->precomputes.back();
	}

	pc->goal = goal;
	pc->skipNodeTypes = skipNodeTypes;
	pc->skipEdgeTypes = skipEdgeTypes;
	pc->costToGoal.swap(dist);
	pc->nextHop.swap(nextHop);

	graphDebugPrint("precomputed paths to node %i (skipNode %i, skipEdge %i)", goal, skipNodeTypes, skipEdgeTypes);
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

	if ( graph->nodes[nodeId].removed )
	{
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
 * graphSave(<graph id>, <relative path>) -> true/false. Writes the graph as a
 * binary G2G1 file through the engine FS (sandboxed to fs_homepath/fs_gamedir,
 * with ".." rejected and the path under 64 bytes). Layout: 20-byte header
 * (magic|version|flags|nodeCount|edgeCount|crc32) then nodeCount node records
 * {origin[3], type, removed} then edgeCount edge records {from, to, type, cost}.
 */
void gsc_graph_save()
{
	int id;
	const char *path;
	AStarGraph *graph;
	unsigned char *buffer;
	unsigned char *p;
	unsigned char *crcField;
	size_t nodeCount;
	size_t edgeCount;
	size_t total;
	unsigned int crc;
	unsigned short version = 1;
	unsigned short flags = 0;
	fileHandle_t f;
	int written;

	if ( !stackGetParams("is", &id, &path) )
	{
		stackError("gsc_graph_save() one or more arguments are undefined or have a wrong type");
		stackPushBool(qfalse);
		return;
	}

	if ( strlen(path) >= GRAPH_MAX_PATH )
	{
		stackError("gsc_graph_save() path '%s' exceeds %d bytes (engine MAX_QPATH)", path, GRAPH_MAX_PATH);
		stackPushBool(qfalse);
		return;
	}

	graph = graphById(id);

	if ( !graph )
	{
		stackError("gsc_graph_save() graph %i does not exist", id);
		stackPushBool(qfalse);
		return;
	}

	nodeCount = graph->nodes.size();
	edgeCount = graph->edgeCount;
	total = 20 + nodeCount * 20 + edgeCount * 16;

	buffer = (unsigned char *)malloc(total);

	if ( !buffer )
	{
		stackError("gsc_graph_save() out of memory for %u bytes", (unsigned int)total);
		stackPushBool(qfalse);
		return;
	}

	p = buffer;
	memcpy(p, "G2G1", 4);
	p += 4;
	memcpy(p, &version, 2);
	p += 2;
	memcpy(p, &flags, 2);
	p += 2;
	{
		unsigned int nc = (unsigned int)nodeCount;
		unsigned int ec = (unsigned int)edgeCount;

		memcpy(p, &nc, 4);
		p += 4;
		memcpy(p, &ec, 4);
		p += 4;
	}
	crcField = p;
	p += 4;

	for ( size_t i = 0; i < nodeCount; i++ )
	{
		GraphNode &node = graph->nodes[i];
		int removed = node.removed ? 1 : 0;

		memcpy(p, node.origin, 12);
		p += 12;
		memcpy(p, &node.type, 4);
		p += 4;
		memcpy(p, &removed, 4);
		p += 4;
	}

	for ( size_t i = 0; i < nodeCount; i++ )
	{
		GraphNode &node = graph->nodes[i];

		for ( size_t e = 0; e < node.edges.size(); e++ )
		{
			unsigned int from = (unsigned int)i;
			unsigned int to = node.edges[e].to;

			memcpy(p, &from, 4);
			p += 4;
			memcpy(p, &to, 4);
			p += 4;
			memcpy(p, &node.edges[e].type, 4);
			p += 4;
			memcpy(p, &node.edges[e].cost, 4);
			p += 4;
		}
	}

	crc = graphCrc32(buffer + 20, total - 20);
	memcpy(crcField, &crc, 4);

	f = FS_FOpenFileWrite(path);

	if ( f == 0 )
	{
		free(buffer);
		stackError("gsc_graph_save() could not open '%s' for writing", path);
		stackPushBool(qfalse);
		return;
	}

	written = FS_Write(buffer, (int)total, f);
	FS_FCloseFile(f);
	free(buffer);

	graphDebugPrint("saved graph %i to '%s' (%u nodes, %u edges, %u bytes)", id, path, (unsigned int)nodeCount, (unsigned int)edgeCount, (unsigned int)total);
	stackPushBool(written == (int)total ? qtrue : qfalse);
}

/*
 * graphLoad(<relative path>, [persist]) -> new graph id, or undefined. Reads a
 * G2G1 file through the engine FS and validates magic, version, node cap, exact
 * file size, crc32 and every edge id before building anything - on any failure
 * it returns undefined and creates no graph (never a partial load).
 */
void gsc_graph_load()
{
	const char *path;
	int persist = 0;
	fileHandle_t f;
	int len;
	int bytesRead;
	unsigned char *buffer;
	unsigned char *p;
	unsigned int nodeCount;
	unsigned int edgeCount;
	unsigned int fileCrc;
	unsigned int calcCrc;
	unsigned short version;
	unsigned short flags;
	size_t expected;
	int id;
	AStarGraph graph;

	if ( !stackGetParams("s", &path) )
	{
		stackError("gsc_graph_load() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( Scr_GetNumParam() >= 2 && !stackGetParamInt(1, &persist) )
	{
		stackError("gsc_graph_load() persist argument has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( strlen(path) >= GRAPH_MAX_PATH )
	{
		stackError("gsc_graph_load() path '%s' exceeds %d bytes (engine MAX_QPATH)", path, GRAPH_MAX_PATH);
		stackPushUndefined();
		return;
	}

	len = FS_FOpenFileByMode(path, &f, FS_READ);

	if ( len <= 0 )
	{
		// Missing or empty file: quiet undefined (supports try-load), but still
		// close a stray handle from an existing 0-byte file so it does not leak.
		if ( f != 0 )
			FS_FCloseFile(f);
		stackPushUndefined();
		return;
	}

	if ( len < 20 )
	{
		FS_FCloseFile(f);
		stackError("gsc_graph_load() '%s' is too small to be a graph file", path);
		stackPushUndefined();
		return;
	}

	buffer = (unsigned char *)malloc(len);

	if ( !buffer )
	{
		FS_FCloseFile(f);
		stackError("gsc_graph_load() out of memory reading '%s'", path);
		stackPushUndefined();
		return;
	}

	bytesRead = FS_Read(buffer, len, f);
	FS_FCloseFile(f);

	if ( bytesRead != len )
	{
		free(buffer);
		stackError("gsc_graph_load() short read on '%s'", path);
		stackPushUndefined();
		return;
	}

	p = buffer;

	if ( memcmp(p, "G2G1", 4) != 0 )
	{
		free(buffer);
		stackError("gsc_graph_load() '%s' has a bad magic (not a G2G1 graph file)", path);
		stackPushUndefined();
		return;
	}

	p += 4;
	memcpy(&version, p, 2);
	p += 2;
	memcpy(&flags, p, 2);
	p += 2;
	memcpy(&nodeCount, p, 4);
	p += 4;
	memcpy(&edgeCount, p, 4);
	p += 4;
	memcpy(&fileCrc, p, 4);
	p += 4;

	if ( version != 1 )
	{
		free(buffer);
		stackError("gsc_graph_load() '%s' has unsupported version %u", path, version);
		stackPushUndefined();
		return;
	}

	if ( (int)nodeCount > sv_graphMaxNodes->current.integer )
	{
		free(buffer);
		stackError("gsc_graph_load() '%s' has %u nodes, over sv_graphMaxNodes %i", path, nodeCount, sv_graphMaxNodes->current.integer);
		stackPushUndefined();
		return;
	}

	expected = 20 + (size_t)nodeCount * 20 + (size_t)edgeCount * 16;

	if ( (size_t)len != expected )
	{
		free(buffer);
		stackError("gsc_graph_load() '%s' size mismatch (%d bytes, expected %u)", path, len, (unsigned int)expected);
		stackPushUndefined();
		return;
	}

	calcCrc = graphCrc32(buffer + 20, (size_t)len - 20);

	if ( calcCrc != fileCrc )
	{
		free(buffer);
		stackError("gsc_graph_load() '%s' checksum mismatch - file is corrupt", path);
		stackPushUndefined();
		return;
	}

	if ( (int)graphs.size() >= sv_graphMaxGraphs->current.integer )
	{
		free(buffer);
		stackError("gsc_graph_load() graph limit reached (sv_graphMaxGraphs is %i)", sv_graphMaxGraphs->current.integer);
		stackPushUndefined();
		return;
	}

	// Everything validated: build the graph into a local, only publish on success
	id = 0;
	while ( graphById(id) )
		id++;

	graph.id = id;
	graph.persist = persist != 0;
	graph.edgeCount = 0;
	graph.scratch.searchId = 0;
	memset(&graph.stats, 0, sizeof(graph.stats));
	graph.nodes.reserve(nodeCount);

	for ( unsigned int i = 0; i < nodeCount; i++ )
	{
		GraphNode node;
		int removed;

		memcpy(node.origin, p, 12);
		p += 12;
		memcpy(&node.type, p, 4);
		p += 4;
		memcpy(&removed, p, 4);
		p += 4;
		node.removed = removed != 0;
		graph.nodes.push_back(node);
	}

	for ( unsigned int e = 0; e < edgeCount; e++ )
	{
		unsigned int from;
		unsigned int to;
		int type;
		float cost;
		GraphEdge edge;

		memcpy(&from, p, 4);
		p += 4;
		memcpy(&to, p, 4);
		p += 4;
		memcpy(&type, p, 4);
		p += 4;
		memcpy(&cost, p, 4);
		p += 4;

		if ( from >= nodeCount || to >= nodeCount )
		{
			free(buffer);
			stackError("gsc_graph_load() '%s' edge %u references an out-of-range node - load aborted", path, e);
			stackPushUndefined();
			return;
		}

		edge.to = to;
		edge.type = type;
		edge.cost = cost;
		graph.nodes[from].edges.push_back(edge);
		graph.edgeCount++;
	}

	free(buffer);

	graphs.push_back(graph);
	graphDebugPrint("loaded graph %i from '%s' (%u nodes, %u edges, persist %i)", id, path, nodeCount, edgeCount, persist);
	stackPushInt(id);
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
