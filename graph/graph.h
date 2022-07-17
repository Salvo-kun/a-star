#ifndef _GRAPH_PRIVATE
#define _GRAPH_PRIVATE

#include "limits.h"
#include "../utils/util.h"

// Data structures declaration
enum {UNDIRECTED, DIRECTED};

typedef struct graph_s graph_t;
typedef struct vertex_s vertex_t;
typedef struct edge_s edge_t;

struct graph_s {
  vertex_t *head;
  int count;
  int type;
};

struct edge_s {
  int weight;
  vertex_t *dest;
  edge_t *next;
};

struct vertex_s {
  int id;
  void *data;
  vertex_t *prev;
  edge_t *head;
  vertex_t *next;
};

// Function prototypes

/*
  Creates a graph loading it from a given file.
  The file format is the following, where each field is separated by a space:
    - One single line with two fields: number of nodes and type of graph (0 if undirected, 1 if directed)
    - One line for each node with variable number of fields: node id, additional data (read through a custom function to be passed, if NULL these lines are not read)
    - Variable number of lines with two fields: start node id, end node id (represents an edge)
  Notice that each line cannot exceed 256 characters.
  Returns NULL if an error occurs, the graph pointer otherwise.
*/
extern graph_t *graph_create(char *filename, void *(*readData)(char *));

/*
  Transposes a given graph.
  Returns NULL if an error occurs, the transposed graph pointer otherwise.
*/
extern graph_t *graph_transpose (graph_t *graph);

/*
  Finds a node inside the graph given its id. If the node is not found, its pointer is set to NULL.
  Returns 0 if an error occurs, 1 otherwise.
*/
extern int graph_find(graph_t *graph, int id, vertex_t **node);

/*
  Frees the allocated memory for the given graph, eventually freeing also the contained data (if freeData is not NULL).
  Returns 0 if an error occurs, 1 otherwise.
*/
extern int graph_destroy(graph_t *graph, void (*freeData)(void *));

#endif
