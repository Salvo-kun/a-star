#include <stdio.h>

#include "../hash_function.h"

int main(int argc, char const *argv[])
{
    mult_hash_t m;
    init_mult_hash(&m, 4);
    printf("a = %d  b = %d  m = %d  p = %d\n", m.a, m.b, m.m, m.p);
    int result = hash_mult(1941814144, &m);
    printf("result = %d\n", result);

    return 0;
}
