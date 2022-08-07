#include "__domain.h"

// Public Methods
void *(*choose_domain_reader(enum Domain d))(char *, int *)
{
    switch (d)
    {
        case GRID_2D:
            return &read_2d_data;
            break;

        default:
            return NULL;
            break;
    }
}

// Private Methods

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