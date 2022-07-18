#include "pathfinding.h"

// Public methods

int seq_a_star_path(graph_t *graph, vertex_t *src, vertex_t *dst, int (*heuristic)(vertex_t *, vertex_t *), queue_t **path)
{
    // Creates the priority queue which will contain unvisited nodes
    heap_t *open_pq = heap_create(0, 1);

    // Create closed nodes set, which will contain visited nodes
    hash_table_t *closed_set = hash_table_create(1);

    // Init open with source
    heap_insert(open_pq, src->id, (void *)src, 0);

    while (!heap_is_empty(open_pq))
    {
        vertex_t *min_node;
        heap_extract(open_pq, (void **)&min_node, NULL);

        hash_table_insert(closed_set, min_node->id, (void *)min_node);

        if (min_node->id == dst->id)
        {
            vertex_t *n = dst;
            
            // TODO: Reconstruct path

            return 1;
        }

        edge_t *e = min_node->head;

        while (e != NULL)
        {
            int true_cost = min_node->true_cost + e->weight;
            int heuristic_cost = heuristic(e->dest, dst);

            void *data;
            hash_table_get(closed_set, e->dest->id, &data);

            if (data != NULL)
            {
                if (true_cost < e->dest->true_cost)
                {
                    hash_table_delete(closed_set, e->dest->id);
                    heap_insert(open_pq, e->dest->id, (void *)e->dest, true_cost + heuristic_cost);
                }
                else
                {
                    continue;
                }
            }
            else
            {
                int *position;
                heap_find(open_pq, e->dest->id, &position);
                if (position == NULL)
                {
                    heap_insert(open_pq, e->dest->id, (void *)e->dest, true_cost + heuristic_cost);
                }
                else if (true_cost >= e->dest->true_cost)
                {
                    continue;
                }
            }

            e->dest->true_cost = true_cost;
            e->dest->heuristic_cost = heuristic_cost;
            e->dest->parent = min_node;
        }
    }

    return 1;
}