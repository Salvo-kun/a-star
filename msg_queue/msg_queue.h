#ifndef _MSG_QUEUE_PUBLIC
#define _MSG_QUEUE_PUBLIC

#include <stdio.h>
#include <pthread.h>

// Type declaration
typedef struct message_queue message_queue_t;

// Function prototypes

/*
  Creates list_size different queues.
  Returns NULL if an error occurs, the qmessage queue pointer otherwise.
*/
extern message_queue_t *message_queue_create(int list_size);

/*
  Returns the number of queues inside the message queue.
*/
extern int message_queue_size(message_queue_t *msg_q);

/*
  Returns the number of the message in the queue identified by id.
*/
extern int message_queue_count(message_queue_t *msg_q, int id);

/*
  Check if all queues are empty
  return 1 if all empty, 0 otherwise
*/
extern int message_queue_all_empty(message_queue_t *msg_q);

/*
  Insert a new element inside the queue selected with the id.
  Returns 0 if an error occurs, 1 otherwise.
*/
extern int message_queue_send(message_queue_t *msg_q, void *data, int id);

/*
  Extracts the head of the queue selected with the id, returning its additional data through a pointer passed as parameter.
  Returns 0 if an error occurs, 1 otherwise.
*/
extern int message_queue_receive(message_queue_t *msg_q, void **data_ptr, int id);

/*
  Frees the allocated memory for all the queues, eventually freeing also the contained data (if freeData is not NULL).
  Returns 0 if an error occurs, 1 otherwise.
*/
extern int message_queue_destroy(message_queue_t *msg_q, void (*freeData)(void *));

/*
  Print message queue stats to the given file. Internal data are printed if printData is not NULL.
*/
extern void message_queue_stats(FILE *fp, message_queue_t *msg_q, void (*printData)(FILE *, void *));

/*
 */

#endif