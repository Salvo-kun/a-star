#ifndef _QUEUE_PUBLIC
#define _QUEUE_PUBLIC

#include <stdio.h>

// Macro definitions
#define queue_empty_m(qp) (queue_count(qp) == 0)

// Type declarations
typedef struct queue queue_t;

// Function prototypes

/*
  Creates a FIFO queue.
  Returns NULL if an error occurs, the queue pointer otherwise.
*/
extern queue_t *queue_create();

/*
  Returns the size of the queue.
*/
extern int queue_count(queue_t *qp);

/*
  Insert a new element inside the queue.
  Returns 0 if an error occurs, 1 otherwise.
*/
extern int queue_put(queue_t *qp, void *data);

/*
  Extracts the head of the queue, returning its additional data through a pointer passed as parameter.
  Returns 0 if an error occurs, 1 otherwise.
*/
extern int queue_get(queue_t *qp, void **data_ptr);

/*
  Frees the allocated memory for the given queue, eventually freeing also the contained data (if freeData is not NULL).
  Returns 0 if an error occurs, 1 otherwise.
*/
extern int queue_destroy(queue_t *qp, void (*freeData)(void *));

/*
  Print queue stats to the given file. Internal data are printed if printData is not NULL.
*/
extern void queue_stats(FILE *fp, queue_t *qp, void (*printData)(FILE *, void *));

#endif
