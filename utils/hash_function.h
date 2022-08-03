#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <limits.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#define STATE_N 1

enum
{
    MAX_ZOBRIST_LENGTH = 256
};

typedef struct zobrist_s
{
    unsigned int hashtab[MAX_ZOBRIST_LENGTH][STATE_N];
} zobrist_t;

void init_zobrist(zobrist_t *k);

/**
 * old state XORed with the value received accessing the matrix with state
 */
int hash_zobrist(unsigned int hash_old_state, int state, unsigned int *hash, zobrist_t *k);

#endif