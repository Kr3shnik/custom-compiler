%{
#define YYDEBUG 1
extern int yydebug;
#include <stdio.h>
int yylex();
int yyerror(const char *s);

%}

%define parse.error verbose

%token IDENT NUMBER FLOAT
%token RPAR LPAR RCUR LCUR
%token EQUAL
%token COMMA SEMICOLON
%token PLUS MINUS MUL DIV CEQ CNE CLT CLE CGT CGE

%start program


%%

program:        stms

stms:           stmt
            |   stms stmt

stmt :          var_decl SEMICOLON
            |   func_decl
            |   expr SEMICOLON

block:          LCUR stms RCUR
            |   LCUR RCUR

var_decl:       ident ident
            |   ident ident EQUAL expr

func_decl:      ident ident LPAR func_param RPAR block

func_param:     /* blank */
            |   var_decl
            |   func_param COMMA var_decl

ident:          IDENT

numeric:        NUMBER
            |   FLOAT

assign:         ident EQUAL expr

expr:           ident EQUAL expr
            |   ident LPAR call_args RPAR
            |   ident
            |   numeric
            |   expr comparison expr
            |   LPAR expr RPAR
            |   assign

call_args:      /* blank */
            |   expr
            |   call_args COMMA expr

comparison:     CEQ | CNE | CLT | CLE | CGT | CGE
            |   PLUS | MINUS | MUL | DIV



%%

int main(void){
    yydebug = 1;
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