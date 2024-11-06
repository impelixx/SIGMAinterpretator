#include "../include/LexemAnalyzer.h"
#include <cctype>
#include <stdexcept>
#include <iostream>
#include <stack>
#include <fstream>

LexemAnalyzer::LexemAnalyzer(const std::string& code)
    : code_(code), ch_('\0'), index_(0), currentPosition_(0) {
    indentStack_.push_back(0);
    std::ifstream keywords("../test/workword");
    if (!keywords.is_open()) {
        throw std::runtime_error("Failed to open file '../test/workword.txt'");
    }
    std::string word;
    while (keywords >> word) {
        keywords_.add(word);
    }
}

void LexemAnalyzer::GetNextChar() {
    if (index_ < code_.length()) {
        ch_ = code_[index_++];
        currentPosition_ = index_;
        std::cout << "GetNextChar() called: ch='" << ch_ << "', index_=" << index_ << std::endl;
    }
    else {
        ch_ = '\0';
        std::cout << "GetNextChar() called: End of input reached." << std::endl;
    }
}

// 
void LexemAnalyzer::SkipWhitespace() {
    std::cout << "Entering SkipWhitespace()" << std::endl;
    while (isspace(ch_)) {
        if (ch_ == '\n') {
            lexems_.emplace_back(Lexem(LexemType::NEWLINE, "\n", index_ - 1, index_));
            std::cout << "Newline lexem added at position " << index_ - 1 << std::endl;
            GetNextChar();
            int indent = 0;
            while (ch_ == ' ') {
                indent++;
                GetNextChar();
            }
            HandleIndentation(indent);
            continue;
        }
        GetNextChar();
    }
    std::cout << "Exiting SkipWhitespace()" << std::endl;
}

// cсчитаем отступы
void LexemAnalyzer::HandleIndentation(int currentIndent) {
    int previousIndent = indentStack_.back();
    if (currentIndent > previousIndent) {
        indentStack_.push_back(currentIndent);
        lexems_.emplace_back(Lexem(LexemType::INDENT, "INDENT", index_ - currentIndent, index_));
        std::cout << "Indentation increased to " << currentIndent << ". INDENT lexem added." << std::endl;
    }
    else {
        while (currentIndent < previousIndent) {
            indentStack_.pop_back();
            lexems_.emplace_back(Lexem(LexemType::DEDENT, "DEDENT", index_, index_));
            std::cout << "Indentation decreased to " << indentStack_.back() << ". DEDENT lexem added." << std::endl;
            if (indentStack_.empty()) {
                throw std::runtime_error("Indentation error: No matching indentation level.");
            }
            previousIndent = indentStack_.back();
        }
        if (currentIndent != previousIndent) {
            throw std::runtime_error("Indentation error: Inconsistent indentation.");
        }
    }
}

void LexemAnalyzer::Analyze() {
    std::cout << "Entering Analyze()" << std::endl;
    GetNextChar();
    AnalyzeProgram();
    std::cout << "Exiting Analyze()" << std::endl;
}

// Analyze the entire program
void LexemAnalyzer::AnalyzeProgram() {
    std::cout << "Entering AnalyzeProgram()" << std::endl;
    int iteration = 0;

    while (ch_ != '\0') {
        SkipWhitespace();
        if (ch_ == '\0') break;
        AnalyzeStatement();
    }

    while (indentStack_.size() > 1) {
        indentStack_.pop_back();
        lexems_.emplace_back(Lexem(LexemType::DEDENT, "DEDENT", index_, index_));
        std::cout << "DEDENT lexem added at EOF. Indentation level now " << indentStack_.back() << "." << std::endl;
    }

    std::cout << "Exiting AnalyzeProgram()" << std::endl;
}

// Analyze a single statement
void LexemAnalyzer::AnalyzeStatement() {
    std::cout << "Entering AnalyzeStatement(): ch='" << ch_ << "'" << std::endl;

    if (ch_ == 'd' && code_.substr(index_ - 1, 3) == "def") {
        AnalyzeFunctionDeclaration();
    }
    else if (isalpha(ch_) || ch_ == '_') {
        size_t tempIndex = index_ - 1;
        std::string word;
        while (tempIndex < code_.length() && (isalnum(code_[tempIndex]) || code_[tempIndex] == '_')) {
            word += code_[tempIndex++];
        }

        if (word == "INT" || word == "FLOAT" || word == "STRING" || word == "BOOL") {
            AnalyzeVariableDeclaration();
        }
        else if (word == "if") {
            AnalyzeIfStatement();
        }
        else if (word == "while") {
            AnalyzeWhileStatement();
        }
        else if (word == "print") {
            AnalyzePrintStatement();
        }
        else if (word == "return") {
            AnalyzeReturnStatement();
        }
        else {
            AnalyzeAssignment();
        }
    }
    else if (ch_ == ';') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, ";", index_ - 1, index_));
        GetNextChar();
    }
    else {
        std::cout << "Unexpected character in AnalyzeStatement(): '" << ch_ << "'" << std::endl;
        GetNextChar();
    }
    std::cout << "Exiting AnalyzeStatement()" << std::endl;
}

// Analyze variable 
void LexemAnalyzer::AnalyzeVariableDeclaration() {
    std::cout << "Entering AnalyzeVariableDeclaration()" << std::endl;

    // SkipWhitespace();
    AnalyzeIdentifier();
    // SkipWhitespace();

    // ищем []
    if (ch_ == '[') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "[", index_ - 1, index_));
        std::cout << "Operator '[' added at position " << index_ - 1 << std::endl;
        GetNextChar();
        AnalyzeExpression();

        if (ch_ == ']') {
            lexems_.emplace_back(Lexem(LexemType::BRACKET, "]", index_ - 1, index_));
            std::cout << "Operator ']' added at position " << index_ - 1 << std::endl;
            GetNextChar();
        }
        else {
            std::cerr << "LexemAnalyzer error: Expected ']' after array size" << std::endl;
            throw std::runtime_error("Expected ']' after array size");
        }

        SkipWhitespace();
    }

    // ищем =
    if (ch_ == '=') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, "=", index_ - 1, index_));
        std::cout << "Assignment operator '=' added at position " << index_ - 1 << std::endl;
        GetNextChar();
        AnalyzeExpression();
        SkipWhitespace();
        if (ch_ == ';') {
            lexems_.emplace_back(Lexem(LexemType::OPERATOR, ";", index_ - 1, index_));
            std::cout << "Semicolon ';' added at position " << index_ - 1 << std::endl;
            GetNextChar();
        }
        else if (ch_ == '\n') {
            lexems_.emplace_back(Lexem(LexemType::NEWLINE, "\n", index_ - 1, index_));
            std::cout << "Newline added after assignment at position " << index_ - 1 << std::endl;
            GetNextChar();
        }
        else {
            // simple warn чтобы не втыкал
            std::cout << "Warning: Expected ';' or newline after assignment but found '" << ch_ << "'" << std::endl;
            GetNextChar();
        }
    }
    else {
        std::cerr << "LexemAnalyzer error: Expected '=' in assignment" << std::endl;
        GetNextChar();
    }

    std::cout << "Exiting AnalyzeVariableDeclaration()" << std::endl;
}

// void LexemAnalyzer::AnalyzeFunctionDeclaration() {
//     std::cout << "Entering AnalyzeFunctionDeclaration()" << std::endl;

//     // Skip 'def' keyword
//     for (int i = 0; i < 3; ++i) {  // 'def' has 3 characters
//         GetNextChar();
//     }
//     lexems_.emplace_back(Lexem(LexemType::KEYWORD, "def", index_ - 3, index_));
//     std::cout << "Keyword 'def' added at positions " << index_ - 3 << " to " << index_ << std::endl;

//     SkipWhitespace();

//     // Parse function name
//     AnalyzeIdentifier();

//     SkipWhitespace();

//     if (ch_ == '(') {
//         lexems_.emplace_back(Lexem(LexemType::OPERATOR, "(", index_ - 1, index_));
//         std::cout << "Operator '(' added at position " << index_ - 1 << std::endl;
//         GetNextChar();
//         AnalyzeParameters();
//         if (ch_ == ')') {
//             lexems_.emplace_back(Lexem(LexemType::OPERATOR, ")", index_ - 1, index_));
//             std::cout << "Operator ')' added at position " << index_ - 1 << std::endl;
//             GetNextChar();
//         }
//         else {
//             std::cerr << "LexemAnalyzer error: Expected ')' after function parameters" << std::endl;
//             throw std::runtime_error("Expected ')' after function parameters");
//         }
//     }
//     else {
//         std::cerr << "LexemAnalyzer error: Expected '(' after function name" << std::endl;
//         throw std::runtime_error("Expected '(' after function name");
//     }

//     SkipWhitespace();

//     if (ch_ == ':') {
//         lexems_.emplace_back(Lexem(LexemType::OPERATOR, ":", index_ - 1, index_));
//         std::cout << "Operator ':' added at position " << index_ - 1 << std::endl;
//         GetNextChar();
//         AnalyzeBlock();
//     }
//     else {
//         std::cerr << "LexemAnalyzer error: Expected ':' after function declaration" << std::endl;
//         throw std::runtime_error("Expected ':' after function declaration");
//     }

//     std::cout << "Exiting AnalyzeFunctionDeclaration()" << std::endl;
// }

// void LexemAnalyzer::AnalyzeParameters() {
//     std::cout << "Entering AnalyzeParameters()" << std::endl;

//     if (ch_ == ')') {
//         // No parameters
//         std::cout << "No parameters to parse." << std::endl;
//         return;
//     }

//     while (ch_ != ')' && ch_ != '\0') {
//         SkipWhitespace();
//         AnalyzeIdentifier();
//         SkipWhitespace();
//         if (ch_ == ',') {
//             lexems_.emplace_back(Lexem(LexemType::OPERATOR, ",", index_ - 1, index_));
//             std::cout << "Operator ',' added at position " << index_ - 1 << std::endl;
//             GetNextChar();
//         }
//         else {
//             break;
//         }
//     }

//     std::cout << "Exiting AnalyzeParameters()" << std::endl;
// }

// Analyze assignments
void LexemAnalyzer::AnalyzeAssignment() {
    std::cout << "Entering AnalyzeAssignment()" << std::endl;

    AnalyzeIdentifier();
    SkipWhitespace();

    if (ch_ == '=') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, "=", index_ - 1, index_));
        std::cout << "Assignment operator '=' added at position " << index_ - 1 << std::endl;
        GetNextChar();
        AnalyzeExpression();
        SkipWhitespace();
        if (ch_ == ';') {
            lexems_.emplace_back(Lexem(LexemType::OPERATOR, ";", index_ - 1, index_));
            std::cout << "Semicolon ';' added at position " << index_ - 1 << std::endl;
            GetNextChar();
        }
        else if (ch_ == '\n') {
            lexems_.emplace_back(Lexem(LexemType::NEWLINE, "\n", index_ - 1, index_));
            std::cout << "Newline added after assignment at position " << index_ - 1 << std::endl;
            GetNextChar();
        }
        else {
            // simple warn чтобы не втыкалing
            std::cout << "Warning: Expected ';' or newline after assignment but found '" << ch_ << "'" << std::endl;
            GetNextChar();
        }
    }
    else {
        std::cerr << "LexemAnalyzer error: Expected '=' in assignment" << std::endl;
        GetNextChar();
    }

    std::cout << "Exiting AnalyzeAssignment()" << std::endl;
}

// Analyze expressions
void LexemAnalyzer::AnalyzeExpression() {
    std::cout << "Entering AnalyzeExpression()" << std::endl;

    while (ch_ != '\0' && ch_ != ';' && ch_ != '\n' && ch_ != ':') {
        if (isdigit(ch_)) {
            AnalyzeNumber();
        }
        else if (isalpha(ch_) || ch_ == '_') {
            AnalyzeIdentifier();
        }
        else if (ch_ == '(') {
            lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", index_ - 1, index_));
            GetNextChar();
            AnalyzeExpression();
            if (ch_ == ')') {
                lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", index_ - 1, index_));
                GetNextChar();
            }
        }
        else if (ch_ == '<' || ch_ == '>' || ch_ == '=' || ch_ == '!') {
            std::string op;
            op += ch_;
            GetNextChar();
            if (ch_ == '=') {
                op += ch_;
                GetNextChar();
            }
            lexems_.emplace_back(Lexem(LexemType::OPERATOR, op, index_ - op.length(), index_));
        }
        else if (ch_ == '[') {
            lexems_.emplace_back(Lexem(LexemType::BRACKET, "[", index_ - 1, index_));
            GetNextChar();
            AnalyzeExpression();
            if (ch_ == ']') {
                lexems_.emplace_back(Lexem(LexemType::BRACKET, "]", index_ - 1, index_));
                GetNextChar();
            }
        }
        else {
            GetNextChar();
        }
        SkipWhitespace();
    }

    std::cout << "Exiting AnalyzeExpression()" << std::endl;
}

// Analyze identifiers
void LexemAnalyzer::AnalyzeIdentifier() {
    std::cout << "Entering AnalyzeIdentifier()" << std::endl;
    std::string identifier;
    size_t startPos = index_ - 1;
    
    while (isalnum(ch_) || ch_ == '_') {
        identifier += ch_;
        GetNextChar();
    }
    
    auto [isKeyword, lexem] = keywords_.has(identifier.c_str(), identifier.length(), startPos);
    if (isKeyword) {
        lexems_.emplace_back(Lexem(LexemType::KEYWORD, identifier, startPos, index_));
    } else {
        lexems_.emplace_back(Lexem(LexemType::IDENTIFIER, identifier, startPos, index_));
    }
}

// Analyze numbers
void LexemAnalyzer::AnalyzeNumber() {
    std::cout << "Entering AnalyzeNumber()" << std::endl;
    std::string number;
    size_t startPos = index_ - 1;

    while (isdigit(ch_) || ch_ == '.') {
        number += ch_;
        GetNextChar();
    }

    lexems_.emplace_back(Lexem(LexemType::NUMBER, number, startPos, index_));
    std::cout << "Number '" << number << "' added from position " << startPos << " to " << index_ << std::endl;
    std::cout << "Exiting AnalyzeNumber()" << std::endl;
}

void LexemAnalyzer::PrintLexems() const {
    for (auto& lexem : lexems_) {
        std::cout << "Type: " << lexem.get_type() << ", Text: " << lexem.get_text() << std::endl;
    }
}

// Analyze function
void LexemAnalyzer::AnalyzeFunctionDeclaration() {
    std::cout << "Entering AnalyzeFunctionDeclaration()" << std::endl;

    // Skip def
    for (int i = 0; i < 3; ++i) {
        GetNextChar();
    }
    lexems_.emplace_back(Lexem(LexemType::KEYWORD, "def", index_ - 3, index_));
    std::cout << "Keyword 'def' added at positions " << index_ - 3 << " to " << index_ << std::endl;

    SkipWhitespace();
    AnalyzeIdentifier(); // Function
    SkipWhitespace();

    if (ch_ == '(') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", index_ - 1, index_));
        std::cout << "Operator '(' added at position " << index_ - 1 << std::endl;
        GetNextChar();
        AnalyzeParameters();
        
        if (ch_ == ')') {
            lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", index_ - 1, index_));
            std::cout << "Operator ')' added at position " << index_ - 1 << std::endl;
            GetNextChar();
        }
        else {
            std::cerr << "LexemAnalyzer error: Expected ')' after function parameters" << std::endl;
            throw std::runtime_error("Expected ')' after function parameters");
        }
    }
    else {
        std::cerr << "LexemAnalyzer error: Expected '(' after function name" << std::endl;
        throw std::runtime_error("Expected '(' after function name");
    }

    SkipWhitespace();

    if (ch_ == ':') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, ":", index_ - 1, index_));
        std::cout << "Operator ':' added at position " << index_ - 1 << " to " << index_ << std::endl;
        GetNextChar();
    }
    else {
        std::cerr << "LexemAnalyzer error: Expected ':' after function declaration" << std::endl;
        throw std::runtime_error("Expected ':' after function declaration");
    }

    std::cout << "Exiting AnalyzeFunctionDeclaration()" << std::endl;
}

// Analyze function parameters
void LexemAnalyzer::AnalyzeParameters() {
    std::cout << "Entering AnalyzeParameters()" << std::endl;

    if (ch_ == ')') {
        // NO
        std::cout << "No parameters to parse." << std::endl;
        return;
    }

    while (ch_ != ')' && ch_ != '\0') {
        SkipWhitespace();
        
        if (isalpha(ch_)) {
            std::string type;
            size_t startPos = index_ - 1;
            while (isalpha(ch_)) {
                type += ch_;
                GetNextChar();
            }
            lexems_.emplace_back(Lexem(LexemType::KEYWORD, type, startPos, index_));
        }
        
        SkipWhitespace();
        AnalyzeIdentifier();  // Param name
        
        // Handle array brackets []
        SkipWhitespace();
        if (ch_ == '[') {
            lexems_.emplace_back(Lexem(LexemType::BRACKET, "[", index_ - 1, index_));
            GetNextChar();
            if (ch_ == ']') {
                lexems_.emplace_back(Lexem(LexemType::BRACKET, "]", index_ - 1, index_));
                GetNextChar();
            } else {
                throw std::runtime_error("Expected ']' after '['");
            }
        }
        
        SkipWhitespace();
        if (ch_ == ',') {
            lexems_.emplace_back(Lexem(LexemType::OPERATOR, ",", index_ - 1, index_));
            std::cout << "Operator ',' added at position " << index_ - 1 << std::endl;
            GetNextChar();
        }
    }
}

// Analyze if statements
void LexemAnalyzer::AnalyzeIfStatement() {
    std::cout << "Entering AnalyzeIfStatement()" << std::endl;

    // Skip if
    for (int i = 0; i < 2; ++i) {
        GetNextChar();
    }
    lexems_.emplace_back(Lexem(LexemType::KEYWORD, "if", index_ - 2, index_));
    std::cout << "Keyword 'if' added at positions " << index_ - 2 << " to " << index_ << std::endl;

    SkipWhitespace();
    AnalyzeExpression();
    SkipWhitespace();

    if (ch_ == ':') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, ":", index_ - 1, index_));
        std::cout << "Operator ':' added at position " << index_ - 1 << " to " << index_ << std::endl;
        GetNextChar();
        // AnalyzeBlock();
    }
    else {
        std::cerr << "LexemAnalyzer error: Expected ':' after if condition" << std::endl;
        throw std::runtime_error("Expected ':' after if condition");
    }

    std::cout << "Exiting AnalyzeIfStatement()" << std::endl;
}

// Analyze while statements
void LexemAnalyzer::AnalyzeWhileStatement() {
    std::cout << "Entering AnalyzeWhileStatement()" << std::endl;

    // Skip while
    for (int i = 0; i < 5; ++i) {
        GetNextChar();
    }
    lexems_.emplace_back(Lexem(LexemType::KEYWORD, "while", index_ - 5, index_));
    std::cout << "Keyword 'while' added at positions " << index_ - 5 << " to " << index_ << std::endl;

    SkipWhitespace();
    
    if (ch_ == '(') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", index_ - 1, index_));
        GetNextChar();
        SkipWhitespace();
        
        // Parse the condition
        while (ch_ != ')' && ch_ != '\0') {
            if (isdigit(ch_)) {
                AnalyzeNumber();
            }
            else if (isalpha(ch_) || ch_ == '_') {
                AnalyzeIdentifier();
            }
            else if (ch_ == '<' || ch_ == '>' || ch_ == '=' || ch_ == '!') {
                std::string op;
                op += ch_;
                GetNextChar();
                if (ch_ == '=') {
                    op += ch_;
                    GetNextChar();
                }
                lexems_.emplace_back(Lexem(LexemType::OPERATOR, op, index_ - op.length(), index_));
            }
            SkipWhitespace();
        }
        
        if (ch_ == ')') {
            lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", index_ - 1, index_));
            GetNextChar();
        } else {
            throw std::runtime_error("Expected ')' after while condition");
        }
    } else {
        throw std::runtime_error("Expected '(' after while keyword");
    }

    SkipWhitespace();

    if (ch_ == ':') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, ":", index_ - 1, index_));
        std::cout << "Operator ':' added at position " << index_ - 1 << std::endl;
        GetNextChar();
    } else {
        std::cerr << "LexemAnalyzer error: Expected ':' after while condition" << std::endl;
        throw std::runtime_error("Expected ':' after while condition");
    }

    std::cout << "Exiting AnalyzeWhileStatement()" << std::endl;
}

// Analyze print statements
void LexemAnalyzer::AnalyzePrintStatement() {
    std::cout << "Entering AnalyzePrintStatement()" << std::endl;

    // Skip print
    for (int i = 0; i < 5; ++i) {
        GetNextChar();
    }
    lexems_.emplace_back(Lexem(LexemType::KEYWORD, "print", index_ - 5, index_));
    std::cout << "Keyword 'print' added at positions " << index_ - 5 << " to " << index_ << std::endl;

    SkipWhitespace();
    if (ch_ == '(') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", index_ - 1, index_));
        std::cout << "Operator '(' added at position " << index_ - 1 << std::endl;
        GetNextChar();
        AnalyzeExpression();
        if (ch_ == ')') {
            lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", index_ - 1, index_));
            std::cout << "Operator ')' added at position " << index_ - 1 << std::endl;
            GetNextChar();
        }
        else {
            std::cerr << "LexemAnalyzer error: Expected ')' after print expression" << std::endl;
            throw std::runtime_error("Expected ')' after print expression");
        }
    }
    else {
        std::cerr << "LexemAnalyzer error: Expected '(' after print" << std::endl;
        throw std::runtime_error("Expected '(' after print");
    }

    SkipWhitespace();
    if (ch_ == ';') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, ";", index_ - 1, index_));
        std::cout << "Semicolon ';' added at position " << index_ - 1 << std::endl;
        GetNextChar();
    }
    else if (ch_ == '\n') {
        lexems_.emplace_back(Lexem(LexemType::NEWLINE, "\n", index_ - 1, index_));
        std::cout << "Newline added after print statement at position " << index_ - 1 << std::endl;
        GetNextChar();
    }
    else {
        //  simple warning  чтобы не втыкал
        std::cout << "Warning: Expected ';' or newline after print statement but found '" << ch_ << "'" << std::endl;
        GetNextChar();
    }

    std::cout << "Exiting AnalyzePrintStatement()" << std::endl;
}

// Analyze return 
void LexemAnalyzer::AnalyzeReturnStatement() {
    std::cout << "Entering AnalyzeReturnStatement()" << std::endl;

    for (int i = 0; i < 6; ++i) {
        GetNextChar();
    }
    lexems_.emplace_back(Lexem(LexemType::KEYWORD, "return", index_ - 6, index_));
    std::cout << "Keyword 'return' added at positions " << index_ - 6 << " to " << index_ << std::endl;

    SkipWhitespace();
    AnalyzeExpression();
    SkipWhitespace();
    if (ch_ == ';') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, ";", index_ - 1, index_));
        std::cout << "Semicolon ';' added at position " << index_ - 1 << std::endl;
        GetNextChar();
    }
    else if (ch_ == '\n') {
        lexems_.emplace_back(Lexem(LexemType::NEWLINE, "\n", index_ - 1, index_));
        std::cout << "Newline added after return statement at position " << index_ - 1 << std::endl;
        GetNextChar();
    }
    else {
        // simple warn чтобы не тыкал
        std::cout << "Warning: Expected ';' or newline after return statement but found '" << ch_ << "'" << std::endl;
        GetNextChar();
    }

    std::cout << "Exiting AnalyzeReturnStatement()" << std::endl;
}

void LexemAnalyzer::AnalyzeType() {
    std::string type;
    size_t startPos = index_ - 1;
    
    while (isalpha(ch_)) {
        type += ch_;
        GetNextChar();
    }
    
    if (type == "INT" || type == "FLOAT" || type == "STRING" || type == "BOOL") {
        lexems_.emplace_back(Lexem(LexemType::KEYWORD, type, startPos, index_));
    } else {
        lexems_.emplace_back(Lexem(LexemType::IDENTIFIER, type, startPos, index_));
    }
}