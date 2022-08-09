#ifndef _HEURISTIC_PRIVATE
#define _HEURISTIC_PRIVATE

#include "../utils/util.h"
#include "heuristic.h"
#include "__domain.h"

// Function prototypes

/*
    This function compute the manhattan distance between two nodes inside a 2d grid.
    It returns the distance between the two nodes.
*/
extern int manhattan(vertex_t *a, vertex_t *b);

#endif
