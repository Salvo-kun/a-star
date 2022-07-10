#ifndef _QUEUE_PUBLIC
#define _QUEUE_PUBLIC

#include <stdio.h>

/* macro definition */
#define queue_empty_m(qp) (queue_count(qp) == 0)

/* type declarations */
typedef struct queue queue_t;

/* extern function prototypes */
extern queue_t *queue_init(int size);
extern int queue_count(queue_t *sp);
extern int queue_put(queue_t *sp, void *data);
extern int queue_get(queue_t *sp, void **data_ptr);
extern void queue_print(FILE *fp, queue_t *sp, void (*print)(FILE *, void *));
extern void queue_dispose(queue_t *sp, void (*quit)(void *));

#endif
