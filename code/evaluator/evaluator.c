#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "evaluator.h"

static int variables[26];


static bool breakFlag = false;
static bool continueFlag = false;
// ---------------------------------- GETTING ADDRESS FROM A NODE

int getSymbolAddress(struct TreeNode* root){
  return root->Gsymbol->address;
}

// ---------------------------------- MAIN EVALUATION FUNCTION FOR STATEMENTS

void evaluate(struct TreeNode* root){

  if(root == NULL){
    return;
  }

  switch(root->op){

    // ASSIGNMENT

    case 4:
      int idx = getSymbolAddress(root->left) - 4096;
      variables[idx] = arithmetic_expressionEvaluator(root->right);
      break;

    // READ OP

    case 11:
      printf("Enter %s : ",root->left->varname);
      scanf("%d",&variables[getSymbolAddress(root->left)-4096]);
      break;

    // WRITE OP

    case 12:
      int num = arithmetic_expressionEvaluator(root->left);
      printf("Output : %d\n",num);
      break;   

    // STATEMENT

    case 13:
      evaluate(root->left);
      if( !breakFlag && !continueFlag ){
        evaluate(root->right);
      }
      break;

    // IF STATEMENT

    case 14:
      bool result = boolean_expressionEvaluator(root->middle);
      if( result ){
        evaluate(root->left);
      }
      else{
        evaluate(root->right);
      }
      break;

    // WHILE STATEMENT

    case 15:
      while(boolean_expressionEvaluator(root->left)){
        // if break is activated
        if( breakFlag ){
          breakFlag = false;
          break;
        }
        // if continue is activated
        if( continueFlag ){
          continueFlag = false;
          continue;
        }

        evaluate(root->right);
      }
      break;

    // BREAK STATEMENT

    case 16:
      breakFlag = true;
      break;

    // CONTINUE STATEMENT

    case 17:
      continueFlag = true;
      break;

    // REPEAT STATEMENT

    case 18:
      do {
        // activate brake
        if( breakFlag ){
          breakFlag = false;
          break;
        }
        // activate continue
        if( continueFlag ){
          continueFlag = false;
          continue;
        }
        evaluate(root->right);

      } while(!boolean_expressionEvaluator(root->left));

      break;

    // DO-WHILE STATEMENT

    case 19:
      do {

        if( breakFlag ){
          breakFlag = false;
          break;
        }
        if( continueFlag ){
          continueFlag = false;
          continue;
        }
        evaluate(root->right);

      } while(boolean_expressionEvaluator(root->left));

      break;
    

  }

 
}

// ------------------------------------ EVALUATION FUNCTION FOR ARITHMETIC EXPRESSIONS

int arithmetic_expressionEvaluator(struct TreeNode* root){

  if( root->left == NULL && root->right == NULL ){
    // IF IT IS A VARIABLE
    if( root->val == -1 ){
      return variables[getSymbolAddress(root)-4096];
    }
    // IF IT IS A CONSTANT
    else{
      return root->val;
    }
  }

  switch(root->op){
    case 0:
      return arithmetic_expressionEvaluator(root->left) + arithmetic_expressionEvaluator(root->right);
      break;

    case 1:
      return arithmetic_expressionEvaluator(root->left) - arithmetic_expressionEvaluator(root->right);
      break;

    case 2:
      return arithmetic_expressionEvaluator(root->left) * arithmetic_expressionEvaluator(root->right);
      break;

    case 3:
      return arithmetic_expressionEvaluator(root->left) / arithmetic_expressionEvaluator(root->right);
      break;

  } 

}

// -------------------------------------- EVALUATION FUNCTION FOR BOOLEAN EXPRESSIONS

bool boolean_expressionEvaluator(struct TreeNode* root){
  int left = arithmetic_expressionEvaluator(root->left);
  int right = arithmetic_expressionEvaluator(root->right);

  switch(root->op){
    case 5:
      return left < right;
    case 6:
      return left <= right;
    case 7:
      return left > right;
    case 8:
      return left >= right;
    case 9:
      return left != right;
    case 10:
      return left == right;
  }
  
  return 0;


}





int getDetails(){
  for(int i=0;i<26;i++){
    printf("%d = %d\n",i+4096,variables[i]);
  }
  return 0;
    

}

