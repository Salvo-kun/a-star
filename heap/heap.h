#ifndef _HEAP_PUBLIC
#define _HEAP_PUBLIC

#include <stdio.h>

// Macro definitions
#define heap_is_empty(heap) (heap_count(heap) == 0)

// Type declarations
typedef struct heap heap_t;

// Function prototypes

/*
  Creates an heap, of a given type (0 if MIN_HEAP, otherwise MAX_HEAP) with a given initial size.
  Returns NULL if an error occurs, the heap pointer otherwise.
*/
extern heap_t *heap_create(int type, int size);

/*
  Insert a new element inside the heap with the given priority.
  Returns 0 if an error occurs, 1 otherwise.
*/
extern int heap_insert(heap_t *heap, int key, void *data, int priority);

/*
  Finds the position of a node inside the heap given its key. If no position is found, it is set to NULL.
  Returns 0 if an error occurs, 1 otherwise.
*/
extern int heap_find(heap_t *heap, int key, int **position);

/*
  Returns the size of the heap.
*/
extern int heap_count(heap_t *heap);

/*
  Extracts the root of the heap, returning its key and additional data through pointers passed as parameters.
  Returns 0 if an error occurs, 1 otherwise.
*/
extern int heap_extract(heap_t *heap, void **data, int *key);

/*
  Updates an entry of the heap, given its key.
  Returns 0 if an error occurs, 1 otherwise.
*/
extern int heap_update(heap_t *heap, int key, int newPriority);

/*
  Frees the allocated memory for the given heap, eventually freeing also the contained data (if freeData is not NULL).
  Returns 0 if an error occurs, 1 otherwise.
*/
extern int heap_destroy(heap_t *heap, void (*freeData)(void *));

/*
  Print heap stats to the given file. Internal data are printed if printData is not NULL.
*/
extern void heap_stats(FILE *fp, heap_t *heap, void (*printData)(FILE *, void *));

#endif
