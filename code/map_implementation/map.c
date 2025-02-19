#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "map.h"

bool present(struct map* head,char* key){
  struct map* cur = head;
  while(cur){
    if( strcmp(cur->key,key) == 0 ){
      return true;
    }
    cur = cur->next;
  }
  return false;
}

struct map* createMapNode(char* key,struct TreeNode* value){
  struct map* temp = (struct map*)malloc(sizeof(struct map));

  temp->key = (char*)malloc(sizeof(char)*100);
  strcpy(temp->key,key);
  temp->value = value;

  return temp;
}

struct map* insert(struct map* head,char* key,struct TreeNode* value){
  printf("cache[%s] = %p\n",key,value);
  struct map* temp = createMapNode(key,value);

  if( head == NULL ){
    head = temp;
  }
  else{
    struct map* cur = head;
    while(cur->next){
      cur = cur->next;
    }
    cur->next = temp;
  }

  return head;

}

struct TreeNode* get(struct map* head,char* key){
  struct map* cur = head;
  while(cur){
    if( strcmp(cur->key,key) == 0 ){
      printf("getting value of [%s]\n",key);
      return cur->value;
    }
    cur = cur->next;
  }
  return NULL;
}


struct TreeNode* returnNode(struct map* head,int val,char* str){
  if( val != -1 ){
    char con[20];
    sprintf(con,"%d",val);
    return get(head,con);
  }
  return get(head,str);
}


void printMap(struct map* head){
  struct map* cur = head;
  while(cur){
    printf("%s : [%p]\n",cur->key,cur->value);
    cur = cur->next;
  }

}




