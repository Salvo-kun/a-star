#include "pathfinding.h"

// Public methods

int seq_djikstra_path(graph_t *graph, vertex_t *src, vertex_t *dst, path_t **path)
{
    return seq_a_star_path(graph, src, dst, NULL, path);
}