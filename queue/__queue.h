#ifndef _QUEUE_PRIVATE
#define _QUEUE_PRIVATE

#include "queue.h"
#include "../utils/util.h"

// Data structures declaration
typedef struct node {
  void *data;
  struct node *next;
} node_t;

struct queue {
  node_t *head;
  node_t *tail;
  int count;
};

#endif
