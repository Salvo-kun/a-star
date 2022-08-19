#ifndef _STACK_PUBLIC
#define _STACK_PUBLIC

#include <stdio.h>

// Macro definitions
#define stack_empty_m(st) (stack_count(st) == 0)

// Type declarations
typedef struct stack stack_t;

// Function prototypes

/*
  Creates a LIFO stack.
  Returns NULL if an error occurs, the stack pointer otherwise.
*/
extern stack_t *stack_create();

/*
  Returns the size of the stack.
*/
extern int stack_count(stack_t *st);

/*
  Insert a new element inside the stack.
  Returns 0 if an error occurs, 1 otherwise.
*/
extern int stack_push(stack_t *st, void *data);

/*
  Extracts the head of the stack, returning its additional data through a pointer passed as parameter.
  Returns 0 if an error occurs, 1 otherwise.
*/
extern int stack_pop(stack_t *st, void **data_ptr);

/*
  Frees the allocated memory for the given stack, eventually freeing also the contained data (if freeData is not NULL).
  Returns 0 if an error occurs, 1 otherwise.
*/
extern int stack_destroy(stack_t *st, void (*freeData)(void *));

/*
  Print stack stats to the given file. Internal data are printed if printData is not NULL.
*/
extern void stack_stats(FILE *fp, stack_t *st, void (*printData)(FILE *, void *));

#endif
