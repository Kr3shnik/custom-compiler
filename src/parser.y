%{

#include <stdio.h>
int yylex();
void yyerror(const char *s);

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
program: FUNC IDENT LPAR RPAR LCUR RCUR {printf("program FUNC IDENT LPAR RPAR LCUR RCUR ");}
%%

int main(void){
    yyparse();
}

void yyerror(const char *s){
    printf("Error: %s\n",s);
}