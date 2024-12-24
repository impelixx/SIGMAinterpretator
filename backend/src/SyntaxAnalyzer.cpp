#include "SyntaxAnalyzer.h"
#include <stdexcept>
#include <string>

/**
 * @brief Advances to the next lexeme in the token stream
 * 
 * Retrieves the next lexeme from the lexical analysis results if available.
 * Updates the current lexeme (curLex_) by incrementing the index counter.
 * Does nothing if all lexemes have been processed (index >= lex.size()).
 */
void SyntaxAnalyzer::GetLexem() {
    if (index < lex.size()) {
        curLex_ = lex[index++];
    }
}

/**
 * @brief Initiates the syntax analysis process for the program.
 * 
 * This function starts the syntax analysis by fetching the first lexeme
 * and then analyzing the program structure according to the grammar rules.
 * It serves as the entry point for the entire syntax analysis process.
 */
void SyntaxAnalyzer::Analyze() {
  GetLexem();
  AnalyzeProgram();
}

/**
 * @brief Analyzes the program by processing statements until the end of code is reached.
 * 
 * This method iteratively processes statements in the program by calling AnalyzeStatement()
 * for each statement until it encounters the end of code (EOC) token. It serves as the
 * main entry point for syntax analysis of the entire program.
 * 
 * @throws May throw syntax analysis related exceptions if invalid syntax is encountered
 */
void SyntaxAnalyzer::AnalyzeProgram() {
    while (curLex_.get_type() != "EOC") {
        AnalyzeStatement();
    }
}

/**
 * @brief Analyzes a single statement in the source code and directs to appropriate analysis functions
 * 
 * This method handles different types of statements including:
 * - Empty lines and indentation changes
 * - Variable assignments
 * - Function declarations
 * - Control flow statements (if, while, for)
 * - Print statements
 * - Return statements
 * - Variable declarations
 * - Break and continue statements
 * 
 * @throws std::runtime_error If an unexpected keyword is encountered
 */
void SyntaxAnalyzer::AnalyzeStatement() {
    if (curLex_.get_type() == "NEWLINE" || curLex_.get_type() == "INDENT" || 
        curLex_.get_type() == "DEDENT") {
        GetLexem();
        return;
    }

    if (curLex_.get_type() != "KEYWORD") {
        AnalyzeAssignment();
        return;
    }

    std::string keyword = curLex_.get_text();
    if (keyword == "def") {
        AnalyzeFunctionDeclaration();
    } else if (keyword == "if") {
        AnalyzeIfStatement();
    } else if (keyword == "while") {
        AnalyzeWhileStatement();
    } else if (keyword == "for") {
        AnalyzeForStatement();
    } else if (keyword == "print") {
        AnalyzePrintStatement();
    } else if (keyword == "return") {
        AnalyzeReturnStatement();
    } else if (keyword == "else") {
      AnalyzeElseStatement();
    } else if (IsType(keyword)) {
        AnalyzeVariableDeclaration();
    } else if (keyword == "break" || keyword == "continue") {
        GetLexem();
    } else {
        throw std::runtime_error("Unexpected keyword: " + keyword + 
                               " at line " + std::to_string(curLex_.get_line()));
    }
}

/**
 * @brief Analyzes the syntax of an 'else' statement block in the code
 * @throws std::runtime_error If ':' is missing after 'else'
 * @throws std::runtime_error If the else block is not properly indented
 * 
 * This method handles the parsing of else statement blocks by:
 * 1. Verifying the presence of ':' after 'else'
 * 2. Checking for proper indentation of the else block
 * 3. Processing all statements within the else block until a DEDENT or EOC is encountered
 * 
 * The method expects the current lexeme to be positioned at 'else' when called.
 */
void SyntaxAnalyzer::AnalyzeElseStatement(){
  GetLexem();
  if (curLex_.get_text() != ":") {
    throw std::runtime_error("Expected ':' after 'else' at line " + 
                std::to_string(curLex_.get_line()));
  }
  GetLexem();

  if (curLex_.get_type() == "NEWLINE") {
    GetLexem();
  }

  if (curLex_.get_type() != "INDENT") {
    throw std::runtime_error("Expected indented block in else statement at line " + 
                std::to_string(curLex_.get_line()));
  }
  GetLexem();

  while (curLex_.get_type() != "DEDENT" && curLex_.get_type() != "EOC") {
    AnalyzeStatement();
  }

  if (curLex_.get_type() == "DEDENT") {
    GetLexem();
  }
}

/**
 * @brief Analyzes variable declarations in the source code, including arrays and initializations.
 * 
 * This method processes variable declarations following this pattern:
 * - Type identifier;
 * - Type identifier[size];
 * - Type identifier = expression;
 * - Type identifier[size] = {element1, element2, ...};
 * 
 * The method handles:
 * - Basic variable declarations
 * - Array declarations with computed size expressions
 * - Direct variable initialization
 * - Array initialization with multiple elements
 * 
 * @throws std::runtime_error If:
 *         - An identifier is missing after the type
 *         - A closing bracket ']' is missing in array declaration
 *         - A closing brace '}' is missing in array initialization
 */
void SyntaxAnalyzer::AnalyzeVariableDeclaration() {
    std::string type = curLex_.get_text();
    GetLexem();
    if (curLex_.get_type() != "IDENTIFIER") {
        throw std::runtime_error("Expected identifier after type at line " + 
                               std::to_string(curLex_.get_line()));
    }
    GetLexem();

    if (curLex_.get_text() == "[") {
        std::cout << "Array declaration" << std::endl;
        GetLexem(); 
        
        std::cout << "Array size expression starts with: " << curLex_.get_text() << std::endl;
        AnalyzeExpression();
        
        if (curLex_.get_text() != "]") {
            throw std::runtime_error("Expected ']' in array declaration at line " + 
                                   std::to_string(curLex_.get_line()));
        }
        GetLexem();
    }

    if (curLex_.get_text() == "=") {
        GetLexem();
        if (curLex_.get_text() == "{") {
            GetLexem();
            
            while (curLex_.get_text() != "}" && curLex_.get_type() != "EOC") {
                AnalyzeExpression();
                if (curLex_.get_text() == ",") {
                    GetLexem();
                }
            }
            
            if (curLex_.get_text() != "}") {
                throw std::runtime_error("Expected '}' in array initialization at line " + 
                                       std::to_string(curLex_.get_line()));
            }
            GetLexem();
        } else {
            AnalyzeExpression();
        }
    }
    
    AnalyzeStatementTerminator();
}

/**
 * @brief Analyzes a function declaration in the source code.
 * 
 * This method processes a function declaration by validating:
 * - Function name (must be an identifier)
 * - Opening parenthesis
 * - Parameter list
 * - Colon after parameters
 * - Newline after colon
 * - Proper indentation of function body
 * - Function body statements
 * 
 * The method advances through tokens using GetLexem() and throws runtime_error 
 * if the syntax does not match expected function declaration format.
 * 
 * The function processes statements within the function body until it encounters
 * either a DEDENT token (end of function block) or EOC (end of code).
 * 
 * @throws std::runtime_error If any part of the function declaration syntax is invalid,
 *         with specific error messages indicating the line number where the error occurred.
 */
void SyntaxAnalyzer::AnalyzeFunctionDeclaration() {
    GetLexem();
    
    if (curLex_.get_type() != "IDENTIFIER") {
        throw std::runtime_error("Expected function name at line " + 
                               std::to_string(curLex_.get_line()));
    }
    GetLexem();
    
    if (curLex_.get_text() != "(") {
        throw std::runtime_error("Expected '(' after function name at line " + 
                               std::to_string(curLex_.get_line()));
    }
    GetLexem();
    
    AnalyzeParameterList();
    
    if (curLex_.get_text() != ":") {
        throw std::runtime_error("Expected ':' after function parameters at line " + 
                               std::to_string(curLex_.get_line()));
    }
    GetLexem();
    
    if (curLex_.get_type() != "NEWLINE") {
        throw std::runtime_error("Expected newline after ':' at line " + 
                               std::to_string(curLex_.get_line()));
    }
    GetLexem();
    
    if (curLex_.get_type() != "INDENT") {
        throw std::runtime_error("Expected indented block after function declaration at line " + 
                               std::to_string(curLex_.get_line()));
    }
    GetLexem();
    
    while (curLex_.get_type() != "DEDENT" && curLex_.get_type() != "EOC") {
        AnalyzeStatement();
    }
    
    if (curLex_.get_type() == "DEDENT") {
        GetLexem();
    }
}

/**
 * @brief Analyzes function parameter list in the source code.
 * 
 * This method processes function parameters between parentheses, verifying that:
 * - Each parameter starts with a valid type
 * - Each type is followed by a valid identifier (parameter name)
 * - Parameters are properly separated by commas
 * The method advances through lexemes until reaching the closing parenthesis.
 * 
 * @throws std::runtime_error If parameter type is invalid or parameter name is missing
 */
void SyntaxAnalyzer::AnalyzeParameterList() {
    while (curLex_.get_text() != ")") {
        if (!IsType(curLex_.get_text())) {
            throw std::runtime_error("Expected type in parameter list at line " + 
                                   std::to_string(curLex_.get_line()));
        }
        GetLexem();
        
        if (curLex_.get_type() != "IDENTIFIER") {
            throw std::runtime_error("Expected parameter name at line " + 
                                   std::to_string(curLex_.get_line()));
        }
        GetLexem();
        
        if (curLex_.get_text() == ",") {
            GetLexem();
        }
    }
    GetLexem();
}

/**
 * @brief Checks if a given word is a valid SIGMA type identifier.
 * 
 * @param word String to check against valid type identifiers.
 * @return true if the word matches any of the valid types (int, float, bool, string, void)
 * @return false otherwise
 */
bool SyntaxAnalyzer::IsType(const std::string& word) {
    return word == "int" || word == "float" || word == "bool" || 
           word == "string" || word == "void";
}

void SyntaxAnalyzer::AnalyzeAssignment() {
    if (curLex_.get_type() != "IDENTIFIER") {
        throw std::runtime_error("Expected identifier at line " + 
                               std::to_string(curLex_.get_line()));
    }
    GetLexem();
    
    if (curLex_.get_text() != "=") {
        throw std::runtime_error("Expected '=' in assignment at line " + 
                               std::to_string(curLex_.get_line()));
    }
    GetLexem();
    
    AnalyzeExpression();
    AnalyzeStatementTerminator();
}

void SyntaxAnalyzer::AnalyzeExpression() {
    std::cout << "Expression token: " << curLex_.get_text() << " type: " << curLex_.get_type() << std::endl;
    
    while (curLex_.get_type() != "NEWLINE" && 
           curLex_.get_text() != ";" && 
           curLex_.get_text() != ")" && 
           curLex_.get_text() != "," && 
           curLex_.get_text() != "]" &&
           curLex_.get_type() != "EOC" &&
           curLex_.get_text() != "}" ) {
        GetLexem();
        std::cout << "Next token: " << curLex_.get_text() << " type: " << curLex_.get_type() << std::endl;
    }
}

void SyntaxAnalyzer::AnalyzeStatementTerminator() {
    if (curLex_.get_text() == ";") {
        GetLexem();
    }
    if (curLex_.get_type() == "NEWLINE") {
        GetLexem();
    }
}

void SyntaxAnalyzer::AnalyzeIfStatement() {
    GetLexem();
    
    if (curLex_.get_text() != "(") {
        throw std::runtime_error("Expected '(' after 'if' at line " + 
                               std::to_string(curLex_.get_line()));
    }
    GetLexem();
    
    AnalyzeExpression();
    GetLexem();
    if (curLex_.get_text() != ":") {
      throw std::runtime_error("Expected ':' after if condition at line " +
                               std::to_string(curLex_.get_line()));
    }
    GetLexem();
    
    if (curLex_.get_type() == "NEWLINE") {
        GetLexem();
    }
    
    if (curLex_.get_type() != "INDENT") {
        throw std::runtime_error("Expected indented block in if statement at line " + 
                               std::to_string(curLex_.get_line()));
    }
    GetLexem();
    
    while (curLex_.get_type() != "DEDENT" && curLex_.get_type() != "EOC") {
        AnalyzeStatement();
    }
    
    if (curLex_.get_type() == "DEDENT") {
        GetLexem();
    }
}



/**
 * @brief Analyzes the syntax of a while statement in the source code.
 * 
 * This method processes a while loop construct following this pattern:
 * while (condition):
 *     statement(s)
 * 
 * The method expects:
 * 1. Opening parenthesis after 'while'
 * 2. A valid expression as the loop condition
 * 3. A colon following the condition
 * 4. An indented block containing the loop body statements
 * 
 * @throws std::runtime_error If the syntax is invalid:
 *         - Missing opening parenthesis
 *         - Missing colon after condition
 *         - Missing indentation for the loop body
 */
