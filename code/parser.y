%{
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "AST.h"
#include "reghandling.h"
#include "evaluator/evaluator.h"
#include "symbol_table/Gsymbol.h"
#include "symbol_table/varList.h"
#include "map_implementation/new_map.h"


struct TreeNode* root;


extern FILE* yyin;

int yylex(void);
void yyerror(char* s);



%}

%union{
  struct TreeNode* node;
  char* string;
  int integer;
  struct list* list;



}
%type<list> VARLIST;
%type<integer> TYPE
%type<node> E ASSG INPUT OUTPUT S SL IFST WHILEST REPEATST DOWHILEST IDENTIFIER CONSTANT
%token STRING ID NUM PLUS MINUS MUL DIV EQUALS 
%token LT LTE GT GTE EQ NEQ 
%token READ WRITE END BEG 
%token IF THEN ELSE ENDIF WHILE DO ENDWHILE BREAK CONTINUE REPEAT UNTIL
%token DECL ENDDECL INT STR
%left EQ NEQ
%left LT LTE GT GTE
%left PLUS MINUS
%left MUL DIV



%%

PROGRAM :
        DECLARATIONS P

DECLARATIONS :
             DECL DL ENDDECL{
                printf("All declarations parsed.\n");
                getAll();
              }
             |
             DECL ENDDECL
             ;

DL :
        DL D
        |
        D
        ;

D :
     TYPE VARLIST ';' {
        addAllSymbols($2,$1,1);
     }
     ;

TYPE :
     INT {
     $$ = $<integer>1;
     }
     |
     STR {
     $$ = $<integer>1;
     }
     ;

VARLIST :
        VARLIST ',' ID {
          $$ = addVariable($1,$<string>3);
        }
        |
        ID {
          $$ = addVariable(NULL,$<string>1);
        }
        ;

P :
  BEG SL END ';' {
    root = $2;
    printf("Valid Program.\n");
    Inorder($2);

  }
  |
  BEG END ';' {
  printf("Valid program.\n");
  }
  ;

SL :
   SL S  {
   $$ = createOpNode(-1,13,$1,$2);
  }
  |
   S  {
    $$ = $1;
  }
   ;

S :
  ASSG ';'
  |
  INPUT ';'
  |
  OUTPUT ';'
  |
  IFST ';'
  |
  WHILEST ';'
  |
  REPEATST ';'
  |
  DOWHILEST ';'
  |
  BREAK ';' {
    $<node>$ = createOpNode(-1,16,NULL,NULL);
  }
  |
  CONTINUE ';' {
    $<node>$ = createOpNode(-1,17,NULL,NULL);
  }
  ;

IFST :
     IF '(' E ')' THEN SL ELSE SL ENDIF {
      $$ = createIfNode($3,$6,$8);
    }
    |
    IF '(' E ')' THEN SL ENDIF {
      $$ = createIfNode($3,$6,NULL);
    }
     ;

WHILEST :
        WHILE '(' E ')' DO SL ENDWHILE {
        $$ = createWhileNode(15,$3,$6);
        }
        ;

REPEATST :
         REPEAT SL UNTIL '(' E ')' {
         $$ = createWhileNode(18,$5,$2);
        }
        ;

DOWHILEST :
          DO SL WHILE '(' E ')' { 
          $$ = createWhileNode(19,$5,$2);
          }
          ;


ASSG :
  IDENTIFIER EQUALS E {
  $$ = createOpNode(-1,4,$<node>1,$3);
  }
  ;

E :
  E PLUS E {
  $$ = createOpNode(0,0,$1,$3);
  }
  |
  E MINUS E {
  $$ = createOpNode(0,1,$1,$3);
  }
  |
  E MUL E {
  $$ = createOpNode(0,2,$1,$3);
  }
  |
  E DIV E {
  $$ = createOpNode(0,3,$1,$3);
  }
  |
  E LT E {
  $$ = createOpNode(1,5,$1,$3);
  }
  |
  E LTE E {
  $$ = createOpNode(1,6,$1,$3);
  }
  |
  E GT E {
  $$ = createOpNode(1,7,$1,$3);
  }
  |
  E GTE E {
  $$ = createOpNode(1,8,$1,$3);
  }
  |
  E NEQ E {
  $$ = createOpNode(1,9,$1,$3);
  }
  |
  E EQ E {
  $$ = createOpNode(1,10,$1,$3);
  }
  |
  '(' E ')' {
  $$ = $2;
  }
  |
  CONSTANT
  |
  IDENTIFIER
  ;

IDENTIFIER : 
           ID { 
            $$ = createIdNode($<string>1);
          }

CONSTANT :
         NUM {
          $$ = createNumNode(atoi($<string>1));
          }
          |
         STRING {
          $$ = createStringNode($<string>1);
          }
          ;
        

INPUT :
       READ '(' IDENTIFIER ')' {
       $$ = createOpNode(-1,11,$<node>3,NULL);
      }
       ;

OUTPUT :
       WRITE '(' E ')' {
        $$ = createOpNode(-1,12,$3,NULL);
      }
       ;




%%









int main(int argc, char* argv[]){


// --------------------------------- PARSING INPUT 
  FILE* f = fopen(argv[1],"r");
  yyin = f;

  bool initialized = init_hashmap();
  yyparse();



// --------------------------------- ASSEMBLY CODE



  FILE* xsm = fopen(argv[2],"w");
  fprintf(xsm,"%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n",0,2056,0,0,0,0,0,0);
  //fprintf(xsm,"BRKP\n");
  codeGen(xsm,root,-1,-1);
  fprintf(xsm,"INT 10\n");



// --------------------------------- EXERCISE 1

/*

  printf("RUNNING EXERCISE1\n");
  evaluate(root);
  getDetails();

*/

  return 0;
}

void yyerror(char* s){
  printf("ERROR:%s\n",s);
}

