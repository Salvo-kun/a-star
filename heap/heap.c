#include "heapPrivate.h"
#include "../utils/util.h"

// Public Methods

heap_t *heap_create(int type)
{
  heap_t *heap;

  // Init the heap
  heap = (heap_t *)util_malloc(sizeof(heap_t));
  heap->nodes = (node_t *)util_malloc(sizeof(node_t));
  heap->count = 0;
  heap->capacity = 1;
  heap->type = type == 0 ? MIN_HEAP : MAX_HEAP;

#if DEBUG
  fprintf(stdout, "Created heap.\n");
  heap_stats(heap);
#endif

  return heap;
}

void heap_insert(heap_t *heap, int key, void *data, int priority)
{
  node_t new_node;

  // Check heap and internal array are not null before starting
  util_check_m(heap != NULL, "Heap cannot be null, terminating...\n");
  util_check_m(heap->nodes != NULL, "Heap nodes cannot be null, terminating...\n");

  // Check the given key is unique
  util_check_m(heap_find(heap, key) == -1, "Key must be unique, terminating...\n");

  // Init new node
  new_node.key = key;
  new_node.data = data;
  new_node.priority = priority;

  // Increment heap size and realloc heap if full
  int position = heap->count++;
  if (heap->count == heap->capacity)
  {
#if DEBUG
    fprintf(stdout, "Heap full, reallocating...\n");
#endif
    heap->nodes = util_realloc(heap->nodes, 2 * (heap->capacity) * sizeof(node_t));
    heap->capacity *= 2;
  }

  // Insert node starting from bottom and pushing it up, if necessary
#if DEBUG
  fprintf(stdout, "Inserting new node with key %d and priority %d...\n", key, priority);
#endif
  heap_move_up(heap, new_node, position);
#if DEBUG
  heap_stats(heap);
#endif
}

int heap_find(heap_t *heap, int key)
{
  // Check heap is not null before starting
  util_check_m(heap != NULL, "Heap cannot be null, terminating...\n");
  util_check_m(heap->nodes != NULL, "Heap nodes cannot be null, terminating...\n");

  for (int i = 0; i < heap->count; ++i)
  {
    if (heap->nodes[i].key == key)
      return i;
  }

  return -1;
}

int heap_extract(heap_t *heap, void **data)
{
  node_t node;

  // Check heap and internal array are not null before starting
  util_check_m(heap != NULL, "Heap cannot be null, terminating...\n");
  util_check_m(heap->nodes != NULL, "Heap nodes cannot be null, terminating...\n");

#if DEBUG
  fprintf(stdout, "Extracting root node with key %d and priority %d...\n", heap->nodes[0].key, heap->nodes[0].priority);
#endif
  heap_swap(heap, 0, heap->count - 1);
  node = heap->nodes[heap->count - 1];

  // Decremwnt heap size and realloc heap if halved capacity is enough
  heap->count--;
  if (heap->count * 2 == heap->capacity)
  {
#if DEBUG
    fprintf(stdout, "Heap half empty, reallocating...\n");
#endif
    heap->nodes = util_realloc(heap->nodes, (heap->count) * sizeof(node_t));
    heap->capacity /= 2;
  }
  data = node.data;

  heap_heapify(heap, 0);
#if DEBUG
  heap_stats(heap);
#endif
  return node.key;
}

void heap_update(heap_t *heap, int position, int newPriority)
{
  node_t old_node;

  // Check heap and internal array are not null before starting
  util_check_m(heap != NULL, "Heap cannot be null, terminating...\n");
  util_check_m(heap->nodes != NULL, "Heap nodes cannot be null, terminating...\n");

  // Init new node from the old one
  old_node = heap->nodes[position];
  node_t new_node;
  new_node.key = old_node.key;
  new_node.data = old_node.data;
  new_node.priority = newPriority;

#if DEBUG
  fprintf(stdout, "Updating node with key %d with new priority %d...\n", old_node.key, newPriority);
#endif

  if (heap_compare(old_node, new_node, heap->type) < 0)
  {
    heap_move_down(heap, new_node, position);
  }
  else
  {
    heap_move_up(heap, new_node, position);
  }

#if DEBUG
  heap_stats(heap);
#endif
}

void heap_destroy(heap_t *heap, void (*freeData)(void *))
{
#if DEBUG
  fprintf(stdout, "Freeing heap...\n");
#endif

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
        freeData(heap->nodes[i].data);
      }
    }

    free(heap->nodes);
  }

  free(heap);
}

// Private Methods

int heap_compare(node_t first_node, node_t second_node, int type)
{
  int result = 0;

  // Compare priorities depending on heap type, if MIN_HEAP, standard compare, otherwise inverted compare
  if (type == MIN_HEAP)
  {
    result = first_node.priority - second_node.priority;
  }
  else
  {
    result = second_node.priority - first_node.priority;
  }

  return result;
}

void heap_heapify(heap_t *heap, int position)
{
  util_check_m(heap != NULL, "Heap cannot be null, terminating...\n");
  util_check_m(heap->nodes != NULL, "Heap nodes cannot be null, terminating...\n");

  int l, r, first;
  l = LEFT(position);
  r = RIGHT(position);

  if (l < heap->count && heap_compare(heap->nodes[l], heap->nodes[position], heap->type) < 0)
    first = l;
  else
    first = position;

  if (r < heap->count && heap_compare(heap->nodes[r], heap->nodes[first], heap->type) < 0)
    first = r;

  if (first != position)
  {
    heap_swap(heap, position, first);
    heap_heapify(heap, first);
  }
}

void heap_move_down(heap_t *heap, node_t node, int position)
{
  util_check_m(heap != NULL, "Heap cannot be null, terminating...\n");
  util_check_m(heap->nodes != NULL, "Heap nodes cannot be null, terminating...\n");

  // Insert node at the current position
  heap->nodes[position] = node;

  // Rebuild heap
  heap_heapify(heap, position);
}

void heap_move_up(heap_t *heap, node_t node, int position)
{
  util_check_m(heap != NULL, "Heap cannot be null, terminating...\n");
  util_check_m(heap->nodes != NULL, "Heap nodes cannot be null, terminating...\n");

  // Move nodes down the heap
  while (position >= 1 && heap_compare(heap->nodes[PARENT(position)], node, heap->type) > 0)
  {
    heap->nodes[position] = heap->nodes[PARENT(position)];
    position = PARENT(position);
  }

  // Insert node at the current position
  heap->nodes[position] = node;
}

void heap_swap(heap_t *heap, int first_pos, int second_pos)
{
  // Check heap is not null before starting
  util_check_m(heap != NULL, "Heap cannot be null, terminating...\n");
  util_check_m(heap->nodes != NULL, "Heap nodes cannot be null, terminating...\n");

  // heap_swap a and b
  node_t temp = heap->nodes[second_pos];
  heap->nodes[second_pos] = heap->nodes[first_pos];
  heap->nodes[first_pos] = temp;
}

void heap_stats(heap_t *heap)
{
  util_check_m(heap != NULL, "Heap cannot be null, terminating...\n");
  util_check_m(heap->nodes != NULL, "Heap nodes cannot be null, terminating...\n");

  fprintf(stdout, "Heap Stats:\ttype = %s\tcount = %d\tcapacity = %d\tarray: ", heap->type == 0 ? "MIN_HEAP" : "MAX_HEAP", heap->count, heap->capacity);

  for (int i = 0; i < heap->count; ++i)
  {
    fprintf(stdout, "%d ", heap->nodes[i].key);
  }

  fprintf(stdout, "\n");
}