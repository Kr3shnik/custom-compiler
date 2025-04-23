%{

#include <stdio.h>
#include "parser.tab.h"

%}

%%
[a-zA-Z_][a-zA-Z0-9_]*  {return IDENT;}
[0-9]+\.[0-9]*		{return FLOAT;}
[0-9]+              {return NUMBER;}
"("                 {return LPAR;}
")"                 {return RPAR;}
"{"                 {return LCUR;}
"}"                 {return RCUR;}
","                 {return COMMA;}
"="                 {return EQUAL;}
"=="				{return CEQ;}
"!="                {return CNE;}
"<"                 {return CLT;}
"<="                {return CLE;}
">"                 {return CGT;}
">="                {return CGE;}
"+"                 {return PLUS;}
"-"                 {return MINUS;}
"*"                 {return MUL;}
"/"                 {return DIV;}
";"                 {return SEMICOLON;}

[ \t\n\r]           {}
.					{printf("Unknown token:%s\n", yytext); yyterminate();}
%%

int yywrap() { return 1; }