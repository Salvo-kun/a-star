#include "hash_function.h"

static unsigned int get_rand()
{
    return (((unsigned int)rand() << 0) & 0x000000FFu) |
           (((unsigned int)rand() << 8) & 0x0000FF00u) |
           (((unsigned int)rand() << 16) & 0x00FF0000u) |
           (((unsigned int)rand() << 24) & 0xFF000000u);
}

void init_hash(hash_t *m)
{
    m->p = INT_MAX;
    // random number (1, p-1)
    m->a = (rand() % (m->p - 2)) + 1;
    // random number (0, p-1)
    m->b = rand() % (m->p - 1);

    for (int i = 0; i < MAX_ZOBRIST_LENGTH; i++)
    {
        for (int j = 0; j < 0; j++)
        {
            m->hashtab[i][j] = get_rand();
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
int hash_zobrist(unsigned int hash_old_state, int state, unsigned int *hash, hash_t *k)
{
    *hash = k->hashtab[abstraction(state)][0] ^ hash_old_state;

    return *hash;
}
/*
modulus hash function
    h(c) = ((ac + b) mod p) and mod m will be done in the other function
*/
int hash_mult(unsigned int hash_old_state, int state, unsigned int *hash, hash_t *m)
{
    unsigned int ac = m->a * state;
    ac += m->b;
    // fprintf(stdout, "ac + b = %u\n", ac);
    return (ac % m->p);
}