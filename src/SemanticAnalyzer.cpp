#include "../include/SemanticAnalyzer.h"
#include "../include/Lexer.h" // For Token enum
#include <iostream>
#include <algorithm>

SemanticAnalyzer::SemanticAnalyzer(SymbolTable& symbolTable) 
    : symbolTable(symbolTable), inFunctionBody(false) {
    initializeBuiltins();
}

void SemanticAnalyzer::initializeBuiltins() {
    // Add built-in functions like print
    symbolTable.declare("print", "void", SymbolType::FUNCTION);
    
    // Add built-in types (these might be used for type checking)
    // Note: In a real compiler, you'd have a separate type table
    // Don't pre-declare main - let the user program declare it
}

bool SemanticAnalyzer::analyze(NProgram* program) {
    if (!program) {
        addError(SemanticErrorType::UNDECLARED_VARIABLE, "No program to analyze", true);
        return false;
    }
    
    analyzeProgram(program);
    
    // Check if main function exists
    if (!hasMainFunction()) {
        addWarning(SemanticErrorType::MISSING_RETURN, "No main function found");
    } else {
        validateMainFunction();
    }
    
    return !hasFatalErrors();
}

void SemanticAnalyzer::analyzeProgram(NProgram* program) {
    if (program->block) {
        analyzeBlock(program->block.get());
    }
}

void SemanticAnalyzer::analyzeBlock(NBlock* block) {
    for (auto& stmt : block->statements) {
        analyzeStatement(stmt.get());
    }
}

void SemanticAnalyzer::analyzeStatement(NStatement* stmt) {
    if (!stmt) return;
    
    if (auto varDecl = dynamic_cast<NVariableDeclaration*>(stmt)) {
        analyzeVariableDeclaration(varDecl);
    } else if (auto funcDecl = dynamic_cast<NFunctionDeclaration*>(stmt)) {
        analyzeFunctionDeclaration(funcDecl);
    } else if (auto exprStmt = dynamic_cast<NExpressionStatement*>(stmt)) {
        if (exprStmt->expression) {
            analyzeExpression(exprStmt->expression.get());
        }
    } else if (auto returnStmt = dynamic_cast<NReturnStatement*>(stmt)) {
        analyzeReturnStatement(returnStmt);
    } else if (auto ifElse = dynamic_cast<NIfElseStatement*>(stmt)) {
        analyzeIfElseStatement(ifElse);
    } else if (auto whileStmt = dynamic_cast<NWhileStatement*>(stmt)) {
        analyzeWhileStatement(whileStmt);
    } else if (auto block = dynamic_cast<NBlock*>(stmt)) {
        symbolTable.enterScope();
        analyzeBlock(block);
        symbolTable.exitScope();
    }
}

void SemanticAnalyzer::analyzeExpression(NExpression* expr) {
    if (!expr) return;
    
    if (auto ident = dynamic_cast<NIdentifier*>(expr)) {
        analyzeIdentifier(ident);
    } else if (auto assignment = dynamic_cast<NAssignment*>(expr)) {
        analyzeAssignment(assignment);
    } else if (auto funcCall = dynamic_cast<NFunctionCall*>(expr)) {
        analyzeFunctionCall(funcCall);
    } else if (auto binOp = dynamic_cast<NBinaryOperator*>(expr)) {
        analyzeBinaryOperator(binOp);
    } else if (auto numeric = dynamic_cast<NNumeric*>(expr)) {
        // Numeric literals are always valid
    }
}

void SemanticAnalyzer::analyzeVariableDeclaration(NVariableDeclaration* varDecl) {
    if (!varDecl || !varDecl->id || !varDecl->type) return;
    
    std::string varName = varDecl->id->name;
    std::string varType = varDecl->type->name;
    
    // Check if variable is already declared in current scope
    // Note: This is a simplified check - in reality you'd check only the current scope
    if (!symbolTable.declare(varName, varType, SymbolType::VARIABLE)) {
        addError(SemanticErrorType::REDECLARATION, 
                "Variable '" + varName + "' is already declared in this scope");
    }
    
    // If there's an assignment expression, analyze it
    if (varDecl->assignmentExpr) {
        analyzeExpression(varDecl->assignmentExpr.get());
        
        // Check type compatibility
        std::string exprType = getExpressionType(varDecl->assignmentExpr.get());
        if (!exprType.empty() && !areTypesCompatible(varType, exprType)) {
            // Try to insert type conversion if possible
            if (getPromotedType(varType, exprType) == varType) {
                addWarning(SemanticErrorType::TYPE_MISMATCH,
                    "Implicit conversion from '" + exprType + "' to '" + varType + 
                    "' in initialization of variable '" + varName + "'");
            } else {
                addError(SemanticErrorType::TYPE_MISMATCH,
                    "Cannot initialize variable '" + varName + "' of type '" + varType + 
                    "' with expression of type '" + exprType + "'");
            }
        }
    }
}

void SemanticAnalyzer::analyzeFunctionDeclaration(NFunctionDeclaration* funcDecl) {
    if (!funcDecl || !funcDecl->id || !funcDecl->returnType) return;
    
    std::string funcName = funcDecl->id->name;
    std::string returnType = funcDecl->returnType->name;
    
    // Check if function is already declared
    if (!symbolTable.declare(funcName, returnType, SymbolType::FUNCTION)) {
        addError(SemanticErrorType::REDECLARATION,
                "Function '" + funcName + "' is already declared");
    }
    
    // Enter new scope for function body
    symbolTable.enterScope();
    
    // Set current function context
    std::string prevReturnType = currentFunctionReturnType;
    bool prevInFunction = inFunctionBody;
    currentFunctionReturnType = returnType;
    inFunctionBody = true;
    
    // Analyze parameters
    for (auto& param : funcDecl->arguments) {
        if (param) {
            analyzeVariableDeclaration(param.get());
        }
    }
    
    // Analyze function body
    if (funcDecl->block) {
        analyzeBlock(funcDecl->block.get());
    }
    
    // Check if non-void function has return statement
    if (returnType != "void" && funcDecl->block) {
        // This is a simplified check - you'd want to do flow analysis
        bool hasReturn = false;
        for (auto& stmt : funcDecl->block->statements) {
            if (dynamic_cast<NReturnStatement*>(stmt.get())) {
                hasReturn = true;
                break;
            }
        }
        if (!hasReturn) {
            addWarning(SemanticErrorType::MISSING_RETURN,
                "Function '" + funcName + "' with non-void return type should have a return statement");
        }
    }
    
    // Restore previous context
    currentFunctionReturnType = prevReturnType;
    inFunctionBody = prevInFunction;
    
    symbolTable.exitScope();
}

void SemanticAnalyzer::analyzeAssignment(NAssignment* assignment) {
    if (!assignment || !assignment->lhs) return;
    
    std::string varName = assignment->lhs->name;
    
    // Check if variable is declared
    auto symbol = symbolTable.lookup(varName);
    if (!symbol) {
        addError(SemanticErrorType::UNDECLARED_VARIABLE,
                "Undeclared variable '" + varName + "' in assignment");
        return;
    }
    
    // Check if trying to assign to a function
    if (symbol->symbolType == SymbolType::FUNCTION) {
        addError(SemanticErrorType::ASSIGNMENT_TO_FUNCTION,
                "Cannot assign to function '" + varName + "'");
        return;
    }
    
    // Analyze RHS
    if (assignment->rhs) {
        analyzeExpression(assignment->rhs.get());
        
        // Type checking
        std::string lhsType = symbol->type;
        std::string rhsType = getExpressionType(assignment->rhs.get());
        
        if (!rhsType.empty()) {
            if (!areTypesCompatible(lhsType, rhsType)) {
                addError(SemanticErrorType::TYPE_MISMATCH,
                    "Cannot assign expression of type '" + rhsType + 
                    "' to variable '" + varName + "' of type '" + lhsType + "'");
            } else if (lhsType == "int" && rhsType == "float") {
                // Even though compatible, warn about potential precision loss
                addWarning(SemanticErrorType::TYPE_MISMATCH,
                    "Potentially unsafe conversion from '" + rhsType + "' to '" + lhsType + 
                    "' in assignment to variable '" + varName + "' (precision may be lost)");
            }
        }
    }
}

void SemanticAnalyzer::analyzeFunctionCall(NFunctionCall* funcCall) {
    if (!funcCall || !funcCall->id) return;
    
    std::string funcName = funcCall->id->name;
    
    // Check if function is declared
    auto symbol = symbolTable.lookup(funcName);
    if (!symbol) {
        addError(SemanticErrorType::UNDECLARED_FUNCTION,
                "Undeclared function '" + funcName + "'");
        return;
    }
    
    // Check if it's actually a function
    if (symbol->symbolType != SymbolType::FUNCTION) {
        addError(SemanticErrorType::UNDECLARED_FUNCTION,
                "'" + funcName + "' is not a function");
        return;
    }
    
    // Analyze arguments
    for (auto& arg : funcCall->arguments) {
        if (arg) {
            analyzeExpression(arg.get());
        }
    }
    
    // Note: For complete function call validation, you'd need to store
    // parameter information in the symbol table or have a separate function table
}

void SemanticAnalyzer::analyzeBinaryOperator(NBinaryOperator* binOp) {
    if (!binOp) return;
    
    // Analyze operands
    if (binOp->left) {
        analyzeExpression(binOp->left.get());
    }
    if (binOp->right) {
        analyzeExpression(binOp->right.get());
    }
    
    // Type checking for binary operations
    std::string leftType = getExpressionType(binOp->left.get());
    std::string rightType = getExpressionType(binOp->right.get());
    
    if (!leftType.empty() && !rightType.empty()) {
        // Check if operation is valid for these types
        Token op = static_cast<Token>(binOp->op);
        
        // For arithmetic operations, both operands should be numeric
        if (op == Token::PLUS || op == Token::MINUS || op == Token::MUL || op == Token::DIV) {
            if ((leftType != "int" && leftType != "float") || 
                (rightType != "int" && rightType != "float")) {
                addError(SemanticErrorType::TYPE_MISMATCH,
                    "Arithmetic operation requires numeric operands");
            }
        }
        
        // For comparison operations, operands should be compatible
        if (op == Token::CEQ || op == Token::CNE || op == Token::CLT || 
            op == Token::CLE || op == Token::CGT || op == Token::CGE) {
            if (!areTypesCompatible(leftType, rightType) && 
                !areTypesCompatible(rightType, leftType)) {
                addWarning(SemanticErrorType::TYPE_MISMATCH,
                    "Comparison between incompatible types '" + leftType + 
                    "' and '" + rightType + "'");
            }
        }
    }
}

void SemanticAnalyzer::analyzeReturnStatement(NReturnStatement* returnStmt) {
    if (!returnStmt) return;
    
    if (!inFunctionBody) {
        addError(SemanticErrorType::TYPE_MISMATCH,
                "Return statement outside function body");
        return;
    }
    
    if (returnStmt->expression) {
        analyzeExpression(returnStmt->expression.get());
        
        // Check return type compatibility
        if (currentFunctionReturnType == "void") {
            addError(SemanticErrorType::TYPE_MISMATCH,
                    "Void function should not return a value");
        } else {
            std::string exprType = getExpressionType(returnStmt->expression.get());
            if (!exprType.empty() && !areTypesCompatible(currentFunctionReturnType, exprType)) {
                if (getPromotedType(currentFunctionReturnType, exprType) == currentFunctionReturnType) {
                    addWarning(SemanticErrorType::TYPE_MISMATCH,
                        "Implicit conversion from '" + exprType + "' to '" + 
                        currentFunctionReturnType + "' in return statement");
                } else {
                    addError(SemanticErrorType::TYPE_MISMATCH,
                        "Cannot return expression of type '" + exprType + 
                        "' from function with return type '" + currentFunctionReturnType + "'");
                }
            }
        }
    } else {
        // Return with no expression
        if (currentFunctionReturnType != "void") {
            addError(SemanticErrorType::TYPE_MISMATCH,
                    "Non-void function must return a value");
        }
    }
}

void SemanticAnalyzer::analyzeIfElseStatement(NIfElseStatement* ifElse) {
    if (!ifElse) return;
    
    // Analyze condition
    if (ifElse->condition) {
        analyzeExpression(ifElse->condition.get());
        
        // Check if condition is boolean-compatible
        std::string condType = getExpressionType(ifElse->condition.get());
        // In many languages, numeric types can be used as conditions
        // This is a design choice for your language
    }
    
    // Analyze then block
    if (ifElse->thenBlock) {
        symbolTable.enterScope();
        analyzeBlock(ifElse->thenBlock.get());
        symbolTable.exitScope();
    }
    
    // Analyze else block if present
    if (ifElse->elseBlock) {
        symbolTable.enterScope();
        analyzeBlock(ifElse->elseBlock.get());
        symbolTable.exitScope();
    }
}

void SemanticAnalyzer::analyzeWhileStatement(NWhileStatement* whileStmt) {
    if (!whileStmt) return;
    
    // Analyze condition
    if (whileStmt->condition) {
        analyzeExpression(whileStmt->condition.get());
    }
    
    // Analyze body
    if (whileStmt->block) {
        symbolTable.enterScope();
        analyzeBlock(whileStmt->block.get());
        symbolTable.exitScope();
    }
}

void SemanticAnalyzer::analyzeIdentifier(NIdentifier* ident) {
    if (!ident) return;
    
    std::string name = ident->name;
    
    // Check if identifier is declared
    auto symbol = symbolTable.lookup(name);
    if (!symbol) {
        addError(SemanticErrorType::UNDECLARED_VARIABLE,
                "Undeclared identifier '" + name + "'");
    }
}

std::string SemanticAnalyzer::getExpressionType(NExpression* expr) {
    if (!expr) return "";
    
    if (auto ident = dynamic_cast<NIdentifier*>(expr)) {
        auto symbol = symbolTable.lookup(ident->name);
        return symbol ? symbol->type : "";
    } else if (auto numeric = dynamic_cast<NNumeric*>(expr)) {
        return numeric->isFloat ? "float" : "int";
    } else if (auto funcCall = dynamic_cast<NFunctionCall*>(expr)) {
        auto symbol = symbolTable.lookup(funcCall->id->name);
        return symbol ? symbol->type : "";
    } else if (auto binOp = dynamic_cast<NBinaryOperator*>(expr)) {
        std::string leftType = getExpressionType(binOp->left.get());
        std::string rightType = getExpressionType(binOp->right.get());
        
        Token op = static_cast<Token>(binOp->op);
        
        // Comparison operations return boolean (represented as int in many C-like languages)
        if (op == Token::CEQ || op == Token::CNE || op == Token::CLT || 
            op == Token::CLE || op == Token::CGT || op == Token::CGE) {
            return "int"; // Assuming boolean results are int
        }
        
        // Arithmetic operations
        return getPromotedType(leftType, rightType);
    } else if (auto assignment = dynamic_cast<NAssignment*>(expr)) {
        auto symbol = symbolTable.lookup(assignment->lhs->name);
        return symbol ? symbol->type : "";
    }
    
    return "";
}

bool SemanticAnalyzer::areTypesCompatible(const std::string& expected, const std::string& actual) {
    if (expected == actual) return true;
    
    // Allow int to float promotion (always safe)
    if (expected == "float" && actual == "int") return true;
    
    // Allow float to int but should generate warning (handled in calling code)
    if (expected == "int" && actual == "float") return true;
    
    // Add more compatibility rules as needed
    return false;
}

std::string SemanticAnalyzer::getPromotedType(const std::string& type1, const std::string& type2) {
    if (type1 == type2) return type1;
    
    // Float promotion rules
    if ((type1 == "float" && type2 == "int") || (type1 == "int" && type2 == "float")) {
        return "float";
    }
    
    // Default to first type if no promotion rule
    return type1;
}

bool SemanticAnalyzer::hasMainFunction() {
    auto symbol = symbolTable.lookup("main");
    return symbol && symbol->symbolType == SymbolType::FUNCTION;
}

void SemanticAnalyzer::validateMainFunction() {
    auto symbol = symbolTable.lookup("main");
    if (!symbol) return;
    
    // In many languages, main should have specific signature
    // This is a simplified check
    if (symbol->type != "int" && symbol->type != "void") {
        addWarning(SemanticErrorType::INVALID_MAIN_SIGNATURE,
                "Main function should typically return 'int' or 'void'");
    }
}

void SemanticAnalyzer::addError(SemanticErrorType type, const std::string& message, bool isFatal) {
    errors.emplace_back(type, message, isFatal);
}

void SemanticAnalyzer::addWarning(SemanticErrorType type, const std::string& message) {
    warnings.emplace_back(type, message, false);
}

bool SemanticAnalyzer::hasFatalErrors() const {
    return std::any_of(errors.begin(), errors.end(), 
                       [](const SemanticError& error) { return error.isFatal; });
}

void SemanticAnalyzer::reportErrors() const {
    for (const auto& error : errors) {
        std::cerr << (error.isFatal ? "Fatal Error: " : "Error: ") 
                  << error.message << std::endl;
    }
}

void SemanticAnalyzer::reportWarnings() const {
    for (const auto& warning : warnings) {
        std::cerr << "Warning: " << warning.message << std::endl;
    }
}
