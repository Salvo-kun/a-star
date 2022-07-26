#include "__hash_table.h"

// Public Methods

hash_table_t *hash_table_create(int size)
{
    hash_table_t *hash_table;

    // Init the hash_table
    hash_table = (hash_table_t *)util_malloc(sizeof(hash_table_t));

    // Could not allocate, return null
    util_check_r(hash_table != NULL, "hash_table cannot be null, returning...\n", NULL);

    hash_table->nodes = (link_t *)util_malloc(size * sizeof(link_t));

    // Could not allocate, return null
    util_check_r(hash_table->nodes != NULL, "hash_table nodes cannot be null, returning...\n", NULL);

    // Bucket pointers null at initialization
    for (int i = 0; i < size; ++i)
    {
        hash_table->nodes[i] = NULL;
    }

    hash_table->count = 0;
    hash_table->capacity = size;

#if DEBUG_HASH
    fprintf(stdout, "Created hash_table.\n");
#endif

    return hash_table;
}

int hash_table_get(hash_table_t *hash_table, int key, void **data)
{
    link_t current;

    // Check hash_table and internal array are not null before starting
    util_check_r(hash_table != NULL, "hash_table cannot be null, returning...\n", 0);
    util_check_r(hash_table->nodes != NULL, "hash_table nodes cannot be null, returning...\n", 0);
    util_check_r(data != NULL, "data pointer cannot be null, returning...\n", 0);

    // Calculate bucket and iterate until node is either found and removed or not found
    int bucket = hash(key, hash_table->capacity);
    current = hash_table->nodes[bucket];
    *data = NULL;

    while (current != NULL)
    {

        if (current->key == key)
        {
            // Node found, break
            *data = current->data;
            break;
        }

        current = current->next;
    }

#if DEBUG_HASH
    fprintf(stdout, "Node with key %d %s...\n", key, *data == NULL ? "not found" : "found");
#endif

    // Node not found, data stays NULL
    return 1;
}

int hash_table_insert(hash_table_t *hash_table, int key, void *data)
{  
    // Check hash_table and internal array are not null before starting
    util_check_r(hash_table != NULL, "hash_table cannot be null, returning...\n", 0);
    util_check_r(hash_table->nodes != NULL, "hash_table nodes cannot be null, returning...\n", 0);

    // Calculate hash of the bucket and add new node to the head of the bucket
    int bucket = hash(key, hash_table->capacity);
    link_t new_node = node_create(key, data, hash_table->nodes[bucket]);
    util_check_r(new_node != NULL, "could not allocate new node, returning...\n", 0);

    hash_table->nodes[bucket] = new_node;

#if DEBUG_HASH
    fprintf(stdout, "Inserted new node with key %d...\n", key);
#endif

    // Increment hash_table size and realloc hash_table if full
    hash_table->count++;

    if (hash_table->count >= (int)(LOAD_THRESHOLD * hash_table->capacity))
    {
#if DEBUG_HASH
        fprintf(stdout, "hash_table full at %d%%, reallocating...\n", (int)(LOAD_THRESHOLD * 100));
#endif
        // Reallocation must also reassign all elements, create a copy of nodes first, then destroy it after!
        link_t *old_nodes = hash_table->nodes;
        hash_table->nodes = (link_t *)util_calloc(hash_table->capacity * 2, sizeof(link_t));
        hash_table->count = 0;
        hash_table->capacity *= 2;

        // Push old nodes into new array
        for (int i = 0; i < hash_table->capacity / 2; ++i)
        {
            link_t current = old_nodes[i];

            while (current != NULL)
            {
                hash_table_insert(hash_table, current->key, current->data);
                current = current->next;
            }
        }

        // Free old array
        for (int i = 0; i < hash_table->capacity / 2; ++i)
        {
            link_t current = old_nodes[i];

            while (old_nodes[i] != NULL)
            {
                current = old_nodes[i];
                old_nodes[i] = current->next;

                free(current);
            }
        }

        free(old_nodes);
    }

    return 1;
}

int hash_table_update(hash_table_t *hash_table, int key, void *data)
{
    link_t current;
    int found = 0;

    // Check hash_table and internal array are not null before starting
    util_check_r(hash_table != NULL, "hash_table cannot be null, returning...\n", 0);
    util_check_r(hash_table->nodes != NULL, "hash_table nodes cannot be null, returning...\n", 0);

    // Calculate bucket and iterate until node is either found and removed or not found
    int bucket = hash(key, hash_table->capacity);
    current = hash_table->nodes[bucket];

    while (current != NULL)
    {
        if (current->key == key)
        {
            // Node found, update data
            found = 1;
            current->data = data;

            break;
        }

        current = current->next;
    }

#if DEBUG_HASH
    fprintf(stdout, "Node with key %d %s...\n", key, found ? "updated" : "not found");
#endif

    return 1;
}

int hash_table_delete(hash_table_t *hash_table, int key)
{
    link_t previous, current;
    int found = 0;

    // Check hash_table and internal array are not null before starting
    util_check_r(hash_table != NULL, "hash_table cannot be null, returning...\n", 0);
    util_check_r(hash_table->nodes != NULL, "hash_table nodes cannot be null, returning...\n", 0);

    // Calculate bucket and iterate until node is either found and removed or not found
    int bucket = hash(key, hash_table->capacity);
    previous = NULL;
    current = hash_table->nodes[bucket];

    while (current != NULL)
    {
        if (current->key == key)
        {
            // Node found
            found = 1;
            if (previous != NULL)
                previous->next = current->next;
            else
                hash_table->nodes[bucket] = current->next;

            // Free deleted node
            free(current);

            break;
        }

        previous = current;
        current = current->next;
    }

    hash_table->count--;

#if DEBUG_HASH
    fprintf(stdout, "Node with key %d %s...\n", key, found ? "deleted" : "not found");
#endif

    return 1;
}

int hash_table_destroy(hash_table_t *hash_table, void (*freeData)(void *))
{
#if DEBUG_HASH
    fprintf(stdout, "Freeing hash_table...\n");
#endif

    // Check hash_table and internal array are not null before starting
    util_check_r(hash_table != NULL, "hash_table is already null, returning...\n", 0);

    if (hash_table->nodes != NULL)
    {
        for (int i = 0; i < hash_table->capacity; ++i)
        {
            link_t current = hash_table->nodes[i];

            while (hash_table->nodes[i] != NULL)
            {
                current = hash_table->nodes[i];
                hash_table->nodes[i] = current->next;

                if (freeData != NULL)
                {
                    freeData(current->data);
                }

                free(current);
            }
        }

        free(hash_table->nodes);
    }

    free(hash_table);
    return 1;
}

void hash_table_stats(FILE *fp, hash_table_t *hash_table, void (*printData)(FILE *, void *))
{
    util_check_no_r(hash_table != NULL, "hash_table cannot be null, returning...\n");
    util_check_no_r(hash_table->nodes != NULL, "hash_table nodes cannot be null, returning...\n");

    fprintf(fp, "\n############## hash_table Stats ##############\n\n");
    fprintf(fp, "Count = %d\n", hash_table->count);
    fprintf(fp, "Capacity = %d\n", hash_table->capacity);
    fprintf(fp, "Load = %d%%\n\n", (int)(100 * (float)hash_table->count / (float)hash_table->capacity));

    for (int i = 0; i < hash_table->capacity; ++i)
    {
        int j = 0;
        link_t current = hash_table->nodes[i];

        while (current != NULL)
        {
            if (++j == 1)
                fprintf(fp, "Bucket %d:\n", i);
            fprintf(fp, "Element %d)\n", j);
            fprintf(fp, "Key = %d\n", current->key);

            if (printData != NULL)
            {
                fprintf(fp, "Data: \n");
                printData(fp, current->data);
            }

            fprintf(fp, "\n");
            current = current->next;
        }
    }

    fprintf(fp, "\n########################################\n\n");
}

// Private Methods
link_t node_create(int key, void *data, link_t next)
{
    link_t node;

    /*
        Allocate new node, beware that you need to pass the size of the data
        pointed by link_t, since link_t is a pointer itself
    */
    node = (link_t)util_malloc(sizeof(*node));

    // Assign fields
    node->key = key;
    node->data = data;
    node->next = next;

    return node;
}