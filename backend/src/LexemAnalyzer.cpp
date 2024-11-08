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
    std::cout << "start add keywords" << std::endl;
    std::cout << word << std::endl;
    while (keywords >> word) {
        keywords_.add(word);
    }
}

void LexemAnalyzer::GetNextChar() {
    if (index_ < code_.length()) {
        ch_ = code_[index_++];
        currentPosition_ = index_;
    }
    else {
        ch_ = '\0';
    }
}

// пропускаем пробелы и новые и комменты
void LexemAnalyzer::SkipWhitespace() {
    while (true) {
        // пробелы
        while (isspace(ch_)) {
            if (ch_ == '\n') {
                lexems_.emplace_back(Lexem(LexemType::NEWLINE, "\\n", index_ - 1, index_));
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

        // однострочный комментарий
        if (ch_ == '/' && index_ < code_.length() && code_[index_] == '/') {
            while (ch_ != '\n' && ch_ != '\0') {
                GetNextChar();
            }
            continue;
        }

        // многострочный комментарий
        if (ch_ == '/' && index_ < code_.length() && code_[index_] == '*') {
            GetNextChar();
            GetNextChar();
            
            while (ch_ != '\0') {
                if (ch_ == '*' && index_ < code_.length() && code_[index_] == '/') {
                    GetNextChar(); // Skip '*'
                    GetNextChar(); // Skip '/'
                    break;
                }
                GetNextChar();
            }
            continue;
        }

        break;
    }
}

// считаем отступы
void LexemAnalyzer::HandleIndentation(int currentIndent) {
    int previousIndent = indentStack_.back();
    if (currentIndent > previousIndent) {
        indentStack_.push_back(currentIndent);
        lexems_.emplace_back(Lexem(LexemType::INDENT, "INDENT", index_ - currentIndent, index_));
    }
    else {
        while (currentIndent < previousIndent) {
            indentStack_.pop_back();
            lexems_.emplace_back(Lexem(LexemType::DEDENT, "DEDENT", index_, index_));
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
    GetNextChar();
    AnalyzeProgram();
}

void LexemAnalyzer::AnalyzeProgram() {
    int iteration = 0;

    while (ch_ != '\0') {
        SkipWhitespace();
        if (ch_ == '\0') break;
        AnalyzeStatement();
    }

    while (indentStack_.size() > 1) {
        indentStack_.pop_back();
        lexems_.emplace_back(Lexem(LexemType::DEDENT, "DEDENT", index_, index_));
    }
}

// Analyze a single statement
void LexemAnalyzer::AnalyzeStatement() {

    if (ch_ == 'd' && code_.substr(index_ - 1, 3) == "def") {
        AnalyzeFunctionDeclaration();
    }
    else if (isalpha(ch_) || ch_ == '_') {
        size_t tempIndex = index_ - 1;
        std::string word;
        while (ch_ != '\0' && (isalnum(ch_) || ch_ == '_')) {
            word += ch_;
            GetNextChar();
        }
        std::cout << "----------------------->" << word << std::endl;
        if (word == "INT" || word == "FLOAT" || word == "STRING" || word == "BOOL") {
            lexems_.emplace_back(Lexem(LexemType::KEYWORD, word, index_ - word.length(), index_));
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
        else if (keywords_.has(word.c_str(), word.length(), index_ - word.length()).first) {
            lexems_.emplace_back(Lexem(LexemType::KEYWORD, word, index_ - word.length(), index_));
        }
        else {
            AnalyzeAssignment();
        }
    }
    else if (ch_ == ';') {
        lexems_.emplace_back(Lexem(LexemType::EOS, ";", index_ - 1, index_));
        GetNextChar();
    }
    else {
        throw std::runtime_error("Unexpected character in AnalyzeStatement(): '" + std::string(1, ch_) + "'");
        GetNextChar();
    }
}

void LexemAnalyzer::AnalyzeKeyword() {
    while (ch_ != '\0' && (isalnum(ch_) || ch_ == '_')) {
        GetNextChar();
    }
    AnalyzeIdentifier();
}

// Analyze variable 
void LexemAnalyzer::AnalyzeVariableDeclaration() {
    SkipWhitespace();
    AnalyzeIdentifier();
    SkipWhitespace();
    //AnalyzeIdentifier();
    SkipWhitespace();

    // ищем []
    if (ch_ == '[') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "[", index_ - 1, index_));
        GetNextChar();
        std::cout << "----------------------->" << ch_ << std::endl;
        if (isdigit(ch_)) {
            AnalyzeNumber();
        }
        else {
            AnalyzeIdentifier();
        }
        // GetNextChar();
        std::cout << "----------------------->" << ch_ << std::endl;
        if (ch_ == ']') {
            lexems_.emplace_back(Lexem(LexemType::BRACKET, "]", index_ - 1, index_));
            SkipWhitespace();
            GetNextChar();
            if (ch_ == '=') {
                GetNextChar();
                AnalyzeArrayDeclaration();
            }
        }
        else {
            throw std::runtime_error("Expected ']' after array size");
        }
        SkipWhitespace();
        std::cout << "----------------------->" << ch_ << std::endl;
        if (ch_ == '=') {
            lexems_.emplace_back(Lexem(LexemType::OPERATOR, "=", index_ - 1, index_));
            SkipWhitespace();
            AnalyzeArrayDeclaration();
        }
    }

    // ищем =
    else if (ch_ == '=') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, "=", index_ - 1, index_));
        GetNextChar();
        AnalyzeExpression();
        SkipWhitespace();
        if (ch_ == ';') {
            lexems_.emplace_back(Lexem(LexemType::EOS, ";", index_ - 1, index_));
            GetNextChar();
        }
        else if (ch_ == '\n') {
            lexems_.emplace_back(Lexem(LexemType::NEWLINE, "\\n", index_ - 1, index_));
            GetNextChar();
        }
        else {
            // simple warn чтобы не втыкал
            std::cout << "Warning: Expected ';' or newline after assignment but found '" << ch_ << "'" << std::endl;
            GetNextChar();
        }
    }
    else {
        throw std::runtime_error("Expected '=' or '[' in assignment");
        GetNextChar();
    }
}


// Analyze array declaration
void LexemAnalyzer::AnalyzeArrayDeclaration() {
    SkipWhitespace();
    GetNextChar();
    GetNextChar();
    std::cout << "----------------------->" << ch_ << std::endl;
    if (ch_ != '{') {
        throw std::runtime_error("Expected '{' at start of array declaration");
    }
    
    lexems_.emplace_back(Lexem(LexemType::BRACKET, "{", index_ - 1, index_));
    GetNextChar();
    SkipWhitespace();
    while (true) {
        if (ch_ == '}') {
            break;
        }
        
        if (isdigit(ch_)) {
            AnalyzeNumber();
        } else if (isalpha(ch_) || ch_ == '_') {
            AnalyzeIdentifier();
        } else {
            throw std::runtime_error("Expected number or identifier in array");
        }
        
        SkipWhitespace();
        
        if (ch_ == ',') {
            lexems_.emplace_back(Lexem(LexemType::OPERATOR, ",", index_ - 1, index_));
            GetNextChar();
            SkipWhitespace();
        } else if (ch_ != '}') {
            throw std::runtime_error("Expected ',' or '}' in array declaration");
        }
    }
    
    lexems_.emplace_back(Lexem(LexemType::BRACKET, "}", index_ - 1, index_));
    GetNextChar();
    SkipWhitespace();
    
    if (ch_ != ';') {
        throw std::runtime_error("Expected ';' after array declaration");
    }
    
    lexems_.emplace_back(Lexem(LexemType::EOS, ";", index_ - 1, index_));
    GetNextChar();
}

// Analyze assignments
void LexemAnalyzer::AnalyzeAssignment() {
    AnalyzeIdentifier();
    SkipWhitespace();

    // Handle function calls
    if (ch_ == '(') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", index_ - 1, index_));
        GetNextChar();
        SkipWhitespace();
        
        // Parse arguments
        while (ch_ != ')') {
            AnalyzeExpression();
            SkipWhitespace();
            
            if (ch_ == ',') {
                lexems_.emplace_back(Lexem(LexemType::OPERATOR, ",", index_ - 1, index_));
                GetNextChar();
                SkipWhitespace();
            } else if (ch_ != ')') {
                throw std::runtime_error("Expected ',' or ')' in function call");
            }
        }
        
        lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", index_ - 1, index_));
        GetNextChar();
        SkipWhitespace();
        
        // Handle semicolon after function call
        if (ch_ == ';') {
            lexems_.emplace_back(Lexem(LexemType::EOS, ";", index_ - 1, index_));
            GetNextChar();
        } else if (ch_ == '\n') {
            lexems_.emplace_back(Lexem(LexemType::NEWLINE, "\\n", index_ - 1, index_));
            GetNextChar();
        }
    }
    // Handle assignments
    else if (ch_ == '=') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, "=", index_ - 1, index_));
        GetNextChar();
        AnalyzeExpression();
        SkipWhitespace();
        
        if (ch_ == ';') {
            lexems_.emplace_back(Lexem(LexemType::EOS, ";", index_ - 1, index_));
            GetNextChar();
        } else if (ch_ == '\n') {
            lexems_.emplace_back(Lexem(LexemType::NEWLINE, "\\n", index_ - 1, index_));
            GetNextChar();
        } else {
            throw std::runtime_error("Expected ';' or newline after assignment");
        }
    }
    else {
        throw std::runtime_error("Expected '=' or '(' after identifier");
    }

    std::cout << "Exiting AnalyzeAssignment()" << std::endl;
}

// Analyze expressions
void LexemAnalyzer::AnalyzeExpression() {
    while (ch_ != '\0' && ch_ != ';' && ch_ != '\n' && ch_ != ':' && ch_ != '}' && ch_ != ',' && ch_ != ')') {
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
            } else {
                throw std::runtime_error("Expected closing parenthesis ')'");
            }
        }
        // Handle operators
        else if (ch_ == '+' || ch_ == '-' || ch_ == '*' || ch_ == '/' || ch_ == '%' ||
                 ch_ == '<' || ch_ == '>' || ch_ == '=' || ch_ == '!' || ch_ == '&' || ch_ == '|') {
            std::string op;
            op += ch_;
            GetNextChar();
            
            // Handle two-character operators
            if (ch_ == '=' || 
                (ch_ == '&' && op[0] == '&') || 
                (ch_ == '|' && op[0] == '|') ||
                (ch_ == '+' && op[0] == '+') ||
                (ch_ == '-' && op[0] == '-')) {
                op += ch_;
                GetNextChar();
            }
            
            if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%" ||
                op == "++" || op == "--" || op == "+=" || op == "-=" || op == "*=" || op == "/=" || op == "%=") {
                lexems_.emplace_back(Lexem(LexemType::OPERATOR, op, index_ - op.length(), index_));
            } else if (op == "==" || op == "!=" || op == "<" || op == ">" || op == "<=" || op == ">=") {
                lexems_.emplace_back(Lexem(LexemType::REL_OP, op, index_ - op.length(), index_));
            } else if (op == "&&" || op == "||") {
                lexems_.emplace_back(Lexem(LexemType::OPERATOR, op, index_ - op.length(), index_));
            }
        }
        else if (ch_ == '[') {
            lexems_.emplace_back(Lexem(LexemType::BRACKET, "[", index_ - 1, index_));
            GetNextChar();
            AnalyzeExpression();
            if (ch_ == ']') {
                lexems_.emplace_back(Lexem(LexemType::BRACKET, "]", index_ - 1, index_));
                GetNextChar();
            } else {
                throw std::runtime_error("Expected closing bracket ']'");
            }
        }
        else {
            GetNextChar();
        }
        SkipWhitespace();
    }
}

// Analyze identifiers
void LexemAnalyzer::AnalyzeIdentifier() {
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
    std::string number;
    size_t startPos = index_ - 1;

    bool was_point = false;

    while (isdigit(ch_) || (ch_ == '.' && !was_point)) {
        number += ch_;
        if (ch_ == '.') {
            was_point = true;
        }
        GetNextChar();
        if (was_point && ch_ == '.'){
            throw std::runtime_error("No second '.' into number");
        }
    }

    lexems_.emplace_back(Lexem(LexemType::NUMBER, number, startPos, index_));
}

void LexemAnalyzer::PrintLexems() const {
    for (auto& lexem : lexems_) {
        std::cout << "Type: " << lexem.get_type() << ", Text: " << lexem.get_text() << std::endl;
    }
}

// Analyze function
void LexemAnalyzer::AnalyzeFunctionDeclaration() {
    // Skip def
    for (int i = 0; i < 3; ++i) {
        GetNextChar();
    }
    lexems_.emplace_back(Lexem(LexemType::KEYWORD, "def", index_ - 3, index_));

    SkipWhitespace();
    AnalyzeIdentifier();
    SkipWhitespace();

    if (ch_ == '(') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", index_ - 1, index_));
        GetNextChar();
        AnalyzeParameters();
        
        if (ch_ == ')') {
            lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", index_ - 1, index_));
            GetNextChar();
        }
        else {
            throw std::runtime_error("Expected ')' after function parameters");
        }
    }
    else {
        throw std::runtime_error("Expected '(' after function name");
    }

    SkipWhitespace();

    if (ch_ == ':') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, ":", index_ - 1, index_));
        GetNextChar();
    }
    else {
        throw std::runtime_error("Expected ':' after function declaration");
    }
}

// Analyze function parameters
void LexemAnalyzer::AnalyzeParameters() {
    if (ch_ == ')') {
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
            if (keywords_.has(type.c_str(), type.length(), startPos).first) {
                lexems_.emplace_back(Lexem(LexemType::KEYWORD, type, startPos, index_));
            } else {
                lexems_.emplace_back(Lexem(LexemType::IDENTIFIER, type, startPos, index_));
            }
        }
        
        SkipWhitespace();
        AnalyzeIdentifier();
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
            GetNextChar();
        }
    }
}

// Analyze if statements
void LexemAnalyzer::AnalyzeIfStatement() {
    // Skip if
    for (int i = 0; i < 2; ++i) {
        GetNextChar();
    }
    lexems_.emplace_back(Lexem(LexemType::KEYWORD, "if", index_ - 2, index_));
    SkipWhitespace();
    AnalyzeExpression();
    SkipWhitespace();

    if (ch_ == ':') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, ":", index_ - 1, index_));
        GetNextChar();
    }
    else {
        throw std::runtime_error("Expected ':' after if condition");
    }
}

// Analyze while statements
void LexemAnalyzer::AnalyzeWhileStatement() {
    // Skip while
    for (int i = 0; i < 5; ++i) {
        GetNextChar();
    }
    lexems_.emplace_back(Lexem(LexemType::KEYWORD, "while", index_ - 5, index_));
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
        GetNextChar();
    } else {
        throw std::runtime_error("Expected ':' after while condition");
    }
}

// Analyze print statements
void LexemAnalyzer::AnalyzePrintStatement() {
    // Skip print
    for (int i = 0; i < 5; ++i) {
        GetNextChar();
    }
    lexems_.emplace_back(Lexem(LexemType::KEYWORD, "print", index_ - 5, index_));
    SkipWhitespace();
    if (ch_ == '(') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", index_ - 1, index_));
        GetNextChar();
        AnalyzeExpression();
        if (ch_ == ')') {
            lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", index_ - 1, index_));
            GetNextChar();
        }
        else {
            throw std::runtime_error("Expected ')' after print expression");
        }
    }
    else {
        throw std::runtime_error("Expected '(' after print");
    }

    SkipWhitespace();
    if (ch_ == ';') {
        lexems_.emplace_back(Lexem(LexemType::EOS, ";", index_ - 1, index_));
        GetNextChar();
    }
    else if (ch_ == '\n') {
        lexems_.emplace_back(Lexem(LexemType::NEWLINE, "\\n", index_ - 1, index_));
        GetNextChar();
    }
    else {
        //  simple warning  чтобы не втыкал
        std::cout << "Warning: Expected ';' or newline after print statement but found '" << ch_ << "'" << std::endl;
        GetNextChar();
    }
}

// Analyze return 
void LexemAnalyzer::AnalyzeReturnStatement() {
    for (int i = 0; i < 6; ++i) {
        GetNextChar();
    }
    lexems_.emplace_back(Lexem(LexemType::KEYWORD, "return", index_ - 6, index_));
    SkipWhitespace();
    AnalyzeExpression();
    SkipWhitespace();
    if (ch_ == ';') {
        lexems_.emplace_back(Lexem(LexemType::EOS, ";", index_ - 1, index_));
        GetNextChar();
    }
    else if (ch_ == '\n') {
        lexems_.emplace_back(Lexem(LexemType::NEWLINE, "\\n", index_ - 1, index_));
        GetNextChar();
    }
    else {
        // simple warn чтобы не тыкал
        std::cout << "Warning: Expected ';' or newline after return statement but found '" << ch_ << "'" << std::endl;
        GetNextChar();
    }
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