#ifndef _PATHFINDING
#define _PATHFINDING

#include "../utils/util.h"
#include "../heap/heap.h"
#include "../queue/queue.h"
#include "../hash_table/hash_table.h"
#include "../graph/graph.h"

// Function prototypes

/*
  Finds path from source to destination in the given graph, using an external heuristic function.
  If heuristic is NULL or returns always 0, A* becomes Djikstra's algorithm.
  If heuristic overestimates the true cost, the path could not be the minimum cost one, otherwise it is guaranteed the path has minimum cost.
  The path is returned as a list of nodes passed as one of the parameters. If NULL, no path was found.
  Returns 0 if an error occurs, 1 otherwise.
*/
extern int seq_a_star_path(graph_t *graph, vertex_t *src, vertex_t *dst, int (*heuristic)(vertex_t *, vertex_t *), queue_t **path);

#endif
