#include <vector>
#include <string>
#include <iostream>
#include <fstream>
enum token
{
    IF,
    ELSE,
    WHILE,
    RETURN,
    IDENTIFIER,
    PRINT,
    ILLEGAL,
    NUMBER,
    FLOAT,
    ALLOCATION,
    EQUAL,
    LESS,
    GREATER,
    LPAR,
    RPAR,
    LCUR,
    RCUR
};

std::vector<token> tokenizer(std::ifstream &file)
{
    std::vector<token> tokens;
    std::string word;

    while (file >> word)
    {
        std::cout << word << std::endl;
        // Check if word is a direct Keyword
        if (word == "if")
        {
            tokens.push_back(IF);
        }
        else if (word == "else")
        {
            tokens.push_back(ELSE);
        }
        else if (word == "while")
        {
            tokens.push_back(WHILE);
        }
        else if (word == "return")
        {
            tokens.push_back(RETURN);
        }
        else if (word == "print")
        {
            tokens.push_back(PRINT);
        }
        else
        {
            // Check if single chars can give us a clue what the word is
            for (int i = 0; i < word.length(); i++)
            {

                if (isalpha(word[i]))
                {
                    tokens.push_back(IDENTIFIER);
                    while (i < word.length() && (isalpha(word[i]) || isdigit(word[i])))
                    {
                        i++;
                    }
                }
                else if (isdigit(word[i]))
                {
                    bool isFloat = false;
                    while (i < word.length())
                    {
                        i++;
                        if (!isdigit(word[i]))
                        {
                            if (word[i] == '.' && !isFloat)
                            {
                                isFloat = true;
                            }
                            else
                            {
                                tokens.push_back(ILLEGAL);
                                break;
                            }
                        }
                    }
                    if (isFloat)
                    {
                        tokens.push_back(FLOAT);
                    }
                    else
                    {
                        tokens.push_back(NUMBER);
                    }
                }
                switch (word[i])
                {
                case '=':
                    tokens.push_back(EQUAL);
                    break;
                case '<':
                    tokens.push_back(LESS);
                    break;
                case '>':
                    tokens.push_back(GREATER);
                    break;
                case '(':
                    tokens.push_back(LPAR);
                    break;
                case ')':
                    tokens.push_back(RPAR);
                    break;
                case '{':
                    tokens.push_back(LCUR);
                    break;
                case '}':
                    tokens.push_back(RCUR);
                    break;

                default:
                    break;
                }
                /*
                if(isalpha(word[0])){
                    tokens.push_back(IDENTIFIER);
                }else if(isdigit(word[0])){
                    bool isFloat = false;
                    for (int i = 1; i < word.length(); i++) {
                        if (!isdigit(word[i])) {
                            tokens.push_back(ILLEGAL);
                            break;
                        }
                        else if(word[i] == '.'){
                            if(isFloat){
                                tokens.push_back(ILLEGAL);
                                break;
                            }
                            else{
                                isFloat = true;
                            }
                        }
                    }
                    if(isFloat){
                        tokens.push_back(FLOAT);
                    }
                    else{
                        tokens.push_back(NUMBER);
                    }
                }*/
            }
            
        }
    }
    return tokens;
}

std::string tokenToString(token t) {
    switch (t) {
        case IF: return "IF";
        case ELSE: return "ELSE";
        case WHILE: return "WHILE";
        case RETURN: return "RETURN";
        case IDENTIFIER: return "IDENTIFIER";
        case PRINT: return "PRINT";
        case ILLEGAL: return "ILLEGAL";
        case NUMBER: return "NUMBER";
        case FLOAT: return "FLOAT";
        case ALLOCATION: return "ALLOCATION";
        case EQUAL: return "EQUAL";
        case LESS: return "LESS";
        case GREATER: return "GREATER";
        case LPAR: return "LPAR";
        case RPAR: return "RPAR";
        case LCUR: return "LCUR";
        case RCUR: return "RCUR";
        default: return "UNKNOWN";
    }
}