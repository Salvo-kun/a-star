#ifndef ST_H_DEFINED
#define ST_H_DEFINED

#include "../BSTLib/treePublic.h"

typedef struct symboltable *ST;

ST  	STinit(int) ;
void 	STinsert(ST, Item) ;
Item	STsearch(ST, Key) ;
void	STdelete(ST, Key) ;
void	STdisplay(ST) ;
void    writeHT(FILE *, ST);
void    listAndBST (node_t *, void *, ST);
void    BSTtoHT (node_t *, ST *);
void *  callListUnique(ST st);
#endif
