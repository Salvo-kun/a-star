#include <stdio.h>
#include <stdlib.h>
#include "../hashTablePublic.h"

int main(int argc, char **argv)
{
  hash_table_t *ht = hash_table_create(30);

  int newData[] = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
  int *data;

  for (int i = 0; i < 10; ++i)
  {
    hash_table_insert(ht, i, (void *)(newData + i));
    hash_table_stats(stdout, ht, NULL);
  }

  hash_table_get(ht, 5, (void **)&data);
  *data = 100;
  hash_table_update(ht, 5, data);
  hash_table_stats(stdout, ht, NULL);
  hash_table_delete(ht, 5);
  hash_table_stats(stdout, ht, NULL);

  hash_table_destroy(ht, NULL);
}