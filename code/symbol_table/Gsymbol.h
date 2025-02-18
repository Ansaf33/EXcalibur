#ifndef Gsymbol_H

#define Gsymbol_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct Gsymbol{
  char* name;
  int type;
  int size;
  int address;
  struct Gsymbol* next;

};


// CREATING A NODE

struct Gsymbol* createNode(char* name,int type,int size);

// ADDING IT TO LL

void addGSymbol(char* name,int type,int size);


// LOOKUP DETAILS BASED ON NAME

struct Gsymbol* lookUp(char* name);

// CHECK IF DECLARED, IF NO, THEN EXIT

bool checkIfDeclared(char* name);

// PRINTING DETAILS

void getAll(void);

#endif
