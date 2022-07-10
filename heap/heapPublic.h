#ifndef _HEAP_PUBLIC
#define _HEAP_PUBLIC

#include <stdio.h>

/* Macro definitions */
#define LEFT(i) (2*i+1)
#define RIGHT(i) (2*i+2)
#define PARENT(i) ((i-1)/2)

/* Type declarations */
typedef struct heap heap_t;

/* Interfaces to methods */
extern heap_t *heap_create(int type);
extern int heap_insert(heap_t *heap, void *data, int priority);
extern int heap_extract(heap_t *heap, void **data);
extern int heap_update(heap_t *heap, int position, int newPriority, void (*freeData)(void *));
extern void heap_destroy(heap_t *heap, void (*freeData)(void *));

#endif
