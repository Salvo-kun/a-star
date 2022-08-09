#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <limits.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#define STATE_N 1

enum
{
    MAX_ZOBRIST_LENGTH = 256
};

typedef struct hash_s
{
    int p;
    int a;
    int b;
    unsigned int hashtab[MAX_ZOBRIST_LENGTH][STATE_N];

} hash_t;

void init_hash(hash_t *m);

/**
 * old state XORed with the value received accessing the matrix with state
 */
int hash_zobrist(unsigned int hash_old_state, int state, unsigned int *hash, hash_t *k);

int hash_mult(unsigned int hash_old_state, int state, unsigned int *hash, hash_t *m);

#endif