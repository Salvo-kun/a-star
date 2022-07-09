#include "util.h"

/*
 *  fopen (with check) utility function
 */
FILE *util_fopen(char *name, char *mode)
{
  FILE *fp=fopen(name, mode);
  util_check_m(fp!=NULL, "could not open file!");
  return fp;
}

/*
 *  malloc (with check) utility function
 */
void *util_malloc(unsigned int size)
{
  void *ptr=malloc(size);
  util_check_m(ptr!=NULL, "memory allocation failed!");
  return ptr;
}

/*
 *  calloc (with check) utility function
 */
void *util_calloc(unsigned int num, unsigned int size)
{
  void *ptr=calloc(num, size);
  util_check_m(ptr!=NULL, "memory allocation failed!");
  return ptr;
}

/*
 *  strdup (with check) utility function
 */
char *util_strdup(char *src)
{
  char *dst=strdup(src);
  util_check_m(dst!=NULL, "memory allocation failed");
  return dst;
}

/*
 *  array de-allocation utility function
 */
void util_array_dispose(void **ptr, unsigned int n, void (*quit)(void *))
{
  int i;

  if (quit != NULL) {
    for (i=0; i<n; i++) {
      quit(ptr[i]);
    }
  }
  free(ptr);
}

/*
 *  matrix allocation utility function
 */
void **util_matrix_alloc(unsigned int n, unsigned int m, unsigned int size)
{
  void **ptr;
  int i;

  ptr = (void **)util_malloc(n*sizeof(void *));
  for (i=0; i<n; i++) {
    ptr[i] = util_calloc(m, size);
  }
  return ptr;
}

/*
 *  matrix de-allocation utility function
 */
void util_matrix_dispose(void ***ptr, unsigned int n, unsigned int m, 
                         void (*quit)(void *))
{
  int i, j;

  for (i=0; i<n; i++) {
    if (quit != NULL) {
      for (j=0; j<m; j++) {
        quit(ptr[i][j]);
      }
    }
    free(ptr[i]);
  }
  free(ptr);
}
