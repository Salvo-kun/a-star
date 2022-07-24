#include <pthread.h>
#include "pathfinding.h"
#include "../msg_queue/msg_queue.h"

#define KEY 69
#define MAX_MSG_LEN 512
/*
    Termination: The termination must prove that there is no better solution.
    Broadcast message to all other threads when a new solution is discovered, use Mattern's time algorithm (uses 2 waves to check that the solution received by all the threads are the same in the 2 wave, in this case the termination condition holds).
    implementation flags/semaphore or counting the number of solutions arriving

    Buffer (message): hash (load balancing and duplicate detection), assign a unique owner to each search nodeaccording to a hash function, duplicate detection is done by the owner. Implementation with MessageQueue, getting messages by ID so can be well balanced between threads

    local OPEN/CLOSE set:

*/

struct thread_d
{
    graph_t *graph;
    vertex_t *src;
    vertex_t *dst;
    int (*heuristic)(vertex_t *, vertex_t *);
    message_queue_t *msg_q;
    int send;
};

struct msg_buffer
{
    long mesg_type;
    char mesg_text[MAX_MSG_LEN];
};

void *thread_function(void *args);

int par_a_star_path(graph_t *graph, vertex_t *src, vertex_t *dst, int (*heuristic)(vertex_t *, vertex_t *), path_t **path, int n_threads_to_use)
{
    printf("ASTAR PATH PAR");

    // Check parameters are not null before starting
    util_check_r(graph != NULL, "Graph cannot be null, returning...\n", 0);
    util_check_r(src != NULL, "Source nodes cannot be null, returning...\n", 0);
    util_check_r(dst != NULL, "Destination nodes cannot be null, returning...\n", 0);

    message_queue_t *msg_q = message_queue_create(2);

    struct thread_d arg1;
    arg1.send = 1;
    arg1.msg_q = msg_q;
    struct thread_d arg2;
    arg2.send = 0;
    arg2.msg_q = msg_q;

    pthread_t thread1, thread2;

    printf("thread creation");

    pthread_create(&thread1, NULL, thread_function, (void *)&arg1);
    pthread_create(&thread2, NULL, thread_function, (void *)&arg2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    message_queue_destroy(msg_q, NULL);
    return 0;
}

void *thread_function(void *args)
{
    struct thread_d *thread_data = (struct thread_d *)args;

    int *data = malloc(sizeof(int));
    if (thread_data->send == 1)
    {
        // sending
        *data = 7;
        sleep(1);
        printf(" message send ");
        message_queue_send(thread_data->msg_q, (void *)data, 1);
    }
    else
    {
        message_queue_receive(thread_data->msg_q, (void **)&data, 1);
        printf("message received ");
        printf("%d", *data);
    }

    pthread_exit(NULL);
}