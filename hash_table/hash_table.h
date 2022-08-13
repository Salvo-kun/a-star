#ifndef _HASH_TABLE_PUBLIC
#define _HASH_TABLE_PUBLIC

#include <stdio.h>
#include <math.h>

// Macro definitions
#define A (sqrt(5) - 1)/2
#define P 8191
#define hash(key, M)  (((int)(key * A)) % P) % M

// Type declarations
typedef struct hash_table hash_table_t;

// Function prototypes

/*
  Creates an hash_table with a given initial size.
  Returns NULL if an error occurs, the hash_table pointer otherwise.
*/
extern hash_table_t *hash_table_create(int size);

/*
  Look for an element with the given key inside the hash table, returning content inside data pointer (NULL if not found).
  Returns 0 if an error occurs, 1 otherwise.
*/
extern int hash_table_get(hash_table_t *hash_table, int key, void **data);

/*
  Insert a new element inside the hash_table with the given key.
  Returns 0 if an error occurs, 1 otherwise.
*/
extern int hash_table_insert(hash_table_t *hash_table, int key, void *data);

/*
  Updates the element inside the hash_table with the given key, eventually freeing also the old data (if freeData is not NULL).
  Returns 0 if an error occurs, 1 otherwise.
*/
extern int hash_table_update(hash_table_t *hash_table, int key, void *data, void (*freeData)(void *));

/*
  Removes an element from the hash_table, given its key, eventually freeing also the contained data (if freeData is not NULL).
  Returns 0 if an error occurs, 1 otherwise.
*/
extern int hash_table_delete(hash_table_t *hash_table, int key, void (*freeData)(void *));

/*
  Frees the allocated memory for the given hash_table, eventually freeing also the contained data (if freeData is not NULL).
  Returns 0 if an error occurs, 1 otherwise.
*/
extern int hash_table_destroy(hash_table_t *hash_table, void (*freeData)(void *));

/*
  Print hash_table stats to the given file. Internal data are printed if printData is not NULL.
*/
extern void hash_table_stats(FILE *fp, hash_table_t *hash_table, void (*printData)(FILE *, void *));

#endif
