#ifndef _DOMAIN_PRIVATE
#define _DOMAIN_PRIVATE

#include "../utils/util.h"
#include "domain.h"

// Data structures declaration

typedef struct coord_2d_s
{
    int x;
    int y;
} coord_2d_t;

// Function prototypes

/*
    This function reads additional info about a node, supposing it is place on a 2d grid.
    It scans an input line and returns a pointer to the structure containing the additional info and the id of the node, passed as parameter.
*/
extern void *read_2d_data(char *line, int *id);

#endif
