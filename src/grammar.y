%{
extern int yydebug;
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "../include/ASTNodes.h"
#include "../include/SymbolTable.h" // Add this include

int yylex();
int yyerror(const char *s);

// Root of the AST
std::unique_ptr<NProgram> programRoot;
// Create a symbol table
SymbolTable symbolTable;
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
    NIfElseStatement* if_else_decl;
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
%token IF ELSE RETURN
%token <stringVal> PLUS MINUS MUL DIV CEQ CNE CLT CLE CGT CGE
%token WHILE

%right EQUAL
%left CEQ CNE
%left CLT CLE CGT CGE
%left PLUS MINUS
%left MUL DIV
%type <node> program
%type <block> stms block
%type <stmt> stmt var_decl func_decl while_stmt if_else_decl
%type <expr> expr numeric
%type <ident> ident
%type <var_decls> func_param
%type <exprs> call_args

%type <intVal> comparison

%start program

%%

program:        stms                            { programRoot = std::make_unique<NProgram>(std::unique_ptr<NBlock>($1)); }

stms:           stmt                            { 
                                                    $$ = new NBlock(); 
                                                    static_cast<NBlock*>($$)->addStatement(std::unique_ptr<NStatement>($1)); 
                                                }
                | stms stmt                     { $1->addStatement(std::unique_ptr<NStatement>($2)); $$ = $1; }

stmt:           var_decl SEMICOLON              { $$ = $1; }
                | RETURN expr SEMICOLON         { $$ = new NReturnStatement(std::unique_ptr<NExpression>($2)); }
                | expr SEMICOLON                { $$ = new NExpressionStatement(std::unique_ptr<NExpression>($1)); }
                | func_decl                     { $$ = $1; }
                | while_stmt                    { $$ = $1; }
                | if_else_decl                  { $$ = $1; }

block:          LCUR                            { symbolTable.enterScope(); } 
                stms RCUR                       { 
                                                    $$ = $3; 
                                                    symbolTable.exitScope();
                                                }
                | LCUR RCUR                     { 
                                                    symbolTable.enterScope();
                                                    $$ = new NBlock(); 
                                                    symbolTable.exitScope();
                                                }

if_else_decl:   IF LPAR expr RPAR block {
                    $$ = new NIfElseStatement(std::unique_ptr<NExpression>($3), std::unique_ptr<NBlock>($5), nullptr);
                }
                | IF LPAR expr RPAR block ELSE block {
                    $$ = new NIfElseStatement(std::unique_ptr<NExpression>($3), 
                                              std::unique_ptr<NBlock>($5), 
                                              std::unique_ptr<NBlock>($7));
                };

var_decl:       ident ident                     {   
                                                    $$ = new NVariableDeclaration(std::unique_ptr<NIdentifier>($1), 
                                                                               std::unique_ptr<NIdentifier>($2));
                                                    // Add to symbol table
                                                    symbolTable.declare($2->name, $1->name, SymbolType::VARIABLE);
                                                }
                | ident ident EQUAL expr       {    
                                                    $$ = new NVariableDeclaration(std::unique_ptr<NIdentifier>($1), 
                                                                               std::unique_ptr<NIdentifier>($2), 
                                                                               std::unique_ptr<NExpression>($4)); 
                                                    // Add to symbol table
                                                    symbolTable.declare($2->name, $1->name, SymbolType::VARIABLE);
                                                }

func_decl:      ident ident LPAR func_param RPAR block
                                                {   
                                                    $$ = new NFunctionDeclaration(std::unique_ptr<NIdentifier>($1),
                                                                              std::unique_ptr<NIdentifier>($2),
                                                                              std::move(*$4),
                                                                              std::unique_ptr<NBlock>($6));
                                                    // Add function to symbol table
                                                    symbolTable.declare($2->name, $1->name, SymbolType::FUNCTION);
                                                    
                                                    // Function body creates a new scope
                                                    symbolTable.enterScope();
                                                    // We would add parameters here in a more complete implementation
                                                    symbolTable.exitScope();
                                                    
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

ident:          IDENT                          { $$ = new NIdentifier(*$1); delete $1; }

numeric:        NUMBER                         { $$ = new NNumeric($1); }
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

int main(void){
    int result = yyparse();
    if (result == 0) {
        // Successfully parsed
        std::cout << "Parsing successful! Dumping AST:" << std::endl;
        if (programRoot) {
            programRoot->print();
            
            // Print the symbol table
            std::cout << "\nSymbol Table Contents:" << std::endl;
            symbolTable.print();
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