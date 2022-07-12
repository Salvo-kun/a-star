#include <stdio.h>
#include <stdlib.h>
#include "../queuePublic.h"

int main(int argc, char **argv)
{
    queue_t *queue = queue_create();

    int id1 = 3;
    int id2 = 22;
    int id3 = 4;
    int id4 = 2;
    int *data;

    queue_put(queue, (void *)&id1);
    queue_put(queue, (void *)&id2);
    queue_put(queue, (void *)&id3);
    queue_put(queue, (void *)&id4);

    queue_stats(stdout, queue, NULL);
    
    queue_get(queue, (void **)&data);
    queue_get(queue, (void **)&data);
    queue_get(queue, (void **)&data);
    queue_get(queue, (void **)&data);
    
    queue_stats(stdout, queue, NULL);

    queue_destroy(queue, NULL);
}