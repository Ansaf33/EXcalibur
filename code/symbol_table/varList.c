#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "varList.h"
#include "symbol.h"


struct list* createVarNode(char* name){
  struct list* temp = (struct list*)malloc(sizeof(struct list));
  temp->name = (char*)malloc(sizeof(char)*100);
  strcpy(temp->name,name);
  temp->next = NULL;

  return temp;
}

struct list* addVariable(struct list* head,char* name){
  struct list* temp = createVarNode(name);
  temp->next = head;
  head = temp;

  return head;
}

void printDetails(struct list* head){
  struct list* cur = head;
  while(cur != NULL){
    printf("%s ",cur->name);
    cur = cur->next;
  }
}

void addAllSymbols(struct list* head, int type,int size){
  struct list* cur = head;
  while(cur != NULL){
    addSymbol(cur->name,type,size);
    cur = cur->next;
  }
}
