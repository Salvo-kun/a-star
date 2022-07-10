#ifndef _QUEUE_PRIVATE
#define _QUEUE_PRIVATE

#include "util.h"
#include "queuePublic.h"

/* structure declarations */
typedef struct node {
  void *data;
  struct node *next;
} node_t;

struct queue {
  node_t *head;
  node_t *tail;
  int num;
};

#endif
