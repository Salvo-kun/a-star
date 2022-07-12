#ifndef _HEAP_PRIVATE
#define _HEAP_PRIVATE

#include "heapPublic.h"
#include "../utils/util.h"

// Macro definitions
#define LEFT(i) (2*i+1)
#define RIGHT(i) (2*i+2)
#define PARENT(i) ((i-1)/2)
#define FULL_FACTOR 0.75
#define EMPTY_FACTOR 0.25

// Data structures declaration
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

// Private methods
extern int heap_compare(node_t first_node, node_t second_node, int type);
extern void heap_heapify(heap_t *heap, int position);
extern void heap_move_down(heap_t *heap, node_t node, int position);
extern void heap_move_up(heap_t *heap, node_t node, int position);
extern void heap_swap(heap_t *heap, int first_pos, int second_pos);

#endif
