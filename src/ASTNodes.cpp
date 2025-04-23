#include "../include/ASTNodes.h"
#include <iostream>
#include <string>

// Helper function to indent output
static void indent(int level) {
    for (int i = 0; i < level; i++) {
        std::cout << "  ";
    }
}

// Convert token to string representation for print
static std::string opToString(int op) {
    switch (op) {
        case '+': return "+";
        case '-': return "-";
        case '*': return "*";
        case '/': return "/";
        case 272: return "=="; // CEQ token
        case 273: return "!="; // CNE token
        case 274: return "<";  // CLT token
        case 275: return "<="; // CLE token
        case 276: return ">";  // CGT token
        case 277: return ">="; // CGE token
        default: return "unknown";
    }
}

void NBlock::print(int indentLevel) const {
    indent(indentLevel);
    std::cout << "Block ->" << std::endl;
    for (const auto& statement : statements) {
        statement->print(indentLevel + 1);
    }
}

void NBlock::addStatement(std::unique_ptr<NStatement> statement) {
    statements.push_back(std::move(statement));
}

void NIdentifier::print(int indentLevel) const {
    indent(indentLevel);
    std::cout << "Identifier: " << name << std::endl;
}

void NNumeric::print(int indentLevel) const {
    indent(indentLevel);
    if (isFloat) {
        std::cout << "Float: " << floatValue << std::endl;
    } else {
        std::cout << "Integer: " << intValue << std::endl;
    }
}

void NBinaryOperator::print(int indentLevel) const {
    indent(indentLevel);
    std::cout << "BinaryOperator: " << opToString(op) << std::endl;
    left->print(indentLevel + 1);
    right->print(indentLevel + 1);
}

void NVariableDeclaration::print(int indentLevel) const {
    indent(indentLevel);
    std::cout << "VariableDeclaration ->" << std::endl;
    indent(indentLevel + 1);
    std::cout << "Type:" << std::endl;
    type->print(indentLevel + 2);
    indent(indentLevel + 1);
    std::cout << "Name:" << std::endl;
    id->print(indentLevel + 2);
    
    if (assignmentExpr) {
        indent(indentLevel + 1);
        std::cout << "InitValue:" << std::endl;
        assignmentExpr->print(indentLevel + 2);
    }
}

void NExpressionStatement::print(int indentLevel) const {
    indent(indentLevel);
    std::cout << "ExpressionStatement ->" << std::endl;
    expression->print(indentLevel + 1);
}

void NAssignment::print(int indentLevel) const {
    indent(indentLevel);
    std::cout << "Assignment ->" << std::endl;
    indent(indentLevel + 1);
    std::cout << "Target:" << std::endl;
    lhs->print(indentLevel + 2);
    indent(indentLevel + 1);
    std::cout << "Value:" << std::endl;
    rhs->print(indentLevel + 2);
}

void NFunctionCall::print(int indentLevel) const {
    indent(indentLevel);
    std::cout << "FunctionCall ->" << std::endl;
    indent(indentLevel + 1);
    std::cout << "Function:" << std::endl;
    id->print(indentLevel + 2);
    
    if (!arguments.empty()) {
        indent(indentLevel + 1);
        std::cout << "Arguments:" << std::endl;
        for (const auto& arg : arguments) {
            arg->print(indentLevel + 2);
        }
    }
}

void NFunctionDeclaration::print(int indentLevel) const {
    indent(indentLevel);
    std::cout << "FunctionDeclaration ->" << std::endl;
    indent(indentLevel + 1);
    std::cout << "ReturnType:" << std::endl;
    returnType->print(indentLevel + 2);
    indent(indentLevel + 1);
    std::cout << "Name:" << std::endl;
    id->print(indentLevel + 2);
    
    if (!arguments.empty()) {
        indent(indentLevel + 1);
        std::cout << "Parameters:" << std::endl;
        for (const auto& arg : arguments) {
            arg->print(indentLevel + 2);
        }
    }
    
    indent(indentLevel + 1);
    std::cout << "Body:" << std::endl;
    block->print(indentLevel + 2);
}

void NProgram::print(int indentLevel) const {
    indent(indentLevel);
    std::cout << "Program ->" << std::endl;
    block->print(indentLevel + 1);
}