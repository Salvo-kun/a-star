#ifndef _GRAPH_PRIVATE
#define _GRAPH_PRIVATE

#include "graph.h"
#include <pthread.h>

// Constants definition
#define MAX_LINE 256

// Data structures declaration
typedef struct parse_graph_s
{
  char *chunk_nodes_start;
  char *chunk_nodes_end;
  char *chunk_edges_start;
  char *chunk_edges_end;
  pthread_mutex_t *lock;
  pthread_barrier_t *barrier;
  graph_t *graph;
} parse_graph_t;

// Private function prototypes
extern vertex_t *new_node(vertex_t *, int, void *);
extern void new_edge(graph_t *, vertex_t *, vertex_t *, int);

#endif
