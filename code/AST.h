#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "symbol_table/symbol.h"

struct TreeNode{

  int val;
  char* string;

  int op;
  int type;
  char* varname;

  struct TreeNode* left;
  struct TreeNode* middle;
  struct TreeNode* right;

  struct symbol* symbol;
};

bool typeSatisfied(struct TreeNode* root);

struct TreeNode* createNumNode(int val);

struct TreeNode* createOpNode(int type,int op,struct TreeNode* left,struct TreeNode* right);

struct TreeNode* createStringNode(char* string);

struct TreeNode* createIdNode(char* varname);

struct TreeNode* createIfNode(struct TreeNode* middle, struct TreeNode* left,struct TreeNode* right);

struct TreeNode* createWhileNode(int op,struct TreeNode* left, struct TreeNode* right);

void Inorder(struct TreeNode* root);

#endif


