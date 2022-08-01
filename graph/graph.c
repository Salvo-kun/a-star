#include "__graph.h"

// Public Methods

graph_t *graph_create(char *filename, void *(*readData)(char *, int *))
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

  // Init graph
  graph->head = NULL;
  graph->nodes_map = hash_table_create(graph->count / 10000 + 1);

  // Could not allocate, return null
  util_check_r(graph->nodes_map != NULL, "Graph map cannot be null, returning...\n", NULL);

  // Create nodes (data not empty only if readData != NULL)
  for (i = 0; i < graph->count; ++i)
  {
    void *data = NULL;
    int id = i;

    if (readData != NULL)
    {
      // Reads line, scans id, reads additional data
      fgets(line, MAX_LINE, fp);
      data = readData(line, &id);
    }

    graph->head = new_node(graph->head, id, data);
    hash_table_insert(graph->nodes_map, id, (void *)graph->head);
  }

  // Create edges
  while (fgets(line, MAX_LINE, fp) != NULL)
  {
    // Check edge lines format
    util_check_r(sscanf(line, "%d%d%d", &i, &j, &weight) == 3, "Edge line has wrong format, returning...\n", NULL);

    // Create edge
    vertex_t *src, *dst;

    src = (vertex_t *)util_malloc(sizeof(vertex_t));
    util_check_r(src != NULL, "Could not allocate src.", 0);

    dst = (vertex_t *)util_malloc(sizeof(vertex_t));
    util_check_r(dst != NULL, "Could not allocate dst.", 0);

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

int graph_find(graph_t *graph, int id, vertex_t **node)
{
  // Graph cannot ben null
  util_check_r(graph != NULL, "Graph cannot be null, returning...\n", 0);
  util_check_r(node != NULL, "Node pointer cannot be null, returning...\n", 0);

  hash_table_get(graph->nodes_map, id, (void **)node);

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

  hash_table_destroy(graph->nodes_map, NULL);
  free(graph);

  return 1;
}

void graph_stats(FILE *fp, graph_t *graph, void (*printData)(FILE *, void *))
{
  util_check_no_r(graph != NULL, "Graph cannot be null, returning...\n");

  fprintf(fp, "\n############## Graph Stats ##############\n\n");
  fprintf(fp, "Type = %s\n", graph->type == 0 ? "UNDIRECTED" : "DIRECTED");
  fprintf(fp, "Count = %d\n", graph->count);

  vertex_t *n = graph->head;

  printf("List of vertices:\n");
  while (n != NULL)
  {
    printf("Node: %2d\n", n->id);

    if (printData != NULL)
    {
      fprintf(fp, "Data: \n");
      printData(fp, n->data);
    }

    edge_t *e = n->head;

    while (e != NULL)
    {
      printf("Edge from [%2d] to [%2d] with weight %d\n", n->id, e->dest->id, e->weight);
      e = e->next;
    }

    n = n->next;
    fprintf(fp, "\n");
  }

  fprintf(fp, "\n########################################\n\n");
}

// Private Methods

vertex_t *new_node(vertex_t *next, int id, void *data)
{
  vertex_t *v;

  v = (vertex_t *)util_malloc(1 * sizeof(vertex_t));
  v->id = id;
  v->true_cost = 0;
  v->heuristic_cost = 0;
  v->data = data;
  v->head = NULL;
  v->parent = NULL;
  v->next = next;

  return v;
}

void new_edge(graph_t *g, vertex_t *src, vertex_t *dst, int weight)
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
