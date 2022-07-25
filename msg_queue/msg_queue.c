#include "__msg_queue.h"

// Public methods

message_queue_t *message_queue_create(int list_size)
{
    int i, res;
    message_queue_t *msg_q = (message_queue_t *)util_malloc(sizeof(message_queue_t));

    msg_q->list_size = list_size;

    util_check_r(msg_q != NULL, "message_queue_t cannot be null, returning...\n", NULL);

    msg_q->queue_list = util_malloc(list_size * sizeof(queue_t *));
    msg_q->mutex_list = util_malloc(list_size * sizeof(pthread_mutex_t *));

    util_check_r(msg_q->queue_list != NULL, "message_queue_t queue list cannot be null, returning...\n", NULL);

    util_check_r(msg_q->mutex_list != NULL, "message_queue_t mutex list cannot be null, returning...\n", NULL);

    for (i = 0; i < list_size; i++)
    {
        msg_q->queue_list[i] = queue_create();

        msg_q->mutex_list[i] = (pthread_mutex_t *)util_malloc(sizeof(pthread_mutex_t));

        util_check_r(msg_q->mutex_list[i] != NULL, "message_queue_t mutex element cannot be null, returning...\n", NULL);

        // initialization of the mutex
        res = pthread_mutex_init(msg_q->mutex_list[i], NULL);

        util_check_r(res == 0, "problems initializing the mutex, returning...\n", NULL);
    }

    return msg_q;
}

int message_queue_size(message_queue_t *msg_q)
{
    // Check message_queue_t is not null before starting
    util_check_r(msg_q != NULL, "message_queue_t cannot be null, returning...\n", 0);

    return msg_q->list_size;
}

int message_queue_count(message_queue_t *msg_q, int id)
{
    // Check message_queue_t is not null before starting
    util_check_r(msg_q != NULL, "message_queue_t cannot be null, returning...\n", 0);

    return queue_count(msg_q->queue_list[id]);
}

int message_queue_send(message_queue_t *msg_q, void *data, int id)
{
    // Check message_queue_t is not null before starting
    util_check_r(msg_q != NULL, "message_queue_t cannot be null, returning...\n", 0);

    pthread_mutex_lock(msg_q->mutex_list[id]);
    queue_put(msg_q->queue_list[id], data);
    pthread_mutex_unlock(msg_q->mutex_list[id]);

    return 1;
}

int message_queue_receive(message_queue_t *msg_q, void **data_ptr, int id)
{
    util_check_r(msg_q != NULL, "message_queue_t cannot be null, returning...\n", 0);

    pthread_mutex_lock(msg_q->mutex_list[id]);
    queue_get(msg_q->queue_list[id], data_ptr);
    pthread_mutex_unlock(msg_q->mutex_list[id]);

    return 1;
}

int message_queue_all_empty(message_queue_t *msg_q)
{
    // Check message_queue_t is not null before starting
    util_check_r(msg_q != NULL, "message_queue_t cannot be null, returning...\n", 0);

    int i;

    for (i = 0; i < msg_q->list_size; i++)
    {
        if (queue_count(msg_q->queue_list[i]) != 0)
            return 0;
    }

    return 1;
}

int message_queue_destroy(message_queue_t *msg_q, void (*freeData)(void *))
{
    int i;
    util_check_r(msg_q != NULL, "message_queue_t is already null, returning...\n", 0);

    for (i = 0; i < msg_q->list_size; i++)
    {
        if (msg_q->queue_list[i] != NULL)
            queue_destroy(msg_q->queue_list[i], freeData);

        if (msg_q->mutex_list[i] != NULL)
            pthread_mutex_destroy(msg_q->mutex_list[i]);
    }

    free(msg_q->mutex_list);
    free(msg_q->queue_list);

    free(msg_q);

    return 1;
}

void message_queue_stats(FILE *fp, message_queue_t *msg_q, void (*printData)(FILE *, void *))
{
    util_check_no_r(msg_q != NULL, "message_queue_t cannot be null, returning...\n");

    fprintf(fp, "############## message_queue_t Stats ##############\n\n");
    fprintf(fp, "Type = List of FIFO queues\n");
    fprintf(fp, "Count = %d\n", msg_q->list_size);

    int i;

    for (i = 0; i < msg_q->list_size; i++)
        queue_stats(fp, msg_q->queue_list[i], printData);

    fprintf(fp, "\n########################################\n\n");
}