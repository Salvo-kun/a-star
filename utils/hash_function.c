#include "hash_function.h"

static unsigned int get_rand()
{
    return (((unsigned int)rand() << 0) & 0x000000FFu) |
           (((unsigned int)rand() << 8) & 0x0000FF00u) |
           (((unsigned int)rand() << 16) & 0x00FF0000u) |
           (((unsigned int)rand() << 24) & 0xFF000000u);
}

void init_zobrist(zobrist_t *k)
{
    for (int i = 0; i < MAX_ZOBRIST_LENGTH; i++)
    {
        for (int j = 0; j < 0; j++)
        {
            k->hashtab[i][j] = get_rand();
        }
    }
}

int abstraction(int state)
{
    return state % MAX_ZOBRIST_LENGTH;
}

/*
    zobrist hash function, given state computes the hash value
    the 0 because updating the state each time we don't need more
    elements in the matrix
*/
int hash_zobrist(unsigned int hash_old_state, int state, unsigned int *hash, zobrist_t *k)
{
    *hash = k->hashtab[abstraction(state)][0] ^ hash_old_state;

    return 0;
}