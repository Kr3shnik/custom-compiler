#include "../include/ASTNodes.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

// Tree visualization helpers
static const std::string TREE_LINE = "│ ";
static const std::string TREE_BRANCH = "├─";
static const std::string TREE_CORNER = "└─";
static const std::string TREE_SPACE = "  ";

// Helper function to create the prefix for tree visualization
static std::string getTreePrefix(int level, bool isLast, std::vector<bool>& isLastChild) {
    std::stringstream ss;
    
    // Add proper prefixes for all parent levels
    for (int i = 0; i < level - 1; ++i) {
        if (isLastChild[i]) {
            ss << TREE_SPACE;
        } else {
            ss << TREE_LINE;
        }
    }
    
    // Add the branch or corner for the current level
    if (level > 0) {
        if (isLast) {
            ss << TREE_CORNER;
        } else {
            ss << TREE_BRANCH;
        }
    }
    
    return ss.str();
}

// Convert token to string representation
static std::string opToString(int op) {
    switch (op) {
        case 268: return "+";
        case 269: return "-";
        case 270: return "*";
        case 271: return "/";
        case 274: return "=="; // CEQ token
        case 275: return "!="; // CNE token
        case 276: return "<";  // CLT token
        case 277: return "<="; // CLE token
        case 278: return ">";  // CGT token
        case 279: return ">="; // CGE token
        default: return "unknown";
    }
}

void NBlock::print(int level) const {
    static std::vector<bool> isLastChild(100, false);  // Track last children at each level
    
    bool empty = statements.empty();
    std::cout << getTreePrefix(level, true, isLastChild) << "Block" << (empty ? " (empty)" : "") << std::endl;
    
    if (!empty) {
        isLastChild[level] = true;  // Mark this level as having the last child shown
        
        for (size_t i = 0; i < statements.size(); ++i) {
            bool isLast = (i == statements.size() - 1);
            isLastChild[level] = isLast;
            statements[i]->print(level + 1);
        }
    }
}

void NBlock::addStatement(std::unique_ptr<NStatement> statement) {
    statements.push_back(std::move(statement));
}

void NIdentifier::print(int level) const {
    static std::vector<bool> isLastChild(100, false);
    std::cout << getTreePrefix(level, true, isLastChild) << "ID: \"" << name << "\"" << std::endl;
}

void NNumeric::print(int level) const {
    static std::vector<bool> isLastChild(100, false);
    if (isFloat) {
        std::cout << getTreePrefix(level, true, isLastChild) << "FLOAT: " << floatValue << std::endl;
    } else {
        std::cout << getTreePrefix(level, true, isLastChild) << "INT: " << intValue << std::endl;
    }
}

void NBinaryOperator::print(int level) const {
    static std::vector<bool> isLastChild(100, false);
    std::cout << op;
    std::cout << getTreePrefix(level, true, isLastChild) << "BINARY_OP: " << opToString(op) << std::endl;
    
    isLastChild[level] = false;  // First child is not the last
    left->print(level + 1);
    
    isLastChild[level] = true;   // Second child is the last
    right->print(level + 1);
}

void NVariableDeclaration::print(int level) const {
    static std::vector<bool> isLastChild(100, false);
    std::cout << getTreePrefix(level, true, isLastChild) << "VAR_DECL" << std::endl;
    
    isLastChild[level] = false;  // Not the last child yet
    
    std::cout << getTreePrefix(level + 1, false, isLastChild) << "TYPE" << std::endl;
    type->print(level + 2);
    
    std::cout << getTreePrefix(level + 1, assignmentExpr ? false : true, isLastChild) << "NAME" << std::endl;
    id->print(level + 2);
    
    if (assignmentExpr) {
        isLastChild[level + 1] = true;  // Last child at this level
        std::cout << getTreePrefix(level + 1, true, isLastChild) << "INIT" << std::endl;
        assignmentExpr->print(level + 2);
    }
}

void NExpressionStatement::print(int level) const {
    static std::vector<bool> isLastChild(100, false);
    std::cout << getTreePrefix(level, true, isLastChild) << "EXPR_STMT" << std::endl;
    
    isLastChild[level] = true;  // This is the last (and only) child
    expression->print(level + 1);
}

void NAssignment::print(int level) const {
    static std::vector<bool> isLastChild(100, false);
    std::cout << getTreePrefix(level, true, isLastChild) << "ASSIGN" << std::endl;
    
    isLastChild[level] = false;  // Not the last child
    std::cout << getTreePrefix(level + 1, false, isLastChild) << "TARGET" << std::endl;
    lhs->print(level + 2);
    
    isLastChild[level] = true;  // This is the last child
    std::cout << getTreePrefix(level + 1, true, isLastChild) << "VALUE" << std::endl;
    rhs->print(level + 2);
}

void NFunctionCall::print(int level) const {
    static std::vector<bool> isLastChild(100, false);
    std::cout << getTreePrefix(level, true, isLastChild) << "FUNC_CALL" << std::endl;
    
    isLastChild[level] = arguments.empty();  // Only last if no arguments
    std::cout << getTreePrefix(level + 1, arguments.empty(), isLastChild) << "FUNCTION" << std::endl;
    id->print(level + 2);
    
    if (!arguments.empty()) {
        std::cout << getTreePrefix(level + 1, true, isLastChild) << "ARGS" << std::endl;
        
        for (size_t i = 0; i < arguments.size(); ++i) {
            isLastChild[level + 2] = (i == arguments.size() - 1);  // Last arg?
            std::cout << getTreePrefix(level + 2, isLastChild[level + 2], isLastChild) << "ARG[" << i << "]" << std::endl;
            arguments[i]->print(level + 3);
        }
    }
}

void NFunctionDeclaration::print(int level) const {
    static std::vector<bool> isLastChild(100, false);
    std::cout << getTreePrefix(level, true, isLastChild) << "FUNC_DECL" << std::endl;
    
    isLastChild[level] = false;  // Not the last child
    
    std::cout << getTreePrefix(level + 1, false, isLastChild) << "RETURN_TYPE" << std::endl;
    returnType->print(level + 2);
    
    std::cout << getTreePrefix(level + 1, arguments.empty() && !block, isLastChild) << "NAME" << std::endl;
    id->print(level + 2);
    
    if (!arguments.empty()) {
        isLastChild[level + 1] = !block;  // Last only if no block
        std::cout << getTreePrefix(level + 1, !block, isLastChild) << "PARAMS" << std::endl;
        
        for (size_t i = 0; i < arguments.size(); ++i) {
            isLastChild[level + 2] = (i == arguments.size() - 1);  // Last param?
            std::cout << getTreePrefix(level + 2, isLastChild[level + 2], isLastChild) << "PARAM[" << i << "]" << std::endl;
            arguments[i]->print(level + 3);
        }
    }
    
    if (block) {
        isLastChild[level + 1] = true;  // This is the last child
        std::cout << getTreePrefix(level + 1, true, isLastChild) << "BODY" << std::endl;
        block->print(level + 2);
    }
}

void NProgram::print(int level) const {
    static std::vector<bool> isLastChild(100, false);
    std::cout << "PROGRAM" << std::endl;
    
    isLastChild[level] = true;  // Program has only one child
    block->print(level + 1);
}