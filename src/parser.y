%{

#include <stdio.h>
int yylex();
int yyerror(const char *s);

%}

%token IDENT
%token NUMBER
%token FUNC
%token RPAR
%token LPAR
%token RCUR
%token LCUR
%start program

%%

program:    functions {printf("program -> functions\n");}

functions:  function functions {printf("function -> function functions\n");}
         |  %empty {printf("functions -> empty\n");}
        

function:   FUNC IDENT LPAR RPAR LCUR RCUR {printf("program FUNC IDENT LPAR RPAR LCUR RCUR\n");}
%%

int main(void){
    int result = yyparse();
    if (result == 0) {
        // Successfully parsed
        return 0;
    } else {
        // Parsing error occurred
        return 1;
    }
}

int yyerror(const char *s){
    printf("Error: %s\n",s);
    return 1;
}