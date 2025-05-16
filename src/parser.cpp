#include "parser.hpp"
#include <stdexcept> // For std::runtime_error
#include <iostream>  // For error reporting

// Assuming your Lexer class has a getNextToken() that returns a struct/pair 
// like {token_type, lexeme_string}
// For simplicity, let's assume lexer.getNextToken(std::string& lexeme) updates currentToken
// and fills the lexeme.

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    advance(); // Initialize currentToken
}

void Parser::advance() {
    // This function needs to get the next token and its string value (lexeme)
    // from your lexer.
    // For example, if your lexer has: std::pair<token, std::string> getNextToken();
    // auto tokenPair = lexer.getNextToken();
    // currentToken = tokenPair.first;
    // currentLexeme = tokenPair.second;
    // This is a placeholder, adapt to your actual lexer interface.
    // For now, let's assume your global tokenizer can be adapted or you have a Lexer class.
    // If your tokenizer returns a vector, you'd manage an index into that vector here.
    std::cout << "Advance called - current token needs to be updated from lexer" << std::endl;
    // Placeholder:
    // currentToken = lexer.getNextToken(currentLexeme); 
}

void Parser::expect(token expectedToken) {
    if (currentToken == expectedToken) {
        advance();
    } else {
        // You'll want better error reporting (line numbers, etc.)
        throw std::runtime_error("Unexpected token: expected " + tokenToString(expectedToken) + 
                                 " but got " + tokenToString(currentToken));
    }
}

std::unique_ptr<NBlock> Parser::parseProgram() {
    // A program is often a block of statements or declarations
    auto programBlock = std::make_unique<NBlock>();
    // Assuming your lexer provides a special EOF token
    while (currentToken != EOF_TOKEN /* Replace EOF_TOKEN with your actual end-of-file token */) {
        // For C-like languages, top-level could be function declarations or global var declarations
        if (currentToken == IDENTIFIER) { // Could be start of function or variable
            // This logic needs to be more sophisticated to distinguish
            // e.g. "int main()" from "int x;"
            // For now, let's assume statements for simplicity
            programBlock->addStatement(parseStatement());
        } else {
             // Or handle specific top-level constructs like function definitions
             // if (currentToken == /* some keyword for function like 'def' or type like 'int' followed by identifier and '(' */ ) {
             //    programBlock->addStatement(parseFunctionDeclaration());
             // } else {
            // Handle error or skip token
            std::cerr << "Skipping unexpected token at program level: " << tokenToString(currentToken) << std::endl;
            advance(); 
            // }
        }
    }
    return programBlock;
}

std::unique_ptr<NStatement> Parser::parseStatement() {
    // Determine the type of statement based on the current token
    switch (currentToken) {
        case IDENTIFIER: {
            // Could be a variable declaration (e.g., "int x;"), an assignment ("x = 5;"),
            // or a function call statement ("myFunc();").
            // You'll need to look ahead or have more info from the lexer.
            // For simplicity, let's assume it could be a variable declaration for now.
            // A more robust parser would peek at the next token.
            // If next token is '(', it's a function call. If next token is '=', it's assignment.
            // If it's a type followed by identifier, it's a declaration.
            // This part needs careful grammar design.
            // Let's assume for now that an IDENTIFIER starting a statement is a variable declaration
            // if the IDENTIFIER is a known type (int, float, etc.)
            // This is a simplification. In C, types are keywords or typedef'd identifiers.
            
            // Let's assume a simplified grammar where "type identifier;" is a declaration
            // This requires your lexer to distinguish type keywords from general identifiers,
            // or your parser to check if an identifier is a known type.
            // For now, let's assume parseVariableDeclaration handles this.
            return parseVariableDeclaration(); 
        }
        // case IF:
        //     return parseIfStatement();
        // case WHILE:
        //     return parseWhileStatement();
        // case RETURN:
        //     return parseReturnStatement();
        // case LCUR: // '{' - start of a block
        //     return parseBlock();
        default:
            // Could be an expression statement (e.g. "x + y;")
            // auto expr = parseExpression();
            // expect(SEMICOLON); // Assuming you add SEMICOLON token
            // return expr; // If expressions can be statements
            throw std::runtime_error("Unknown statement starting with token: " + tokenToString(currentToken));
    }
}

std::unique_ptr<NVariableDeclaration> Parser::parseVariableDeclaration() {
    // Example: "int x = 10;" or "float y;"
    // Assumes currentToken is the type (e.g., an IDENTIFIER that is a type, or a specific TYPE token)
    // This is highly dependent on how your lexer and grammar define types.
    // Let's assume the first token is an IDENTIFIER representing the type.
    if (currentToken != IDENTIFIER && currentToken != ALLOCATION /* or specific type tokens */) {
        throw std::runtime_error("Expected type identifier for variable declaration.");
    }
    std::string typeName = currentLexeme; // Store the type name
    advance(); // Consume type

    if (currentToken != IDENTIFIER) {
        throw std::runtime_error("Expected identifier name for variable declaration.");
    }
    NIdentifier identifier(currentLexeme); // Create NIdentifier for the variable name
    advance(); // Consume identifier name

    std::unique_ptr<NExpression> assignmentExpr = nullptr;
    if (currentToken == EQUAL) {
        advance(); // Consume '='
        assignmentExpr = parseExpression(); // Parse the expression on the RHS
    }

    // expect(SEMICOLON); // Consume ';' - you'll need a SEMICOLON token

    // Create NVariableDeclaration node
    // The NVariableDeclaration constructor would take type and identifier,
    // and optionally the assignment expression.
    // This is a guess at your NVariableDeclaration structure.
    // return std::make_unique<NVariableDeclaration>(NIdentifier(typeName), identifier, std::move(assignmentExpr));
    
    // Simpler version if NVariableDeclaration takes type as string:
    auto varDecl = std::make_unique<NVariableDeclaration>(NIdentifier(typeName), identifier);
    if (assignmentExpr) {
        // varDecl->setAssignment(std::move(assignmentExpr)); // If NVariableDeclaration has such a method
    }
    return varDecl;
}


std::unique_ptr<NExpression> Parser::parseExpression() {
    // This is where you'd implement Pratt parsing or precedence climbing for binary operators,
    // handle unary operators, function calls, literals, parenthesized expressions.
    // See Kaleidoscope's ParseExpression, ParsePrimary, ParseBinOpRHS.
    // For now, a very simple placeholder:
    if (currentToken == NUMBER || currentToken == FLOAT) {
        // return parseNumber();
        std::string numVal = currentLexeme;
        advance();
        // You'd need to convert numVal to double/int and create appropriate NNumeric AST node
        // return std::make_unique<NNumeric>(std::stod(numVal)); // Example
    } else if (currentToken == IDENTIFIER) {
        // return parseIdentifierExpr(); // Could be a variable or a function call
    }
    // ... and so on for other primary expressions
    throw std::runtime_error("parseExpression not fully implemented or unexpected token for expression.");
    return nullptr; 
}

// ... Implementations for parseFunctionDeclaration, parseIdentifier, parseNumber, etc.