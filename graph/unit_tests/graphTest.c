#include <stdio.h>
#include "../graph.h"

int main(int argc, char *argv[])
{
  graph_t *g;
  vertex_t *src, *n;
  int i;

  util_check_m(argc >= 2, "missing parameter.");
  g = graph_create(argv[1], NULL);

  printf("Initial vertex? ");
  scanf("%d", &i);

  printf("Incoming/Outgoing Edges\n");
  graph_find(g, i, &src);
  printf("Node info: %d\n", src->id);

  n = g->head;
  printf("List of vertices:\n");
  while (n != NULL)
  {
    printf("Node: %2d\n", n->id);

    edge_t *e = n->head;

    while (e != NULL)
    {
      printf("Edge from [%2d] to [%2d] with weight %d\n", n->id, e->dest->id, e->weight);
      e = e->next;
    }

    n = n->next;
  }

  graph_destroy(g, NULL);

  return EXIT_SUCCESS;
}