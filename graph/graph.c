#include "graph.h"

// Constants definition
#define MAX_LINE 256

// Static function prototypes
static vertex_t *new_node(vertex_t *, int, void *);
static void new_edge(graph_t *, vertex_t *, vertex_t *, int);

graph_t *graph_load(char *filename, void *(*readData)(char *))
{
  graph_t *graph;
  char line[MAX_LINE];
  int i, j, weight;
  FILE *fp;

  // Allocate graph
  graph = (graph_t *)util_malloc(sizeof(graph_t));

  // Could not allocate, return null
  util_check_r(graph != NULL, "Graph cannot be null, returning...\n", NULL);

  fp = util_fopen(filename, "r");

  // Could not open file, return null
  util_check_r(fp != NULL, "Could not open file, returning...\n", NULL);

  fgets(line, MAX_LINE, fp);

  // Check first line format
  util_check_r(sscanf(line, "%d%d", &graph->count, &graph->type) == 2, "First line has wrong format, returning...\n", NULL);
  util_check_r(graph->type == UNDIRECTED || graph->type == DIRECTED, "Graph type can only be 0 or 1, returning...\n", NULL);

  // Create nodes (data not empty only if readData != NULL)
  for (i = 0; i < graph->count; ++i)
  {
    void *data = NULL;
    int id = i;

    if (readData != NULL)
    {
      // Reads line, scans id, reads additional data
      fgets(line, MAX_LINE, fp);
      sscanf(line, "%d", &id);
      data = readData(line);
    }

    graph->head = new_node(graph->head, id, data);
  }

  // Create edges
  while (fgets(line, MAX_LINE, fp) != NULL)
  {

    // Check edge lines format
    util_check_r(sscanf(line, "%d%d%d", &i, &j, &weight) == 3, "Edge line has wrong format, returning...\n", NULL);

    // Create edge
    vertex_t *src, *dst;
    graph_find(graph, i, &src);
    graph_find(graph, j, &dst);
    new_edge(graph, src, dst, weight);

    // If undirected, create also reverse direction edge
    if (graph->type == UNDIRECTED)
    {
      new_edge(graph, dst, src, weight);
    }
  }

  fclose(fp);

  return graph;
}

graph_t *graph_transpose(graph_t *graph)
{
  // Graph cannot be null
  util_check_r(graph != NULL, "Graph cannot be null, returning...\n", NULL);

  graph_t *graphT = NULL;
  vertex_t *tmp;
  edge_t *e;

  graphT = (graph_t *)util_malloc(sizeof(graph_t));

  // Could not allocate, return null
  util_check_r(graphT != NULL, "Graph cannot be null, returning...\n", NULL);

  // Copy number of nodes
  graphT->count = graph->count;

  // Iterate and recreate nodes
  tmp = graph->head;

  while (tmp != NULL)
  {
    graphT->head = new_node(graphT->head, tmp->id, tmp->data);
    tmp = tmp->next;
  }

  // Iterate trhough nodes and recreate edges
  tmp = graph->head;

  while (tmp != NULL)
  {
    e = tmp->head;
    while (e != NULL)
    {
      new_edge(graphT, e->dest, tmp, e->weight);
      e = e->next;
    }
    tmp = tmp->next;
  }

  return graphT;
}

int graph_find(graph_t *graph, int id, vertex_t **node)
{
  // Graph cannot ben null
  util_check_r(graph != NULL, "Graph cannot be null, returning...\n", 0);

  vertex_t *v;

  v = graph->head;
  while (v != NULL)
  {
    if (v->id == id)
    {
      // Node found
      *node = v;
      return 1;
    }
    v = v->next;
  }

  // Node not found
  *node = NULL;
  return 1;
}

int graph_destroy(graph_t *graph, void (*freeData)(void *))
{
  // Check graph is not null before starting
  util_check_r(graph != NULL, "Graph is already null, returning...\n", 0);

  vertex_t *v, *tmp;
  edge_t *e;

  // Iterate over nodes and free edges for each node, then the node
  v = graph->head;
  while (v != NULL)
  {
    // Iterate over edges
    while (v->head != NULL)
    {
      e = v->head;
      v->head = e->next;
      free(e);
    }
    tmp = v->next;

    // If possible, free node data
    if (freeData != NULL)
    {
      freeData(v->data);
    }

    free(v);
    v = tmp;
  }

  return 1;
}

// Private Methods

static vertex_t *new_node(vertex_t *next, int id, void *data)
{
  vertex_t *v;

  v = (vertex_t *)util_malloc(1 * sizeof(vertex_t));
  v->id = id;
  v->data = data;
  v->head = NULL;
  v->prev = NULL;
  v->next = next;

  return v;
}

static void new_edge(graph_t *g, vertex_t *src, vertex_t *dst, int weight)
{
  // Source and Destination cannot be null
  util_check_no_r(src != NULL, "Source cannot be null, returning...\n");
  util_check_no_r(dst != NULL, "Destination cannot be null, returning...\n");

  edge_t *e;

  e = (edge_t *)util_malloc(1 * sizeof(edge_t));
  e->dest = dst;
  e->weight = weight;
  e->next = src->head;
  src->head = e;

  return;
}
