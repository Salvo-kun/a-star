#include "heapPrivate.h"

// Public Methods

heap_t *heap_create(int type, int size)
{
  heap_t *heap;

  // Init the heap
  heap = (heap_t *)util_malloc(sizeof(heap_t));

  // Could not allocate, return null
  util_check_r(heap != NULL, "Heap cannot be null, returning...\n", NULL);

  heap->nodes = (link_t *)util_malloc(sizeof(link_t));

  // Could not allocate, return null
  util_check_r(heap->nodes != NULL, "Heap nodes cannot be null, returning...\n", NULL);

  heap->dict = hash_table_create(size);

  // Pointers null at initialization
  heap->nodes[0] = NULL;
  heap->count = 0;
  heap->capacity = 1;
  heap->type = type == 0 ? MIN_HEAP : MAX_HEAP;

#if DEBUG
  fprintf(stdout, "Created heap.\n");
#endif

  return heap;
}

int heap_insert(heap_t *heap, int key, void *data, int priority)
{
  link_t new_node;

  // Check heap and internal array are not null before starting
  util_check_r(heap != NULL, "Heap cannot be null, returning...\n", 0);
  util_check_r(heap->nodes != NULL, "Heap nodes cannot be null, returning...\n", 0);

  // Check the given key is unique
  int *pos = (int *)util_malloc(sizeof(int));
  util_check_r(heap_find(heap, key, &pos) && pos == NULL, "Key must be unique, returning...\n", 0);

  // Init new node
  new_node = (link_t)util_malloc(sizeof(*new_node));
  new_node->key = key;
  new_node->data = data;
  new_node->priority = priority;

  // Increment heap size and realloc heap if full
  int position = heap->count++;

  // Insert into the hash table, this way e always have that what is inside the heap is already inside the hash table
  hash_table_insert(heap->dict, new_node->key, &position);

  if (heap->count >= (int)(FULL_FACTOR * heap->capacity))
  {
#if DEBUG
    fprintf(stdout, "Heap full at %d%%, reallocating...\n", (int)(FULL_FACTOR * 100));
#endif
    heap->nodes = util_realloc(heap->nodes, 2 * (heap->capacity) * sizeof(link_t));
    heap->capacity *= 2;
  }

  // Insert node starting from bottom and pushing it up, if necessary
#if DEBUG
  fprintf(stdout, "Inserting new node with key %d and priority %d...\n", key, priority);
#endif

  heap_move_up(heap, new_node, position);

  return 1;
}

int heap_find(heap_t *heap, int key, int **position)
{
  // Check heap is not null before starting
  util_check_r(heap != NULL, "Heap cannot be null, returning...\n", 0);
  util_check_r(heap->nodes != NULL, "Heap nodes cannot be null, returning...\n", 0);
  util_check_r(*position != NULL, "Position cannot be null, returning...\n", 0);

  return hash_table_get(heap->dict, key, (void **)position);
}

int heap_count(heap_t *heap)
{
  // Check heap and internal array are not null before starting
  util_check_r(heap != NULL, "Heap cannot be null, returning...\n", 0);
  util_check_r(heap->nodes != NULL, "Heap nodes cannot be null, returning...\n", 0);

  return heap->count;
}

int heap_extract(heap_t *heap, void **data, int *key)
{
  link_t node;

  // Check heap and internal array are not null before starting
  util_check_r(heap != NULL, "Heap cannot be null, returning...\n", 0);
  util_check_r(heap->nodes != NULL, "Heap nodes cannot be null, returning...\n", 0);

  // Check internal array is not empty
  util_check_r(!heap_is_empty(heap), "Heap nodes must be non-empty, returning...\n", 0);

#if DEBUG
  fprintf(stdout, "Extracting root node with key %d and priority %d...\n", heap->nodes[0]->key, heap->nodes[0]->priority);
#endif
  heap_swap(heap, 0, heap->count - 1);
  node = heap->nodes[heap->count - 1];

  // Remove from hash table and free memory
  hash_table_delete(heap->dict, node->key);
  free(node);

  // Decremwnt heap size and realloc heap if halved capacity is enough
  heap->count--;
  if (heap->count <= (int)(EMPTY_FACTOR * heap->capacity))
  {
#if DEBUG
    fprintf(stdout, "Heap empty at %d%%, reallocating...\n", (int)(EMPTY_FACTOR * 100));
#endif
    heap->nodes = util_realloc(heap->nodes, (heap->count) * sizeof(link_t));
    heap->capacity /= 2;
  }

  if (data != NULL)
    *data = node->data;

  if (key != NULL)
    *key = node->key;

  heap_heapify(heap, 0);

  return 1;
}

int heap_update(heap_t *heap, int key, int newPriority)
{
  link_t old_node;

  // Check heap and internal array are not null before starting
  util_check_r(heap != NULL, "Heap cannot be null, returning...\n", 0);
  util_check_r(heap->nodes != NULL, "Heap nodes cannot be null, returning...\n", 0);

  // Check internal array is not empty
  util_check_r(!heap_is_empty(heap), "Heap nodes must be non-empty, returning...\n", 0);

  // Find position for the key
  int *position = (int *)util_malloc(sizeof(int));
  util_check_r(heap_find(heap, key, &position) && position != NULL, "Key must be unique, returning...\n", 0);

  // Init new node from the old one
  old_node = heap->nodes[*position];
  link_t new_node;
  new_node = (link_t)util_malloc(sizeof(*new_node));
  new_node->key = old_node->key;
  new_node->data = old_node->data;
  new_node->priority = newPriority;

#if DEBUG
  fprintf(stdout, "Updating node with key %d with new priority %d...\n", old_node->key, newPriority);
#endif

  if (heap_compare(old_node, new_node, heap->type) < 0)
  {
    heap_move_down(heap, new_node, *position);
  }
  else
  {
    heap_move_up(heap, new_node, *position);
  }

  return 1;
}

int heap_destroy(heap_t *heap, void (*freeData)(void *))
{
#if DEBUG
  fprintf(stdout, "Freeing heap...\n");
#endif

  // Check heap and internal array are not null before starting
  util_check_r(heap != NULL, "Heap is already null, returning...\n", 0);

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
  return 1;
}

void heap_stats(FILE *fp, heap_t *heap, void (*printData)(FILE *, void *))
{
  util_check_no_r(heap != NULL, "Heap cannot be null, returning...\n");
  util_check_no_r(heap->nodes != NULL, "Heap nodes cannot be null, returning...\n");

  fprintf(fp, "\n############## Heap Stats ##############\n\n");
  fprintf(fp, "Type = %s\n", heap->type == 0 ? "MIN_HEAP" : "MAX_HEAP");
  fprintf(fp, "Count = %d\n", heap->count);
  fprintf(fp, "Capacity = %d\n", heap->capacity);

  for (int i = 0; i < heap->count; ++i)
  {
    fprintf(fp, "Element %d)\n", i + 1);
    fprintf(fp, "Key = %d\n", heap->nodes[i]->key);

    if (printData != NULL)
    {
      fprintf(fp, "Data: \n");
      printData(fp, heap->nodes[i]->data);
    }

    fprintf(fp, "\n");
  }

  fprintf(fp, "\n########################################\n\n");
}

// Private Methods

int heap_compare(link_t first_node, link_t second_node, int type)
{
  int result = 0;
  util_check_r(first_node != NULL, "node cannot be null, returning...\n", 0);
  util_check_r(second_node != NULL, "node cannot be null, returning...\n", 0);

  // Compare priorities depending on heap type, if MIN_HEAP, standard compare, otherwise inverted compare
  if (type == MIN_HEAP)
  {
    result = first_node->priority - second_node->priority;
  }
  else
  {
    result = second_node->priority - first_node->priority;
  }

  return result;
}

void heap_heapify(heap_t *heap, int position)
{
  util_check_no_r(heap != NULL, "Heap cannot be null, returning...\n");
  util_check_no_r(heap->nodes != NULL, "Heap nodes cannot be null, returning...\n");

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

void heap_move_down(heap_t *heap, link_t node, int position)
{
  util_check_no_r(heap != NULL, "Heap cannot be null, returning...\n");
  util_check_no_r(heap->nodes != NULL, "Heap nodes cannot be null, returning...\n");
  util_check_no_r(node != NULL, "node cannot be null, returning...\n");

  // Insert node at the current position
  heap->nodes[position] = node;

  // Track node position
  hash_table_update(heap->dict, node->key, &position);

  // Rebuild heap
  heap_heapify(heap, position);
}

void heap_move_up(heap_t *heap, link_t node, int position)
{
  util_check_no_r(heap != NULL, "Heap cannot be null, returning...\n");
  util_check_no_r(heap->nodes != NULL, "Heap nodes cannot be null, returning...\n");
  util_check_no_r(node != NULL, "node cannot be null, returning...\n");

  // Move nodes down the heap
  while (position >= 1 && heap_compare(heap->nodes[PARENT(position)], node, heap->type) > 0)
  {
    heap->nodes[position] = heap->nodes[PARENT(position)];

    // Track parent node position, do a copy to avoid problem due to passing references
    int pos = position;
    hash_table_update(heap->dict, heap->nodes[PARENT(position)]->key, &pos);

    position = PARENT(position);
  }

  // Insert node at the current position
  heap->nodes[position] = node;

  // Track node position
  hash_table_update(heap->dict, node->key, &position);
}

void heap_swap(heap_t *heap, int first_pos, int second_pos)
{
  // Check heap is not null before starting
  util_check_no_r(heap != NULL, "Heap cannot be null, returning...\n");
  util_check_no_r(heap->nodes != NULL, "Heap nodes cannot be null, returning...\n");

  // heap_swap a and b
  link_t temp = heap->nodes[second_pos];
  heap->nodes[second_pos] = heap->nodes[first_pos];
  heap->nodes[first_pos] = temp;

  // Track nodes' positions
  hash_table_update(heap->dict, heap->nodes[first_pos]->key, &first_pos);
  hash_table_update(heap->dict, heap->nodes[second_pos]->key, &second_pos);
}