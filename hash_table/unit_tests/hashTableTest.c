#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "item.h"
#include "st.h"

int mainHT () {
  Item item;
  Key k;
  int i,  cont, maxN;

  printf("Input size of hash table to guarantee load factor < 1/2:");
  scanf("%d", &maxN);

  ST st = STinit(maxN);
  cont = 1;
  while(cont) {
    printf("\nOperations on hash tables\n");
    printf("===============\n");
    printf("1.Insert\n");
    printf("2.Search\n");
    printf("3.Delete\n");
    printf("4.Display\n");
    printf("5.Exit\n");
    printf("Enter your choice : ");
    if(scanf("%d",&i)<=0) {
      printf("Integers only!\n");
      exit(0);
    }
    else {
      switch(i) {
                case 1:     printf("Enter item: \n");
                            item = ITEMscan();
                            if (ITEMcheckvoid(STsearch(st, KEYget(item))))
                              STinsert(st, item);
                            break;
                case 2:     printf("Enter key: \n");
                            k = KEYscan();
                            if (ITEMcheckvoid(STsearch(st, k)))
                              printf("data with given key not found!\n");
                            else
                              printf("data with given key found!\n");
                            break;
                case 3:     printf("Enter key: \n");
                            k = KEYscan();
                            if (ITEMcheckvoid(STsearch(st, k)))
                              printf("data with given key not in symbol table!\n");
                            else
                              STdelete(st, k);
                            break;
                case 4:     STdisplay(st);
                            break;
                case 5:     cont = 0;
                            break;
                default:    printf("Invalid option\n");
            }
      }
    }
  return 0;
}
