#include <stdio.h>
#include <stdlib.h>
#include "../stack.h"

void printData(FILE *f, void *d);

int main(int argc, char **argv)
{
    stack_t *stack = stack_create();

    int id1 = 3;
    int id2 = 22;
    int id3 = 4;
    int id4 = 2;
    int *data = (int *)util_malloc(sizeof(int));

    stack_push(stack, (void *)&id1);
    stack_push(stack, (void *)&id2);
    stack_push(stack, (void *)&id3);
    stack_push(stack, (void *)&id4);

    stack_stats(stdout, stack, printData);

    stack_pop(stack, (void **)&data);
    stack_pop(stack, (void **)&data);
    stack_pop(stack, (void **)&data);
    stack_pop(stack, (void **)&data);

    printf("%p %d\n%p %d\n\n", data, *data, &id1, id1);

    stack_stats(stdout, stack, printData);

    stack_destroy(stack, NULL);
}

void printData(FILE *f, void *d)
{
    fprintf(f, "%d", *((int *)d));
}