#ifndef _HEAP_PUBLIC
#define _HEAP_PUBLIC

#include <stdio.h>

// Macro definitions
#define LEFT(i) (2*i+1)
#define RIGHT(i) (2*i+2)
#define PARENT(i) ((i-1)/2)

// Type declarations
typedef struct heap heap_t;

// Function prototypes

/*
  Creates and return an heap, of a given type (0 if MIN_HEAP, otherwise MAX_HEAP)
*/
extern heap_t *heap_create(int type);

/*
  Insert a new element inside the heap with the given priority
*/
extern void heap_insert(heap_t *heap, int key, void *data, int priority);

/*
  Returns the position of a node inside the heap given its key. If no position is found, returns -1
*/
extern int heap_find(heap_t *heap, int key);

/*
  Extracts the root of the heap, returning its key. Additional data are returned through a pointer passed as parameter.
*/
extern int heap_extract(heap_t *heap, void **data);

/*
  Updates an entry of the heap, given its position
*/
extern void heap_update(heap_t *heap, int position, int newPriority);

/*
  Frees the allocated memory for the given heap, eventually freeing also the contained data (if freeData is not NULL)
*/
extern void heap_destroy(heap_t *heap, void (*freeData)(void *));

#endif
