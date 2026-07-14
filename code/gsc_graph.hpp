#ifndef _GSC_GRAPH_HPP_
#define _GSC_GRAPH_HPP_

#include "gsc.hpp"

#if COMPILE_GRAPH == 1

// Waypoint-graph subsystem with A* pathfinding for server-side AI.
// From-scratch implementation; the A* shape (binary heap + lazy deletion)
// follows the approach popularized by Justin Heyes-Jones' astar-algorithm-cpp.
// All state is owned by the main server thread - no locking, by design.

void gsc_graph_create();
void gsc_graph_remove();
void gsc_graph_removeall();
void gsc_graph_add_node();
void gsc_graph_add_edge();
void gsc_graph_get_node_origin();
void gsc_graph_get_node_count();
void gsc_graph_get_edge_count();

void gsc_graph_cleanup_on_spawn_server(void);

#endif

#endif
