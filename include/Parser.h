#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <string>
#include <memory> // For std::unique_ptr
#include "lexer.hpp" // Assuming your lexer.cpp is refactored into lexer.hpp/cpp
                     // and provides tokens one by one.
#include "ASTNodes.h" // Your AST node definitions

class Parser {
public:
    // Constructor: takes a lexer or the input stream to create one
    Parser(Lexer& lexer); // Or Parser(std::ifstream& fileStream);

    std::unique_ptr<NBlock> parseProgram(); // Top-level parsing function

private:
    Lexer& lexer; // Reference to your lexer
    token currentToken;
    std::string currentLexeme; // To store the actual string value of IDENTIFIER, NUMBER, etc.

    void advance(); // Consumes the current token and gets the next one
    void expect(token expectedToken); // Consumes token if it's the expected one, else errors

    // Parsing functions for different grammar rules
    std::unique_ptr<NStatement> parseStatement();
    std::unique_ptr<NVariableDeclaration> parseVariableDeclaration();
    std::unique_ptr<NExpression> parseExpression(); // This will be complex
    std::unique_ptr<NFunctionDeclaration> parseFunctionDeclaration();
    std::unique_ptr<NIdentifier> parseIdentifier();
    std::unique_ptr<NNumeric> parseNumber();
    // ... other parsing functions for if, while, return, assignments, etc.
    // For example:
    // std::unique_ptr<NIfStatement> parseIfStatement();
    // std::unique_ptr<NWhileStatement> parseWhileStatement();
    // std::unique_ptr<NReturnStatement> parseReturnStatement();
    // std::unique_ptr<NAssignment> parseAssignment(std::unique_ptr<NIdentifier> varName);
    // std::unique_ptr<NBlock> parseBlock();
};

#endif // PARSER_HPP