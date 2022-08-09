#ifndef _HEURISTIC
#define _HEURISTIC

#include "domain.h"
#include "graph.h"

// Data structures declaration
enum Heuristic
{
  MANHATTAN
};


// Function prototypes

/*
    This function returns a pointer to the chosen heuristic function for a given domain.
    If the specified function does not exist or does not suit the specified domain, a NULL pointer is returned.
*/
extern int (*choose_heuristic(enum Heuristic h, enum Domain d))(vertex_t *a, vertex_t *b);

#endif
