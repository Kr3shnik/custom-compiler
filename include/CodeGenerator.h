#ifndef __CODEGENERATOR_H__
#define __CODEGENERATOR_H__

#include <string>
#include <memory>
#include <vector>
#include <map>

#include "ASTNodes.h"
#include "SymbolTable.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>

class CodeGenerator {
private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    SymbolTable& symbolTable;
    std::vector<std::map<std::string, llvm::AllocaInst*>> scopedValues;
    llvm::Function* currentFunction = nullptr;

    void enterScope();
    void exitScope();
    llvm::AllocaInst* findValue(const std::string& name);
    llvm::Type* getLLVMType(const std::string& typeName);

    // Visitor methods for each AST node type
    llvm::Value* visit(Node* node);
    llvm::Value* visit(NProgram* node);
    llvm::Value* visit(NBlock* node);
    llvm::Value* visit(NIdentifier* node);
    llvm::Value* visit(NNumeric* node);
    llvm::Value* visit(NBinaryOperator* node);
    llvm::Value* visit(NVariableDeclaration* node);
    llvm::Value* visit(NExpressionStatement* node);
    llvm::Value* visit(NAssignment* node);
    llvm::Value* visit(NFunctionCall* node);
    llvm::Function* visit(NFunctionDeclaration* node);
    llvm::Value* visit(NReturnStatement* node);
    llvm::Value* visit(NIfElseStatement* node);
    llvm::Value* visit(NWhileStatement* node);

public:
    CodeGenerator(SymbolTable& symbolTable);
    ~CodeGenerator();
    void generate(NProgram* ast, const std::string& outputFilename);
};

#endif
