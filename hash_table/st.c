#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "item.h"
#include "st.h"
#include "../BSTLib/treePrivate.h"

typedef struct STnode* link;
struct STnode { Item item; link next; link next2; } ;

struct symboltable { link *heads; int M; link z; };

link NEW( Item item, link next) {
  link x = malloc(sizeof *x);
  x->item = item; x->next = next; x->next2 = next;
  return x;
}

ST STinit(int maxN) {
  int i;
  ST st = malloc(sizeof *st) ;
  st->M = maxN/5;
  st->heads = malloc(st->M*sizeof(link));
  st->z = NEW(ITEMsetvoid(), NULL);
  for (i=0; i < st->M; i++)
    st->heads[i] = st->z;
  return st;
}

int hash(Key v, int M) {
  int h = 0, base = 127;
  for ( ; *v != '\0'; v++)
    h = (base * h + *v) % M;
  return h;
}

int hashU(Key v, int M) {
  int h, a = 31415, b = 27183;
  for ( h = 0; *v != '\0'; v++, a = a*b % (M-1))
    h = (a*h + *v) % M;
  return h;
}

void STinsert (ST st, Item item) {
  int i = hash(KEYget(item), st->M);
  printf("hash =%d\n", i);
  st->heads[i] = NEW(item, st->heads[i]);
}

Item searchST(link t, Key k, link z) {
  if (t == z)
    return ITEMsetvoid();
  if ((KEYcompare(KEYget(t->item), k))==0)
    return t->item;
  return
     searchST(t->next, k, z);
}

Item STsearch(ST st, Key k) {
  return searchST(st->heads[hash(k, st->M)], k, st->z);
}

link deleteR(link x, Key k) {
  if ( x == NULL )
    return NULL;
  if ((KEYcompare(KEYget(x->item), k))==0) {
    link t = x->next;
    free(x);
    return t;
  }
  x->next = deleteR(x->next, k);
  return x;
}

void STdelete(ST st, Key k) {
  int i = hash(k, st->M);
  st->heads[i] = deleteR(st->heads[i], k);
}

void visitR(link h, link z, FILE *fp) {
  if (h == z)
    return;
  ITEMshow(h->item, fp);
  visitR(h->next, z, fp);
  }

void STdisplay(ST st)  {
  int i;
  for (i=0; i < st->M; i++) {
    printf("st->heads[%d] = ", i);
    visitR(st->heads[i], st->z, stdout);
    printf("\n");
  }
}

void writeHT(FILE *fp, ST st)
{
  int i;
  for (i=0; i < st->M; i++) {
    fprintf(fp, "st->heads[%d] = ", i);
    visitR(st->heads[i], st->z, fp);
    fprintf(fp, "\n");
  }
}

link listUnique (ST st)
{
    link p = NULL, head, tmp;
    int i, first = 1, assigned = 0;

    for (i=0; i < st->M; i++, assigned = 1)
    {
        tmp = st->heads[i];
        if (tmp != st->z)
        {
            while (tmp->next != st->z)
            {
                if (first)
                {
                    head = tmp;
                    first = 0;
                }
                if (p != NULL)
                {
                    p->next2 = st->heads[i];
                    assigned = 0;
                }
                tmp->next2 = tmp->next;
                tmp = tmp->next;
            }

            if (first)
            {
                head = tmp;
                first = 0;
            }

            if (p != NULL && assigned)
                p->next2 = tmp;

            p = tmp;
        }
    }
    printf("Printing unique list...\n\n");
    tmp = head;
    while (tmp != st->z)
    {
        ITEMshow(tmp->item, stdout);
        printf("\n");
        tmp = tmp->next2;
    }
    printf("\n\n");
    return head;
}

void listAndBST (node_t *rp, void *head, ST st)
{
    printf("Printing levels of the elements of the unique list found in the BST...\n\n");
    int curr;
    link list = (link) head;

    while (list != st->z)
    {
        curr = 0;
        searchR(rp, ITEMgetData(list->item), &curr);
        printf("%s found at level %d\n", ITEMgetData(list->item), curr);
        list = list->next2;
    }
}

void BSTtoHT (node_t *root, ST *st)
{
    if (root == NULL)
        return;

    char *reg, *name;
    int lc, pc;
    reg = name = NULL;

    getData2(root, &reg, &name, &lc, &pc);

    Item tmp = ITEMscan2(reg, name, lc, pc);

    if (ITEMcheckvoid(STsearch(*st, KEYget(tmp))))
        STinsert(*st, tmp);

    BSTtoHT (root->left, st);
    BSTtoHT (root->right, st);

    return;
}

void *callListUnique(ST st)
{
    return (void *)listUnique(st);
}

