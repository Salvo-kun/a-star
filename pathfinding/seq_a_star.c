#include "pathfinding.h"

// Public methods

int seq_a_star_path(graph_t *graph, vertex_t *src, vertex_t *dst, int (*heuristic)(vertex_t *, vertex_t *), path_t **path)
{
    // Check parameters are not null before starting
    util_check_r(graph != NULL, "Graph cannot be null, returning...\n", 0);
    util_check_r(src != NULL, "Source nodes cannot be null, returning...\n", 0);
    util_check_r(dst != NULL, "Destination nodes cannot be null, returning...\n", 0);

    // Declarations
    int visited_nodes, revisited_nodes, result;

    // Init variables
    *path = NULL;
    visited_nodes = revisited_nodes = 0;
    result = 1;

    // Creates the priority queue which will contain unvisited nodes
    heap_t *open_pq = heap_create(0, 10);

    // Check allocation was successful
    util_check_r(open_pq != NULL, "Could not allocate open set, returning...\n", 0);

    // Create closed nodes set, which will contain visited nodes
    hash_table_t *closed_set = hash_table_create(10);

    // Check allocation was successful
    util_check_r(closed_set != NULL, "Could not allocate closed set, returning...\n", 0);

// Init open with source
#if DEBUG_ASTAR
    fprintf(stdout, "Add node with id %d to open set\n", src->id);
#endif
    result = heap_insert(open_pq, src->id, (void *)src, 0);

    // Check no errors occurred
    util_check_r(result, "Could not insert in the open set, returning...\n", 0);

    while (!heap_is_empty(open_pq))
    {
        vertex_t **min_node = (vertex_t **)util_malloc(sizeof(vertex_t *));

        // Check allocation was successful
        util_check_r(min_node != NULL, "Could not allocate min_node, returning...\n", 0);

#if DEBUG_ASTAR
        fprintf(stdout, "Extract node with minimum cost from open set\n");
#endif
        result = heap_extract(open_pq, (void **)min_node, NULL);

        // Check no errors occurred
        util_check_r(result, "Could not extract from the open set, returning...\n", 0);

#if DEBUG_ASTAR
        fprintf(stdout, "Add node with id %d to closed set\n", (*min_node)->id);
#endif
        result = hash_table_insert(closed_set, (*min_node)->id, (void *)*min_node);

        // Check no errors occurred
        util_check_r(result, "Could not insert in the closed set, returning...\n", 0);

        visited_nodes++;

        if ((*min_node)->id == dst->id)
        {
            vertex_t *n = dst;

            *path = (path_t *)util_malloc(sizeof(path_t));

            // Check allocation was successful
            util_check_r(path != NULL, "Could not allocate path, returning...\n", 0);

            (*path)->nodes = stack_create();

            // Check allocation was successful
            util_check_r((*path)->nodes != NULL, "Could not allocate path's stack, returning...\n", 0);

            (*path)->cost = dst->true_cost;
            (*path)->visited_nodes = visited_nodes;
            (*path)->revisited_nodes = revisited_nodes;

            while (n != NULL)
            {
                result = stack_push((*path)->nodes, (void *)&(n->id));

                // Check no errors occurred
                util_check_r(result, "Could not insert in the path's stack, returning...\n", 0);

                if (n == src)
                {
                    // Avoid going backward.
                    break;
                }

                n = n->parent;
            }

            // Avoid memory leaks
            free(min_node);
            break;
        }

        edge_t *e = (*min_node)->head;

        while (e != NULL)
        {
#if DEBUG_ASTAR
            fprintf(stdout, "Checking edge from node with id %d to node with id %d\n", (*min_node)->id, e->dest->id);
#endif
            int true_cost = (*min_node)->true_cost + e->weight;
            int heuristic_cost = heuristic == NULL ? 0 : heuristic(e->dest, dst);

            vertex_t **data = (vertex_t **)util_malloc(sizeof(vertex_t *));

            // Check allocation was successful
            util_check_r(data != NULL, "Could not allocate data to extract from closed set, returning...\n", 0);

            result = hash_table_get(closed_set, e->dest->id, (void **)data);

            // Check no errors occurred
            util_check_r(result, "Could not get from the closed set, returning...\n", 0);

            int already_processed = *data != NULL;
            free(data);

            if (already_processed)
            {
#if DEBUG_ASTAR
                fprintf(stdout, "Node with id %d already in closed set\n", e->dest->id);
#endif
                if (true_cost < e->dest->true_cost)
                {

#if DEBUG_ASTAR
                    fprintf(stdout, "Found smaller cost for node with id %d, readding it to open set\n", e->dest->id);
#endif
                    result = hash_table_delete(closed_set, e->dest->id, NULL);

                    // Check no errors occurred
                    util_check_r(result, "Could not delete from the closed set, returning...\n", 0);

                    result = heap_insert(open_pq, e->dest->id, (void *)e->dest, true_cost + heuristic_cost);

                    // Check no errors occurred
                    util_check_r(result, "Could not insert in the open set, returning...\n", 0);

                    revisited_nodes++;
                }
                else
                {
                    e = e->next;
                    continue;
                }
            }
            else
            {
#if DEBUG_ASTAR
                fprintf(stdout, "Node with id %d not in closed set\n", e->dest->id);
#endif
                int **position = (int **)util_malloc(sizeof(int *));

                // Check allocation was successful
                util_check_r(position != NULL, "Could not allocate position to extract from open set, returning...\n", 0);

                result = heap_find(open_pq, e->dest->id, position);

                // Check no errors occurred
                util_check_r(result, "Could not search in the open set, returning...\n", 0);

                int not_enqueued = *position == NULL;

                free(position);

                if (not_enqueued)
                {
#if DEBUG_ASTAR
                    fprintf(stdout, "Add node with id %d to open set\n", e->dest->id);
#endif
                    result = heap_insert(open_pq, e->dest->id, (void *)e->dest, true_cost + heuristic_cost);

                    // Check no errors occurred
                    util_check_r(result, "Could not insert in the open set, returning...\n", 0);
                }
                else if (true_cost >= e->dest->true_cost)
                {
                    e = e->next;
                    continue;
                }
                else
                {
                    result = heap_update(open_pq, e->dest->id, true_cost + heuristic_cost, NULL);

                    // Check no errors occurred
                    util_check_r(result, "Could not update in the open set, returning...\n", 0);
                }
            }

#if DEBUG_ASTAR
            fprintf(stdout, "Updating costs and parent for node with id %d to open set\n", e->dest->id);
#endif
            e->dest->true_cost = true_cost;
            e->dest->heuristic_cost = heuristic_cost;
            e->dest->parent = *min_node;
            e = e->next;
        }

        free(min_node);
    }

    // Free memory
    result = heap_destroy(open_pq, NULL);

    // Check no errors occurred
    util_check_r(result, "Could not destroy open set, returning...\n", 0);

    result = hash_table_destroy(closed_set, NULL);

    // Check no errors occurred
    util_check_r(result, "Could not destroy closed set, returning...\n", 0);

    return 1;
}