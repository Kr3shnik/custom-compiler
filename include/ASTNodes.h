#ifndef __ASTNODES_H__
#define __ASTNODES_H__

#include <iostream>
#include <vector>
#include <string>
#include <memory>

// Forward declarations
class NExpression;
class NStatement;
class NBlock;
class NIdentifier;
class NVariableDeclaration;
class NFunctionDeclaration;

// Base AST node class
class Node {
public:
    virtual ~Node() {} 
    virtual void print(int indent = 0) const = 0;  // Print AST node for debugging
};

// Base class for all expressions
class NExpression : public Node {
public:
    virtual ~NExpression() {}
};

// Base class for all statements
class NStatement : public Node {
public:
    virtual ~NStatement() {}
};

// Block of statements
class NBlock : public NStatement {
public:
    std::vector<std::unique_ptr<NStatement>> statements;
    
    void print(int indent = 0) const override;
    void addStatement(std::unique_ptr<NStatement> statement);
};

// Identifier (variable or function name)
class NIdentifier : public NExpression {
public:
    std::string name;
    
    NIdentifier(const std::string& name) : name(name) {}
    void print(int indent = 0) const override;
};

// Numeric literal
class NNumeric : public NExpression {
public:
    union {
        int intValue;
        float floatValue;
    };
    bool isFloat;
    
    NNumeric(int value) : intValue(value), isFloat(false) {}
    NNumeric(float value) : floatValue(value), isFloat(true) {}
    void print(int indent = 0) const override;
};

// Binary operation (e.g., +, -, *, /, comparison operators)
class NBinaryOperator : public NExpression {
public:
    std::unique_ptr<NExpression> left;
    std::unique_ptr<NExpression> right;
    int op;  // Operator token from parser
    
    NBinaryOperator(std::unique_ptr<NExpression> left, 
                   int op, 
                   std::unique_ptr<NExpression> right)
        : left(std::move(left)), right(std::move(right)), op(op) {}
    void print(int indent = 0) const override;
};

// Variable declaration
class NVariableDeclaration : public NStatement {
public:
    std::unique_ptr<NIdentifier> type;
    std::unique_ptr<NIdentifier> id;
    std::unique_ptr<NExpression> assignmentExpr;
    
    NVariableDeclaration(std::unique_ptr<NIdentifier> type, 
                         std::unique_ptr<NIdentifier> id)
        : type(std::move(type)), id(std::move(id)), assignmentExpr(nullptr) {}
        
    NVariableDeclaration(std::unique_ptr<NIdentifier> type, 
                         std::unique_ptr<NIdentifier> id, 
                         std::unique_ptr<NExpression> assignmentExpr)
        : type(std::move(type)), id(std::move(id)), 
          assignmentExpr(std::move(assignmentExpr)) {}
          
    void print(int indent = 0) const override;
};

// Expression statement
class NExpressionStatement : public NStatement {
public:
    std::unique_ptr<NExpression> expression;
    
    NExpressionStatement(std::unique_ptr<NExpression> expression) 
        : expression(std::move(expression)) {}
    void print(int indent = 0) const override;
};

// Assignment expression
class NAssignment : public NExpression {
public:
    std::unique_ptr<NIdentifier> lhs;
    std::unique_ptr<NExpression> rhs;
    
    NAssignment(std::unique_ptr<NIdentifier> lhs, 
                std::unique_ptr<NExpression> rhs)
        : lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    void print(int indent = 0) const override;
};

// Function call
class NFunctionCall : public NExpression {
public:
    std::unique_ptr<NIdentifier> id;
    std::vector<std::unique_ptr<NExpression>> arguments;
    
    NFunctionCall(std::unique_ptr<NIdentifier> id)
        : id(std::move(id)) {}
    void print(int indent = 0) const override;
};

// Function declaration
class NFunctionDeclaration : public NStatement {
public:
    std::unique_ptr<NIdentifier> returnType;
    std::unique_ptr<NIdentifier> id;
    std::vector<std::unique_ptr<NVariableDeclaration>> arguments;
    std::unique_ptr<NBlock> block;
    
    NFunctionDeclaration(std::unique_ptr<NIdentifier> returnType,
                        std::unique_ptr<NIdentifier> id,
                        std::vector<std::unique_ptr<NVariableDeclaration>> arguments,
                        std::unique_ptr<NBlock> block)
        : returnType(std::move(returnType)), id(std::move(id)), 
          arguments(std::move(arguments)), block(std::move(block)) {}
    void print(int indent = 0) const override;
};

// If statement
class NIfStatement : public NStatement {
public:
    std::unique_ptr<NExpression> condition;
    std::unique_ptr<NBlock> thenBlock;

    NIfStatement(std::unique_ptr<NExpression> condition, std::unique_ptr<NBlock> thenBlock)
        : condition(std::move(condition)), thenBlock(std::move(thenBlock)) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "IF" << std::endl;
        condition->print(indent + 2);
        thenBlock->print(indent + 2);
    }
};

// If-Else statement
class NIfElseStatement : public NStatement {
public:
    std::unique_ptr<NExpression> condition;
    std::unique_ptr<NBlock> thenBlock;
    std::unique_ptr<NBlock> elseBlock;

    NIfElseStatement(std::unique_ptr<NExpression> condition, 
                     std::unique_ptr<NBlock> thenBlock, 
                     std::unique_ptr<NBlock> elseBlock)
        : condition(std::move(condition)), 
          thenBlock(std::move(thenBlock)), 
          elseBlock(std::move(elseBlock)) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "IF" << std::endl;
        condition->print(indent + 2);
        thenBlock->print(indent + 2);
        if (elseBlock) {
            std::cout << std::string(indent, ' ') << "ELSE" << std::endl;
            elseBlock->print(indent + 2);
        }
    }
};
// While statement
class NWhileStatement : public NStatement {
public:
    std::unique_ptr<NExpression> condition;
    std::unique_ptr<NBlock> block;
    
    NWhileStatement(std::unique_ptr<NExpression> condition,
                   std::unique_ptr<NBlock> block)
        : condition(std::move(condition)), block(std::move(block)) {}
    void print(int indent = 0) const override;
};

// Program (root node)
class NProgram : public Node {
public:
    std::unique_ptr<NBlock> block;
    
    NProgram(std::unique_ptr<NBlock> block) : block(std::move(block)) {}
    void print(int indent = 0) const override;
};

#endif