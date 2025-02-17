%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"
#include "AST.h"


%}




%%

[0-9]+ {
  yylval.string = (char*)malloc(sizeof(char)*16);
  strcpy(yylval.string,yytext);
  return NUM;
}

"decl" {
  return DECL;
}

"enddecl" {
  return ENDDECL;
}

"int" {
  yylval.integer = 0;
  return INT;
}

"str" {
  yylval.integer = 2;
  return STR;
}

"begin" {
  return BEG;
}

"end" {
  return END;
}

"read" {
  return READ;
}

"write" {
  return WRITE;
}

"if" {
  return IF;
}

"then" {
  return THEN;
}

"else" {
  return ELSE;
}

"endif" {
  return ENDIF;
}

"while" {
  return WHILE;
}

"do" {
  return DO;
}

"endwhile" {
  return ENDWHILE;
}

"break" {
  return BREAK;
}

"continue" {
  return CONTINUE;
}

"repeat" {
  return REPEAT;
}

"until" {
  return UNTIL;
}

[+] {
  return PLUS;
}

[-] {
  return MINUS;
}

[*] {
  return MUL;
}

[/] {
  return DIV;
}

[=] {
  return EQUALS;
}

[<] {
  return LT;
}

"<=" {
  return LTE;
}

[>] {
  return GT;
}

">=" {
  return GTE;
}

"==" {
  return EQ;
}

"!=" {
  return NEQ;
}

[a-zA-Z][a-zA-Z0-9]* {
  yylval.string = (char*)malloc(sizeof(char)*100);
  strcpy(yylval.string,yytext);
  //yylval.node = createIdTree(0,yytext);
  return ID;
}

["][a-zA-Z0-9_ !@#$%^&*()]*["] {
  yylval.string = (char*)malloc(sizeof(char)*100);
  strcpy(yylval.string,yytext);

  return STRING;
}


[ \t] ;


[(] {
  return '(';
}
[)] {
  return ')';
}
[;] {
  return ';';
}
. {
  return yytext[0];
}


%%


int yywrap(){
  return 1;
}
