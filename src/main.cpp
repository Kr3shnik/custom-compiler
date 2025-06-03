#include <iostream>
#include <fstream>
#include <string>
#include <vector> // Added for std::vector
#include "../include/Lexer.h" 
#include "../include/Parser.h" 
#include "../include/SymbolTable.h" 
#include "../include/ASTNodes.h"
#include "../include/SemanticAnalyzer.h" 

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
    SymbolTable parsingSymbolTable; // For parsing (minimal use)
    Parser parser(lexer, parsingSymbolTable); 

    try {
        std::unique_ptr<NProgram> ast = parser.parseProgram();
        if (ast) { 
            std::cout << "Parsing successful." << std::endl;
            
            // Create a fresh symbol table for semantic analysis
            SymbolTable semanticSymbolTable;
            SemanticAnalyzer semanticAnalyzer(semanticSymbolTable);
            bool semanticsOk = semanticAnalyzer.analyze(ast.get());
            
            // Report semantic warnings and errors
            if (semanticAnalyzer.hasWarnings()) {
                semanticAnalyzer.reportWarnings();
            }
            
            if (semanticAnalyzer.hasErrors()) {
                semanticAnalyzer.reportErrors();
                if (semanticAnalyzer.hasFatalErrors()) {
                    std::cerr << "Compilation failed due to fatal semantic errors." << std::endl;
                    return 1;
                }
                std::cerr << "Compilation completed with semantic errors." << std::endl;
            }
            
            if (semanticsOk && !semanticAnalyzer.hasErrors()) {
                std::cout << "Semantic analysis successful." << std::endl;
            }
            
            std::cout << "\n--- Abstract Syntax Tree ---" << std::endl;
            ast->print(); 
            std::cout << "\n--- Symbol Table ---" << std::endl;
            semanticSymbolTable.print();
            
            // Print semantic analysis summary
            std::cout << "\n--- Semantic Analysis Summary ---" << std::endl;
            std::cout << "Errors: " << semanticAnalyzer.getErrorCount() << std::endl;
            std::cout << "Warnings: " << semanticAnalyzer.getWarningCount() << std::endl;
            
            return semanticAnalyzer.hasErrors() ? 1 : 0;
        } else {
            std::cerr << "Parsing failed or completed with errors." << std::endl;
            return 1;
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