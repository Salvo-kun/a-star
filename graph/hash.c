#include "__hash.h"

// Public Methods
hash_t *choose_hash(enum Hash h, int nThreads)
{
    switch (h)
    {
        case MODULE:
            return init_module_hash(nThreads);
            break;

        case MULTIPLICATIVE:
            return init_mult_hash(nThreads);
            break;

        default:
            return NULL;
            break;
    }
}

// Private Methods

hash_t *init_module_hash(int nThreads)
{
    hash_t *hash_data = (hash_t *)util_malloc(sizeof(hash_t));
    util_check_r(hash_data != NULL, "Could not allocate data for hash\n", NULL);

    module_hash_t *module_hash_data = (module_hash_t *)util_malloc(sizeof(module_hash_t));
    util_check_r(module_hash_data != NULL, "Could not allocate data for module hash\n", NULL);

    module_hash_data->m = nThreads;

    hash_data->data = (void *)module_hash_data;
    hash_data->hash_func = &hash_module;

    return hash_data;
}

/*
modulus hash function
    h(c) = ((ac + b) mod p) and mod m will be done in the other function
*/
int hash_module(int state, hash_t *hash_data)
{
    module_hash_t *module_hash_data = ((module_hash_t *)hash_data->data);

    return (state % module_hash_data->m);
}

hash_t *init_mult_hash(int nThreads)
{
    hash_t *hash_data = (hash_t *)util_malloc(sizeof(hash_t));
    util_check_r(hash_data != NULL, "Could not allocate data for hash\n", NULL);

    mult_hash_t *mult_hash_data = (mult_hash_t *)util_malloc(sizeof(mult_hash_t));
    util_check_r(mult_hash_data != NULL, "Could not allocate data for multiplicative hash\n", NULL);

    mult_hash_data->p = INT_MAX;
    // random number (1, p-1)
    mult_hash_data->a = (rand() % (mult_hash_data->p - 2)) + 1;
    // random number (0, p-1)
    mult_hash_data->b = rand() % (mult_hash_data->p - 1);

    hash_data->data = (void *)mult_hash_data;
    hash_data->hash_func = &hash_mult;

    return hash_data;
}

/*
modulus hash function
    h(c) = ((ac + b) mod p) and mod m will be done in the other function
*/
int hash_mult(int state, hash_t *hash_data)
{
    mult_hash_t *mult_hash_data = ((mult_hash_t *)hash_data->data);
    unsigned int ac = mult_hash_data->a * state;
    ac += mult_hash_data->b;

    return (ac % mult_hash_data->p);
}

int hash_destroy(hash_t *hash_data)
{
    util_check_r(hash_data != NULL, "Hash already freed\n", 0);
    
    if (hash_data->data != NULL)
    {
        free(hash_data->data);
    }

    free(hash_data);

    return 1;
}