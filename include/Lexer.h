#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <istream>
#include <vector>

enum class Token {
    // Keywords
    IF, ELSE, WHILE, RETURN,
    // Identifiers and Literals
    IDENT, NUMBER, FLOAT,
    // Operators and Punctuation
    LPAR, RPAR, LCUR, RCUR,
    EQUAL, // Assignment =
    COMMA, SEMICOLON,
    PLUS, MINUS, MUL, DIV,
    CEQ, CNE, CLT, CLE, CGT, CGE, // ==, !=, <, <=, >, >=
    // Special
    END_OF_FILE, UNKNOWN
};

std::string tokenToString(Token token); // For debugging

class Lexer {
public:
    Lexer(std::istream& input);

    Token getNextToken();

    Token getCurrentToken() const { return m_currentToken; }
    const std::string& getLexeme() const { return m_lexeme; }
    int getIntVal() const { return m_intVal; }
    float getFloatVal() const { return m_floatVal; }

private:
    std::istream& m_input;
    char m_lastChar;

    Token m_currentToken;
    std::string m_lexeme;
    int m_intVal;
    float m_floatVal;

    char advance();
    Token lexIdentifier();
    Token lexNumber();
    Token lexCommentOrDiv();
};

#endif // CUSTOM_LEXER_H