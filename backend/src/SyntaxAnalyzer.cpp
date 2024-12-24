#include "SyntaxAnalyzer.h"
#include <stdexcept>
#include <string>

void SyntaxAnalyzer::GetLexem() {
    if (index < lex.size()) {
        curLex_ = lex[index++];
    }
}

void SyntaxAnalyzer::Analyze() {
  GetLexem();
  AnalyzeProgram();
}

void SyntaxAnalyzer::AnalyzeProgram() {
    while (curLex_.get_type() != "EOC") {
        AnalyzeStatement();
    }
}

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

void SyntaxAnalyzer::AnalyzeWhileStatement() {
  GetLexem();
  if (curLex_.get_text() != "(") {
    throw std::runtime_error("Expected '(' after 'while' at line " +
                             std::to_string(curLex_.get_line()));
  }
    GetLexem();
    
    AnalyzeExpression();

    GetLexem();
    
    if (curLex_.get_text() != ":") {
        throw std::runtime_error("Expected ':' after while condition at line " + 
                               std::to_string(curLex_.get_line()));
    }
    GetLexem();
    
    if (curLex_.get_type() == "NEWLINE") {
        GetLexem();
    }
    
    if (curLex_.get_type() != "INDENT") {
        throw std::runtime_error("Expected indented block in while statement at line " + 
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

void SyntaxAnalyzer::AnalyzeForStatement() {
    GetLexem();
    
    if (curLex_.get_type() != "IDENTIFIER") {
        throw std::runtime_error("Expected identifier in for statement at line " + 
                               std::to_string(curLex_.get_line()));
    }
    GetLexem();
    
    if (curLex_.get_text() != "in") {
        throw std::runtime_error("Expected 'in' in for statement at line " + 
                               std::to_string(curLex_.get_line()));
    }
    GetLexem();
    
    AnalyzeExpression();
    
    if (curLex_.get_text() != ":") {
        throw std::runtime_error("Expected ':' after for expression at line " + 
                               std::to_string(curLex_.get_line()));
    }
    GetLexem();
    
    if (curLex_.get_type() == "NEWLINE") {
        GetLexem();
    }
    
    if (curLex_.get_type() != "INDENT") {
        throw std::runtime_error("Expected indented block in for statement at line " + 
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

void SyntaxAnalyzer::AnalyzePrintStatement() {
    GetLexem();
    
    if (curLex_.get_text() != "(") {
        throw std::runtime_error("Expected '(' after 'print' at line " + 
                               std::to_string(curLex_.get_line()));
    }
    GetLexem();
    
    AnalyzeExpression();
    
    if (curLex_.get_text() != ")") {
        throw std::runtime_error("Expected ')' after print expression at line " + 
                               std::to_string(curLex_.get_line()));
    }
    GetLexem();
    
    AnalyzeStatementTerminator();
}

void SyntaxAnalyzer::AnalyzeReturnStatement() {
    GetLexem();
    
    if (curLex_.get_type() != "NEWLINE") {
        AnalyzeExpression();
    }
    
    AnalyzeStatementTerminator();
}
