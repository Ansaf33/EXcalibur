
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "map_implementation/map.h"
#include "map_implementation/new_map.h"
#include "LRUcache/LRUcache.h"
#include "AST.h"
#include "operators/optrans.h"
#include "symbol_table/Gsymbol.h"

// ------------------------------------------------------------------------------------------------------------- HASHMAP AND CACHE
HashMap* hashmap = NULL;
LRUCache* cache = NULL;

bool init_hashmap(){
  hashmap = createHashMap();
  return true;
}

bool init_cache(){
  cache = create_cache();
  return true;
}

LRUCache* get_cache(){
  return cache;
}

// ------------------------------------------------------------------------------------------------------------- TYPE CHECKING

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

// -------------------------------------------------------------------------------------------- CREATE NUMBER NODE

struct TreeNode* createNumNode(int val){

  // ---------------------- HANDLES MAPPING ---------------

  char con[20];
  sprintf(con,"%d",val);
  struct TreeNode* cached_val = get(hashmap,con);
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
  insert(hashmap,temp->content,temp);


  return temp;
  
}

// -------------------------------------------------------------------------------------------- CREATE OPERATOR NODE


struct TreeNode* createOpNode(int type,int op,struct TreeNode* left,struct TreeNode* right){

  char* content = (char*)malloc(sizeof(char)*100); 

  // ---------------------- HANDLES MAPPING ---------------
  if( op <= 10 && op != 4 ){
    if( left && right ) {
      strcat(content,left->content);
      strcat(content,map(op));
      strcat(content,right->content);
    }

    struct TreeNode* cached_val = get(hashmap,content);
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

  if( op <= 10 && op != 4 ){
    insert(hashmap,temp->content,temp);

    // add dependencies to operator node
    addDependencies(temp,left,right);

    put(cache,content,0,temp->dependencies);
 
  } 


  if( left  ){ 
    if(!typeSatisfied(temp)){
      exit(1);
    }
  }


  return temp;
}

// ----------------------------------------------------------------------------------------------------------- CREATE NODE FOR STRINGS


struct TreeNode* createStringNode(char* string){

  // ---------------------- HANDLES MAPPING ---------------
  struct TreeNode* cached_val = get(hashmap,string);
  put(cache,string,-1,NULL);
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

  insert(hashmap,temp->content,temp);
  return temp;

}



// -------------------------------------------------------------------------------------------------------------------- CREATE NODE FOR IDs

struct TreeNode* createIdNode(char* varname){
  // ---------------------- HANDLES MAPPING ---------------
  struct TreeNode* cached_val = get(hashmap,varname);
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
  insert(hashmap,temp->content,temp);

  // add dependency to ID node
  temp->dependencies[0] = (char*)malloc(sizeof(char)*10);
  strcpy(temp->dependencies[0],varname);

  return temp;

}

// ----------------------------------------------------------------------------------------------------------------- CREATE NODE FOR IF STATEMENTS

struct TreeNode* createIfNode(struct TreeNode* middle,struct TreeNode* left,struct TreeNode* right){

  char* content = (char*)malloc(sizeof(char)*100);
  // ---------------------- HANDLES MAPPING --------------- ONLY UNDER ARITHMETIC OPERATORS AND LOGICAL OPERATORS
  strcat(content,middle->left->content);
  strcat(content,map(middle->op));
  strcat(content,middle->right->content);

  struct TreeNode* cached_val = get(hashmap,content);

   // ------------------- FINISH MAPPING -----------------------

  struct TreeNode* temp = (struct TreeNode*)malloc(sizeof(struct TreeNode));
  temp->val = -1;
  temp->string = NULL;
  temp->op = 14;
  temp->type = -1;
  temp->left = left;
  temp->middle = (cached_val)?cached_val:middle;
  temp->right = right;
  temp->Gsymbol = NULL;
  temp->content = (char*)malloc(sizeof(char)*100);
  strcpy(temp->content,content);


  // CHECK IF SATISFIABLE

  if( middle ){ 
    if(!typeSatisfied(temp)){
      printf("If Condition : Type not matching.\n");
      exit(1);
    }
        
  }

  if( !cached_val ){
    insert(hashmap,temp->content,temp);
  }

  return temp;

}

// ---------------------------------------------------------------------------------------------------------------- CREATE NODE FOR WHILE STATEMENTS

struct TreeNode* createWhileNode(int op,struct TreeNode* left,struct TreeNode* right){

  char* content = (char*)malloc(sizeof(char)*100); 
  // ---------------------- HANDLES MAPPING --------------- ONLY UNDER ARITHMETIC OPERATORS AND LOGICAL OPERATORS
  strcat(content,left->left->content);
  strcat(content,map(left->op));
  strcat(content,left->right->content);

  struct TreeNode* cached_val = get(hashmap,content);
  // ------------------- FINISH MAPPING -----------------------
  
  struct TreeNode* temp = (struct TreeNode*)malloc(sizeof(struct TreeNode));  
  temp->val = -1;
  temp->string = NULL;
  temp->op = op;
  temp->type = -1;
  temp->varname = NULL;
  temp->left = (cached_val)?cached_val:left;
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

  if( !cached_val ){
    insert(hashmap,temp->content,temp);
  }
  

  return temp;


}

// ------------------------------------------------------------------------------------------------------------------ INORDER TRAVERSAL

void Inorder(struct TreeNode* root){
  if(root == NULL){
    return;
  }
  Inorder(root->left);
  // IT IS A NUMBER
  if(root->val != -1 ){
    printf("[ %d ] ----- [ %p ] ----- \n",root->val,root);
  }
  // IT IS A STRING
  if(root->string != NULL ){
    printf("[ %s ] ------ [ %p ] ----- \n",root->string,root);
  }
  // IT IS AN OPERATOR
  else if(root->op != -1 ){
    printf("[ %s ] ----- [ %p ] ----- ",map(root->op),root);
    getDependencies(root);
  }
  // IT IS A VARIABLE
  else if( root->varname != NULL ){
    printf("[ %s ] ----- [ %p ] -----",root->varname,root);
    getDependencies(root);
  }

  Inorder(root->middle);
  Inorder(root->right);
}

// ------------ ADDING DEPENDENCIES

void addDependencies(struct TreeNode* temp,struct TreeNode* left,struct TreeNode* right){
  // add left to temp's dependencies
  int idx = 0;
  if( left->dependencies ){
    int i = 0;

    while( left->dependencies[i] != NULL ){

      // -------------------------------------- checking duplicate
      int dup = 0;
      for(int j=0;j<idx;j++){
        if( strcmp(temp->dependencies[j],left->dependencies[i]) == 0 ){
          dup = 1;
          break;
        }
        j++;
      }
      if( dup ){
        i++;
        continue;
      }
      // --------------------------------------- finished checking
      temp->dependencies[idx] = (char*)malloc(sizeof(char)*10);
      strcpy(temp->dependencies[idx],left->dependencies[i]);
      idx++;
      i++;
    }
  }
  // add right to temp's dependencies
  if( right->dependencies ){
    int i = 0;
    while(right->dependencies[i] != NULL ){
      // -------------------------------------- checking duplicate
      int dup = 0;
      for(int j=0;j<idx;j++){
        if( strcmp(temp->dependencies[j],right->dependencies[i]) == 0 ){
          dup = 1;
          break;
        }
        j++;
      }
      if( dup ){
        i++;
        continue;
      }
      // -------------------------------- finished checking
      
      temp->dependencies[idx] = (char*)malloc(sizeof(char)*10);      
      strcpy(temp->dependencies[idx],right->dependencies[i]);
      idx++;
      i++;
    }
  }


}

void getDependencies(struct TreeNode* root){
  if( root->dependencies ){
    int i = 0;
    printf("[ ");
    while( root->dependencies[i] != NULL ){
      printf("%s, ",root->dependencies[i]);
      i++;
    }
    printf("]\n");
  }
}
