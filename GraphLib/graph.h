#ifndef _GRAPH
#define _GRAPH

#include "limits.h"
#include "util.h"

/* constant declaration */
#define MAX_LINE 100
enum {WHITE, GREY, BLACK};

typedef struct graph_s graph_t;
typedef struct vertex_s vertex_t;
typedef struct edge_s edge_t;

/* graph wrapper */
struct graph_s {
  vertex_t *g;
  int nv;
};

/* list (vertices) of lists (edges): edges */
struct edge_s {
  int weight;
  vertex_t *dst;
  edge_t *next;
};

/* list (vertices) of lists (edges): vertices */
struct vertex_s {
  int id;
  int color;
  int dist;
  int disc_time;
  int endp_time;
  int scc;
  vertex_t *pred;
  edge_t *head;
  vertex_t *next;
};

/* extern function prototypes */
extern graph_t *graph_load (char *);
extern void graph_attribute_init (graph_t *);
extern graph_t *graph_transpose (graph_t *);
extern vertex_t *graph_find (graph_t *, int);
extern void graph_dispose (graph_t *);

#endif
