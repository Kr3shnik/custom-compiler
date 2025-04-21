%{

#include <stdio.h>
#include "parser.tab.h"

%}

%%
[0-9]+      {return NUMBER;}
"func"      {return FUNC;}
"("         {return LPAR;}
")"         {return RPAR;}
"{"         {return LCUR;}
"}"         {return RCUR;}
[a-z]+      {return IDENT;}
[ \t\n]     {}
%%

int yywrap() { return 1; }