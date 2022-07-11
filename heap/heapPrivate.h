#ifndef _HEAP_PRIVATE
#define _HEAP_PRIVATE

#include "heapPublic.h"

/* Data structures declaration */
enum { MIN_HEAP, MAX_HEAP };

typedef struct node {
  void *data;
  int priority;
} node_t;

struct heap {
  node_t *nodes;
  int count;
  int capacity;
  int type;
};

/* Private methods */
extern int heap_compare(node_t a, node_t b, int type);
extern int heap_heapify(heap_t *heap, int position);
extern int heap_move_down(heap_t *heap, node_t node, int position);
extern int heap_move_up(heap_t *heap, node_t node, int position);
extern int swap(heap_t *heap, int a, int b);
extern void heap_stats(heap_t *heap);

#endif
