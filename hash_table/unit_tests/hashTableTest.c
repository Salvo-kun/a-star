#include <stdio.h>
#include <stdlib.h>
#include "../hash_table.h"

void printData(FILE *f, void *d)
{
    fprintf(f, "%d", *((int *)d));
}

int main(int argc, char **argv)
{
  hash_table_t *ht = hash_table_create(30);

  int newData[] = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
  int *data = (int *)util_malloc(sizeof(int));

  for (int i = 0; i < 10; ++i)
  {
    hash_table_insert(ht, i, (void *)(newData + i));
    hash_table_stats(stdout, ht, printData);
  }

  hash_table_get(ht, 5, (void **)&data);

  printf("%p %d\n%p %d\n\n", data, *data, (newData + 5), *(newData + 5));

  *data = 100;
  hash_table_update(ht, 5, data);
  hash_table_stats(stdout, ht, printData);
  hash_table_delete(ht, 5);
  hash_table_stats(stdout, ht, printData);

  hash_table_destroy(ht, NULL);
}