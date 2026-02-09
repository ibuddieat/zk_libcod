#include <iostream>
#include <stdio.h>
#include <math.h>
#include <limits>

using namespace std;

#include "gsc_graph.hpp" // See header for copyright and usage information
#include "utils.hpp"

extern vector<AStarGraph> AStarGraphs;

//
// GraphEdge class methods
//

// Value updater for copy operations, like custom copy operator
void AStarGraphEdge::Update(AStarGraphNode *_start, AStarGraphNode *_end, int _type, float _cost)
{
	start = _start;
	end = _end;
	type = _type;
	cost = _cost;
}

//
// GraphNode class methods
//

// Given this node, what does it cost to move to the given connected node
float AStarGraphNode::GetCostToNext(AStarGraphNode& nextNode)
{
#if USE_FSA_MEMORY
	for ( unsigned int i = 0; i < numEdges; i++ )
	{
		if ( edges[i].end->id == nextNode.id )
			return edges[i].cost;
	}
#else
	for ( auto edge = begin(edges); edge != end(edges); ++edge )
	{
		if ( edge->end->id == nextNode.id )
			return edge->cost;
	}
#endif
	return numeric_limits<float>::infinity(); // We should not end up here
}

// Here's the heuristic function that estimates the distance from a Node
// to the Goal
float AStarGraphNode::GetCostToGoalEstimate(AStarGraphNode& goalNode)
{
	// Euclidean distance
	return Get3DDistance(origin, goalNode.origin);
}

size_t AStarGraphNode::GetHash(void)
{
	return HashCombine(HashCombine(hash<float>{}(origin[0]), origin[1]), origin[2]);
}

// This generates the successors to the given Node. It uses a helper function
// called AddSuccessor to give the successors to the AStar class. The A*
// specific initialisation is done for each node internally, so here we just
// set the state information that is specific to this application
bool AStarGraphNode::GetSuccessors(
	AStarSearch<AStarGraphNode>* astarSearch,
	AStarGraphNode* parentNode,
	vector<unsigned int>* skippedNodes,
	unsigned int skippedNodeTypes,
	unsigned int skippedEdgeTypes)
{
#if USE_FSA_MEMORY
	unsigned int i = 0;
	for ( auto edge = begin(edges); i < numEdges; ++edge, ++i )
#else
	for ( auto edge = begin(edges); edge != end(edges); ++edge )
#endif
	{
		if ( edge->type & skippedEdgeTypes )
			continue;

		if ( edge->end->type & skippedNodeTypes )
			continue;

		bool skip = false;
		for ( auto skippedNode = begin(*skippedNodes); skippedNode != end(*skippedNodes); ++skippedNode )
		{
			if ( *skippedNode == edge->end->id )
			{
				skip = true;
				break;
			}
		}
		if ( skip )
			continue;

		astarSearch->AddSuccessor(*edge->end);
	}

	return true;
}

bool AStarGraphNode::IsSameNode(AStarGraphNode &checkNode)
{
	if ( id == checkNode.id )
		return true;

	return false;
}

bool AStarGraphNode::IsSameNodeByOrigin(AStarGraphNode &checkNode)
{
	if ( FloatsApproximatelyEqual(origin[0], checkNode.origin[0]) &&
	     FloatsApproximatelyEqual(origin[1], checkNode.origin[1]) &&
		 FloatsApproximatelyEqual(origin[2], checkNode.origin[2]) )
	{
		return true;
	}

	return false;
}

//
// AStarGraph class methods
//

AStarGraphNode* AStarGraph::GetNodeById(unsigned int id)
{
	auto it = find_if(nodes.begin(), nodes.end(), [id](const AStarGraphNode& node) -> bool { return node.id == id; });
	if ( it != nodes.end() )
		return &(*it);

	return NULL;
}

int AStarGraph::GetNodeIndex(AStarGraphNode* searchNode)
{
	auto it = find_if(nodes.begin(), nodes.end(), [searchNode](const AStarGraphNode& node) -> bool { return &node == searchNode; });
	if ( it != nodes.end() )
	{
		auto index = distance(nodes.begin(), it);
		return index;
	}
	return -1;
}

//
// Helper functions
//

AStarGraph* GetGraphById(unsigned int id)
{
	for ( auto graph = begin(AStarGraphs); graph != end(AStarGraphs); ++graph )
	{
		if ( graph->id == id )
		{
			return &(*graph);
		}
	}
	return NULL;
}

//
// GSC interface
//

extern customStringIndex_t custom_scr_const;

void gsc_graph_create_graph(void)
{
	unsigned int id = 0;
	bool persist = 0;

	// Get next minimal free graph ID
	while ( true )
	{
		bool availableID = true;
		for ( auto graph = begin(AStarGraphs); graph != end(AStarGraphs); ++graph )
		{
			if ( graph->id == id )
			{
				availableID = false;
				break;
			}
		}
		if ( availableID )
			break;

		id++;
	}

	if ( Scr_GetNumParam() > 0 )
		persist = Scr_GetInt(0);

	AStarGraphs.emplace_back(id, persist);

	stackPushInt(id);
}

void gsc_graph_remove_graph(void)
{
	unsigned int graphId = Scr_GetInt(0);
	AStarGraph* graphPointer = GetGraphById(graphId);
	if ( !graphPointer )
	{
		stackError("gsc_graph_remove_graph() graph %d does not exist", graphId);
		stackPushUndefined();
		return;
	}

	for ( auto graph = begin(AStarGraphs); graph != end(AStarGraphs); )
	{
		if ( &(*graph) == graphPointer )
		{
			AStarGraphs.erase(graph);

			stackPushBool(qtrue);
			return;
		}
	}
}

void gsc_graph_remove_graphs(void)
{
	for ( auto graph = begin(AStarGraphs); graph != end(AStarGraphs); )
		graph = AStarGraphs.erase(graph);

	stackPushBool(qtrue);
}

void gsc_graph_add_node(void)
{
	unsigned int graphId = Scr_GetInt(0);
	AStarGraph* graphPointer = GetGraphById(graphId);
	if ( !graphPointer )
	{
		stackError("gsc_graph_add_node() graph %d does not exist", graphId);
		stackPushUndefined();
		return;
	}
	AStarGraph& graph = *graphPointer;

#if USE_FSA_MEMORY
	if ( graph.nodes.size() >= MAX_NODES )
	{
		stackError("gsc_graph_add_node() graph %d reached max. number of nodes", graphId);
		stackPushUndefined();
		return;
	}
#endif

	vec3_t origin;
	Scr_GetVector(1, origin);
	int type = 0; 
	if ( Scr_GetNumParam() > 2 )
		type = Scr_GetInt(2);

	graph.nodes.emplace_back(graph.nodes.size(), origin, type);

	stackPushBool(qtrue);
}

void gsc_graph_get_node_properties(void)
{
	unsigned int graphId = Scr_GetInt(0);
	AStarGraph* graphPointer = GetGraphById(graphId);
	if ( !graphPointer )
	{
		stackError("gsc_graph_get_node_properties() graph %d does not exist", graphId);
		stackPushUndefined();
		return;
	}
	AStarGraph& graph = *graphPointer;

	unsigned int nodeId = Scr_GetInt(1);
	AStarGraphNode* node = graph.GetNodeById(nodeId);
	if ( !node )
	{
		stackError("gsc_graph_get_node_properties() node %d not found in graph %d", nodeId, graphId);
		stackPushUndefined();
		return;
	}

	vec3_t origin;

	VectorCopy(node->origin, origin);

	Scr_MakeArray();
	Scr_AddVector(origin);
	Scr_AddArrayStringIndexed(custom_scr_const.origin);
	Scr_AddInt(node->type);
	Scr_AddArrayStringIndexed(custom_scr_const.type);
}

void gsc_graph_remove_node(void)
{
	unsigned int graphId = Scr_GetInt(0);
	AStarGraph* graphPointer = GetGraphById(graphId);
	if ( !graphPointer )
	{
		stackError("gsc_graph_remove_node() graph %d does not exist", graphId);
		stackPushUndefined();
		return;
	}
	AStarGraph& graph = *graphPointer;

	int nodeId = Scr_GetInt(1);
	AStarGraphNode* searchNode = graph.GetNodeById(nodeId);
	if ( searchNode )
	{
		int index = graph.GetNodeIndex(searchNode);
		if ( index != -1 )
		{
			// Remove edges to node
			for ( auto node = begin(graph.nodes); node != end(graph.nodes); ++node )
			{
#if USE_FSA_MEMORY
				unsigned int currentNumEdges = node->numEdges;

				for ( unsigned int i = 0; i < currentNumEdges; i++ )
				{
					if ( node->edges[i].end == searchNode )
					{
						if ( i < ( MAX_EDGES - 1 ) )
						{
							for ( unsigned int j = i; j < MAX_EDGES - 1; j++ )
							{
								node->edges[j].start = node->edges[j + 1].start;
								node->edges[j].end = node->edges[j + 1].end;
								node->edges[j].type = node->edges[j + 1].type;
								node->edges[j].cost = node->edges[j + 1].cost;
							}
						}
						node->numEdges--;
						break; // Assuming a node cannot have multiple edges to another node
					}
				}
#else
				for ( auto edge = begin(node->edges); edge != end(node->edges); )
				{
					if ( edge->end == searchNode )
					{
						edge = node->edges.erase(edge);
						break; // Assuming a node cannot have multiple edges to another node
					}
					else
					{
						++edge;
					}
				}
#endif
			}

			// Remove node itself
			graph.nodes.erase(graph.nodes.begin() + index);

			stackPushBool(qtrue);
		}
	}

	stackPushBool(qfalse);
}

void gsc_graph_add_edge(void)
{
	unsigned int graphId = Scr_GetInt(0);
	AStarGraph* graphPointer = GetGraphById(graphId);
	if ( !graphPointer )
	{
		stackError("gsc_graph_add_edge() graph %d does not exist", graphId);
		stackPushUndefined();
		return;
	}
	AStarGraph& graph = *graphPointer;

	unsigned int fromNodeId = Scr_GetInt(1);
	AStarGraphNode* fromNode = graph.GetNodeById(fromNodeId);
	if ( !fromNode )
	{
		stackError("gsc_graph_add_edge() start node %d not found in graph %d", fromNodeId, graphId);
		stackPushUndefined();
		return;
	}

#if USE_FSA_MEMORY
	if ( fromNode->numEdges >= MAX_EDGES )
	{
		stackError("gsc_graph_add_edge() node %d reached max. number of edges in graph %d", fromNodeId, graphId);
		stackPushUndefined();
		return;
	}
#endif

	unsigned int toNodeId = Scr_GetInt(2);
	AStarGraphNode* toNode = graph.GetNodeById(toNodeId);
	if ( !toNode )
	{
		stackError("gsc_graph_add_edge() end node %d not found in graph %d", toNodeId, graphId);
		stackPushUndefined();
		return;
	}

	if ( fromNodeId == toNodeId )
	{
		stackError("gsc_graph_add_edge() edge cannot have same start and end");
		stackPushUndefined();
		return;
	}

#if USE_FSA_MEMORY
	for ( unsigned int i = 0; i < fromNode->numEdges; i++ )
	{
		if ( fromNode->edges[i].end == toNode )
#else
	for ( auto edge = begin(fromNode->edges); edge != end(fromNode->edges); ++edge )
	{
		if ( edge->end == toNode )
#endif
		{
			stackError("gsc_graph_add_edge() there already is an edge from node %d to %d in graph %d", fromNodeId, toNodeId, graphId);
			stackPushUndefined();
			return;
		}
	}

	int type = 0;
	if ( Scr_GetNumParam() > 3 )
		type = Scr_GetInt(3);

	float cost;
	if ( Scr_GetNumParam() > 4 )
		cost = Scr_GetFloat(4);
	else
		cost = Get3DDistance(fromNode->origin, toNode->origin);	// Euclidean distance

#if USE_FSA_MEMORY
	fromNode->edges[fromNode->numEdges].Update(fromNode, toNode, type, cost);
	fromNode->numEdges++;
#else
	fromNode->edges.emplace_back(fromNode, toNode, type, cost);
#endif

	stackPushBool(qtrue);
}

void gsc_graph_get_edge_properties(void)
{
	unsigned int graphId = Scr_GetInt(0);
	AStarGraph* graphPointer = GetGraphById(graphId);
	if ( !graphPointer )
	{
		stackError("gsc_graph_get_edge_properties() graph %d does not exist", graphId);
		stackPushUndefined();
		return;
	}
	AStarGraph& graph = *graphPointer;

	unsigned int fromNodeId = Scr_GetInt(1);
	AStarGraphNode* fromNode = graph.GetNodeById(fromNodeId);
	if ( !fromNode )
	{
		stackError("gsc_graph_get_edge_properties() start node %d not found in graph", fromNodeId);
		stackPushUndefined();
		return;
	}

	unsigned int toNodeId = Scr_GetInt(2);
	AStarGraphNode* toNode = graph.GetNodeById(toNodeId);
	if ( !toNode )
	{
		stackError("gsc_graph_get_edge_properties() end node %d not found in graph", toNodeId);
		stackPushUndefined();
		return;
	}

#if USE_FSA_MEMORY
	unsigned int i = 0;
	for ( auto edge = begin(fromNode->edges); i < fromNode->numEdges; ++edge, ++i )
#else
	for ( auto edge = begin(fromNode->edges); edge != end(fromNode->edges); ++edge )
#endif
	{
		if ( edge->end == toNode )
		{
			Scr_MakeArray();
			Scr_AddInt(fromNodeId);
			Scr_AddArrayStringIndexed(custom_scr_const.start);
			Scr_AddInt(toNodeId);
			Scr_AddArrayStringIndexed(custom_scr_const.end);
			Scr_AddInt(edge->type);
			Scr_AddArrayStringIndexed(custom_scr_const.type);
			Scr_AddFloat(edge->cost);
			Scr_AddArrayStringIndexed(custom_scr_const.cost);
			return;
		}
	}

	stackError("gsc_graph_get_edge_properties() no edge with start node %d and end node %d found in graph %d", toNodeId, fromNodeId, graphId);
	stackPushUndefined();
}

void gsc_graph_remove_edge(void)
{
	unsigned int graphId = Scr_GetInt(0);
	AStarGraph* graphPointer = GetGraphById(graphId);
	if ( !graphPointer )
	{
		stackError("gsc_graph_remove_edge() graph %d does not exist", graphId);
		stackPushUndefined();
		return;
	}
	AStarGraph& graph = *graphPointer;

	unsigned int fromNodeId = Scr_GetInt(1);
	AStarGraphNode* fromNode = graph.GetNodeById(fromNodeId);
	if ( !fromNode )
	{
		stackError("gsc_graph_remove_edge() start node %d not found in graph %d", fromNodeId, graphId);
		stackPushUndefined();
		return;
	}

#if USE_FSA_MEMORY
	if ( fromNode->numEdges < 1 )
	{
		stackError("gsc_graph_remove_edge() start node %d has no edges in graph %d", fromNodeId, graphId);
		stackPushUndefined();
		return;
	}
#endif

	unsigned int toNodeId = Scr_GetInt(2);
	AStarGraphNode* toNode = graph.GetNodeById(toNodeId);
	if ( !toNode )
	{
		stackError("gsc_graph_remove_edge() end node %d not found in graph %d", toNodeId, graphId);
		stackPushUndefined();
		return;
	}

#if USE_FSA_MEMORY
	unsigned int currentNumEdges = fromNode->numEdges;

	for ( unsigned int i = 0; i < currentNumEdges; i++ )
	{
		if ( fromNode->edges[i].end == toNode )
		{
			if ( i < ( MAX_EDGES - 1 ) )
			{
				for ( unsigned int j = i; j < MAX_EDGES - 1; j++ )
				{
					fromNode->edges[j].start = fromNode->edges[j + 1].start;
					fromNode->edges[j].end = fromNode->edges[j + 1].end;
					fromNode->edges[j].type = fromNode->edges[j + 1].type;
					fromNode->edges[j].cost = fromNode->edges[j + 1].cost;
				}
			}
			fromNode->numEdges--;

			stackPushBool(qtrue);
			return;
		}
	}
#else
	for ( auto edge = begin(fromNode->edges); edge != end(fromNode->edges); )
	{
		if ( edge->end == toNode )
		{
			fromNode->edges.erase(edge);

			stackPushBool(qtrue);
			return;
		}
		else
		{
			++edge;
		}
	}
#endif

	stackPushBool(qfalse);
}

void gsc_graph_find_path_astar(void)
{
	unsigned int graphId = Scr_GetInt(0);
	AStarGraph* graphPointer = GetGraphById(graphId);
	if ( !graphPointer )
	{
		stackError("gsc_graph_find_path_astar() graph %d does not exist", graphId);
		stackPushUndefined();
		return;
	}
	AStarGraph& graph = *graphPointer;

	if ( graph.nodes.size() < 2 )
	{
		stackError("gsc_graph_find_path_astar() graph %d has less than two nodes", graphId);
		stackPushUndefined();
		return;
	}

	unsigned int start = Scr_GetInt(1);
	unsigned int end = Scr_GetInt(2);
	AStarSearch<AStarGraphNode> astarSearch;
	AStarGraphNode* nodeStart = graph.GetNodeById(start);
	if ( !nodeStart )
	{
		stackError("gsc_graph_find_path_astar() start node %d not found in graph %d", start, graphId);
		stackPushUndefined();
		return;
	}
	AStarGraphNode* nodeEnd = graph.GetNodeById(end);
	if ( !nodeEnd )
	{
		stackError("gsc_graph_find_path_astar() end node %d not found in graph %d", end, graphId);
		stackPushUndefined();
		return;
	}

	vector<unsigned int>skipNodes;
	if ( Scr_GetNumParam() > 3 )
	{
		if ( Scr_GetType(3) == STACK_OBJECT && Scr_GetPointerType(3) == STACK_ARRAY )
		{
			unsigned int arrIndex;
			VariableValue *var;

			var = &scrVmPub.top[-3];
			arrIndex = var->u.pointerValue;

			unsigned int arraysize = GetArraySize(arrIndex);
			if ( arraysize )
			{
				unsigned int index = arrIndex;
				VariableValueInternal_u *value;
				unsigned int skip;
	
				for ( unsigned int i = 0; i < arraysize; i++ )
				{
					index = FindNextSibling(index);
					value = GetVariableValueAddress(index);
	
					if ( GetVarType(index) != STACK_INT )
					{
						stackError("gsc_graph_find_path_astar() element %d of skip node array is type %s, not integer", i, getParamTypeAsString(index));
						stackPushUndefined();
						return;
					}
	
					skip = value->u.intValue;
					if ( skip == start || skip == end )
					{
						stackError("gsc_graph_find_path_astar() cannot skip start node %d or end node %d", start, end);
						stackPushUndefined();
						return;
					}
					skipNodes.push_back(skip);
				}
				astarSearch.SetSkippedNodes(skipNodes);
			}
		}
		else
		{
			stackError("gsc_graph_find_path_astar() skip nodes must be an array of node IDs");
			stackPushUndefined();
			return;
		}
	}

	unsigned int skipNodeTypes = 0;
	if ( Scr_GetNumParam() > 4 )
	{
		skipNodeTypes = Scr_GetInt(4);
	}
	astarSearch.SetSkippedNodeTypes(skipNodeTypes);

	unsigned int skipEdgeTypes = 0;
	if ( Scr_GetNumParam() > 5 )
	{
		skipEdgeTypes = Scr_GetInt(5);
	}
	astarSearch.SetSkippedEdgeTypes(skipEdgeTypes);

	unsigned int SearchState;
	qboolean success;

	astarSearch.SetStartAndGoalStates(*nodeStart, *nodeEnd);
	do
	{
		SearchState = astarSearch.SearchStep();
	}
	while ( SearchState == AStarSearch<AStarGraphNode>::SEARCH_STATE_SEARCHING );

	if ( SearchState == AStarSearch<AStarGraphNode>::SEARCH_STATE_SUCCEEDED )
	{
		success = qtrue;

		stackPushArray();
		AStarGraphNode* node = astarSearch.GetSolutionStart();

		for ( ;; )
		{
			node = astarSearch.GetSolutionNext();

			if ( !node )
			{
				break;
			}
			else
			{
				stackPushInt(node->id);
				stackPushArrayLast();
			}
		};

		astarSearch.FreeSolutionNodes();
	}
	else if ( SearchState == AStarSearch<AStarGraphNode>::SEARCH_STATE_FAILED ) 
	{
		success = qfalse;
	}

	astarSearch.EnsureMemoryFreed();

	if ( !success )
		stackPushUndefined();
}

void gsc_graph_find_closest_node(void)
{
	unsigned int graphId = Scr_GetInt(0);
	AStarGraph* graphPointer = GetGraphById(graphId);
	if ( !graphPointer )
	{
		stackError("gsc_graph_find_closest_node() graph %d does not exist", graphId);
		stackPushUndefined();
		return;
	}
	AStarGraph& graph = *graphPointer;

	if ( graph.nodes.size() < 1 )
	{
		stackError("gsc_graph_find_closest_node() graph %d has no nodes", graphId);
		stackPushUndefined();
		return;
	}

	vec3_t origin;
	float dist;
	float closestDist = numeric_limits<float>::infinity();
	unsigned int nodeId = 0;
	unsigned int closestNodeId;

	Scr_GetVector(1, origin);

	for ( auto node = begin(graph.nodes); node != end(graph.nodes); ++node, ++nodeId )
	{
		dist = Get3DDistanceSquared(node->origin, origin);
		if ( dist < closestDist )
		{
			closestDist = dist;
			closestNodeId = nodeId;
		}
	}

	Scr_AddInt(closestNodeId);
}

void gsc_graph_find_closest_edge(void)
{
	unsigned int graphId = Scr_GetInt(0);
	AStarGraph* graphPointer = GetGraphById(graphId);
	if ( !graphPointer )
	{
		stackError("gsc_graph_find_closest_edge() graph %d does not exist", graphId);
		stackPushUndefined();
		return;
	}
	AStarGraph& graph = *graphPointer;

	if ( graph.nodes.size() < 2 )
	{
		stackError("gsc_graph_find_closest_edge() graph %d has less than two nodes", graphId);
		stackPushUndefined();
		return;
	}

	vec3_t origin;
	AStarGraphNode* currentNode;
	AStarGraphEdge* currentEdge;
	vec3_t o;
	float dist;
	float closestEdgeDist = numeric_limits<float>::infinity();
	vec3_t closestPoint;
	AStarGraphEdge* closestEdge = NULL;

	Scr_GetVector(1, origin);

	// Get closest point on each edge, return closest. We do not search the
	// closest node first, as it may have zero edges
	for ( auto node = begin(graph.nodes); node != end(graph.nodes); ++node )
	{
		currentNode = &(*node);

#if USE_FSA_MEMORY
		unsigned int i = 0;
		for ( auto edge = begin(currentNode->edges); i < currentNode->numEdges; ++edge, ++i )
#else
		for ( auto edge = begin(currentNode->edges); edge != end(currentNode->edges); ++edge )
#endif
		{
			currentEdge = &(*edge);
			ProjectPointOnLine(currentEdge->start->origin, currentEdge->end->origin, origin, o);
			dist = Get3DDistanceSquared(o, origin);
			if ( dist < closestEdgeDist )
			{
				closestEdgeDist = dist;
				VectorCopy(o, closestPoint);
				closestEdge = &(*edge);
			}
		}
	}

	if ( closestEdge == NULL )
	{
		stackError("gsc_graph_find_closest_edge() graph %d has no edges", graphId);
		stackPushUndefined();
		return;
	}

	Scr_MakeArray();
	Scr_AddInt(closestEdge->start->id);
	Scr_AddArrayStringIndexed(custom_scr_const.start);
	Scr_AddInt(closestEdge->end->id);
	Scr_AddArrayStringIndexed(custom_scr_const.end);
	Scr_AddVector(closestPoint);
	Scr_AddArrayStringIndexed(custom_scr_const.origin);
	Scr_AddFloat(closestEdge->cost);
	Scr_AddArrayStringIndexed(custom_scr_const.cost);
	Scr_AddInt(closestEdge->type);
	Scr_AddArrayStringIndexed(custom_scr_const.type);
}