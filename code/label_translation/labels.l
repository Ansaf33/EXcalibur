%{
#include <stdio.h>
#include <stdlib.h>
#include "table.h"
int run;
int yyerror(void);
extern FILE* yyin;
FILE* f;
// 2056 + 2*(instruction_number-1)

int in;
%} 


%%


[L][0-9]+":""\n" {

  // THIS IS THE LINE WE WANT TO REPLACE WITH THE JMP STATEMENT (EASY TO IMPLEMENT)

  // Get the label

  char* label = (char*)malloc(sizeof(char)*3);
  int i = 0;
  while( yytext[i] != ':' ){
    label[i] = yytext[i];
    i++;
  }

  // Calculate address of the next line 

  int address = 2056 + 2*(in-8);
  add(label,address);

  // since newline, instruction number increases
  in++;

  // in the second run, replace the line by this one.
  if( run == 1 ){
      fprintf(f,"JMP %d\n",address);
  }

}

[L][0-9]+"\n" {

  // THIS IS THE LINE TYPICALLY FOUND WITH "JNZ R1 L0" TYPE STATEMENTS

  // get the label

  char* label = (char*)malloc(sizeof(char)*3);
  int i = 0;
  while( yytext[i] != '\n' ){
    label[i] = yytext[i];
    i++;
  }

  // in the second run, replace the line by this one
  if(run == 1 ){
     fprintf(f,"%d\n",getAddress(label));
  }

  // since newline, instruction number increases

  in++;

}


[\n] {
  in++;
  if( run == 1 ){
      fprintf(f,"\n");
  }
}


. {
  if( run == 1 ){
      fprintf(f,"%s",yytext);
  }

}

%%

int yywrap(){
  return 1;
}

int main(int argc,char* argv[]){

  // first run, fill the array with labels and corresponding addresses
  in = 1;
  run = 0;
  yyin = fopen(argv[1],"r");
  yylex();
  //printTable();

  // second run, replace the labels with addresses

  in = 1;
  f = fopen(argv[2],"w");
  yyin = fopen(argv[1],"r");
  run++;
  yylex();

  return 0;
}

