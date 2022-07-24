#ifndef _MSG_QUEUE_PRIVATE
#define _MSG_QUEUE_PRIVATE

#include "../queue/queue.h"
#include "msg_queue.h"
#include "../utils/util.h"

// Data structures declaration

struct message_queue
{
    queue_t **queue_list;
    int list_size;
    pthread_mutex_t **mutex_list;
};

#endif
