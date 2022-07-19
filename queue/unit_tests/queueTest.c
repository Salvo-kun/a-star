#include <stdio.h>
#include <stdlib.h>
#include "../queue.h"

void printData(FILE *f, void *d);

int main(int argc, char **argv)
{
    queue_t *queue = queue_create();

    int id1 = 3;
    int id2 = 22;
    int id3 = 4;
    int id4 = 2;
    int *data = (int *)util_malloc(sizeof(int));

    queue_put(queue, (void *)&id1);
    queue_put(queue, (void *)&id2);
    queue_put(queue, (void *)&id3);
    queue_put(queue, (void *)&id4);

    queue_stats(stdout, queue, printData);
    
    queue_get(queue, (void **)&data);
    queue_get(queue, (void **)&data);
    queue_get(queue, (void **)&data);
    queue_get(queue, (void **)&data);

    printf("%p %d\n%p %d\n\n", data, *data, &id4, id4);
    
    queue_stats(stdout, queue, printData);

    queue_destroy(queue, NULL);
}

void printData(FILE *f, void *d)
{
    fprintf(f, "%d", *((int *)d));
}