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

typedef struct zobrist_s
{
    unsigned int hashtab[MAX_ZOBRIST_LENGTH][STATE_N];
} zobrist_t;

typedef struct mult_hash_s
{
    int p;
    int a;
    int b;
    int m;
} mult_hash_t;

void init_zobrist(zobrist_t *k);
void init_mult_hash(mult_hash_t *m, int modul);

/**
 * old state XORed with the value received accessing the matrix with state
 */
int hash_zobrist(unsigned int hash_old_state, int state, unsigned int *hash, zobrist_t *k);

int hash_mult(int state, mult_hash_t *m);

#endif