#ifndef PARSER_H
#define PARSER_H

#include "Lexer.h"
#include "ASTNodes.h"      // Assuming this is in the include path
#include "SymbolTable.h"   // Assuming this is in the include path
#include <vector>
#include <memory>
#include <stdexcept> // For std::runtime_error

// Forward declaration
class NBlock;
class NStatement;
class NExpression;
class NVariableDeclaration;
class NFunctionDeclaration;
class NIdentifier;
class NNumeric; // Assuming NNumeric can handle both int and float from ASTNodes.h
class NReturnStatement;
class NExpressionStatement;
class NIfElseStatement;
class NWhileStatement;
class NAssignment;
class NFunctionCall;
class NBinaryOperator;
class NProgram;


class ParserError : public std::runtime_error {
public:
    ParserError(const std::string& message) : std::runtime_error(message) {}
};

class Parser {
public:
    Parser(Lexer& lexer, SymbolTable& symbolTable);

    std::unique_ptr<NProgram> parseProgram();

private:
    Lexer& m_lexer;
    SymbolTable& m_symbolTable;
    Token m_currentToken;

    void advance();
    void expect(Token expectedToken, const std::string& errorMessage);
    std::string getCurrentLexeme(); // Helper to get lexeme from lexer

    // Parsing methods based on grammar rules
    std::unique_ptr<NBlock> parseBlock();
    std::vector<std::unique_ptr<NStatement>> parseStatementList();
    std::unique_ptr<NStatement> parseStatement();
    
    std::unique_ptr<NVariableDeclaration> parseVariableDeclaration();
    std::unique_ptr<NFunctionDeclaration> parseFunctionDeclaration();
    std::vector<std::unique_ptr<NVariableDeclaration>> parseFunctionParameters();
    std::unique_ptr<NIfElseStatement> parseIfElseStatement();
    std::unique_ptr<NWhileStatement> parseWhileStatement();
    std::unique_ptr<NReturnStatement> parseReturnStatement();
    std::unique_ptr<NExpressionStatement> parseExpressionStatement();

    std::unique_ptr<NExpression> parseExpression();
    std::unique_ptr<NExpression> parseAssignmentExpression();
    std::unique_ptr<NExpression> parseBinaryOpRHS(int exprPrec, std::unique_ptr<NExpression> LHS);
    std::unique_ptr<NExpression> parsePrimary();
    std::unique_ptr<NIdentifier> parseIdentifierNode(); // Renamed to avoid conflict
    std::unique_ptr<NExpression> parseNumeric();
    std::vector<std::unique_ptr<NExpression>> parseCallArguments();
    
    int getTokenPrecedence(Token tok);
};

#endif // CUSTOM_PARSER_H