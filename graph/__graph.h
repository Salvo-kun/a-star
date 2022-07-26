#ifndef _GRAPH_PRIVATE
#define _GRAPH_PRIVATE

#include "graph.h"

// Constants definition
#define MAX_LINE 256

// Private function prototypes
extern vertex_t *new_node(vertex_t *, int, void *);
extern void new_edge(graph_t *, vertex_t *, vertex_t *, int);

#endif
