#include "__heuristic.h"

// Private Methods
int (*choose_heuristic(enum Heuristic h, enum Domain d))(vertex_t *a, vertex_t *b)
{
    switch (d)
    {
        case GRID_2D:
            switch (h)
            {
                case MANHATTAN:
                    return &manhattan;
                    break;

                default:
                    return NULL;
                    break;
            }
            break;

        default:
            return NULL;
            break;
    }
}

// Public Methods

int manhattan(vertex_t *a, vertex_t *b)
{
    util_check_r(a != NULL, "Node a cannot be null\n", 0);
    util_check_r(b != NULL, "Node b cannot be null\n", 0);
    util_check_r(a->data != NULL, "Node a's data cannot be null\n", 0);
    util_check_r(b->data != NULL, "Node b's data cannot be null\n", 0);

    return abs(((coord_2d_t *)a->data)->x - ((coord_2d_t *)b->data)->x) + abs(((coord_2d_t *)a->data)->y - ((coord_2d_t *)b->data)->y);
}