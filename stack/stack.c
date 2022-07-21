#include "__stack.h"

// Public methods

stack_t *stack_create()
{
  stack_t *st;

  st = (stack_t *)util_malloc(sizeof(stack_t));

  // Could not allocate, return null
  util_check_r(st != NULL, "Stack cannot be null, returning...\n", NULL);

  st->head = NULL;
  st->count = 0;

  return st;
}

int stack_count(stack_t *st)
{
  // Check stack is not null before starting
  util_check_r(st != NULL, "Stack cannot be null, returning...\n", 0);

  return st->count;
}

int stack_push(stack_t *st, void *data)
{
  node_t *node;

  // Check stack is not null before starting
  util_check_r(st != NULL, "Stack cannot be null, returning...\n", 0);

  node = (node_t *)malloc(sizeof(node_t));

  // Check node is not null
  util_check_r(st != NULL, "Node cannot be null, returning...\n", 0);

  node->data = data;
  node->next = st->head;
  st->head = node;
  st->count++;

  return 1;
}

int stack_pop(stack_t *st, void **data_ptr)
{
  node_t *node;

  // Check stack and stack head are not null before starting
  util_check_r(st != NULL, "Stack cannot be null, returning...\n", 0);
  util_check_r(st->head != NULL, "Stack head cannot be null, returning...\n", 0);
  util_check_r(data_ptr != NULL, "Data pointer cannot be null, returning...\n", 0);

  node = st->head;
  *data_ptr = node->data;
  st->head = node->next;

  st->count--;

  free(node);

  return 1;
}

int stack_destroy(stack_t *st, void (*freeData)(void *))
{
  node_t *node;

  // Check stack and internal array are not null before starting
  util_check_r(st != NULL, "Stack is already null, returning...\n", 0);

  while (st->head != NULL)
  {
    node = st->head;
    st->head = node->next;

    if (freeData != NULL)
    {
      freeData(node->data);
    }

    free(node);
  }

  free(st);

  return 1;
}

void stack_stats(FILE *fp, stack_t *stack, void (*printData)(FILE *, void *))
{
  util_check_no_r(stack != NULL, "stack cannot be null, returning...\n");

  fprintf(fp, "############## Stack Stats ##############\n\n");
  fprintf(fp, "Type = LIFO\n");
  fprintf(fp, "Count = %d\n", stack->count);

  node_t *node;
  int i;

  for (node = stack->head, i = 0; i < stack->count && node != NULL; ++i, node = node->next)
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