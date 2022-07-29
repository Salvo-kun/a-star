#ifndef _UTIL
#define _UTIL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* macro definition */
#define util_check_m(expr, msg)          \
  if (!(expr))                           \
  {                                      \
    fprintf(stderr, "Error: %s\n", msg); \
  }

#define util_check_r(expr, msg, retVal)    \
  if (!(expr))                             \
  {                                        \
    if (msg != NULL)                       \
    {                                      \
      fprintf(stderr, "Error: %s\n", msg); \
    }                                      \
    return retVal;                         \
  }

#define util_check_no_r(expr, msg)         \
  if (!(expr))                             \
  {                                        \
    if (msg != NULL)                       \
    {                                      \
      fprintf(stderr, "Error: %s\n", msg); \
    }                                      \
    return;                                \
  }

#define NS_TO_S(x) ((double)(x) / 1000 / 1000 / 1000)

/* extern function prototypes */
extern FILE *util_fopen(char *name, char *mode);
extern void *util_malloc(unsigned int size);
extern void *util_calloc(unsigned int num, unsigned int size);
extern void *util_realloc(void *ptr, unsigned int size);
extern void util_free(void *ptr);
extern char *util_strdup(char *src);
extern void util_array_dispose(void **ptr, unsigned int n, void (*quit)(void *));
extern void **util_matrix_alloc(unsigned int n, unsigned int m, unsigned int size);
extern void util_matrix_dispose(void ***ptr, unsigned int n, unsigned int m, void (*quit)(void *));
extern uint64_t nano_count();
#endif
