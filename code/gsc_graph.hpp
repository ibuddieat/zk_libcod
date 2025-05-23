/*
A* Algorithm Implementation using STL is
Copyright (C)2001-2005 Justin Heyes-Jones
https://github.com/justinhj/astar-algorithm-cpp

Permission is given by the author to freely redistribute and 
include this code in any program as long as this credit is 
given where due.
 
  COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS, 
  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, 
  INCLUDING, WITHOUT LIMITATION, WARRANTIES THAT THE COVERED CODE 
  IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
  OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND 
  PERFORMANCE OF THE COVERED CODE IS WITH YOU. SHOULD ANY COVERED 
  CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT THE INITIAL 
  DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY 
  NECESSARY SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF 
  WARRANTY CONSTITUTES AN ESSENTIAL PART OF THIS LICENSE. NO USE 
  OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
  THIS DISCLAIMER.
 
  Use at your own risk!
*/

#ifndef _GSC_GRAPH_HPP_
#define _GSC_GRAPH_HPP_

// gsc functions
#include "gsc.hpp"

void gsc_graph_create_graph(void);
void gsc_graph_remove_graph(void);
void gsc_graph_remove_graphs(void);
void gsc_graph_add_node(void);
void gsc_graph_get_node_properties(void);
void gsc_graph_remove_node(void);
void gsc_graph_add_edge(void);
void gsc_graph_get_edge_properties(void);
void gsc_graph_remove_edge(void);
void gsc_graph_find_path_astar(void);
void gsc_graph_find_closest_node(void);
void gsc_graph_find_closest_edge(void);

// Used for text debugging
#include <iostream>
#include <stdio.h>
#include <assert.h>

// STL includes
#include <algorithm>
#include <unordered_set>
#include <vector>
#include <array>
#include <cfloat>

// Fast fixed size memory allocator, used for fast node memory management
// during search
#include "gsc_graph_fsa.hpp"

// Fixed size memory allocator can be disabled to compare performance
// Uses std new and delete instead if you turn it off
#define USE_FSA_MEMORY 0

// Limits when using fixed size memory allocator
#if USE_FSA_MEMORY
#define MAX_NODES 1024
#define MAX_EDGES 16
#endif

// Forward declarations
struct AStarGraphNode;
struct AStarGraph;
template <class UserState> class AStarSearch;

class AStarGraphEdge
{
public:
	AStarGraphNode *start = NULL;
	AStarGraphNode *end = NULL;
	int type = 0;
	float cost = 0.0;

	AStarGraphEdge()
	{}

	AStarGraphEdge(AStarGraphNode *_start, AStarGraphNode *_end, int _type, float _cost)
	{
		start = _start;
		end = _end;
		type = _type;
		cost = _cost;
	}

	void Update(AStarGraphNode *_start, AStarGraphNode *_end, int _type, float _cost);
};

class AStarGraphNode
{
public:
	unsigned int id = 0;
	vec3_t origin = {0.0, 0.0, 0.0};
	int type = 0;
#if USE_FSA_MEMORY
	std::array<AStarGraphEdge, MAX_EDGES>edges{};
	unsigned int numEdges = 0;
#else
	std::vector<AStarGraphEdge>edges;
#endif

	AStarGraphNode()
	{}

	AStarGraphNode(unsigned int _id, float *_origin, int _type)
	{
		id = _id;
		VectorCopy(_origin, origin);
		type = type;
	}

	float GetCostToNext(AStarGraphNode& nextNode);
	float GetCostToGoalEstimate(AStarGraphNode& goalNode);
	size_t GetHash(void);
	bool GetSuccessors(
		AStarSearch<AStarGraphNode>* astarSearch,
		AStarGraphNode* parentNode,
		std::vector<unsigned int>* skippedNodes,
		unsigned int skippedNodeTypes,
		unsigned int skippedEdgeTypes);
	bool IsSameNode(AStarGraphNode& checkNode);
	bool IsSameNodeByOrigin(AStarGraphNode& checkNode);
};

class AStarGraph // Not using fixed size memory allocator
{
public:
	unsigned int id;
	bool persist;
	unsigned int numNodes;
	unsigned int numEdges;
	std::vector<AStarGraphNode> nodes;

	AStarGraph(unsigned int _id, bool _persist)
	{
		id = _id;
		persist = _persist;
	}

	~AStarGraph()
	{}

	AStarGraphNode* GetNodeById(unsigned int id);
	int GetNodeIndex(AStarGraphNode* searchNode);
};

// The AStar search class. UserState is the users state space type
template <class UserState> class AStarSearch
{

public: // Data

	enum
	{
		SEARCH_STATE_NOT_INITIALISED,
		SEARCH_STATE_SEARCHING,
		SEARCH_STATE_SUCCEEDED,
		SEARCH_STATE_FAILED,
		SEARCH_STATE_OUT_OF_MEMORY,
		SEARCH_STATE_INVALID
	};


	// A node represents a possible state in the search
	// The user provided state type is included inside this type

	public:

	class Node
	{
		public:

			Node *parent; // Used during the search to record the parent of successor nodes
			Node *child; // Used after the search for the application to view the search in reverse
			
			float g; // Cost of this node + its predecessors
			float h; // Heuristic estimate of distance to goal
			float f; // Sum of cumulative cost of predecessors and self and heuristic

			Node() :
				parent( 0 ),
				child( 0 ),
				g( 0.0f ),
				h( 0.0f ),
				f( 0.0f )
			{			
			}

			bool operator==(const Node& otherNode) const
			{
				return this->m_UserState.IsSameNode(otherNode->m_UserState);
			}

			UserState m_UserState;
	};

	// For sorting the heap the STL needs compare function that lets us compare
	// the f value of two nodes
	class HeapCompare_f 
	{
		public:

			bool operator()(const Node *x, const Node *y) const
			{
				return x->f > y->f;
			}
	};


public: // Methods


	// Constructor just initialises private data
	AStarSearch() :
		m_State(SEARCH_STATE_NOT_INITIALISED),
		m_CurrentSolutionNode(NULL),
#if USE_FSA_MEMORY
		m_FixedSizeAllocator(MAX_NODES),
#endif
		m_AllocateNodeCount(0),
		m_CancelRequest(false)
	{
	}

	// Call at any time to cancel the search and free up all the memory
	void CancelSearch(void)
	{
		m_CancelRequest = true;
	}

	// Set nodes to skip in path finding
	void SetSkippedNodes(std::vector<unsigned int> NodeIDs)
	{
		m_SkippedNodes = NodeIDs;
	}

	void SetSkippedNodeTypes(int mask)
	{
		m_SkippedNodeTypes = mask;
	}

	void SetSkippedEdgeTypes(int mask)
	{
		m_SkippedEdgeTypes = mask;
	}

	// Set Start and goal states
	void SetStartAndGoalStates(UserState &Start, UserState &Goal)
	{
		m_CancelRequest = false;

		m_Start = AllocateNode();
		m_Goal = AllocateNode();

		assert((m_Start != NULL && m_Goal != NULL));
		
		m_Start->m_UserState = Start;
		m_Goal->m_UserState = Goal;

		m_State = SEARCH_STATE_SEARCHING;
		
		// Initialise the AStar specific parts of the Start Node
		// The user only needs fill out the state information
		m_Start->g = 0; 
		m_Start->h = m_Start->m_UserState.GetCostToGoalEstimate(m_Goal->m_UserState);
		m_Start->f = m_Start->g + m_Start->h;
		m_Start->parent = 0;

		// Push the start node on the Open list
		m_OpenList.push_back(m_Start); // Heap now unsorted

		// Sort back element into heap
		push_heap(m_OpenList.begin(), m_OpenList.end(), HeapCompare_f());

		// Initialise counter for search steps
		m_Steps = 0;
	}

	// Advances search one step 
	unsigned int SearchStep(void)
	{
		// Firstly break if the user has not initialised the search
		assert ( (m_State > SEARCH_STATE_NOT_INITIALISED) &&
				 (m_State < SEARCH_STATE_INVALID) );

		// Next I want it to be safe to do a searchstep once the search has succeeded...
		if ( (m_State == SEARCH_STATE_SUCCEEDED) ||
			 (m_State == SEARCH_STATE_FAILED) 
		  )
		{
			return m_State; 
		}

		// Failure is defined as emptying the open list as there is nothing left to 
		// search...
		// New: Allow user abort
		if ( m_OpenList.empty() || m_CancelRequest )
		{
			FreeAllNodes();
			m_State = SEARCH_STATE_FAILED;
			return m_State;
		}
		
		// Incremement step count
		m_Steps ++;

		// Pop the best node (the one with the lowest f) 
		Node *n = m_OpenList.front(); // get pointer to the node
		pop_heap(m_OpenList.begin(), m_OpenList.end(), HeapCompare_f());
		m_OpenList.pop_back();

		// Check for the goal, once we pop that we're done
		if ( n->m_UserState.IsSameNode(m_Goal->m_UserState) )
		{
			// The user is going to use the Goal Node he passed in 
			// so copy the parent pointer of n 
			m_Goal->parent = n->parent;
			m_Goal->g = n->g;

			// A special case is that the goal was passed in as the start state
			// so handle that here
			if ( false == n->m_UserState.IsSameNode(m_Start->m_UserState) )
			{
				FreeNode(n);

				// Set the child pointers in each node (except Goal which has no child)
				Node *nodeChild = m_Goal;
				Node *nodeParent = m_Goal->parent;

				do 
				{
					nodeParent->child = nodeChild;

					nodeChild = nodeParent;
					nodeParent = nodeParent->parent;
				
				} 
				while ( nodeChild != m_Start ); // Start is always the first node by definition

			}

			// Delete nodes that aren't needed for the solution
			FreeUnusedNodes();

			m_State = SEARCH_STATE_SUCCEEDED;

			return m_State;
		}
		else // Not goal
		{

			// We now need to generate the successors of this node
			// The user helps us to do this, and we keep the new nodes in
			// m_Successors ...
			m_Successors.clear(); // empty vector of successor nodes to n

			// User provides this functions and uses AddSuccessor to add each successor of
			// node 'n' to m_Successors
			bool ret = n->m_UserState.GetSuccessors(this, n->parent ? &n->parent->m_UserState : NULL, &m_SkippedNodes, m_SkippedNodeTypes, m_SkippedEdgeTypes); 

			if ( !ret )
			{

			    typename std::vector<Node *>::iterator successor;

				// Free the nodes that may previously have been added 
				for ( successor = m_Successors.begin(); successor != m_Successors.end(); successor++ )
				{
					FreeNode((*successor));
				}

				m_Successors.clear(); // Empty vector of successor nodes to n

				// Free up everything else we allocated
				FreeNode((n));
				FreeAllNodes();

				m_State = SEARCH_STATE_OUT_OF_MEMORY;
				return m_State;
			}
			
			// Now handle each successor to the current node ...
			for ( typename std::vector<Node *>::iterator successor = m_Successors.begin(); successor != m_Successors.end(); successor++ )
			{
				// 	The g value for this successor ...
				float newg = n->g + n->m_UserState.GetCostToNext((*successor)->m_UserState);

				// Now we need to find whether the node is on the open or closed lists
				// If it is but the node that is already on them is better (lower g)
				// then we can forget about this successor

				// First linear search of open list to find node
				typename std::vector<Node *>::iterator openlist_result;

				for ( openlist_result = m_OpenList.begin(); openlist_result != m_OpenList.end(); openlist_result++ )
				{
					if ( (*openlist_result)->m_UserState.IsSameNode((*successor)->m_UserState) )
					{
						break;					
					}
				}

				if ( openlist_result != m_OpenList.end() )
				{

					// We found this state on open
					if ( (*openlist_result)->g <= newg )
					{
						FreeNode((*successor));

						// The one on Open is cheaper than this one
						continue;
					}
				}
				typename std::unordered_set<Node*, NodeHash, NodeEqual>::iterator closedlist_result;

				closedlist_result = m_ClosedList.find(*successor);

				if ( closedlist_result != m_ClosedList.end() )
				{

					// We found this state on closed
					if ( (*closedlist_result)->g <= newg )
					{
						// The one on Closed is cheaper than this one
						FreeNode((*successor));

						continue;
					}
				}

				// This node is the best node so far with this particular state
				// so lets keep it and set up its AStar specific data ...
				(*successor)->parent = n;
				(*successor)->g = newg;
				(*successor)->h = (*successor)->m_UserState.GetCostToGoalEstimate(m_Goal->m_UserState);
				(*successor)->f = (*successor)->g + (*successor)->h;

				// Successor in closed list
				// 1 - Update old version of this node in closed list
				// 2 - Move it from closed to open list
				// 3 - Sort heap again in open list
				if ( closedlist_result != m_ClosedList.end() )
				{
					// Update closed node with successor node AStar data
					//*(*closedlist_result) = *(*successor);
					(*closedlist_result)->parent = (*successor)->parent;
					(*closedlist_result)->g      = (*successor)->g;
					(*closedlist_result)->h      = (*successor)->h;
					(*closedlist_result)->f      = (*successor)->f;

					// Free successor node
					FreeNode((*successor));

					// Push closed node into open list 
					m_OpenList.push_back((*closedlist_result));

					// Remove closed node from closed list
					m_ClosedList.erase(closedlist_result);

					// Sort back element into heap
					push_heap(m_OpenList.begin(), m_OpenList.end(), HeapCompare_f());

					// Fix thanks to ...
					// Greg Douglas <gregdouglasmail@gmail.com>
					// who noticed that this code path was incorrect
					// Here we have found a new state which is already CLOSED
				}

				// Successor in open list
				// 1 - Update old version of this node in open list
				// 2 - sort heap again in open list
				else if( openlist_result != m_OpenList.end() )
				{
					// Update open node with successor node AStar data
					//*(*openlist_result) = *(*successor);
					(*openlist_result)->parent = (*successor)->parent;
					(*openlist_result)->g      = (*successor)->g;
					(*openlist_result)->h      = (*successor)->h;
					(*openlist_result)->f      = (*successor)->f;

					// Free successor node
					FreeNode((*successor));

					// re-make the heap 
					// make_heap rather than sort_heap is an essential bug fix
					// thanks to Mike Ryynanen for pointing this out and then explaining
					// it in detail. sort_heap called on an invalid heap does not work
					make_heap(m_OpenList.begin(), m_OpenList.end(), HeapCompare_f());
				}

				// New successor
				// 1 - Move it from successors to open list
				// 2 - sort heap again in open list
				else
				{
					// Push successor node into open list
					m_OpenList.push_back((*successor));

					// Sort back element into heap
					push_heap(m_OpenList.begin(), m_OpenList.end(), HeapCompare_f());
				}

			}

			// Push n onto Closed, as we have expanded it now
			m_ClosedList.insert(n);

		} // End else (not goal so expand)

 		return m_State; // Succeeded bool is false at this point

	}

	// User calls this to add a successor to a list of successors
	// when expanding the search frontier
	bool AddSuccessor(UserState &State)
	{
		Node *node = AllocateNode();

		if ( node )
		{
			node->m_UserState = State;

			m_Successors.push_back(node);

			return true;
		}

		return false;
	}

	// Free the solution nodes
	// This is done to clean up all used Node memory when you are done with the
	// search
	void FreeSolutionNodes(void)
	{
		Node *n = m_Start;

		if ( m_Start->child )
		{
			do
			{
				Node *del = n;
				n = n->child;
				FreeNode(del);

				del = NULL;

			} while ( n != m_Goal );

			FreeNode( n ); // Delete the goal

		}
		else
		{
			// If the start node is the solution we need to just delete the start and goal
			// nodes
			FreeNode(m_Start);
			FreeNode(m_Goal);
		}

	}

	// Functions for traversing the solution

	// Get start node
	UserState *GetSolutionStart(void)
	{
		m_CurrentSolutionNode = m_Start;
		if ( m_Start )
		{
			return &m_Start->m_UserState;
		}
		else
		{
			return NULL;
		}
	}
	
	// Get next node
	UserState *GetSolutionNext(void)
	{
		if ( m_CurrentSolutionNode )
		{
			if ( m_CurrentSolutionNode->child )
			{
				Node *child = m_CurrentSolutionNode->child;
				m_CurrentSolutionNode = m_CurrentSolutionNode->child;
				return &child->m_UserState;
			}
		}

		return NULL;
	}
	
	// Get end node
	UserState *GetSolutionEnd(void)
	{
		m_CurrentSolutionNode = m_Goal;
		if ( m_Goal )
		{
			return &m_Goal->m_UserState;
		}
		else
		{
			return NULL;
		}
	}
	
	// Step solution iterator backwards
	UserState *GetSolutionPrev(void)
	{
		if ( m_CurrentSolutionNode )
		{
			if ( m_CurrentSolutionNode->parent )
			{
				Node *parent = m_CurrentSolutionNode->parent;
				m_CurrentSolutionNode = m_CurrentSolutionNode->parent;
				return &parent->m_UserState;
			}
		}

		return NULL;
	}

	// Get final cost of solution
	// Returns FLT_MAX if goal is not defined or there is no solution
	float GetSolutionCost(void)
	{
		if ( m_Goal && m_State == SEARCH_STATE_SUCCEEDED )
		{
			return m_Goal->g;
		}
		else
		{
			return FLT_MAX;
		}
	}

	// For educational use and debugging it is useful to be able to view
	// the open and closed list at each step, here are two functions to allow that.
	UserState *GetOpenListStart(void)
	{
		float f, g, h;
		return GetOpenListStart(f, g, h);
	}

	UserState *GetOpenListStart(float &f, float &g, float &h)
	{
		iterDbgOpen = m_OpenList.begin();
		if ( iterDbgOpen != m_OpenList.end() )
		{
			f = (*iterDbgOpen)->f;
			g = (*iterDbgOpen)->g;
			h = (*iterDbgOpen)->h;
			return &(*iterDbgOpen)->m_UserState;
		}

		return NULL;
	}

	UserState *GetOpenListNext(void)
	{
		float f, g, h;
		return GetOpenListNext(f, g, h);
	}

	UserState *GetOpenListNext(float &f, float &g, float &h)
	{
		iterDbgOpen++;
		if ( iterDbgOpen != m_OpenList.end() )
		{
			f = (*iterDbgOpen)->f;
			g = (*iterDbgOpen)->g;
			h = (*iterDbgOpen)->h;
			return &(*iterDbgOpen)->m_UserState;
		}

		return NULL;
	}

	UserState *GetClosedListStart(void)
	{
		float f, g, h;
		return GetClosedListStart(f, g, h);
	}

	UserState *GetClosedListStart(float &f, float &g, float &h)
	{
		iterDbgClosed = m_ClosedList.begin();
		if ( iterDbgClosed != m_ClosedList.end() )
		{
			f = (*iterDbgClosed)->f;
			g = (*iterDbgClosed)->g;
			h = (*iterDbgClosed)->h;

			return &(*iterDbgClosed)->m_UserState;
		}

		return NULL;
	}

	UserState *GetClosedListNext(void)
	{
		float f, g, h;
		return GetClosedListNext(f, g, h);
	}

	UserState *GetClosedListNext(float &f, float &g, float &h)
	{
		iterDbgClosed++;
		if ( iterDbgClosed != m_ClosedList.end() )
		{
			f = (*iterDbgClosed)->f;
			g = (*iterDbgClosed)->g;
			h = (*iterDbgClosed)->h;

			return &(*iterDbgClosed)->m_UserState;
		}

		return NULL;
	}

	// Get the number of steps
	int GetStepCount(void) { return m_Steps; }

	void EnsureMemoryFreed(void)
	{
#if USE_FSA_MEMORY
		assert(m_AllocateNodeCount == 0);
#endif

	}

private: // Methods

	// This is called when a search fails or is cancelled to free all used
	// memory 
	void FreeAllNodes(void)
	{
		// Iterate open list and delete all nodes
		typename std::vector<Node *>::iterator iterOpen = m_OpenList.begin();

		while ( iterOpen != m_OpenList.end() )
		{
			Node *n = (*iterOpen);
			FreeNode(n);

			iterOpen++;
		}

		m_OpenList.clear();

		// Iterate closed list and delete unused nodes
		typename std::unordered_set<Node*, NodeHash, NodeEqual>::iterator iterClosed;

		for ( iterClosed = m_ClosedList.begin(); iterClosed != m_ClosedList.end(); iterClosed++ )
		{
			Node *n = (*iterClosed);
			FreeNode(n);
		}

		m_ClosedList.clear();

		// Delete the goal

		FreeNode(m_Goal);
	}


	// This call is made by the search class when the search ends. A lot of nodes may be
	// created that are still present when the search ends. They will be deleted by this 
	// routine once the search ends
	void FreeUnusedNodes(void)
	{
		// Iterate open list and delete unused nodes
		typename std::vector<Node *>::iterator iterOpen = m_OpenList.begin();

		while ( iterOpen != m_OpenList.end() )
		{
			Node *n = (*iterOpen);

			if ( !n->child )
			{
				FreeNode(n);

				n = NULL;
			}

			iterOpen++;
		}

		m_OpenList.clear();

		// Iterate closed list and delete unused nodes
		typename std::unordered_set<Node*, NodeHash, NodeEqual>::iterator iterClosed;

		for ( iterClosed = m_ClosedList.begin(); iterClosed != m_ClosedList.end(); iterClosed++ )
		{
			Node *n = (*iterClosed);

			if ( !n->child )
			{
				FreeNode(n);
				n = NULL;
			}
		}

		m_ClosedList.clear();
	}

	// Node memory management
	Node *AllocateNode(void)
	{

#if !USE_FSA_MEMORY
		m_AllocateNodeCount++;
		Node *p = new Node;
		return p;
#else
		Node *address = m_FixedSizeAllocator.alloc();

		if ( !address )
		{
			return NULL;
		}
		m_AllocateNodeCount ++;
		Node *p = new (address) Node;
		return p;
#endif
	}

	void FreeNode(Node *node)
	{

		m_AllocateNodeCount --;

#if !USE_FSA_MEMORY
		delete node;
#else
		node->~Node();
		m_FixedSizeAllocator.free(node);
#endif
	}

private: // Data

	// Heap (simple vector but used as a heap, cf. Steve Rabin's game gems article)
	std::vector<Node *> m_OpenList;

	// Closed is an unordered_set
	struct NodeHash {
		size_t operator() (Node* const& n) const {
			return n->m_UserState.GetHash();
		}
	};
	struct NodeEqual {
		bool operator()(Node* a, Node* b) const {
			return a->m_UserState.IsSameNode(b->m_UserState);
  	}
	};
	std::unordered_set<Node*, NodeHash, NodeEqual> m_ClosedList;


	// Successors is a vector filled out by the user each type successors to a node
	// are generated
	std::vector<Node *> m_Successors;

	// State
	unsigned int m_State;

	// Counts steps
	int m_Steps;

	// Start and goal state pointers
	Node *m_Start;
	Node *m_Goal;

	Node *m_CurrentSolutionNode;

#if USE_FSA_MEMORY
 	FixedSizeAllocator<Node> m_FixedSizeAllocator;
#endif
	
	// Debug : need to keep these two iterators around
	// for the user Dbg functions
	typename std::vector<Node *>::iterator iterDbgOpen;
	typename std::vector<Node *>::iterator iterDbgClosed;

	// Debugging : count memory allocation and free's
	int m_AllocateNodeCount;

	// Flag to cancel search
	bool m_CancelRequest;

	// Optional definitions to skip nodes/edges in successor search
	std::vector<unsigned int> m_SkippedNodes;
	unsigned int m_SkippedNodeTypes;
	unsigned int m_SkippedEdgeTypes;
};

#endif // Defined _GSC_GRAPH_HPP_