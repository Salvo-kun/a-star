#ifndef _HASH
#define _HASH

// Data structures declaration
typedef struct hash_s hash_t;

enum Hash
{
  MODULE,
  MULTIPLICATIVE
};

struct hash_s 
{
    void *data;
    int (*hash_func)(int, hash_t *);
};

// Function prototypes

/*
    This function returns the hash structure related to a specific hash type.
    If the specified hash type does not exist, a NULL pointer is returned.
*/
extern hash_t *choose_hash(enum Hash h, int nThreads);
extern int hash_destroy(hash_t *hash_data);

#endif
