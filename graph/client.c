#include <stdio.h>
#include "queuePublic.h"
#include "graph.h"

/* function prototypes */
void graph_bfs (graph_t *, vertex_t *);
void graph_dfs (graph_t *, vertex_t *);
int graph_dfs_r (graph_t *, vertex_t *, int);

/*
 *  main program
 */
int main (int argc, char *argv[]) {
  graph_t *g;
  vertex_t *src, *n;
  int i;

  util_check_m(argc>=2, "missing parameter.");
  g = graph_load (argv[1]);

  printf("Initial vertex? ");
  scanf("%d", &i);

  printf ("Incoming/Outgoing Edges\n");
  src = graph_find (g, i);

  /* DFS */
  printf ("DFS\n");
  // Redundant after load
  graph_attribute_init (g);
  graph_dfs (g, src);

  /* BFS */
  printf ("BFS\n");
  graph_attribute_init (g);
  graph_bfs (g, src);

  /* print reachable nodes with info */
  n = g->g;
  printf("List of vertices:\n");
  while (n != NULL) {
    if (n->color != WHITE) {
      printf("%2d: [%d] (%d)\n", n->id, n->dist, n->pred ? n->pred->id : -1);
    }
    n = n->next;
  }

  // /* BFS count */
  // printf ("\n\nBFS count: ");
  // graph_attribute_init (g);
  // printf("%d\n\n", graph_bfs_count(g, src));

  // /* BFS distance */
  // printf ("BFS distance: ");
  // graph_attribute_init (g);
  // printf("%d\n\n", graph_bfs_distance(g, src));

  // /* DFS type & depth */
  // printf ("DFS type & depth:\n");
  // graph_attribute_init (g);
  // int t, b, f, c, d;
  // t = b = f = c = d = 0;
  // graph_dfs_type(g, src, &t, &b, &f, &c, &d);
  // printf("Tree: %d\nBack: %d\nForward: %d\nCross: %d\nDepth: %d\n\n", t, b, f, c, d);

  // /* DFS Hamiltonian */
  // printf("DFS Hamiltonian from vertex %d: \n", src->id);
  // graph_attribute_init (g);
  // int len = graph_dfs_r_max_path(g, src, 0, 0);

  // if (len != 0)
  //   printf("Path length: %d\n\n", len);
  // else
  //   printf("Path length: not found\n\n");

  graph_dispose (g);

  return EXIT_SUCCESS;
}

/*
 *  perform BFS from vertex n
 */
void graph_bfs (graph_t *g, vertex_t *n) {
  queue_t *qp;
  vertex_t *d;
  edge_t *e;

  qp = queue_init (g->nv);
  n->color = GREY;
  n->dist = 0;
  n->pred = NULL;
  queue_put (qp, (void *)n);

  while (!queue_empty_m (qp)) {
    queue_get (qp, (void **)&n);
    e = n->head;
    while (e != NULL) {
      d = e->dst;
      if (d->color == WHITE) {
	d->color = GREY;
	d->dist = n->dist + 1;
	d->pred = n;
	queue_put (qp, (void *)d);
      }
      e = e->next;
    }
    n->color = BLACK;
  }
  queue_dispose (qp, NULL);

  return;
}

/*
 *  perform DFS from vertex i
 */
void graph_dfs (graph_t *g, vertex_t *n) {
  int currTime=0;
  vertex_t *tmp, *tmp2;

  printf("List of edges:\n");
  currTime = graph_dfs_r (g, n, currTime);
  for (tmp=g->g; tmp!=NULL; tmp=tmp->next) {
    if (tmp->color == WHITE) {
      currTime = graph_dfs_r (g, tmp, currTime);
    }
  }

  printf("List of vertices:\n");
  for (tmp=g->g; tmp!=NULL; tmp=tmp->next) {
    tmp2 = tmp->pred;
    printf("%2d: %2d/%2d (%d)\n",
      tmp->id, tmp->disc_time, tmp->endp_time,
      (tmp2!=NULL) ? tmp->pred->id : -1);
  }
}

/*
 *  perform recursive DFS from vertex i:
 *  labels nodes with td/tq, edges as T/F/B/C
 */
int graph_dfs_r(graph_t *g, vertex_t *n, int currTime) {
  edge_t *e;
  vertex_t *tmp;

  n->color = GREY;
  n->disc_time = ++currTime;
  e = n->head;
  while (e != NULL) {
    tmp = e->dst;
    switch (tmp->color) {
      case WHITE: printf("%d -> %d : T\n", n->id, tmp->id); break;
      case GREY : printf("%d -> %d : B\n", n->id, tmp->id); break;
      case BLACK: if (n->disc_time < tmp->disc_time) {
                    printf("%d -> %d : F\n", n->disc_time, tmp->disc_time);
                  } else {
                    printf("%d -> %d : C\n", n->id, tmp->id);
                  }
    }
    if (tmp->color == WHITE) {
      tmp->pred = n;
      currTime = graph_dfs_r (g, tmp, currTime);
    }
    e = e->next;
  }
  n->color = BLACK;
  n->endp_time = ++currTime;

  return currTime;
}
