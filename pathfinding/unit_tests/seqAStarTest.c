#include <stdio.h>
#include <stdlib.h>
#include "../pathfinding.h"
#include "../../graph/graph.h"

int djikstra_heuristic(vertex_t *a, vertex_t *b);

int main(int argc, char **argv)
{
    graph_t *g;
    vertex_t *src, *dst;
    path_t *path;

    util_check_m(argc >= 4, "missing parameter.");
    g = graph_create(argv[1], NULL);

    graph_stats(stdout, g, NULL);

    graph_find(g, atoi(argv[2]), &src);
    graph_find(g, atoi(argv[3]), &dst);

    fprintf(stdout, "Source: %d\n", src->id);
    fprintf(stdout, "Destination: %d\n", dst->id);

    seq_a_star_path(g, src, dst, djikstra_heuristic, &path);

    if (path != NULL)
    {
        fprintf(stdout, "Found path with cost = %d and %d nodes\n", path->cost, path->count);

        while (!stack_empty_m(path->nodes))
        {
            int *id;

            stack_pop(path->nodes, (void **)&id);
            fprintf(stdout, "%d\n", *id);
        }
    }
    else
    {
        fprintf(stdout, "Path not found\n");
    }
}

int djikstra_heuristic(vertex_t *a, vertex_t *b)
{
    return 0;
}