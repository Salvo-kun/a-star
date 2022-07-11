#include <stdio.h>
#include <stdlib.h>
#include "heapPublic.h"

int main(int argc, char **argv)
{
    heap_t *heap = heap_create(0);

    int id1 = 3;
    int id2 = 22;
    int id3 = 4;
    int id4 = 2;
    int id5 = 9;
    int id6 = 8;
    int id7 = 7;
    int *data;

    heap_insert(heap, (void *)&id1, 2);
    heap_insert(heap, (void *)&id2, 8);
    heap_insert(heap, (void *)&id3, 1);
    heap_insert(heap, (void *)&id4, 5);
    heap_insert(heap, (void *)&id5, 7);
    heap_insert(heap, (void *)&id6, 88);
    heap_insert(heap, (void *)&id7, 3);

    heap_extract(heap, (void **) &data);
    heap_extract(heap, (void **) &data);
    heap_extract(heap, (void **) &data);
    heap_extract(heap, (void **) &data);
    

    heap_destroy(heap, NULL);
}