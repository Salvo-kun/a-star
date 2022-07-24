#ifndef _PATHFINDING
#define _PATHFINDING

#include "../utils/util.h"
#include "../heap/heap.h"
#include "../queue/queue.h"
#include "../stack/stack.h"
#include "../hash_table/hash_table.h"
#include "../graph/graph.h"

// Data structures declaration

typedef struct path_s
{
  stack_t *nodes;
  int cost;
  int visited_nodes;
  int revisited_nodes;
} path_t;

// Function prototypes

/*
  Finds path from source to destination in the given graph.
  The path is returned through an input parameter, and it contains:
    - the number of nodes in the path
    - the cost of the path
    - a stack containing the id of the nodes, from source to destination
  If NULL, no path was found.
  Returns 0 if an error occurs, 1 otherwise.
*/
extern int seq_djikstra_path(graph_t *graph, vertex_t *src, vertex_t *dst, path_t **path);

/*
  Finds path from source to destination in the given graph, using an external heuristic function.
  If heuristic overestimates the true cost, the path could not be the minimum cost one, otherwise it is guaranteed the path has minimum cost.
  The path is returned through an input parameter, and it contains:
    - the number of nodes in the path
    - the cost of the path
    - a stack containing the id of the nodes, from source to destination
  If NULL, no path was found.
  Returns 0 if an error occurs, 1 otherwise.
*/
extern int seq_a_star_path(graph_t *graph, vertex_t *src, vertex_t *dst, int (*heuristic)(vertex_t *, vertex_t *), path_t **path);

/*
  Finds path from source to destination in the given graph, using an external heuristic function paralelizing it using n_threads_to_use threads.
  If heuristic overestimates the true cost, the path could not be the minimum cost one, otherwise it is guaranteed the path has minimum cost.
  The path is returned through an input parameter, and it contains:
    - the number of nodes in the path
    - the cost of the path
    - a stack containing the id of the nodes, from source to destination
  If NULL, no path was found.
  Returns 0 if an error occurs, 1 otherwise.
*/

extern int par_a_star_path(graph_t *graph, vertex_t *src, vertex_t *dst, int (*heuristic)(vertex_t *, vertex_t *), path_t **path, int n_threads_to_use);

#endif
