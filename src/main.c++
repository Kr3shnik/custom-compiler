#include <iostream>
#include <fstream>
#include <string>
#include "token.c++"
/*
enum token{
    IF,
    ELSE,
    WHILE,
    RETURN,
    IDENTIFIER,
    PRINT
};

token tokenizer(std::string word){
    //Check if word is a direct Keyword
    if (word == "if") {
        return IF;
    } else if (word == "else") {
        return ELSE;
    } else if (word == "while") {
        return WHILE;
    } else if (word == "return") {
        return RETURN;
    } else if (word == "print") {
        return PRINT;
    } 
    else{
        //Check if single chars can give us a clue what the word is
        return IDENTIFIER;
    }
}


std::string tokenToString(token t) {
    switch (t) {
        case IF: return "IF";
        case ELSE: return "ELSE";
        case WHILE: return "WHILE";;
        case RETURN: return "RETURN";
        case IDENTIFIER: return "IDENTIFIER";
        case PRINT: return "PRINT";
        default: return "UNKNOWN";
    }
}

int main() {
    // Print "Hello, World!" to the console
    std::cout << "Hello, World!" << std::endl;

    std::fstream file("../tests/success/test7");
    if (!file) {
        std::cerr << "Error opening file." << std::endl;
        return 1;
    }
    
    
    while (file) {
        std::string word;
        file >> word;
        token t = tokenizer(word);
        std::cout << tokenToString(t) << std::endl;
    }

    return 0;
}
    */
int main(){
    std::ifstream file("../tests/success/test1");
    std::vector<token> tokens = tokenizer(file);
    for (int i = 0; i < tokens.size(); i++)
    {
        std::cout << tokenToString(tokens[i]) << std::endl;
    }
}