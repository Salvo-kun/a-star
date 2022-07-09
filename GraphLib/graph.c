#include "graph.h"

/* static function prototypes */
static vertex_t *new_node (vertex_t *, int);
static void new_edge (graph_t *, int, int, int);

/*
 *  load a graph from file: adjacency lists
 */
graph_t *graph_load (char *filename) {
  graph_t *g;
  char line[MAX_LINE];
  int i, j, weight, dir;
  FILE *fp;

  g = (graph_t *) util_calloc (1, sizeof(graph_t));

  fp = util_fopen(filename, "r");
  fgets(line, MAX_LINE, fp);
  if (sscanf(line, "%d%d", &g->nv, &dir) != 2) {
    sscanf(line, "%d", &g->nv);
    dir = 1;
  }

  /* create initial structure for vertices */
  for (i=g->nv-1; i>=0; i--) {
    g->g = new_node (g->g, i);
  }

  /* load edges */
  while (fgets(line, MAX_LINE, fp) != NULL) {
    if (sscanf(line, "%d%d%d", &i, &j, &weight) != 3) {
      sscanf(line, "%d%d", &i, &j);
      weight = 1;
    }
    new_edge (g, i, j, weight);
    if (dir == 0) {
      new_edge (g, j, i, weight);
    }
  } 
  fclose(fp);

  return g;
}

/*
 *  init all graph attributes
 */
void graph_attribute_init (graph_t *g) {
  vertex_t *v;

  v = g->g;
  while (v!=NULL) {
    v->color = WHITE;
    v->dist = INT_MAX;
    v->disc_time = -1;
    v->endp_time = -1;
    v->scc = -1;
    v->pred = NULL;
    v = v->next;
  }

  return;
}

/*
 *  transpose a given graph
 */
graph_t *graph_transpose (graph_t *g) {
  graph_t *h = NULL;
  vertex_t *tmp;
  edge_t *e;
  int i;

  h = (graph_t *) util_calloc (1, sizeof(graph_t));
  h->nv = g->nv;
  for (i=h->nv-1; i>=0; i--) {
    h->g = new_node (h->g, i);
  }
  tmp = g->g;
  while (tmp != NULL) {
    e = tmp->head;
    while (e != NULL) {
      new_edge (h, e->dst->id, tmp->id, e->weight);
      e = e->next;
    }
    tmp = tmp->next;
  }

  return h;
}

/*
 *  find the vertex element with a given id
 */
vertex_t *graph_find (graph_t *g, int id) {
  vertex_t *v;

  v = g->g;
  while (v != NULL) {
    if (v->id == id) {
      return v;
    }
    v = v->next;
  }

  return NULL;
}

/*
 *  quit the memory associated to a graph
 */
void graph_dispose (graph_t *g) {
  vertex_t *v, *tmp;
  edge_t *e;

  v = g->g;
  while (v != NULL) {
    while (v->head != NULL) {
      e = v->head;
      v->head = e->next;
      free (e);
    }
    tmp = v->next;
    free (v);
    v = tmp;
  }

  return;
}

/*
 *  initialize the list element for a new vertex
 */
static vertex_t *new_node (vertex_t *g, int id) {
  vertex_t *v;

  v = (vertex_t *) util_malloc (1 * sizeof(vertex_t));
  v->id = id;
  v->color = WHITE;
  v->dist = INT_MAX;
  v->disc_time = -1;
  v->endp_time = -1;
  v->head = NULL;
  v->pred = NULL;
  v->scc = -1;
  v->next = g;

  return v;
}

/*
 *  insert a new edge in the graph
 */
static void new_edge (graph_t *g, int i, int j, int weight)  {
  vertex_t *src, *dst;
  edge_t *e;

  src = graph_find (g, i);
  dst = graph_find (g, j);

  e = (edge_t *) util_malloc (1 * sizeof(edge_t));
  e->dst = dst;
  e->weight = weight;
  e->next = src->head;
  src->head = e;

  return;
}
