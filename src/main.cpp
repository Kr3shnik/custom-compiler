#include <iostream>
#include <fstream>
#include <string>
#include <vector> // Added for std::vector
#include "../include/Lexer.h" 
#include "../include/Parser.h" 
#include "../include/SymbolTable.h" 
#include "../include/ASTNodes.h" 

// The old enum token and tokenizer function should be removed or fully commented out if not used.
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
*/

int main(){
    std::istream& input = std::cin;

    Lexer lexer(input);
    SymbolTable symbolTable; 
    Parser parser(lexer, symbolTable); 

    try {
        std::unique_ptr<NProgram> ast = parser.parseProgram();
        if (ast) { // This check is now more meaningful
            // Only print AST and Symbol Table if parsing was truly successful (no errors encountered)
            std::cout << "Parsing successful." << std::endl; // This message might be misleading if errors were recovered from.
                                                          // Consider changing it or removing it if errors were present.
                                                          // For the test script, the exit code is what matters most.
            std::cout << "\n--- Abstract Syntax Tree ---" << std::endl;
            ast->print(); 
            std::cout << "\n--- Symbol Table ---" << std::endl;
            symbolTable.print(); 
            return 0; // Explicitly return 0 on full success
        } else {
            // This block will now be hit if parseProgram returns nullptr due to has_encountered_error
            std::cerr << "Parsing failed or completed with errors." << std::endl;
            return 1; // Non-zero exit code for failure
        }
    } catch (const ParserError& e) {
        std::cerr << "Parser Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "An unexpected error occurred: " << e.what() << std::endl;
        return 1;
    }
    // Should not be reached if logic is correct
    return 1; 
}