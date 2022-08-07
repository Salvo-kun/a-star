#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../pathfinding.h"
#include "../../graph/graph.h"

typedef struct coord_2d_s coord_2d_t;

struct coord_2d_s
{
    int x;
    int y;
};

int heuristic(vertex_t *a, vertex_t *b);
void *read_2d_data(char *line, int *id);
void smallGraphTest(char *filename, int srcId, int dstId);
void cityGraphTest(char *filename, int srcId, int dstId);
void djikstraTest(char *filename, int srcId, int dstId, void *(*readData)(char *, int *));
void astarTest(char *filename, int srcId, int dstId, int (*heuristic)(vertex_t *, vertex_t *), void *(*readData)(char *, int *));
void astarParTest(char *filename, int srcId, int dstId, int (*heuristic)(vertex_t *, vertex_t *), void *(*readData)(char *, int *));

int main(int argc, char **argv)
{
    util_check_r(argc >= 5, "missing parameter.", 0);

    // First parameter is the option of the graph:
    //  - 0: small graph, i.e. no additional data, so no heuristic
    //  - 1: city graph, i.e. additional data are x, y, so any planar heuristic

    switch (atoi(argv[1]))
    {
    case 0:
        fprintf(stdout, "\n############## Small graph test ##############\n\n");
        smallGraphTest(argv[2], atoi(argv[3]), atoi(argv[4]));
        fprintf(stdout, "\n#########################################\n\n");
        break;

    case 1:
        fprintf(stdout, "\n############## City graph test ##############\n\n");
        cityGraphTest(argv[2], atoi(argv[3]), atoi(argv[4]));
        fprintf(stdout, "\n#########################################\n\n");
        break;

    default:
        break;
    }

    return 1;
}

int heuristic(vertex_t *a, vertex_t *b)
{
    // fprintf(stdout, "Start (%d) %d %d, End (%d) %d %d\n", a->id, ((coord_2d_t *)a->data)->x, ((coord_2d_t *)a->data)->y, b->id, ((coord_2d_t *)b->data)->x, ((coord_2d_t *)b->data)->y);
    return abs(((coord_2d_t *)a->data)->x - ((coord_2d_t *)b->data)->x) + abs(((coord_2d_t *)a->data)->y - ((coord_2d_t *)b->data)->y);
}

void *read_2d_data(char *line, int *id)
{
    int x, y;
    coord_2d_t *data;

    sscanf(line, "%d %d %d", id, &x, &y);

    data = (coord_2d_t *)util_malloc(sizeof(coord_2d_t));
    util_check_r(data != NULL, "Could not allocate data.", 0);

    data->x = x;
    data->y = y;

    return (void *)data;
}

void djikstraTest(char *filename, int srcId, int dstId, void *(*readData)(char *, int *))
{
    graph_t *g;
    vertex_t *src, *dst;
    path_t *path;

    src = (vertex_t *)util_malloc(sizeof(vertex_t));
    util_check_no_r(src != NULL, "Could not allocate src.");

    dst = (vertex_t *)util_malloc(sizeof(vertex_t));
    util_check_no_r(dst != NULL, "Could not allocate dst.");

    path = (path_t *)util_malloc(sizeof(path_t));
    util_check_no_r(path != NULL, "Could not allocate path.");

    uint64_t tg = nano_count();
    g = graph_create(filename, readData == NULL ? NULL : readData);
    tg = nano_count() - tg;
    fprintf(stdout, "Graph created in %f seconds\n", NS_TO_S(tg));

    // graph_stats(stdout, g, NULL);

    graph_find(g, srcId, &src);
    graph_find(g, dstId, &dst);

    fprintf(stdout, "Source: %d\n", src->id);
    fprintf(stdout, "Destination: %d\n", dst->id);

    fprintf(stdout, "DJIKSTRA\n\n");

    uint64_t t = nano_count();

    seq_djikstra_path(g, src, dst, &path);

    t = nano_count() - t;

    if (path != NULL)
    {
        fprintf(stdout, "\nFound path in %f seconds with cost = %d and length = %d. Visited nodes = %d\tRevisited nodes = %d\n", NS_TO_S(t), path->cost, stack_count(path->nodes) - 1, path->visited_nodes, path->revisited_nodes);

        while (!stack_empty_m(path->nodes))
        {
            int *id;

            stack_pop(path->nodes, (void **)&id);
            fprintf(stdout, "%d%s", *id, stack_empty_m(path->nodes) ? "" : ", ");
        }

        fprintf(stdout, "\n");
    }
    else
    {
        fprintf(stdout, "Path not found\n");
    }

    graph_destroy(g, readData == NULL ? NULL : free);
    free(path);

    fprintf(stdout, "\n\n");
}

void astarTest(char *filename, int srcId, int dstId, int (*heuristic)(vertex_t *, vertex_t *), void *(*readData)(char *, int *))
{
    graph_t *g;
    vertex_t *src, *dst;
    path_t *path;

    src = (vertex_t *)util_malloc(sizeof(vertex_t));
    util_check_no_r(src != NULL, "Could not allocate src.");

    dst = (vertex_t *)util_malloc(sizeof(vertex_t));
    util_check_no_r(dst != NULL, "Could not allocate dst.");

    path = (path_t *)util_malloc(sizeof(path_t));
    util_check_no_r(path != NULL, "Could not allocate path.");

    uint64_t tg = nano_count();
    g = graph_create(filename, readData == NULL ? NULL : readData);
    tg = nano_count() - tg;
    fprintf(stdout, "Graph created in %f seconds\n", NS_TO_S(tg));

    // graph_stats(stdout, g, NULL);

    graph_find(g, srcId, &src);
    graph_find(g, dstId, &dst);

    fprintf(stdout, "Source: %d\n", src->id);
    fprintf(stdout, "Destination: %d\n", dst->id);

    fprintf(stdout, "A*\n\n");

    uint64_t t = nano_count();

    seq_a_star_path(g, src, dst, heuristic == NULL ? NULL : heuristic, &path);

    t = nano_count() - t;

    if (path != NULL)
    {
        fprintf(stdout, "\nFound path in %f seconds with cost = %d and length = %d. Visited nodes = %d\tRevisited nodes = %d\n", NS_TO_S(t), path->cost, stack_count(path->nodes) - 1, path->visited_nodes, path->revisited_nodes);

        while (!stack_empty_m(path->nodes))
        {
            int *id;

            stack_pop(path->nodes, (void **)&id);
            fprintf(stdout, "%d%s", *id, stack_empty_m(path->nodes) ? "" : ", ");
        }

        fprintf(stdout, "\n");
    }
    else
    {
        fprintf(stdout, "Path not found\n");
    }

    graph_destroy(g, readData == NULL ? NULL : free);
    free(path);

    fprintf(stdout, "\n\n");
}

void astarParTest(char *filename, int srcId, int dstId, int (*heuristic)(vertex_t *, vertex_t *), void *(*readData)(char *, int *))
{
    graph_t *g;
    vertex_t *src, *dst;
    path_t *path;

    src = (vertex_t *)util_malloc(sizeof(vertex_t));
    util_check_no_r(src != NULL, "Could not allocate src.");

    dst = (vertex_t *)util_malloc(sizeof(vertex_t));
    util_check_no_r(dst != NULL, "Could not allocate dst.");

    path = (path_t *)util_malloc(sizeof(path_t));
    util_check_no_r(path != NULL, "Could not allocate path.");

    uint64_t tg = nano_count();
    g = graph_create(filename, readData == NULL ? NULL : readData);
    tg = nano_count() - tg;
    fprintf(stdout, "Graph created in %f seconds\n", NS_TO_S(tg));

    // graph_stats(stdout, g, NULL);

    graph_find(g, srcId, &src);
    graph_find(g, dstId, &dst);

    fprintf(stdout, "Source: %d\n", src->id);
    fprintf(stdout, "Destination: %d\n", dst->id);

    fprintf(stdout, "PAR A*\n\n");

    uint64_t t = nano_count();

    par_a_star_path(g, src, dst, heuristic == NULL ? NULL : heuristic, &path, 16);

    t = nano_count() - t;

    if (path != NULL)
    {
        fprintf(stdout, "\nFound path in %f seconds with cost = %d and length = %d. Visited nodes = %d\tRevisited nodes = %d\n", NS_TO_S(t), path->cost, stack_count(path->nodes) - 1, path->visited_nodes, path->revisited_nodes);

        while (!stack_empty_m(path->nodes))
        {
            int *id;

            stack_pop(path->nodes, (void **)&id);
            fprintf(stdout, "%d%s", *id, stack_empty_m(path->nodes) ? "" : ", ");
        }

        fprintf(stdout, "\n");
    }
    else
    {
        fprintf(stdout, "Path not found\n");
    }

    graph_destroy(g, readData == NULL ? NULL : free);
    free(path);

    fprintf(stdout, "\n\n");
}

void smallGraphTest(char *filename, int srcId, int dstId)
{
    // djikstraTest(filename, srcId, dstId, NULL);
    // astarTest(filename, srcId, dstId, NULL, NULL);
    astarParTest(filename, srcId, dstId, NULL, NULL);
}

void cityGraphTest(char *filename, int srcId, int dstId)
{
    djikstraTest(filename, srcId, dstId, read_2d_data);
    fprintf(stdout, "\n-------------------------\n");
    astarTest(filename, srcId, dstId, heuristic, read_2d_data);
    fprintf(stdout, "\n-------------------------\n");
    astarParTest(filename, srcId, dstId, heuristic, read_2d_data);
}