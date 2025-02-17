#include <stdio.h>
#include <stdlib.h>

char* optable[20] = {"+","-","*","/","=","<","<=",">",">=","!=","==","READ","WRITE","STATEMENT","IF","WHILE","BREAK","CONTINUE","REPEAT","DO WHILE"};


char* map(int i){
  return optable[i];
}
