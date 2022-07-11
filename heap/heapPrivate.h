#ifndef _HEAP_PRIVATE
#define _HEAP_PRIVATE

#include "heapPublic.h"

/* Data structures declaration */
enum { MIN_HEAP, MAX_HEAP };

typedef struct node {
  void *data;
  int priority;
  int key;
} node_t;

struct heap {
  node_t *nodes;
  int count;
  int capacity;
  int type;
};

/* Private methods */
extern int heap_compare(node_t first_node, node_t second_node, int type);
extern void heap_heapify(heap_t *heap, int position);
extern void heap_move_down(heap_t *heap, node_t node, int position);
extern void heap_move_up(heap_t *heap, node_t node, int position);
extern void heap_swap(heap_t *heap, int first_pos, int second_pos);
extern void heap_stats(heap_t *heap);

#endif
