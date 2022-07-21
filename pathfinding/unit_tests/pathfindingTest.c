#include <stdio.h>
#include <stdlib.h>
#include "../pathfinding.h"
#include "../../graph/graph.h"

int heuristic(vertex_t *a, vertex_t *b);

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

    fprintf(stdout, "\n\nDJIKSTRA\n\n");

    seq_djikstra_path(g, src, dst, &path);

    if (path != NULL)
    {
        fprintf(stdout, "\nFound path with cost = %d and length = %d. Visited nodes = %d\n", path->cost, stack_count(path->nodes), path->visited_nodes);

        while (!stack_empty_m(path->nodes))
        {
            int *id;

            stack_pop(path->nodes, (void **)&id);
            fprintf(stdout, "%d%s", *id, stack_empty_m(path->nodes) ? "" : " -> ");
        }

        fprintf(stdout, "\n");
    }
    else
    {
        fprintf(stdout, "Path not found\n");
    }

    fprintf(stdout, "\n\A*\n\n");

    seq_a_star_path(g, src, dst, heuristic, &path);

    if (path != NULL)
    {
        fprintf(stdout, "\nFound path with cost = %d and length = %d. Visited nodes = %d\n", path->cost, stack_count(path->nodes), path->visited_nodes);

        while (!stack_empty_m(path->nodes))
        {
            int *id;

            stack_pop(path->nodes, (void **)&id);
            fprintf(stdout, "%d%s", *id, stack_empty_m(path->nodes) ? "" : " -> ");
        }

        fprintf(stdout, "\n");
    }
    else
    {
        fprintf(stdout, "Path not found\n");
    }
}

int heuristic(vertex_t *a, vertex_t *b)
{
    return 0;
}