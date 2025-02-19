#ifndef MAP_H 
#define MAP_H

#include <stdio.h>
#include <stdbool.h>
#include "../AST.h"

struct map{
  char* key;
  struct TreeNode* value;
  struct map* next;
};

struct map* createMapNode(char* key,struct TreeNode* value);

bool present(struct map* head,char* key);

struct TreeNode* get(struct map* head,char* key);

struct map* insert(struct map* head,char* key,struct TreeNode* value);

struct TreeNode* returnNode(struct map* head,int val,char* key);

void printMap(struct map* head);






#endif
