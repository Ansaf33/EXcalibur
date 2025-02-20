#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "map.h"

/*
static int memAddress = 5000;
struct map* mapHead = NULL;

bool present(char* key){
  struct map* cur = mapHead;
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
  temp->memAddress = memAddress++;

  return temp;
}

struct map* insert(char* key,struct TreeNode* value){
  printf("cache[%s] = %p\n",key,value);
  struct map* temp = createMapNode(key,value);

  if( mapHead == NULL ){
    mapHead = temp;
  }
  else{
    struct map* cur = mapHead;
    while(cur->next){
      cur = cur->next;
    }
    cur->next = temp;
  }

  return mapHead;

}

struct TreeNode* get(char* key){
  struct map* cur = mapHead;
  while(cur){
    if( strcmp(cur->key,key) == 0 ){
      printf("getting value of [%s]\n",key);
      return cur->value;
    }
    cur = cur->next;
  }
  return NULL;
}


struct TreeNode* returnNode(int val,char* str){
  if( val != -1 ){
    char con[20];
    sprintf(con,"%d",val);
    return get(con);
  }
  return get(str);
}


void printMap(){
  struct map* cur = mapHead;
  while(cur){
    printf("Expression : %s | Address of node : %p | Memory Address : %d\n",cur->key,cur->value,cur->memAddress);
    cur = cur->next;
  }

}

*/


