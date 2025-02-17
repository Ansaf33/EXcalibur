#include "reghandling.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "AST.h"
#include "symbol_table/symbol.h"

// define the maximum limit of registers R0 to R19
#define NOR 20

// INITIALLY, NO REGISTERS ARE USED
static int highestUsedReg = -1;
// INITIALLY, NO LABELS ARE USED
static int highestUsedLabel = -1;

// --------------------------------------------------------- GET SYMBOL ADDRESS

int getSymbolAddress(struct TreeNode* root){
  return root->symbol->address;
}

// --------------------------------------------------------- GET LABEL FUNCTION

int getLabel(void){
  ++highestUsedLabel;
  return highestUsedLabel;
}


// ---------------------------------------------------------- GET REG FUNCTION

int getReg(void){
  // FIRST CHECK IF ANY REGISTER IS FREE
  if( highestUsedReg == NOR - 1 ){
      printf("Cannot allocate more registers.");
      exit(1);
  }
  return ++highestUsedReg;

}

// ----------------------------------------------------------- FREE REG FUNCTION

void freeReg(){
  // CHECK IF ALL REGISTERS ARE FREE
  if( highestUsedReg >= 0 ){
    --highestUsedReg;
  }
  // IF ALL REGISTERS ARE ALREADY FREE
  else{
    printf("All registers already free.");
  }

}

// ----------------------------------------------------------- PRINTING SOMETHING IN CONSOLE 

void getInput(FILE* f,char* s){


  fprintf(f,"MOV SP, %d\n",4500);

  // pushing "write"
  int r1 = getReg();
  fprintf(f,"MOV R%d, \"write\"\n",r1);
  fprintf(f,"PUSH R%d\n",r1);
  freeReg();

  // pushing -1 -> argument 1
  r1 = getReg();
  fprintf(f,"MOV R%d, -2\n",r1);
  fprintf(f,"PUSH R%d\n",r1);
  freeReg();

  // pushing data -> argument 2
  r1 = getReg();
  fprintf(f,"MOV R%d, \"%s\"\n",r1,s);
  fprintf(f,"PUSH R%d\n",r1);
  freeReg();

  // pushing empty register -> argument 3
  r1 = getReg();
  fprintf(f,"PUSH R%d\n",r1);
  freeReg();

  // pushing register for return value
  r1 = getReg();
  fprintf(f,"PUSH R%d\n",r1);
  freeReg();

  // CALL
  fprintf(f,"CALL 0\n");

  // POPPING REGISTERS
  r1 = getReg();
  fprintf(f,"POP R%d\n",r1);
  fprintf(f,"POP R%d\n",r1);
  fprintf(f,"POP R%d\n",r1);
  fprintf(f,"POP R%d\n",r1);
  fprintf(f,"POP R%d\n",r1);
  freeReg();

}


// --------------------------------------------------------- CODE GENERATION FOR ARITHMETIC EXPRESSIONS ( E )

int arithmetic_expression_codeGen(FILE* f,struct TreeNode* root){

  // IF AT LEAF NODE, THEN ONLY NUMBER OR CONSTANT
  if(root->left == NULL && root->right == NULL){
    int regIdx = getReg();
    // IF NUMBER, MOVE THE NUMBER TO REGISTER
    if(root->val != -1 ){
      fprintf(f,"MOV R%d, %d\n",regIdx,root->val);
    }
    // IF STRING, MOVE THE STRING TO REGISTER
    if(root->string != NULL ){
      fprintf(f,"MOV R%d, %s\n",regIdx,root->string);
    }
    // IF VARIABLE, MOVE FROM MEMORY LOCATION TO REGISTER
    else if(root->varname != NULL){
      int memlocation = getSymbolAddress(root);
      fprintf(f, "MOV R%d, [%d]\n",regIdx,memlocation);
    }


    return regIdx;
  }

  int lReg = arithmetic_expression_codeGen(f,root->left);
  int rReg = arithmetic_expression_codeGen(f,root->right);

  switch(root->op){
      case 0:
        fprintf(f,"ADD R%d, R%d\n",lReg,rReg);
        break;
      case 1:
        fprintf(f,"SUB R%d, R%d\n",lReg,rReg);
        break;
      case 2:
        fprintf(f,"MUL R%d, R%d\n",lReg,rReg);
        break;
      case 3:
        fprintf(f,"DIV R%d, R%d\n",lReg,rReg);
        break;
  }

  // freeing the right register
  freeReg();

  // ONE REGISTER IS USED, MUST BE FREED IN STATEMENTS THAT CALL THIS FUNCTION
  return lReg;

}

// -------------------------------------------------------------------- CODE GENERATION FOR BOOLEAN EXPRESSIONS ( E < E )

int boolean_expression_codeGen(FILE* f,struct TreeNode* root){

  // BOOLEAN EXPRESSIONS ARE OF THE FORM E < E SO EVALUATE BOTH OF THEM FIRST
  int lReg = arithmetic_expression_codeGen(f,root->left);
  int rReg = arithmetic_expression_codeGen(f,root->right);

  switch(root->op){
    case 5:
      fprintf(f,"LT R%d, R%d\n",lReg,rReg);
      break;
    case 6:
      fprintf(f,"LE R%d, R%d\n",lReg,rReg);
      break;
    case 7:
      fprintf(f,"GT R%d, R%d\n",lReg,rReg);
      break;
    case 8:
      fprintf(f,"GE R%d, R%d\n",lReg,rReg);
      break;
    case 9:
      fprintf(f,"NE R%d, R%d\n",lReg,rReg);
      break;
    case 10:
      fprintf(f,"EQ R%d, R%d\n",lReg,rReg);
      break;

  }

  // free rReg
  freeReg();

  // lReg contains either 0 (if false) or 1 (if true)
  return lReg;

}


// -------------------------------------------------------------------- CODE GENERATION FOR ASSIGNMENTS ( a = 3*4+7 )

void assignment_codeGen(FILE* f,struct TreeNode* root){



  // Get address from symbol table
  int memAddress = getSymbolAddress(root->left);

  // store the register contents in the memory location 'storeIn'
  int r1 = getReg();
  
  // move the memory location to a register
  fprintf(f,"MOV R%d, %d\n",r1,memAddress);


  int fReg = arithmetic_expression_codeGen(f,root->right);

  // move the contents of fReg to memory location specified by r1
  fprintf(f,"MOV [R%d], R%d\n",r1, fReg);

  // freeing expression register
  freeReg();
  // freeing r1
  freeReg();


  

}

// ----------------------------------------------------------------------CODE GENERATION FOR READ OPERATIONS

void read_codeGen(FILE* f,struct TreeNode* root){

  int memAddress = getSymbolAddress(root->left);


  // STACK POINTER
  fprintf(f,"MOV SP, %d\n",4500);

  // pushing "read"
  int r1 = getReg();
  fprintf(f,"MOV R%d, \"read\"\n",r1);
  fprintf(f,"PUSH R%d\n",r1);
  freeReg();

  // pushing -1 -> argument 1
  r1 = getReg();
  fprintf(f,"MOV R%d, -1\n",r1);
  fprintf(f,"PUSH R%d\n",r1);
  freeReg();

  // pushing buffer -> argument 2
  r1 = getReg();
  fprintf(f,"MOV R%d, %d\n",r1,memAddress);
  fprintf(f,"PUSH R%d\n",r1);
  freeReg();

  // pushing empty register -> argument 3
  r1 = getReg();
  fprintf(f,"PUSH R%d\n",r1);
  freeReg();

  // pushing register for return value
  r1 = getReg();
  fprintf(f,"PUSH R%d\n",r1);
  freeReg();

  // CALL
  fprintf(f,"CALL 0\n");

  // POPPING REGISTERS
  r1 = getReg();
  fprintf(f,"POP R%d\n",r1);
  fprintf(f,"POP R%d\n",r1);
  fprintf(f,"POP R%d\n",r1);
  fprintf(f,"POP R%d\n",r1);
  fprintf(f,"POP R%d\n",r1);
  freeReg();



}

// --------------------------------------------------------- CODE GENERATION FOR WRITE ASSIGNMENTS

void write_codeGen(FILE* f,struct TreeNode* root){
  

  // stack pointer
  fprintf(f,"MOV SP, %d\n",4500);

  // pushing "write"
  int r1 = getReg();
  fprintf(f,"MOV R%d, \"write\"\n",r1);
  fprintf(f,"PUSH R%d\n",r1);
  freeReg();

  // pushing -1 -> argument 1
  r1 = getReg();
  fprintf(f,"MOV R%d, -2\n",r1);
  fprintf(f,"PUSH R%d\n",r1);
  freeReg();

  // pushing R -> argument 2 (data)
  int R = arithmetic_expression_codeGen(f,root->left);
  fprintf(f,"PUSH R%d\n",R);
  // freeing the register storing the result of the expression
  freeReg();

  // pushing empty register -> argument 3
  r1 = getReg();
  fprintf(f,"PUSH R%d\n",r1);
  freeReg();

  // pushing register for return value
  r1 = getReg();
  fprintf(f,"PUSH R%d\n",r1);
  freeReg();

  // CALL
  fprintf(f,"CALL 0\n");

  // POPPING REGISTERS
  r1 = getReg();
  fprintf(f,"POP R%d\n",r1);
  fprintf(f,"POP R%d\n",r1);
  fprintf(f,"POP R%d\n",r1);
  fprintf(f,"POP R%d\n",r1);
  fprintf(f,"POP R%d\n",r1);
  freeReg();


}

// ------------------------------------------------------------- CODE GENERATION FOR IF STATEMENTS

void if_codeGen(FILE* f,struct TreeNode* root,int bl,int cl){

  int boolReg = boolean_expression_codeGen(f,root->middle);
  int l0 = getLabel();
  int l1 = getLabel();

  // if the boolean expression is false, go to else condition
  fprintf(f,"JZ R%d, L%d\n",boolReg,l0);
  // no need for the register storing the result of the boolean expression anymore
  freeReg();

  // code if 'if' statement is true
  codeGen(f,root->left,bl,cl);

  // jump to the end
  fprintf(f,"JMP L%d\n",l1);

  // code if 'if' statement is false (ie else condition)
  fprintf(f,"L%d:\n",l0);
  codeGen(f,root->right,bl,cl);

  fprintf(f,"L%d:\n",l1);

}

// ------------------------------------------------------------- CODE GENERATION FOR WHILE LOOPS

void while_codeGen(FILE* f,struct TreeNode* root){

  int l0 = getLabel();
  int l1 = getLabel();

  // BREAK LABEL AND CONTINUE LABEL
  int breakLabel = l1;
  int continueLabel = l0;

  // code for going through while loop again
  fprintf(f,"L%d:\n",l0);
  int boolReg = boolean_expression_codeGen(f,root->left);

  // if false, go out of the loop of l1
  fprintf(f,"JZ R%d, L%d\n",boolReg,l1);
  freeReg();

  // generate code for body of while loop
  codeGen(f,root->right,breakLabel,continueLabel);

  // go to while loop again 
  fprintf(f,"JMP L%d\n",l0);

  // exit while loop 
  fprintf(f,"L%d:\n",l1);

  
}

// ------------------------------------------------------------- CODE GENERATION FOR BREAK STATEMENT

void break_codeGen(FILE* f,struct TreeNode* root,int label){
  if(label!=-1){
   fprintf(f,"JMP L%d\n",label);
  }

}

// ------------------------------------------------------------- CODE GENERATION FOR CONTINUE STATEMENT

void continue_codeGen(FILE* f,struct TreeNode* root,int label){
  if(label!=-1){
    fprintf(f,"JMP L%d\n",label);
  }

}

// ------------------------------------------------------------- CODE GENERATION FOR REPEAT STATEMENT

void repeat_codeGen(FILE* f,struct TreeNode* root){


  int l0 = getLabel();
  int l1 = getLabel();
  int bl = l1;
  int cl = l0;

  // STARTING POINT
  fprintf(f,"L%d:\n",l0);

  // CODE FOR STATEMENT LIST
  codeGen(f,root->right,bl,cl);

  // CONDITION
  int boolReg = boolean_expression_codeGen(f,root->left);

  // IF IT IS FALSE, GO BACK TO L0
  fprintf(f,"JZ R%d, L%d\n",boolReg,l0);
  freeReg();

  // IF IT IS TRUE, GO OUTSIDE
  fprintf(f,"L%d:\n",l1);

}

// ------------------------------------------------------------- CODE GENERATION FOR DO-WHILE STATEMENT

void dowhile_codeGen(FILE* f,struct TreeNode* root){

  int l0 = getLabel();
  int l1 = getLabel();
  int bl = l1;
  int cl = l0;

  // STARTING POINT
  fprintf(f,"L%d:\n",l0);

  // CODE FOR STATEMENT LIST
  codeGen(f,root->right,bl,cl);

  // CONDITION
  int boolReg = boolean_expression_codeGen(f,root->left);

  // IF IT IS TRUE, GO BACK TO L0, EXECUTE WHILE LOOP AGAIN
  fprintf(f,"JNZ R%d, L%d\n",boolReg,l0);
  freeReg();

  // IF IT IS FALSE, EXIT THE WHILE LOOP
  fprintf(f,"L%d:\n",l1);
  

}

// ------------------------------------------------------------- BOOLEAN FUNCTIONS

bool isAssignment(struct TreeNode* root){
    return root->op == 4;
}
bool isRead(struct TreeNode* root){
    return root->op == 11;
}
bool isWrite(struct TreeNode* root){
    return root->op == 12;
}
bool isArithmeticExpression(struct TreeNode* root){
    return root->op >= 0 && root->op <= 3;
}
bool isBooleanExpression(struct TreeNode* root){
    return root->op >= 5 && root->op <= 10;
}
bool isIf(struct TreeNode* root){
    return root->op == 14;
}
bool isWhile(struct TreeNode* root){
    return root->op == 15;
}
bool isBreak(struct TreeNode* root){
    return root->op == 16;
}
bool isContinue(struct TreeNode* root){
    return root->op == 17;
}
bool isRepeat(struct TreeNode* root){
    return root->op == 18;
}
bool isDoWhile(struct TreeNode* root){
    return root->op == 19;
}


// ------------------------------------------------------------------- MAIN CODEGEN FUNCTION

void codeGen(FILE* f,struct TreeNode* root,int bl,int cl){
    if(root == NULL){
      return;
    }
    switch(root->op){
    // ASSIGNMENT STATEMENT
    case 4:
          assignment_codeGen(f,root);
          break;
    // READ STATEMENT
    case 11:
          read_codeGen(f,root);
          break;
    // WRITE STATEMENT
    case 12:
          write_codeGen(f,root);
          break;
    // STATEMENT S
    case 13:
          codeGen(f,root->left,bl,cl);
          codeGen(f,root->right,bl,cl);
          break;
    // IF STATEMENT
    case 14:
          if_codeGen(f,root,bl,cl);
          break;
    // WHILE STATEMENT
    case 15: 
          while_codeGen(f,root);
          break;
    // BREAK STATEMENT
    case 16:
          break_codeGen(f,root,bl);
          break;
    // CONTINUE STATEMENT
    case 17:
          continue_codeGen(f,root,cl);
          break;
    // REPEAT UNTIL STATEMENT
    case 18:
          repeat_codeGen(f,root);
          break;
    // DO WHILE STATEMENT
    case 19:
          dowhile_codeGen(f,root);
          break;
    }
}

