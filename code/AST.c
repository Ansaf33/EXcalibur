
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "map_implementation/map.h"
#include "AST.h"
#include "operators/optrans.h"
#include "symbol_table/Gsymbol.h"

struct map* cache = NULL;


// ------------- CHECK IF TYPE IS SAME

bool typeSatisfied(struct TreeNode* root){
    
    // if arithmetic operator, left and right should be integers
    if(root->op >=0 && root->op <= 3){
      return root->left->type == 0 && root->right->type == 0;
    }
    // if assigns, left should be an identifier (integer) and right should be integer
    else if( root->op == 4 ){
       return root->left->type == 0 && root->right->type == 0 || root->left->type == 2 && root->right->type == 2;
    }
    // if logical operators, left and right type should be the type for expressions (integer)
    else if( root->op >= 5 && root->op <= 10 ){
          return root->left->type == 0 && root->right->type == 0;
   }
    // if READ statement, left type should be integer/string
    else if( root->op == 11 ){
        return root->left->type == 0 || root->left->type == 2;
   }
   // if WRITE statement, left type should be integer/string
    else if( root->op == 12 ){
        return root->left->type == 0 || root->left->type == 2;
   }
    // if IF statement, condition type should be boolean
    else if( root->op == 14 ){
        return root->middle->type == 1;
    }
    // if WHILE statement, condition type should be boolean
    else if( root->op == 15 ){
        return root->left->type == 1;
    }
    // if REPEAT statement, condition type should be boolean
    else if( root->op == 18 ){
        return root->left->type == 1;
    }
    // if DOWHILE statement, condition type should be boolean
    else if( root->op == 19 ){
        return root->left->type == 1;
    }

   return true;

}

// -------------- CREATE NODE FOR NUMBERS

struct TreeNode* createNumNode(int val){

  // ---------------------- HANDLES MAPPING ---------------
  char con[20];
  sprintf(con,"%d",val);
  struct TreeNode* cached_val = get(cache,con);
  if( cached_val ) {
    return cached_val;
  }


  struct TreeNode* temp = (struct TreeNode*)malloc(sizeof(struct TreeNode));
  temp->val = val;
  temp->string = NULL;
  temp->op = -1;
  temp->type = 0;
  temp->varname = NULL;
  temp->left = NULL;
  temp->right = NULL;
  temp->middle = NULL;
  temp->Gsymbol = NULL;

  temp->content = (char*)malloc(sizeof(char)*100);
  strcpy(temp->content,con);
  cache = insert(cache,temp->content,temp);


  return temp;
  
}

// -------------- CREATE NODE FOR OPERATORS


struct TreeNode* createOpNode(int type,int op,struct TreeNode* left,struct TreeNode* right){

  char* content = (char*)malloc(sizeof(char)*100); 
  // ---------------------- HANDLES MAPPING --------------- ONLY UNDER ARITHMETIC OPERATORS AND LOGICAL OPERATORS
  if( op <= 10 ){
    if( left && right ) {
      strcat(content,left->content);
      strcat(content,map(op));
      strcat(content,right->content);
    }

    struct TreeNode* cached_val = get(cache,content);
    if( cached_val ) {
      return cached_val;
    }
  }
  // ------------------- FINISH MAPPING -----------------------

  struct TreeNode* temp = (struct TreeNode*)malloc(sizeof(struct TreeNode));
  temp->val = -1;
  temp->string = NULL;
  temp->op = op;
  temp->type = type;
  temp->varname = "\0";
  temp->left = left;
  temp->right = right;
  temp->middle = NULL;
  temp->Gsymbol = NULL;
  temp->content = (char*)malloc(sizeof(char)*100);
  strcpy(temp->content,content);

  if( op <= 10 ){
    cache = insert(cache,temp->content,temp);
  }

  if( left  ){ 
    if(!typeSatisfied(temp)){
      exit(1);
    }
  }


  return temp;
}

// --------------- CREATE NODE FOR STRINGS


struct TreeNode* createStringNode(char* string){

  // ---------------------- HANDLES MAPPING ---------------
  struct TreeNode* cached_val = get(cache,string);
  if( cached_val ) {
    return cached_val;
  }
  // ---------------------- MAPPING FINISHED ----------------

  struct TreeNode* temp = (struct TreeNode*)malloc(sizeof(struct TreeNode));
  temp->val = -1;
  temp->string = (char*)malloc(sizeof(char)*100);
  strcpy(temp->string,string);
  temp->op = -1;
  temp->type = 2;
  temp->varname = NULL;
  temp->left = NULL;
  temp->right = NULL;
  temp->middle = NULL;
  temp->Gsymbol = NULL;

  temp->content = (char*)malloc(sizeof(char)*100);
  strcpy(temp->content,string);
  cache = insert(cache,temp->content,temp);
  return temp;

}



// -------------- CREATE NODE FOR IDs

struct TreeNode* createIdNode(char* varname){
  // ---------------------- HANDLES MAPPING ---------------
  struct TreeNode* cached_val = get(cache,varname);
  if( cached_val ) {
      return cached_val;
  }
  // ---------------------- MAPPING FINISHED ----------------


  struct TreeNode* temp = (struct TreeNode*)malloc(sizeof(struct TreeNode));
  temp->Gsymbol = lookUp(varname);

  if(temp->Gsymbol == NULL){
    printf("Cannot declare variables outside declaration scope\n");
    exit(1);
  }
 
  temp->val = -1;
  temp->string = NULL;
  temp->op = -1;
  temp->type = temp->Gsymbol->type;
  temp->varname = (char*)malloc(sizeof(char)*100);
  strcpy(temp->varname,varname);
  temp->left = NULL;
  temp->right = NULL;
  temp->middle = NULL;
  temp->content = (char*)malloc(sizeof(char)*100);
  strcpy(temp->content,varname);
  cache = insert(cache,temp->content,temp);


  return temp;

}

// -------------- CREATE NODE FOR IF STATEMENTS

struct TreeNode* createIfNode(struct TreeNode* middle,struct TreeNode* left,struct TreeNode* right){

  struct TreeNode* temp = (struct TreeNode*)malloc(sizeof(struct TreeNode));
  temp->val = -1;
  temp->string = NULL;
  temp->op = 14;
  temp->type = -1;
  temp->varname = NULL;
  temp->left = left;
  temp->middle = middle;
  temp->right = right;
  temp->Gsymbol = NULL;
  temp->content = (char*)malloc(sizeof(char)*100);


  // CHECK IF SATISFIABLE

  if( middle ){ 
    if(!typeSatisfied(temp)){
      printf("If Condition : Type not matching.\n");
      exit(1);
    }
        
  }

  return temp;

}

// ---------------- CREATE NODE FOR WHILE STATEMENTS

struct TreeNode* createWhileNode(int op,struct TreeNode* left,struct TreeNode* right){
  struct TreeNode* temp = (struct TreeNode*)malloc(sizeof(struct TreeNode));  
  temp->val = -1;
  temp->string = NULL;
  temp->op = op;
  temp->type = -1;
  temp->varname = NULL;
  temp->left = left;
  temp->right = right;
  temp->middle = NULL;
  temp->Gsymbol = NULL;
  temp->content = (char*)malloc(sizeof(char)*100);

  // CHECK IF SATISFIABLE

  if( left && right ){ 
    if(!typeSatisfied(temp)){
      printf("While Condition : Type not matching.\n");
      exit(1);
    }
  }
  

  return temp;


}


void Inorder(struct TreeNode* root){
  if(root == NULL){
    return;
  }
  Inorder(root->left);
  // IT IS A NUMBER
  if(root->val != -1 ){
    printf(" [%d][%p]\n",root->val,root);
  }
  // IT IS A STRING
  if(root->string != NULL ){
    printf("[%s][%p]\n",root->string,root);
  }
  // IT IS AN OPERATOR
  else if(root->op != -1 ){
    printf("[%s][%p]\n",map(root->op),root);
  }
  // IT IS A VARIABLE
  else if( root->varname != NULL ){
    printf("[%s][%p]\n",root->varname,root);
  }

  Inorder(root->middle);
  Inorder(root->right);
}
