#include <stdio.h>
#include "../graph.h"

int main(int argc, char *argv[])
{
  graph_t *g;
  vertex_t *src;

  util_check_m(argc >= 3, "missing parameter.");
  g = graph_create(argv[1], NULL);

  printf("Incoming/Outgoing Edges\n");
  graph_find(g, atoi(argv[2]), &src);
  printf("Node info: %d\n", src->id);

  graph_stats(stdout, g, NULL);

  graph_destroy(g, NULL);

  return EXIT_SUCCESS;
}