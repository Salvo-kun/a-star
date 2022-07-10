#include "heapPrivate.h"
#include "../utils/util.h"

/*
  Creates and return an heap, of a given type (0 if MIN_HEAP, otherwise MAX_HEAP)
*/
heap_t *heap_create(int type)
{
  heap_t *heap;

  // Init the heap
  heap = (heap_t *)util_malloc(sizeof(heap_t));
  heap->nodes = (node_t **)util_malloc(sizeof(node_t *));
  heap->count = 0;
  heap->capacity = 1;
  heap->type = type == 0 ? MIN_HEAP : MAX_HEAP;

  return heap;
}

int heap_insert(heap_t *heap, void *data, int priority)
{
  node_t *new_node;

  // Check heap and internal array are not null before starting
  if (heap == NULL || heap->nodes == NULL)
  {
    return 0;
  }

  // Init new node
  new_node = (node_t *)util_malloc(sizeof(node_t));
  new_node->data = data;
  new_node->priority = priority;

  // Increment heap size and realloc heap if full
  int position = heap->count++;
  if (heap->count == heap->capacity)
  {
    heap->nodes = util_realloc(heap->nodes, 2 * (heap->capacity));
    heap->capacity *= 2;
  }

  // Insert node starting from bottom and pushing it up, if necessary
  position = heap_move_up(heap, new_node, position);

  return position;
}

int heap_extract(heap_t *heap, void **data)
{
  node_t *node;
  
  swap(heap, 0, heap->count - 1);
  node = heap->nodes[heap->count - 1];
  heap->count--;
  data = node->data;
  free(node);
  
  heapify(heap, 0);
  
  return 1;
}

int heap_update(heap_t *heap, int position, int newPriority, void (*freeData)(void *))
{
  int position;
  node_t *old_node;

  // Check heap and internal array are not null before starting
  if (heap == NULL || heap->nodes == NULL || heap->nodes[position] == NULL)
  {
    return 0;
  }

  // Init new node from the old one
  old_node = heap->nodes[position]->data;
  node_t *new_node = (node_t *)util_malloc(sizeof(node_t));
  new_node->data = old_node;
  new_node->priority = newPriority;

  if (heap_compare(old_node, new_node, heap->type) < 0)
  {
    position = heap_move_down(heap, new_node, position);
  }
  else
  {
    position = heap_move_up(heap, new_node, position);
  }

  // Free old node
  if (freeData != NULL)
  {
    freeData(old_node->data);
  }
  free(old_node);

  return position;
}

void heap_destroy(heap_t *heap, void (*freeData)(void *))
{
  if (heap == NULL)
  {
    return;
  }

  if (heap->nodes != NULL)
  {
    for (int i = 0; i < heap->count; ++i)
    {
      if (freeData != NULL)
      {
        freeData(heap->nodes[i]->data);
      }
      free(heap->nodes[i]);
    }

    free(heap->nodes);
  }

  free(heap);
}

int heap_compare(node_t *a, node_t *b, int type)
{
  int result = 0;

  // Check nodes are not null before starting
  if (a == NULL || b == NULL)
  {
    fprintf(stderr, "Cannot compare null nodes!");
    exit(EXIT_FAILURE);
  }

  // Compare priorities depending on heap type, if MIN_HEAP, standard compare, otherwise inverted compare
  if (type == MIN_HEAP)
  {
    result = a->priority - b->priority;
  }
  else
  {
    result = b->priority - a->priority;
  }

  return result;
}

int heap_heapify(heap_t *heap, int position)
{
  int l, r, first;
  l = LEFT(position);
  r = RIGHT(position);
  if (l < heap->count && heap_compare(heap->nodes[l], heap->nodes[position], heap->type) > 0)
    first = l;
  else
    first = position;
  if (r < heap->count && heap_compare(heap->nodes[r], heap->nodes[first], heap->type) > 0)
    first = r;
  if (first != position)
  {
    swap(heap, position, first);
    heapify(heap, first);
  }
  return;
}

int heap_move_down(heap_t *heap, node_t *node, int position)
{
  // Insert node at the current position
  heap->nodes[position] = node;

  // Rebuild heap
  heap_heapify(heap, position);
}

int heap_move_up(heap_t *heap, node_t *node, int position)
{
  // Move nodes down the heap
  while (position >= 1 && heap_compare(heap->nodes[PARENT(position)], node, heap->type) > 0)
  {
    heap->nodes[position] = heap->nodes[PARENT(position)];
    position = PARENT(position);
  }

  // Insert node at the current position
  heap->nodes[position] = node;
  return position;
}

int swap(heap_t *heap, int a, int b)
{
  // Check heap is not null before starting
  if (heap == NULL || heap->nodes == NULL)
  {
    return 0;
  }

  // Swap a and b
  node_t *temp = heap->nodes[b];
  heap->nodes[b] = heap->nodes[a];
  heap->nodes[a] = temp;

  return 1;
}