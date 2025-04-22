%{

#include <stdio.h>
#include "parser.tab.h"

%}

%%
"print"     {return PRINT;}
"int"       {return INT_KEYWORD;}
"func"      {return FUNC;}
"("         {return LPAR;}
")"         {return RPAR;}
"{"         {return LCUR;}
"}"         {return RCUR;}
";"         {return SEMICOLON;}
"="         {return EQUAL;}
"+"         {return PLUS;}
[a-zA-Z]+   {return IDENT;}
[0-9]+      {return NUMBER;}
[ \t\n]     {}
%%

int yywrap() { return 1; }