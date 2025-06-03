#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "ASTNodes.h"
#include "SymbolTable.h"
#include <string>
#include <vector>
#include <memory>

// Semantic error types
enum class SemanticErrorType {
    UNDECLARED_VARIABLE,
    UNDECLARED_FUNCTION,
    REDECLARATION,
    TYPE_MISMATCH,
    WRONG_ARGUMENT_COUNT,
    ASSIGNMENT_TO_FUNCTION,
    VOID_EXPRESSION_USED,
    MISSING_RETURN,
    INVALID_MAIN_SIGNATURE
};

// Semantic error information
struct SemanticError {
    SemanticErrorType type;
    std::string message;
    bool isFatal;
    
    SemanticError(SemanticErrorType type, const std::string& message, bool isFatal = false)
        : type(type), message(message), isFatal(isFatal) {}
};

class SemanticAnalyzer {
private:
    SymbolTable& symbolTable;
    std::vector<SemanticError> errors;
    std::vector<SemanticError> warnings;
    
    // Current function context for return type checking
    std::string currentFunctionReturnType;
    bool inFunctionBody;
    
    // Built-in function information
    void initializeBuiltins();
    
    // Type checking helpers
    std::string getExpressionType(NExpression* expr);
    bool areTypesCompatible(const std::string& expected, const std::string& actual);
    std::string getPromotedType(const std::string& type1, const std::string& type2);
    
    // AST traversal methods
    void analyzeNode(Node* node);
    void analyzeBlock(NBlock* block);
    void analyzeStatement(NStatement* stmt);
    void analyzeExpression(NExpression* expr);
    
    // Specific node analysis
    void analyzeProgram(NProgram* program);
    void analyzeVariableDeclaration(NVariableDeclaration* varDecl);
    void analyzeFunctionDeclaration(NFunctionDeclaration* funcDecl);
    void analyzeAssignment(NAssignment* assignment);
    void analyzeFunctionCall(NFunctionCall* funcCall);
    void analyzeBinaryOperator(NBinaryOperator* binOp);
    void analyzeReturnStatement(NReturnStatement* returnStmt);
    void analyzeIfElseStatement(NIfElseStatement* ifElse);
    void analyzeWhileStatement(NWhileStatement* whileStmt);
    void analyzeIdentifier(NIdentifier* ident);
    
    // Helper methods
    void addError(SemanticErrorType type, const std::string& message, bool isFatal = false);
    void addWarning(SemanticErrorType type, const std::string& message);
    bool hasMainFunction();
    void validateMainFunction();
    
public:
    SemanticAnalyzer(SymbolTable& symbolTable);
    
    // Main analysis entry point
    bool analyze(NProgram* program);
    
    // Error and warning reporting
    void reportErrors() const;
    void reportWarnings() const;
    bool hasErrors() const { return !errors.empty(); }
    bool hasFatalErrors() const;
    bool hasWarnings() const { return !warnings.empty(); }
    
    // Get error counts
    size_t getErrorCount() const { return errors.size(); }
    size_t getWarningCount() const { return warnings.size(); }
};

#endif // SEMANTIC_ANALYZER_H
