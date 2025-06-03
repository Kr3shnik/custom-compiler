#include "../include/Lexer.h"
#include <cctype>
#include <unordered_map>

std::string tokenToString(Token token) {
    static const std::unordered_map<Token, std::string> tokenMap = {
        {Token::IF, "IF"}, {Token::ELSE, "ELSE"}, {Token::WHILE, "WHILE"},
        {Token::RETURN, "RETURN"}, {Token::IDENT, "IDENT"}, {Token::NUMBER, "NUMBER"},
        {Token::FLOAT, "FLOAT"}, {Token::LPAR, "LPAR"}, {Token::RPAR, "RPAR"},
        {Token::LCUR, "LCUR"}, {Token::RCUR, "RCUR"}, {Token::EQUAL, "EQUAL"},
        {Token::COMMA, "COMMA"}, {Token::SEMICOLON, "SEMICOLON"}, {Token::PLUS, "PLUS"},
        {Token::MINUS, "MINUS"}, {Token::MUL, "MUL"}, {Token::DIV, "DIV"},
        {Token::CEQ, "CEQ"}, {Token::CNE, "CNE"}, {Token::CLT, "CLT"},
        {Token::CLE, "CLE"}, {Token::CGT, "CGT"}, {Token::CGE, "CGE"},
        {Token::END_OF_FILE, "END_OF_FILE"}, {Token::UNKNOWN, "UNKNOWN"}
    };
    auto it = tokenMap.find(token);
    if (it != tokenMap.end()) {
        return it->second;
    }
    return "UNKNOWN_TOKEN_IN_TO_STRING";
}


Lexer::Lexer(std::istream& input) : m_input(input), m_lastChar(' ') {
    // Prime the first token
    // getNextToken(); // Or call this explicitly after creating the lexer
}

char Lexer::advance() {
    m_lastChar = m_input.get();
    return m_lastChar;
}

Token Lexer::lexIdentifier() {
    m_lexeme = m_lastChar;
    while (isalnum(advance()) || m_lastChar == '_') {
        m_lexeme += m_lastChar;
    }

    if (m_lexeme == "if") return Token::IF;
    if (m_lexeme == "else") return Token::ELSE;
    if (m_lexeme == "while") return Token::WHILE;
    if (m_lexeme == "return") return Token::RETURN;
    return Token::IDENT;
}

Token Lexer::lexNumber() {
    std::string numStr;
    numStr += m_lastChar;
    bool isFloat = false;

    while (isdigit(advance()) || m_lastChar == '.') {
        if (m_lastChar == '.') {
            if (isFloat) { // Second dot
                m_lexeme = numStr + m_lastChar; // Include the problematic char
                return Token::UNKNOWN;
            }
            isFloat = true;
        }
        numStr += m_lastChar;
    }
    // m_lastChar is now the char AFTER the number

    m_lexeme = numStr;
    if (isFloat) {
        try {
            m_floatVal = std::stof(numStr);
            return Token::FLOAT;
        } catch (const std::out_of_range& oor) {
            return Token::UNKNOWN; // Or handle error
        }
    } else {
         try {
            m_intVal = std::stoi(numStr);
            return Token::NUMBER;
        } catch (const std::out_of_range& oor) {
            return Token::UNKNOWN; // Or handle error
        }
    }
}

Token Lexer::lexCommentOrDiv() {
    if (m_input.peek() == '*') { // Start of a block comment
        advance(); // Consume '*'
        advance(); // Consume the char after '*' to start the loop
        while (true) {
            if (m_lastChar == EOF) {
                m_lexeme = "/*";
                return Token::UNKNOWN; // Unterminated comment
            }
            if (m_lastChar == '*') {
                if (m_input.peek() == '/') {
                    advance(); // Consume '/'
                    advance(); // Consume char after comment
                    return getNextToken(); // Get the next actual token
                }
            }
            advance();
        }
    } else { // It's a division operator
        m_lexeme = "/";
        advance(); // Consume char after /
        return Token::DIV;
    }
}


Token Lexer::getNextToken() {
    // Skip whitespace
    while (isspace(m_lastChar)) {
        advance();
    }

    if (m_lastChar == EOF || m_input.eof()) {
        m_lexeme = "";
        return m_currentToken = Token::END_OF_FILE;
    }

    if (isalpha(m_lastChar) || m_lastChar == '_') {
        return m_currentToken = lexIdentifier();
    }

    if (isdigit(m_lastChar) || (m_lastChar == '.' && isdigit(m_input.peek()))) {
        return m_currentToken = lexNumber();
    }
    
    // Check for operators and punctuation
    switch (m_lastChar) {
        case '(': m_lexeme = "("; advance(); return m_currentToken = Token::LPAR;
        case ')': m_lexeme = ")"; advance(); return m_currentToken = Token::RPAR;
        case '{': m_lexeme = "{"; advance(); return m_currentToken = Token::LCUR;
        case '}': m_lexeme = "}"; advance(); return m_currentToken = Token::RCUR;
        case ',': m_lexeme = ","; advance(); return m_currentToken = Token::COMMA;
        case ';': m_lexeme = ";"; advance(); return m_currentToken = Token::SEMICOLON;
        case '+': m_lexeme = "+"; advance(); return m_currentToken = Token::PLUS;
        case '-': m_lexeme = "-"; advance(); return m_currentToken = Token::MINUS;
        case '*': m_lexeme = "*"; advance(); return m_currentToken = Token::MUL;
        case '/': return m_currentToken = lexCommentOrDiv();
        case '=':
            advance();
            if (m_lastChar == '=') {
                m_lexeme = "=="; advance(); return m_currentToken = Token::CEQ;
            }
            m_lexeme = "="; return m_currentToken = Token::EQUAL; // Assignment
        case '!':
            advance();
            if (m_lastChar == '=') {
                m_lexeme = "!="; advance(); return m_currentToken = Token::CNE;
            }
            m_lexeme = "!"; return m_currentToken = Token::UNKNOWN; // Or a NOT
        case '<':
            advance();
            if (m_lastChar == '=') {
                m_lexeme = "<="; advance(); return m_currentToken = Token::CLE;
            }
            m_lexeme = "<"; return m_currentToken = Token::CLT;
        case '>':
            advance();
            if (m_lastChar == '=') {
                m_lexeme = ">="; advance(); return m_currentToken = Token::CGE;
            }
            m_lexeme = ">"; return m_currentToken = Token::CGT;
    }

    m_lexeme = m_lastChar;
    advance();
    return m_currentToken = Token::UNKNOWN;
}