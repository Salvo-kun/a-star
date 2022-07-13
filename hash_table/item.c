#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "item.h"

struct item { char *reg; char *name; int lc; int pc;};

Item ITEMscan() {
  char reg[MAXST], name[MAXST];
  int lc, pc;

  printf("reg = ");
  scanf("%s", reg);
  printf("name = ");
  scanf("%s", name);
  printf("lc = ");
  scanf("%d", &lc);
  printf("pc = ");
  scanf("%d", &pc);

  Item tmp = (Item) malloc(sizeof(struct item));
  if (tmp == NULL)
    return ITEMsetvoid();

  else {
    tmp->reg = strdup(reg);
    tmp->lc = lc;
    tmp->name = strdup(name);
    tmp->pc = pc;
  }
  return tmp;
}

Item ITEMscan2(char *reg, char *name, int lc, int pc) {

  Item tmp = (Item) malloc(sizeof(struct item));
  if (tmp == NULL)
    return ITEMsetvoid();

  else {
    tmp->reg = strdup(reg);
    tmp->lc = lc;
    tmp->name = strdup(name);
    tmp->pc = pc;
  }
  return tmp;
}

void ITEMshow(Item data, FILE *fp) {
  fprintf(fp, "reg = %s name = %s lc = %d pc = %d ", data->reg, data->name, data->lc, data->pc);
}

int ITEMcheckvoid(Item data) {
  Key k1, k2="";
  k1 = KEYget(data);
  if (KEYcompare(k1,k2)==0)
    return 1;
  else
    return 0;
}

Item ITEMsetvoid() {
  char reg[MAXST]="";
  Item tmp = (Item) malloc(sizeof(struct item));
  if (tmp != NULL) {
    tmp->reg = strdup(reg);
    tmp->name = strdup(reg);
    tmp->lc = -1;
    tmp->pc = -1;
  }
  return tmp;
}

Key KEYscan() {
  char reg[MAXST];
  Key k;
  scanf("%s", reg);
  k = strdup(reg);
  return k;
}

int  KEYcompare(Key k1, Key k2) {
  return strcmp(k1,k2);
}

Key KEYget(Item data) {
  return data->reg;
}

char *ITEMgetData(Item data)
{
    return data->reg;
}
