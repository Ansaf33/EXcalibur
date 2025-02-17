#ifndef varList_H

#define varList_H

#include <stdio.h>
#include <stdlib.h>

struct list{
  char* name;
  struct list* next;
};

struct list* createVarNode(char* name);

struct list* addVariable(struct list* head,char* name);

void printDetails(struct list* head);

void addAllSymbols(struct list* head,int type,int size);


#endif
