#include "../include/Parser.h"
#include <iostream> // For error reporting

// Assuming ASTNodes.h provides these constructors and methods:
// NProgram(std::unique_ptr<NBlock>)
// NBlock()
// NBlock::addStatement(std::unique_ptr<NStatement>)
// NVariableDeclaration(std::unique_ptr<NIdentifier> type, std::unique_ptr<NIdentifier> name)
// NVariableDeclaration(std::unique_ptr<NIdentifier> type, std::unique_ptr<NIdentifier> name, std::unique_ptr<NExpression> assignmentExpr)
// NFunctionDeclaration(std::unique_ptr<NIdentifier> type, std::unique_ptr<NIdentifier> name, std::vector<std::unique_ptr<NVariableDeclaration>> params, std::unique_ptr<NBlock> body)
// NReturnStatement(std::unique_ptr<NExpression>)
// NExpressionStatement(std::unique_ptr<NExpression>)
// NIfElseStatement(std::unique_ptr<NExpression> cond, std::unique_ptr<NBlock> thenBlock, std::unique_ptr<NBlock> elseBlock)
// NWhileStatement(std::unique_ptr<NExpression> cond, std::unique_ptr<NBlock> body)
// NIdentifier(const std::string& name)
// NNumeric(int value) / NNumeric(float value) or a template / variant
// NAssignment(std::unique_ptr<NIdentifier> lhs, std::unique_ptr<NExpression> rhs)
// NFunctionCall(std::unique_ptr<NIdentifier> name)
// NFunctionCall::arguments (std::vector<std::unique_ptr<NExpression>>)
// NBinaryOperator(std::unique_ptr<NExpression> lhs, int op, std::unique_ptr<NExpression> rhs) // op as int or your Token enum

Parser::Parser(Lexer& lexer, SymbolTable& symbolTable)
    : m_lexer(lexer), m_symbolTable(symbolTable) {
    advance(); // Initialize m_currentToken
}

void Parser::advance() {
    m_currentToken = m_lexer.getNextToken();
}

std::string Parser::getCurrentLexeme() {
    return m_lexer.getLexeme();
}

void Parser::expect(Token expectedToken, const std::string& errorMessage) {
    if (m_currentToken == expectedToken) {
        advance();
    } else {
        std::string errorMsg = errorMessage + ". Expected " + tokenToString(expectedToken) +
                               ", but got " + tokenToString(m_currentToken) +
                               " ('" + m_lexer.getLexeme() + "')";
        throw ParserError(errorMsg);
    }
}

std::unique_ptr<NProgram> Parser::parseProgram() {
    auto programLevelBlock = std::make_unique<NBlock>();
    bool has_encountered_error = false; // Track if any error occurs

    while (m_currentToken != Token::END_OF_FILE) {
        try {
            programLevelBlock->addStatement(parseStatement());
        } catch (const ParserError& e) {
            has_encountered_error = true; // Mark that an error occurred
            std::cerr << "Error during parsing: " << e.what() << std::endl;
            std::cerr << "Attempting to recover..." << std::endl;
            
            bool recovered = false;
            while(m_currentToken != Token::END_OF_FILE) {
                advance(); 
                if (m_currentToken == Token::IDENT || m_currentToken == Token::END_OF_FILE) { 
                    recovered = true;
                    break;
                }
            }
            if (!recovered && m_currentToken == Token::END_OF_FILE) {
                 std::cerr << "Recovery failed: Reached EOF." << std::endl;
                 // If recovery fails and we are at EOF, we should break the main parsing loop.
                 // The has_encountered_error flag will ensure we return nullptr.
                 break; 
            }
            if(recovered) {
                std::cerr << "Recovery attempted, continuing parsing from token: " << tokenToString(m_currentToken) << " ('" << m_lexer.getLexeme() << "')" << std::endl;
            } else {
                // If not recovered and not EOF, something is wrong, break to be safe.
                break;
            }
        }
    }

    if (has_encountered_error) {
        std::cerr << "Parsing completed with errors." << std::endl;
        return nullptr; // Signal that parsing was not fully successful
    }

    return std::make_unique<NProgram>(std::move(programLevelBlock));
}


std::vector<std::unique_ptr<NStatement>> Parser::parseStatementList() {
    std::vector<std::unique_ptr<NStatement>> statements;
    // In your grammar, `stms` can be one statement or `stms stmt`.
    // A block ends with RCUR or EOF for the main program.
    while (m_currentToken != Token::RCUR && m_currentToken != Token::END_OF_FILE) {
        statements.push_back(parseStatement());
    }
    return statements;
}

std::unique_ptr<NBlock> Parser::parseBlock() {
    expect(Token::LCUR, "Expected '{' to start a block");
    m_symbolTable.enterScope();
    auto block = std::make_unique<NBlock>();
    
    // parseStatementList will read until RCUR
    std::vector<std::unique_ptr<NStatement>> stmts = parseStatementList();
    for(auto& stmt : stmts) {
        block->addStatement(std::move(stmt));
    }
    
    expect(Token::RCUR, "Expected '}' to end a block");
    m_symbolTable.exitScope();
    return block;
}


std::unique_ptr<NStatement> Parser::parseStatement() {
    // Ensure m_currentToken is up-to-date if lexer state was manipulated elsewhere (shouldn't be the case normally)
    // m_currentToken = m_lexer.getCurrentToken(); // This line is usually not needed here if advance() is used consistently.

    switch (m_currentToken) {
        case Token::IDENT: { // Open a new scope for the IDENT case
            // This is a lookahead simulation using the lexer's state
            // The logic here attempts to distinguish between var/func declarations and expression statements.
            // It consumes tokens to look ahead.

            // Save the initial state of m_currentToken for this case
            Token initialTokenForIdentCase = m_currentToken;
            std::string lexeme1 = m_lexer.getLexeme(); // Lexeme of the first IDENT

            // "Peek" next token by getting it from lexer
            // This advances the lexer's internal state, and thus m_lastChar.
            // The parser's m_currentToken will be updated via advance() or by direct assignment.
            Token token2 = m_lexer.getNextToken(); 

            if (token2 == Token::IDENT) {
                std::string lexeme2 = m_lexer.getLexeme(); // Lexeme of the second IDENT
                Token token3 = m_lexer.getNextToken();    // Token after the second IDENT

                // Update parser's current token to what's after "type name"
                m_currentToken = token3;

                auto typeNode = std::make_unique<NIdentifier>(lexeme1);
                auto nameNode = std::make_unique<NIdentifier>(lexeme2);

                if (m_currentToken == Token::LPAR) { // Function Declaration
                    std::vector<std::unique_ptr<NVariableDeclaration>> params = parseFunctionParameters();
                    std::unique_ptr<NBlock> body = parseBlock();
                    m_symbolTable.declare(nameNode->name, typeNode->name, SymbolType::FUNCTION);
                    return std::make_unique<NFunctionDeclaration>(std::move(typeNode), std::move(nameNode), std::move(params), std::move(body));
                } else { // Variable Declaration
                    std::unique_ptr<NExpression> expr = nullptr;
                    if (m_currentToken == Token::EQUAL) {
                        advance(); // consume '='
                        expr = parseExpression();
                    }
                    expect(Token::SEMICOLON, "Expected ';' after variable declaration");
                    m_symbolTable.declare(nameNode->name, typeNode->name, SymbolType::VARIABLE);
                    return std::make_unique<NVariableDeclaration>(std::move(typeNode), std::move(nameNode), std::move(expr));
                }
            } else {
                // First token was IDENT, second was not. It's an expression statement.
                // The lexer has advanced past the first IDENT (lexeme1).
                // The parser's m_currentToken should be token2.
                m_currentToken = token2;

                // Create the LHS for the expression from the first IDENT
                auto expr_lhs = std::make_unique<NIdentifier>(lexeme1);
                
                // Now parse the rest of the expression (e.g., assignment, binary op, function call)
                // starting with expr_lhs as the primary, and m_currentToken as the token *after* lexeme1.
                // parseAssignmentExpression or parseBinaryOpRHS can be used here.
                // Since parseExpression calls parseAssignmentExpression, which calls parseBinaryOpRHS,
                // we can construct the expression starting from the known LHS.

                // If it's an assignment:
                if (m_currentToken == Token::EQUAL) {
                    advance(); // Consume '='
                    auto rhs = parseAssignmentExpression(); // Assignment is right-associative
                    expect(Token::SEMICOLON, "Expected ';' after assignment statement.");
                    return std::make_unique<NExpressionStatement>(
                        std::make_unique<NAssignment>(std::move(expr_lhs), std::move(rhs))
                    );
                } else {
                    // Otherwise, it's a binary operation, function call, or just the identifier.
                    // parseBinaryOpRHS will handle operators or return expr_lhs if no operator.
                    // If expr_lhs was a function name, parsePrimary (called by parseBinaryOpRHS if no op)
                    // would need to handle the call part if LPAR follows.
                    // The current structure of parsePrimary expects to parse the IDENT itself.
                    // This path for single IDENT expression needs careful handling.

                    // Let's refine: expr_lhs is the NIdentifier.
                    // If m_currentToken is LPAR, it's a function call.
                    if (m_currentToken == Token::LPAR) {
                        auto callNode = std::make_unique<NFunctionCall>(std::move(expr_lhs));
                        callNode->arguments = parseCallArguments(); // parseCallArguments consumes LPAR...RPAR
                        // Now check for semicolon after the call
                        expect(Token::SEMICOLON, "Expected ';' after function call statement.");
                        return std::make_unique<NExpressionStatement>(std::move(callNode));
                    }

                    // If not a call, it could be part of a binary operation or standalone.
                    auto expr = parseBinaryOpRHS(0, std::move(expr_lhs));
                    expect(Token::SEMICOLON, "Expected ';' after expression statement.");
                    return std::make_unique<NExpressionStatement>(std::move(expr));
                }
            }
            // Ensure all paths within this IDENT case return.
            // The logic above should cover all sub-cases of IDENT.
            // If somehow a path is missed, the compiler warning for non-void function would appear.
            // Adding a fallback throw here just in case, though ideally unreachable.
            // throw ParserError("Internal parser error: Unhandled path in IDENT statement parsing.");
        } // Close the scope for Token::IDENT case
        case Token::RETURN: return parseReturnStatement();
        case Token::IF:     return parseIfElseStatement();
        case Token::WHILE:  return parseWhileStatement();
        case Token::LCUR:   return parseBlock(); // A block can be a statement itself
        default: {
            // If no other statement matches, try to parse as an expression statement
            // This handles cases like `numeric SEMICOLON` or `LPAR expr RPAR SEMICOLON`
            // Or if the IDENT logic above falls through (which it shouldn't with current structure).
            auto expr = parseExpression();
            expect(Token::SEMICOLON, "Expected ';' after expression statement");
            return std::make_unique<NExpressionStatement>(std::move(expr));
        }
    }
    throw ParserError("Unknown statement starting with token: " + tokenToString(m_currentToken));
}


std::unique_ptr<NIdentifier> Parser::parseIdentifierNode() {
    if (m_currentToken != Token::IDENT) {
        throw ParserError("Expected identifier, got " + tokenToString(m_currentToken));
    }
    std::string name = getCurrentLexeme();
    advance(); // Consume IDENT
    return std::make_unique<NIdentifier>(name);
}

std::unique_ptr<NExpression> Parser::parseNumeric() {
    std::unique_ptr<NExpression> result;
    if (m_currentToken == Token::NUMBER) {
        result = std::make_unique<NNumeric>(m_lexer.getIntVal());
    } else if (m_currentToken == Token::FLOAT) {
        result = std::make_unique<NNumeric>(m_lexer.getFloatVal());
    } else {
        throw ParserError("Expected number or float, got " + tokenToString(m_currentToken));
    }
    advance(); // Consume NUMBER or FLOAT
    return result;
}

std::vector<std::unique_ptr<NVariableDeclaration>> Parser::parseFunctionParameters() {
    expect(Token::LPAR, "Expected '(' for function parameters");
    std::vector<std::unique_ptr<NVariableDeclaration>> params;
    if (m_currentToken != Token::RPAR) {
        while (true) {
            // Each param is `type name`
            auto type = parseIdentifierNode();
            auto name = parseIdentifierNode();
            // Add to symbol table (within function's new scope, handled by func_decl or block)
            // For now, assume SymbolTable handles this when NFunctionDeclaration is processed.
            // Or, add here if scope is already entered.
            // The grammar.y adds var_decl to symbol table.
            m_symbolTable.declare(name->name, type->name, SymbolType::VARIABLE); // Param is a var
            params.push_back(std::make_unique<NVariableDeclaration>(std::move(type), std::move(name)));

            if (m_currentToken == Token::RPAR) break;
            expect(Token::COMMA, "Expected ',' or ')' in parameter list");
        }
    }
    expect(Token::RPAR, "Expected ')' after function parameters");
    return params;
}


std::unique_ptr<NReturnStatement> Parser::parseReturnStatement() {
    expect(Token::RETURN, "Expected 'return'");
    auto expr = parseExpression();
    expect(Token::SEMICOLON, "Expected ';' after return statement");
    return std::make_unique<NReturnStatement>(std::move(expr));
}

std::unique_ptr<NIfElseStatement> Parser::parseIfElseStatement() {
    expect(Token::IF, "Expected 'if'");
    expect(Token::LPAR, "Expected '(' after 'if'");
    auto condition = parseExpression();
    expect(Token::RPAR, "Expected ')' after if condition");
    auto thenBlock = parseBlock();
    std::unique_ptr<NBlock> elseBlock = nullptr;
    if (m_currentToken == Token::ELSE) {
        advance(); // Consume 'else'
        elseBlock = parseBlock();
    }
    return std::make_unique<NIfElseStatement>(std::move(condition), std::move(thenBlock), std::move(elseBlock));
}

std::unique_ptr<NWhileStatement> Parser::parseWhileStatement() {
    expect(Token::WHILE, "Expected 'while'");
    expect(Token::LPAR, "Expected '(' after 'while'");
    auto condition = parseExpression();
    expect(Token::RPAR, "Expected ')' after while condition");
    auto body = parseBlock();
    return std::make_unique<NWhileStatement>(std::move(condition), std::move(body));
}


// Expression Parsing (Operator-Precedence Climbing)
int Parser::getTokenPrecedence(Token tok) {
    // Based on grammar.y: %left PLUS MINUS, %left MUL DIV, %left CEQ CNE etc.
    // Higher value = higher precedence
    switch (tok) {
        case Token::MUL:
        case Token::DIV:
            return 40;
        case Token::PLUS:
        case Token::MINUS:
            return 30;
        case Token::CLT: // <
        case Token::CLE: // <=
        case Token::CGT: // >
        case Token::CGE: // >=
            return 20;
        case Token::CEQ: // ==
        case Token::CNE: // !=
            return 10;
        // EQUAL (assignment) has very low precedence and is right-associative,
        // handled by parseAssignmentExpression.
        default:
            return -1; // Not a binary operator or lowest precedence
    }
}

std::unique_ptr<NExpression> Parser::parseExpression() {
    // Entry point for expressions, handles assignment separately due to right-associativity
    return parseAssignmentExpression();
}

std::unique_ptr<NExpression> Parser::parseAssignmentExpression() {
    auto lhs = parsePrimary(); // Start with highest precedence stuff, or what could be LHS of an op

    // Check if it's an assignment: ident EQUAL expr
    // parsePrimary would return NIdentifier if it's just an ident.
    // We need to parse the LHS, then check for '='.
    // This is tricky because parsePrimary might consume more than just an ident (e.g. function call).
    // If LHS is an NIdentifier and current token is EQUAL:
    
    // The grammar.y `expr: ident EQUAL expr` is right associative.
    // A common way to handle this in RD is:
    // expr : equality_expr ( ASSIGN expr )? ;
    // equality_expr : relational_expr ( (EQ | NE) relational_expr )* ;
    // ...
    // primary_expr : IDENT | NUMBER | LPAREN expr RPAREN | call_expr ;
    // This means assignment is handled at a very low precedence level.

    // Let's use the Pratt parser style: parse LHS, then loop for ops.
    // Assignment is special. If `lhs` is an NIdentifier and `m_currentToken == Token::EQUAL`:
    if (NIdentifier* identLHS = dynamic_cast<NIdentifier*>(lhs.get())) {
        if (m_currentToken == Token::EQUAL) {
            advance(); // Consume '='
            auto rhs = parseAssignmentExpression(); // Recursive call for right-associativity
            // Must move ownership from identLHS if it's part of lhs unique_ptr
            // This requires lhs to be an NIdentifier specifically.
            // The dynamic_cast checks this. We need to reconstruct the NIdentifier unique_ptr for NAssignment.
            return std::make_unique<NAssignment>(std::make_unique<NIdentifier>(identLHS->name), std::move(rhs));
        }
    }
    // If not an assignment, proceed with standard operator precedence parsing
    return parseBinaryOpRHS(0, std::move(lhs));
}


std::unique_ptr<NExpression> Parser::parsePrimary() {
    std::unique_ptr<NExpression> result;
    switch (m_currentToken) {
        case Token::IDENT: {
            auto identNode = parseIdentifierNode(); // Consumes IDENT
            if (m_currentToken == Token::LPAR) { // Function Call
                auto callNode = std::make_unique<NFunctionCall>(std::move(identNode));
                callNode->arguments = parseCallArguments(); // Consumes LPAR ... RPAR
                result = std::move(callNode);
            } else { // Simple identifier
                result = std::move(identNode);
            }
            break;
        }
        case Token::NUMBER:
        case Token::FLOAT:
            result = parseNumeric(); // Consumes NUMBER/FLOAT
            break;
        case Token::LPAR:
            advance(); // Consume '('
            result = parseExpression();
            expect(Token::RPAR, "Expected ')' after parenthesized expression");
            break;
        default:
            throw ParserError("Unexpected token in expression: " + tokenToString(m_currentToken) + " ('" + getCurrentLexeme() + "')");
    }
    return result;
}

std::unique_ptr<NExpression> Parser::parseBinaryOpRHS(int exprPrec, std::unique_ptr<NExpression> lhs) {
    while (true) {
        int tokPrec = getTokenPrecedence(m_currentToken);
        if (tokPrec < exprPrec) { // Also handles tokPrec == -1 (not an operator)
            return lhs;
        }

        Token binOpToken = m_currentToken;
        advance(); // Consume binary operator

        // In grammar.y, RHS of binop could be another binop of higher precedence, or primary.
        // parsePrimary is for the start of an expression or the RHS of a lower precedence op.
        // For Pratt parsing, the RHS is parsed with the current operator's precedence (or +1 for right-associativity).
        auto rhs = parsePrimary(); // This was the original line.
                                   // For correct precedence climbing, RHS should be parsed with higher precedence.
                                   // auto rhs = parseExpressionWithMinPrecedence(tokPrec + (isRightAssociative(binOpToken) ? 0 : 1));
                                   // For simplicity, let's assume parsePrimary() is sufficient for now,
                                   // and complex precedence is handled by the loop and recursive calls to parseBinaryOpRHS.

        int nextPrec = getTokenPrecedence(m_currentToken);
        if (tokPrec < nextPrec) { 
            // If the next operator has higher precedence, it binds more tightly to RHS.
            // Example: LHS + RHS * NextRHS. We need to parse (RHS * NextRHS) first.
            rhs = parseBinaryOpRHS(tokPrec + 1, std::move(rhs)); // Pass tokPrec+1 (or just nextPrec if preferred for clarity)
        }
        
        lhs = std::make_unique<NBinaryOperator>(std::move(lhs), static_cast<int>(binOpToken), std::move(rhs));
    }
}

std::vector<std::unique_ptr<NExpression>> Parser::parseCallArguments() {
    expect(Token::LPAR, "Expected '(' for call arguments"); // Should be consumed by caller if IDENT LPAR
    std::vector<std::unique_ptr<NExpression>> args;
    if (m_currentToken != Token::RPAR) {
        while (true) {
            args.push_back(parseExpression());
            if (m_currentToken == Token::RPAR) break;
            expect(Token::COMMA, "Expected ',' or ')' in argument list");
        }
    }
    expect(Token::RPAR, "Expected ')' after call arguments");
    return args;
}

/* Placeholder for parseVariableDeclaration and parseFunctionDeclaration if called directly
// These are complex due to the IDENT IDENT start and are partially integrated into parseStatement's logic.
// If they were to be standalone:
std::unique_ptr<NVariableDeclaration> Parser::parseVariableDeclaration() {
    // Assumes currentToken is the first IDENT (type)
    auto type = parseIdentifierNode();
    auto name = parseIdentifierNode();
    std::unique_ptr<NExpression> expr = nullptr;
    if (m_currentToken == Token::EQUAL) {
        advance(); // consume '='
        expr = parseExpression();
    }
    // SEMICOLON is handled by the caller (parseStatement)
    m_symbolTable.declare(name->name, type->name, SymbolType::VARIABLE);
    return std::make_unique<NVariableDeclaration>(std::move(type), std::move(name), std::move(expr));
}

std::unique_ptr<NFunctionDeclaration> Parser::parseFunctionDeclaration() {
    // Assumes currentToken is the first IDENT (type)
    auto type = parseIdentifierNode();
    auto name = parseIdentifierNode();
    // LPAR is handled by parseFunctionParameters
    auto params = parseFunctionParameters();
    // RPAR is handled by parseFunctionParameters
    auto body = parseBlock();
    m_symbolTable.declare(name->name, type->name, SymbolType::FUNCTION);
    return std::make_unique<NFunctionDeclaration>(std::move(type), std::move(name), std::move(params), std::move(body));
}
*/
// Ensure the class definition is properly closed if these were the last methods.
// The error "expected '}' at end of input" for parser.cpp suggests a missing brace for the Parser class or a namespace.
// However, the notes point to function-level braces. The fix for parseStatement should resolve this.