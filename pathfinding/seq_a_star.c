#include "pathfinding.h"

// Public methods

int seq_a_star_path(graph_t *graph, vertex_t *src, vertex_t *dst, int (*heuristic)(vertex_t *, vertex_t *), path_t **path)
{
    // Init path to null
    *path = NULL;

    // Creates the priority queue which will contain unvisited nodes
    heap_t *open_pq = heap_create(0, 10);

    // Create closed nodes set, which will contain visited nodes
    hash_table_t *closed_set = hash_table_create(10);

    // Init open with source
    fprintf(stdout, "Add node with id %d to open set\n", src->id);
    heap_insert(open_pq, src->id, (void *)src, 0);

    while (!heap_is_empty(open_pq))
    {
        vertex_t *min_node;

        fprintf(stdout, "Extract node with minimum cost from open set\n");
        heap_extract(open_pq, (void **)&min_node, NULL);

        fprintf(stdout, "Add node with id %d to closed set\n", min_node->id);
        hash_table_insert(closed_set, min_node->id, (void *)min_node);

        if (min_node->id == dst->id)
        {
            vertex_t *n = dst;

            *path = (path_t *)util_malloc(sizeof(path_t));

            (*path)->nodes = stack_create();
            (*path)->count = 0;
            (*path)->cost = dst->true_cost;

            while (n != NULL)
            {
                (*path)->count++;
                stack_push((*path)->nodes, (void *)&(n->id));
                n = n->parent;
            }

            return 1;
        }

        edge_t *e = min_node->head;

        while (e != NULL)
        {

            fprintf(stdout, "Checking edge from node with id %d to node with id %d\n", min_node->id, e->dest->id);
            int true_cost = min_node->true_cost + e->weight;
            int heuristic_cost = heuristic(e->dest, dst);

            void *data;
            hash_table_get(closed_set, e->dest->id, (void **)&data);

            if (data != NULL)
            {

                fprintf(stdout, "Node with id %d already in closed set\n", e->dest->id);
                if (true_cost < e->dest->true_cost)
                {

                    fprintf(stdout, "Found smaller cost for node with id %d, readding it to open set\n", e->dest->id);
                    hash_table_delete(closed_set, e->dest->id);
                    heap_insert(open_pq, e->dest->id, (void *)e->dest, true_cost + heuristic_cost);
                }
                else
                {
                    e = e->next;
                    continue;
                }
            }
            else
            {

                fprintf(stdout, "Node with id %d not in closed set\n", e->dest->id);
                int *position;
                heap_find(open_pq, e->dest->id, &position);
                if (position == NULL)
                {

                    fprintf(stdout, "Add node with id %d to open set\n", e->dest->id);
                    heap_insert(open_pq, e->dest->id, (void *)e->dest, true_cost + heuristic_cost);
                }
                else if (true_cost >= e->dest->true_cost)
                {
                    e = e->next;
                    continue;
                }
            }

            fprintf(stdout, "Updating costs and parent for node with id %d to open set\n", e->dest->id);
            e->dest->true_cost = true_cost;
            e->dest->heuristic_cost = heuristic_cost;
            e->dest->parent = min_node;
            e = e->next;
        }
    }

    // Free memory
    heap_destroy(open_pq, NULL);
    hash_table_destroy(closed_set, NULL);

    return 1;
}