#include "__queue.h"

// Public methods

queue_t *queue_create()
{
  queue_t *qp;

  qp = (queue_t *)util_malloc(sizeof(queue_t));

  // Could not allocate, return null
  util_check_r(qp != NULL, "Queue cannot be null, returning...\n", NULL);

  qp->head = qp->tail = NULL;
  qp->count = 0;

  return qp;
}

int queue_count(queue_t *qp)
{
  // Check queue is not null before starting
  util_check_r(qp != NULL, "Queue cannot be null, returning...\n", 0);

  return qp->count;
}

int queue_put(queue_t *qp, void *data)
{
  node_t *node;

  // Check queue is not null before starting
  util_check_r(qp != NULL, "Queue cannot be null, returning...\n", 0);

  node = (node_t *)malloc(sizeof(node_t));

  // Check node is not null
  util_check_r(qp != NULL, "Node cannot be null, returning...\n", 0);

  node->data = data;
  node->next = NULL;

  if (qp->head == NULL)
  {
    qp->head = node;
  }
  else
  {
    qp->tail->next = node;
  }

  qp->tail = node;
  qp->count++;

  return 1;
}

int queue_get(queue_t *qp, void **data_ptr)
{
  node_t *node;

  // Check queue and queue head are not null before starting
  util_check_r(qp != NULL, "Queue cannot be null, returning...\n", 0);
  util_check_r(qp->head != NULL, "Queue head cannot be null, returning...\n", 0);

  node = qp->head;
  *data_ptr = node->data;
  qp->head = node->next;

  if (qp->head == NULL)
  {
    qp->tail = NULL;
  }

  qp->count--;

  free(node);

  return 1;
}

int queue_destroy(queue_t *qp, void (*freeData)(void *))
{
  node_t *node;

  // Check queue and internal array are not null before starting
  util_check_r(qp != NULL, "Queue is already null, returning...\n", 0);

  while (qp->head != NULL)
  {
    node = qp->head;
    qp->head = node->next;

    if (freeData != NULL)
    {
      freeData(node->data);
    }

    free(node);
  }

  free(qp);

  return 1;
}

void queue_stats(FILE *fp, queue_t *queue, void (*printData)(FILE *, void *))
{
  util_check_no_r(queue != NULL, "queue cannot be null, returning...\n");

  fprintf(fp, "############## Queue Stats ##############\n\n");
  fprintf(fp, "Type = FIFO\n");
  fprintf(fp, "Count = %d\n", queue->count);

  node_t *node;
  int i;

  for (node = queue->head, i = 0; i < queue->count && node != NULL; ++i, node = node->next)
  {
    fprintf(fp, "Element %d)\n", i + 1);

    if (printData != NULL)
    {
      fprintf(fp, "Data: \n");
      printData(fp, node->data);
    }
    
    fprintf(fp, "\n");
  }

  fprintf(fp, "\n########################################\n\n");
}