#include <stdio.h>
#include <stdlib.h>
#include "../heapPublic.h"

int main(int argc, char **argv)
{
    heap_t *heap = heap_create(0, 100);

    int id1 = 3;
    int id2 = 22;
    int id3 = 4;
    int id4 = 2;
    int id5 = 9;
    int id6 = 8;
    int id7 = 7;
    int *data = (int *)util_malloc(sizeof(int));
    int key;
    int *pos = (int *)util_malloc(sizeof(int));

    heap_insert(heap, 1, (void *)&id1, 2);
    heap_insert(heap, 2, (void *)&id2, 8);
    heap_insert(heap, 3, (void *)&id3, 1);
    heap_insert(heap, 4, (void *)&id4, 5);
    heap_insert(heap, 5, (void *)&id5, 7);
    heap_insert(heap, 6, (void *)&id6, 88);
    heap_insert(heap, 7, (void *)&id7, 3);

    heap_stats(stdout, heap, NULL);

    heap_extract(heap, (void **)&data, &key);
    heap_extract(heap, (void **)&data, &key);
    heap_extract(heap, (void **)&data, &key);
    heap_extract(heap, (void **)&data, &key);

    heap_stats(stdout, heap, NULL);

    heap_find(heap, 6, &pos);
    heap_update(heap, 6, 0);

    heap_stats(stdout, heap, NULL);

    heap_destroy(heap, NULL);
}