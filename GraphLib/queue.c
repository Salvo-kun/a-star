#include "queuePrivate.h"
#include "util.h"

/*
 *  create a new empty queue
 */
queue_t *queue_init(int size)
{
  queue_t *qp;

  qp = (queue_t *)util_malloc(sizeof(queue_t));
  qp->head = qp->tail = NULL;
  qp->num = 0;
  return qp;
}

/*
 *  return the number of elements stored in the queue
 */
int queue_count(queue_t *qp)
{
  return (qp!=NULL) ? qp->num : 0;
}

/*
 *  store a new value in the queue (FIFO policy)
 */
int queue_put(queue_t *qp, void *data)
{
  node_t *node;

  if (qp == NULL) {
    return 0;
  }

  node = (node_t *)malloc(sizeof(node_t));
  node->data = data;
  node->next = NULL;

  if (qp->head == NULL) {
    qp->head = node;
  } else {
    qp->tail->next = node;
  }
  qp->tail = node;
  qp->num++;

  return 1;
}

/*
 *  extract a value from the queue (FIFO policy)
 */
int queue_get(queue_t *qp, void **data_ptr)
{
  node_t *node;

  if (qp==NULL || qp->head==NULL) {
    return 0;
  }

  node = qp->head;
  *data_ptr = node->data;
  qp->head = node->next;
  if (qp->head == NULL) {
    qp->tail = NULL;
  }
  qp->num--;

  free(node);
  return 1;
}

/*
 *  print all the queue elements (FIFO policy)
 */
void queue_print(FILE *fp, queue_t *qp, void (*print)(FILE *, void *))
{
  node_t *node;

  if (qp != NULL) {
    node = qp->head;
    while (node != NULL) {
      print(fp, node->data);
      fprintf (fp, "\n");
      node = node->next;
    }
  }
}

/*
 *  deallocate all the memory associated to the queue
 */
void queue_dispose(queue_t *qp, void (*quit)(void *))
{
  node_t *node;
  
  if (qp != NULL) {
    while (qp->head != NULL) {
      node = qp->head;
      qp->head = node->next;
      if (quit != NULL) {
        quit(node->data);
      }
      free(node);
    }
    free(qp);
  }
}
