#ifndef _GSC_GRAPH_HPP_
#define _GSC_GRAPH_HPP_

#include "gsc.hpp"

#if COMPILE_GRAPH == 1

/*
 * Waypoint-graph subsystem with A* pathfinding for server-side AI.
 * The A* shape (binary-heap open list with lazy deletion) follows the approach
 * popularized by Justin Heyes-Jones' astar-algorithm-cpp:
 * https://github.com/justinhj/astar-algorithm-cpp
 *
 * All graph state is owned by the main server thread - no locking, by design.
 */

void gsc_graph_create();
void gsc_graph_find_path();
void gsc_graph_find_closest_node();
void gsc_graph_remove();
void gsc_graph_removeall();
void gsc_graph_add_node();
void gsc_graph_add_edge();
void gsc_graph_get_node_origin();
void gsc_graph_get_node_count();
void gsc_graph_get_edge_count();
void gsc_graph_get_stats();
void gsc_graph_find_closest_edge();
void gsc_graph_get_all_nodes();
void gsc_graph_get_all_edges();
void gsc_graph_get_edge_properties();
void gsc_graph_set_edge_cost();
void gsc_graph_set_edge_type();
void gsc_graph_remove_edge();
void gsc_graph_get_node_properties();
void gsc_graph_set_node_type();
void gsc_graph_set_node_origin();
void gsc_graph_get_node_ids_accessible_from();
void gsc_graph_get_node_ids_accessible_to();
void gsc_graph_remove_node();

void gsc_graph_cleanup_on_spawn_server(void);

#endif

#endif
