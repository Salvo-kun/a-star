#ifndef _HASH_TABLE_PRIVATE
#define _HASH_TABLE_PRIVATE

#include "hashTablePublic.h"
#include "../utils/util.h"

// Macro definitions
#define LOAD_THRESHOLD 0.75

// Data structures declaration
typedef struct node *link_t;

typedef struct node {
  void *data;
  int key;
  link_t next;
} node_t;

struct hash_table {
  link_t *nodes;
  int count;
  int capacity;
};

// Private Methods
extern link_t node_create(int key, void *data, link_t next);

#endif
