#ifndef SYMBOL_H

#define SYMBOL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct symbol{
  char* name;
  int type;
  int size;
  int address;
  struct symbol* next;

};


// CREATING A NODE

struct symbol* createNode(char* name,int type,int size);

// ADDING IT TO LL

void addSymbol(char* name,int type,int size);


// LOOKUP DETAILS BASED ON NAME

struct symbol* lookUp(char* name);

// CHECK IF DECLARED, IF NO, THEN EXIT

bool checkIfDeclared(char* name);

// PRINTING DETAILS

void getAll(void);

#endif
