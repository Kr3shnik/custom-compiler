%{

#include <stdio.h>
int yylex();
int yyerror(const char *s);

%}

%define parse.error verbose

%token IDENT
%token NUMBER
%token FUNC
%token RPAR
%token LPAR
%token RCUR
%token LCUR
%token PRINT
%token SEMICOLON
%token INT_KEYWORD
%token EQUAL
%token PLUS

%start program


%%

program:    functions {printf("program -> functions\n");}

functions:  function functions {printf("function -> function functions\n");}
         |  %empty {printf("functions -> empty\n");}

function:   FUNC IDENT LPAR RPAR LCUR statements RCUR {printf("function -> FUNC IDENT LPAR RPAR LCUR RCUR\n");}

statements: statement statements {printf("statements -> statement statement\n");}
         | %empty {printf("statements -> %empty\n");}

statement: PRINT LPAR variable RPAR SEMICOLON {printf("statement -> PRINT LPAR variable RPAR\n");}
        | IDENT EQUAL variable PLUS variable SEMICOLON {printf("statement -> IDENT EQUAL variable PLUS variable SEMICOLON\n");}
        | IDENT EQUAL variable SEMICOLON {printf("statement -> IDENT EQUAL variable SEMICOLON\n");}
        | INT_KEYWORD IDENT SEMICOLON {printf("statement -> INT_KEYWORD IDENT SEMICOLON\n");}
        

variable: IDENT {printf("variable -> IDENT\n");}
        | NUMBER {printf("variable -> NUMBER\n");}
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