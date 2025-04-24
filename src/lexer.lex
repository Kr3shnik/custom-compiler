%{
#include <stdio.h>
#include <string>
#include "../include/ASTNodes.h"
#include "parser.tab.h"

// Utility function to create string values
std::string* createString(const char* text) {
    return new std::string(text);
}
%}

%%
"while"                  { return WHILE; }
[a-zA-Z_][a-zA-Z0-9_]*  { yylval.stringVal = createString(yytext); return IDENT; }
[0-9]+\.[0-9]*          { yylval.floatVal = atof(yytext); return FLOAT; }
[0-9]+                  { yylval.intVal = atoi(yytext); return NUMBER; }
"("                     { return LPAR; }
")"                     { return RPAR; }
"{"                     { return LCUR; }
"}"                     { return RCUR; }
","                     { return COMMA; }
"="                     { return EQUAL; }
"=="                    { return CEQ; }
"!="                    { return CNE; }
"<"                     { return CLT; }
"<="                    { return CLE; }
">"                     { return CGT; }
">="                    { return CGE; }
"+"                     { return PLUS; }
"-"                     { return MINUS; }
"*"                     { return MUL; }
"/"                     { return DIV; }
";"                     { return SEMICOLON; }

[ \t\n\r]              {}
.                      { printf("Unknown token:%s\n", yytext); yyterminate(); }
%%

int yywrap() { return 1; }