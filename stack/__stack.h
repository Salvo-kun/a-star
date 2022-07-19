#ifndef _STACK_PRIVATE
#define _STACK_PRIVATE

#include "stack.h"
#include "../utils/util.h"

// Data structures declaration
typedef struct node {
  void *data;
  struct node *next;
} node_t;

struct stack {
  node_t *head;
  int count;
};

#endif
