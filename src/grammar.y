%{
extern int yydebug;
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "../include/ASTNodes.h"

int yylex();
int yyerror(const char *s);

// Root of the AST
std::unique_ptr<NProgram> programRoot;
%}

%define parse.error verbose

%union {
    std::string* stringVal;
    int intVal;
    float floatVal;
    Node* node;
    NStatement* stmt;
    NExpression* expr;
    NBlock* block;
    NIdentifier* ident;
    NVariableDeclaration* var_decl;
    NFunctionDeclaration* func_decl;
    NWhileStatement* while_stmt;
    std::vector<std::unique_ptr<NStatement>>* stmts;
    std::vector<std::unique_ptr<NExpression>>* exprs;
    std::vector<std::unique_ptr<NVariableDeclaration>>* var_decls;
}

%token <stringVal> IDENT
%token <intVal> NUMBER
%token <floatVal> FLOAT
%token RPAR LPAR RCUR LCUR
%token EQUAL
%token COMMA SEMICOLON
%token PLUS MINUS MUL DIV CEQ CNE CLT CLE CGT CGE
%token WHILE

%right EQUAL
%left CEQ CNE
%left CLT CLE CGT CGE
%left PLUS MINUS
%left MUL DIV
%type <node> program
%type <block> stms block
%type <stmt> stmt var_decl func_decl while_stmt
%type <expr> expr numeric
%type <ident> ident
%type <var_decls> func_param
%type <exprs> call_args
%type <intVal> comparison

%start program

%%

program:        stms                           { programRoot = std::make_unique<NProgram>(std::unique_ptr<NBlock>($1)); }

stms:           stmt                            { 
                                                    $$ = new NBlock(); 
                                                    static_cast<NBlock*>($$)->addStatement(std::unique_ptr<NStatement>($1)); 
                                                }
                | stms stmt                     { $1->addStatement(std::unique_ptr<NStatement>($2)); $$ = $1; }

stmt:           var_decl SEMICOLON              { $$ = $1; }
                | func_decl                     { $$ = $1; }
| while_stmt                    { $$ = $1; }
                | expr SEMICOLON                { $$ = new NExpressionStatement(std::unique_ptr<NExpression>($1)); }

block:          LCUR stms RCUR                  { $$ = $2; }
                | LCUR RCUR                     { $$ = new NBlock(); }

var_decl:       ident ident                     {   $$ = new NVariableDeclaration(  std::unique_ptr<NIdentifier>($1), 
                                                                                    std::unique_ptr<NIdentifier>($2));
                                                }
                | ident ident EQUAL expr       {    $$ = new NVariableDeclaration(  std::unique_ptr<NIdentifier>($1), 
                                                                                    std::unique_ptr<NIdentifier>($2), 
                                                                                    std::unique_ptr<NExpression>($4)); 
                                                }

func_decl:      ident ident LPAR func_param RPAR block
                                                {   
                                                    $$ = new NFunctionDeclaration( std::unique_ptr<NIdentifier>($1),
                                                                                std::unique_ptr<NIdentifier>($2),
                                                                                std::move(*$4),
                                                                                std::unique_ptr<NBlock>($6));
                                                    delete $4; 
                                                }

while_stmt:     WHILE LPAR expr RPAR block     {
                                                    $$ = new NWhileStatement(std::unique_ptr<NExpression>($3),
                                                                           std::unique_ptr<NBlock>($5));
                                                }

func_param:     /* blank */                     {   $$ = new std::vector<std::unique_ptr<NVariableDeclaration>>(); }
                | var_decl                      {   $$ = new std::vector<std::unique_ptr<NVariableDeclaration>>();
                                                    $$->push_back(std::unique_ptr<NVariableDeclaration>(static_cast<NVariableDeclaration*>($1))); 
                                                }
                | func_param COMMA var_decl     {   
                                                    $1->push_back(std::unique_ptr<NVariableDeclaration>(
                                                    static_cast<NVariableDeclaration*>($3)));
                                                    $$ = $1; 
                                                }

ident:          IDENT                         { $$ = new NIdentifier(*$1); delete $1; }

numeric:        NUMBER                        { $$ = new NNumeric($1); }
                | FLOAT                        { $$ = new NNumeric($1); }

expr:            ident EQUAL expr              { $$ = new NAssignment(std::unique_ptr<NIdentifier>($1), 
                                                                  std::unique_ptr<NExpression>($3)); }
                | ident LPAR call_args RPAR    { NFunctionCall* call = new NFunctionCall(std::unique_ptr<NIdentifier>($1));
                                               for (auto& expr : *$3) {
                                                   call->arguments.push_back(std::move(expr));
                                               }
                                               delete $3;
                                               $$ = call; }
                | expr comparison expr         { $$ = new NBinaryOperator(  std::unique_ptr<NExpression>($1), 
                                                                            $2, 
                                                                        std::unique_ptr<NExpression>($3)); }
                | ident                        { $$ = $1; }
                | numeric                      { $$ = $1; }
                | LPAR expr RPAR               { $$ = $2; }

call_args:      /* blank */                     {   $$ = new std::vector<std::unique_ptr<NExpression>>(); }
                | expr                          {   $$ = new std::vector<std::unique_ptr<NExpression>>();
                                                    $$->push_back(std::unique_ptr<NExpression>($1));
                                                }
                | call_args COMMA expr          {   $1->push_back(std::unique_ptr<NExpression>($3));
                                                    $$ = $1; 
                                                }

comparison:     CEQ                             { $$ = CEQ;  }
                | CNE                           { $$ = CNE;  }
                | CLT                           { $$ = CLT;  }
                | CLE                           { $$ = CLE;  }
                | CGT                           { $$ = CGT;  }
                | CGE                           { $$ = CGE;  }
                | PLUS                          { $$ = PLUS; }
                | MINUS                         { $$ = MINUS;}
                | MUL                           { $$ = MUL;  }
                | DIV                           { $$ = DIV;  }

%%

main(void){
    int result = yyparse();
    if (result == 0) {
        // Successfully parsed
        std::cout << "Parsing successful! Dumping AST:" << std::endl;
        if (programRoot) {
            programRoot->print();
        }
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