#include "../include/CodeGenerator.h"
#include "../include/ASTNodes.h"
#include "../include/SymbolTable.h"
#include "../include/Parser.h"
#include "../include/Lexer.h"
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/MC/TargetRegistry.h>
#include <iostream>

CodeGenerator::CodeGenerator(SymbolTable& symbolTable) : symbolTable(symbolTable) {
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("MyModule", *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
}

CodeGenerator::~CodeGenerator() = default;

void CodeGenerator::generate(NProgram* ast, const std::string& outputFilename) {
    visit(ast);

    // Initialize the target registry etc.
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    module->setTargetTriple(targetTriple);

    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, error);

    if (!target) {
        llvm::errs() << error;
        return;
    }

    auto CPU = "generic";
    auto Features = "";

    llvm::TargetOptions opt;
    auto RM = llvm::Optional<llvm::Reloc::Model>();
    auto targetMachine = target->createTargetMachine(targetTriple, CPU, Features, opt, RM);

    module->setDataLayout(targetMachine->createDataLayout());

    std::error_code EC;
    llvm::raw_fd_ostream dest(outputFilename, EC, llvm::sys::fs::OF_None);

    if (EC) {
        llvm::errs() << "Could not open file: " << EC.message();
        return;
    }

    llvm::legacy::PassManager pass;
    auto FileType = llvm::CGFT_ObjectFile;

    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
        llvm::errs() << "TheTargetMachine can't emit a file of this type";
        return;
    }

    pass.run(*module);
    dest.flush();
}

llvm::Type* CodeGenerator::getLLVMType(const std::string& typeName) {
    if (typeName == "int") {
        return llvm::Type::getInt32Ty(*context);
    } else if (typeName == "float") {
        return llvm::Type::getFloatTy(*context);
    } else if (typeName == "void") {
        return llvm::Type::getVoidTy(*context);
    }
    return nullptr; // Should not happen with semantic analysis
}

void CodeGenerator::enterScope() {
    scopedValues.emplace_back();
}

void CodeGenerator::exitScope() {
    scopedValues.pop_back();
}

llvm::AllocaInst* CodeGenerator::findValue(const std::string& name) {
    for (auto it = scopedValues.rbegin(); it != scopedValues.rend(); ++it) {
        if (it->count(name)) {
            return (*it)[name];
        }
    }
    return nullptr;
}

llvm::Value* CodeGenerator::visit(Node* node) {
    if (auto specificNode = dynamic_cast<NProgram*>(node)) {
        return visit(specificNode);
    } else if (auto specificNode = dynamic_cast<NBlock*>(node)) {
        return visit(specificNode);
    } else if (auto specificNode = dynamic_cast<NExpressionStatement*>(node)) {
        return visit(specificNode);
    } else if (auto specificNode = dynamic_cast<NVariableDeclaration*>(node)) {
        return visit(specificNode);
    } else if (auto specificNode = dynamic_cast<NAssignment*>(node)) {
        return visit(specificNode);
    } else if (auto specificNode = dynamic_cast<NFunctionDeclaration*>(node)) {
        return visit(specificNode);
    } else if (auto specificNode = dynamic_cast<NReturnStatement*>(node)) {
        return visit(specificNode);
    } else if (auto specificNode = dynamic_cast<NIfElseStatement*>(node)) {
        return visit(specificNode);
    } else if (auto specificNode = dynamic_cast<NWhileStatement*>(node)) {
        return visit(specificNode);
    } else if (auto specificNode = dynamic_cast<NIdentifier*>(node)) {
        return visit(specificNode);
    } else if (auto specificNode = dynamic_cast<NNumeric*>(node)) {
        return visit(specificNode);
    } else if (auto specificNode = dynamic_cast<NBinaryOperator*>(node)) {
        return visit(specificNode);
    } else if (auto specificNode = dynamic_cast<NFunctionCall*>(node)) {
        return visit(specificNode);
    }
    return nullptr;
}

llvm::Value* CodeGenerator::visit(NProgram* node) {
    visit(node->block.get());
    return nullptr;
}

llvm::Value* CodeGenerator::visit(NBlock* node) {
    enterScope();
    for (auto& statement : node->statements) {
        visit(statement.get());
    }
    exitScope();
    return nullptr;
}

llvm::Value* CodeGenerator::visit(NExpressionStatement* node) {
    return visit(node->expression.get());
}

llvm::Value* CodeGenerator::visit(NVariableDeclaration* node) {
    llvm::Type* type = getLLVMType(node->type->name);
    llvm::AllocaInst* alloca = builder->CreateAlloca(type, nullptr, node->id->name);
    scopedValues.back()[node->id->name] = alloca;
    if (node->assignmentExpr) {
        llvm::Value* value = visit(node->assignmentExpr.get());
        builder->CreateStore(value, alloca);
    }
    return alloca;
}

llvm::Value* CodeGenerator::visit(NAssignment* node) {
    llvm::AllocaInst* var = findValue(node->lhs->name);
    llvm::Value* value = visit(node->rhs.get());
    builder->CreateStore(value, var);
    return value;
}

llvm::Function* CodeGenerator::visit(NFunctionDeclaration* node) {
    std::vector<llvm::Type*> argTypes;
    for (auto& arg : node->arguments) {
        argTypes.push_back(getLLVMType(arg->type->name));
    }
    llvm::Type* returnType = getLLVMType(node->returnType->name);
    llvm::FunctionType* funcType = llvm::FunctionType::get(returnType, argTypes, false);
    llvm::Function* function = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, node->id->name, module.get());

    unsigned i = 0;
    for (auto& arg : function->args()) {
        arg.setName(node->arguments[i++]->id->name);
    }

    llvm::BasicBlock* basicBlock = llvm::BasicBlock::Create(*context, "entry", function);
    builder->SetInsertPoint(basicBlock);
    currentFunction = function;

    enterScope();
    for (auto& arg : function->args()) {
        llvm::AllocaInst* alloca = builder->CreateAlloca(arg.getType(), nullptr, arg.getName());
        builder->CreateStore(&arg, alloca);
        scopedValues.back()[std::string(arg.getName())] = alloca;
    }

    visit(node->block.get());

    if (returnType->isVoidTy()) {
        if (!builder->GetInsertBlock()->getTerminator()) {
            builder->CreateRetVoid();
        }
    } else {
        if (!builder->GetInsertBlock()->getTerminator()) {
             if (returnType->isIntegerTy()) {
                builder->CreateRet(llvm::ConstantInt::get(returnType, 0));
            } else if (returnType->isFloatTy()) {
                builder->CreateRet(llvm::ConstantFP::get(returnType, 0.0));
            }
        }
    }

    exitScope();
    currentFunction = nullptr;
    return function;
}

llvm::Value* CodeGenerator::visit(NReturnStatement* node) {
    if (node->expression) {
        llvm::Value* returnValue = visit(node->expression.get());
        builder->CreateRet(returnValue);
        return returnValue;
    }
    builder->CreateRetVoid();
    return nullptr;
}

llvm::Value* CodeGenerator::visit(NIfElseStatement* node) {
    llvm::Value* cond = visit(node->condition.get());
    cond = builder->CreateICmpNE(cond, llvm::ConstantInt::get(*context, llvm::APInt(32, 0)), "ifcond");

    llvm::Function* function = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(*context, "then", function);
    llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(*context, "else");
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*context, "ifcont");

    if (node->elseBlock) {
        builder->CreateCondBr(cond, thenBB, elseBB);
    } else {
        builder->CreateCondBr(cond, thenBB, mergeBB);
    }

    builder->SetInsertPoint(thenBB);
    visit(node->thenBlock.get());
    if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateBr(mergeBB);
    }
    
    if (node->elseBlock) {
        function->getBasicBlockList().push_back(elseBB);
        builder->SetInsertPoint(elseBB);
        visit(node->elseBlock.get());
        if (!builder->GetInsertBlock()->getTerminator()) {
            builder->CreateBr(mergeBB);
        }
    }

    function->getBasicBlockList().push_back(mergeBB);
    builder->SetInsertPoint(mergeBB);
    
    return nullptr;
}

llvm::Value* CodeGenerator::visit(NWhileStatement* node) {
    llvm::Function* function = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* condBB = llvm::BasicBlock::Create(*context, "loopcond", function);
    llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(*context, "loop", function);
    llvm::BasicBlock* afterBB = llvm::BasicBlock::Create(*context, "afterloop", function);

    builder->CreateBr(condBB);
    builder->SetInsertPoint(condBB);
    llvm::Value* cond = visit(node->condition.get());
    cond = builder->CreateICmpNE(cond, llvm::ConstantInt::get(*context, llvm::APInt(32, 0)), "whilecond");
    builder->CreateCondBr(cond, loopBB, afterBB);

    builder->SetInsertPoint(loopBB);
    visit(node->block.get());
    builder->CreateBr(condBB);

    builder->SetInsertPoint(afterBB);
    return nullptr;
}

llvm::Value* CodeGenerator::visit(NIdentifier* node) {
    llvm::AllocaInst* var = findValue(node->name);
    return builder->CreateLoad(var->getAllocatedType(), var, node->name.c_str());
}

llvm::Value* CodeGenerator::visit(NNumeric* node) {
    if (node->isFloat) {
        return llvm::ConstantFP::get(*context, llvm::APFloat(node->floatValue));
    } else {
        return llvm::ConstantInt::get(*context, llvm::APInt(32, node->intValue));
    }
}

llvm::Value* CodeGenerator::visit(NBinaryOperator* node) {
    llvm::Value* L = visit(node->left.get());
    llvm::Value* R = visit(node->right.get());

    // This assumes the types are integers for simplicity. 
    // A real implementation would handle float and int operations, and type conversions.
    switch (static_cast<Token>(node->op)) {
        case Token::PLUS: return builder->CreateAdd(L, R, "addtmp");
        case Token::MINUS: return builder->CreateSub(L, R, "subtmp");
        case Token::MUL: return builder->CreateMul(L, R, "multmp");
        case Token::DIV: return builder->CreateSDiv(L, R, "divtmp");
        case Token::CEQ: return builder->CreateICmpEQ(L, R, "eqtmp");
        case Token::CNE: return builder->CreateICmpNE(L, R, "netmp");
        case Token::CLT: return builder->CreateICmpSLT(L, R, "lttmp");
        case Token::CLE: return builder->CreateICmpSLE(L, R, "letmp");
        case Token::CGT: return builder->CreateICmpSGT(L, R, "gttmp");
        case Token::CGE: return builder->CreateICmpSGE(L, R, "getmp");
        default: return nullptr;
    }
}

llvm::Value* CodeGenerator::visit(NFunctionCall* node) {
    llvm::Function* callee = module->getFunction(node->id->name);
    std::vector<llvm::Value*> args;
    for (auto& arg : node->arguments) {
        args.push_back(visit(arg.get()));
    }
    return builder->CreateCall(callee, args, "calltmp");
}
