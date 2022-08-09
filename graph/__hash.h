#ifndef _HASH_PRIVATE
#define _HASH_PRIVATE

#include <limits.h>
#include "../utils/util.h"
#include "hash.h"

// Data structures declaration
typedef struct module_hash_s module_hash_t;
typedef struct mult_hash_s mult_hash_t;

struct module_hash_s
{
    int m;
};

struct mult_hash_s
{
    int p;
    int a;
    int b;
};

// Function prototypes

extern hash_t *init_module_hash(int nThreads);
extern int hash_module(int state, hash_t *hash_data);
extern hash_t *init_mult_hash(int nThreads);
extern int hash_mult(int state, hash_t *hash_data);

#endif
