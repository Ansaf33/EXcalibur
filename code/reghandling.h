#ifndef reghandling_h
#define reghandling_h

#include <stdio.h>
#include <stdbool.h>
#include "AST.h"

// GETTING THE SMALLEST FREE REGISTER


int getReg(void);

// FREEING THE HIGHEST OCCUPIED REGISTER

void freeReg(void);

// GETTING LABEL

int getLabel(void);

// PRINT OUT SOMETHING IN CONSOLE

void getInput(FILE* f,char* s);

// code generation for arithmetic expressions -> returns the register with the final value 

int arithmetic_expression_codeGen(FILE* f, struct TreeNode* root);

// code generation for boolean expressions -> returns the register containing 0 or 1

int boolean_expression_codeGen(FILE* f,struct TreeNode* root);

// code generation for assignments -> ID = E
// call expression_codeGen on right subtree, we get the register with the final value
// now we need to move the contents of that register to a memory location using MOV

void assignment_codeGen(FILE* f, struct TreeNode* root);

// code generation for read
// LHS contains the variable character
// memory address to be read into = 4095 + (int)(character)
// perform read operation using read system call


void read_codeGen(FILE* f,struct TreeNode* root);


// code generation for write
// LHS contains an expression, so call expression_codeGen on Left Subtree first
// result is stored in a register
// simple write the contents of that register

void write_codeGen(FILE* f,struct TreeNode* root);

// CODE GEN FOR IF STATEMENT

void if_codeGen(FILE* f,struct TreeNode* root,int bl,int cl);

// CODE GEN FOR WHILE STATEMENT

void while_codeGen(FILE* f,struct TreeNode* root);

// CODE GEN FOR BREAK STATEMENT

void break_codeGen(FILE* f,struct TreeNode* root,int label);

// CODE GEN FOR CONTINUE STATEMENT

void continue_codeGen(FILE* f,struct TreeNode* root,int label);

// CODE GEN FOR REPEAT STATEMENT

void repeat_codeGen(FILE* f,struct TreeNode* root);

// CODE GEN FOR DO WHILE STATEMENTS

void dowhile_codeGen(FILE* f,struct TreeNode* root);

// codeGen for entire statement lists

void codeGen(FILE* f,struct TreeNode* root,int bl,int cl);


// boolean return values if the root value is an assignment/read/write/expression node

bool isAssignment(struct TreeNode* root);

bool isRead(struct TreeNode* root);

bool isWrite(struct TreeNode* root);

bool isArithmeticExpression(struct TreeNode* root);

bool isBooleanExpression(struct TreeNode* root);

bool isIf(struct TreeNode* root);

bool isWhile(struct TreeNode* root);

bool isBreak(struct TreeNode* root);

bool isContinue(struct TreeNode* root);

bool isRepeat(struct TreeNode* root);

bool isDoWhile(struct TreeNode* root);


#endif
