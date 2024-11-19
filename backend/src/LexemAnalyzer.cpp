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
    } else {
        ch_ = '\0';
    }
}

// пропускаем пробелы и новые и комменты
void LexemAnalyzer::SkipWhitespace() {
    while (true) {
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
                    GetNextChar();
                    GetNextChar();
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
    lexems_.emplace_back(Lexem(LexemType::EOC, std::string("eof"), index_, index_+1));
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

// просто выражение
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
        if (word == "int" || word == "float" || word == "string" || word == "bool") {
            lexems_.emplace_back(Lexem(LexemType::KEYWORD, word, index_ - word.length(), index_));
            AnalyzeVariableDeclaration();
        } else if (word == "if") {
            AnalyzeIfStatement();
        } else if (word == "while") {
            AnalyzeWhileStatement();
        } else if (word == "print") {
            AnalyzePrintStatement();
        } else if (word == "else") {
            AnalyzeElseStatement();
        } else if (word == "return") {
            AnalyzeReturnStatement();
        } else if (word == "for") {
            AnalyzeForStatement();
        } else if (keywords_.has(word.c_str(), word.length(), index_ - word.length()).first) {
            lexems_.emplace_back(Lexem(LexemType::KEYWORD, word, index_ - word.length(), index_));
        } else {
            lexems_.emplace_back(Lexem(LexemType::IDENTIFIER, word, index_ - word.length(), index_));
            AnalyzeAssignment();
        }
    } else if (ch_ == ';') {
        lexems_.emplace_back(Lexem(LexemType::EOS, ";", index_ - 1, index_));
        GetNextChar();
    } else {
        throw std::runtime_error("Unexpected character in AnalyzeStatement(): '" + std::string(1, ch_) + "'");
        GetNextChar();
    }
}

void LexemAnalyzer::AnalyzeElseStatement() {
    if (ch_ == ':') {
        lexems_.emplace_back(Lexem(LexemType::KEYWORD, "else", index_ - 4, index_));
        GetNextChar();
        SkipWhitespace();
    }
}

// Analyze variable Declaration
void LexemAnalyzer::AnalyzeVariableDeclaration() {
    SkipWhitespace();
    AnalyzeIdentifier();
    SkipWhitespace();
    if (lexems_.back().get_text().empty()) {
        throw std::runtime_error("Expected variable name in variable declaration, but get '" + lexems_.back().get_text() + "'");
    }

    // Handle array declarations
    if (ch_ == '[') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "[", index_ - 1, index_));
        GetNextChar();
        SkipWhitespace();
        AnalyzeExpression();
        SkipWhitespace();

        if (ch_ == ']') {
            lexems_.emplace_back(Lexem(LexemType::BRACKET, "]", index_ - 1, index_));
            GetNextChar();
            SkipWhitespace();
        } else {
            throw std::runtime_error("Expected ']' after array size expression, but get" + std::string(1, ch_));
        }
    }

    SkipWhitespace();
    
    if (ch_ == '=') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, "=", index_ - 1, index_));
        SkipWhitespace();
        GetNextChar();
        SkipWhitespace();
        if (ch_ == '{') {
            //lexems_.emplace_back(Lexem(LexemType::OPERATOR, "=", index_, index_ + 1));
            AnalyzeArrayDeclaration();
            return;
        }
        else {
            AnalyzeExpression();
        }
        // std::cout << lexems_.back().get_type() << std::endl;
        // std::cout << lexems_.back().get_text() << std::endl;
        if (lexems_.back().get_type() != "NUMBER" && lexems_.back().get_type() != "IDENTIFIER" && lexems_.back().get_type() != "STRING") {
            throw std::runtime_error("Expected number or identifier after '=' in variable declaration, but get '" + lexems_.back().get_text() + "'");
        }
        SkipWhitespace();
        if (ch_ == ';') {
            lexems_.emplace_back(Lexem(LexemType::EOS, ";", index_ - 1, index_));
            GetNextChar();
        } else if (ch_ == '\n') {
            lexems_.emplace_back(Lexem(LexemType::NEWLINE, "\\n", index_ - 1, index_));
            GetNextChar();
        } else {
            throw std::runtime_error("Expected ';' or newline after assignment, but get '" + std::string(1, ch_) + "'");
        }
    }
}

void LexemAnalyzer::AnalyzeForStatement() {
    lexems_.emplace_back(Lexem(LexemType::KEYWORD, "for", index_ - 1, index_ + 2));
    SkipWhitespace();

    if (!(isalpha(ch_) || ch_ == '_')) {
        throw std::runtime_error("Expected type name in for statement, but get '" + std::string(1, ch_) + "'");
    }
    AnalyzeIdentifier();
    SkipWhitespace();

    if (!(isalpha(ch_) || ch_ == '_')) {
        throw std::runtime_error("Expected variable name in for statement, but get '" + std::string(1, ch_) + "'");
    }
    AnalyzeIdentifier();
    SkipWhitespace();
    if (code_.substr(index_-1, 2) != "in") {
        throw std::runtime_error("Expected 'in' keyword in for statement but get '" + code_.substr(index_-1, 2) + "'");
    }
    GetNextChar();
    lexems_.emplace_back(Lexem(LexemType::KEYWORD, "in", index_ - 2, index_));
    GetNextChar(); 
    SkipWhitespace();

    if (ch_ != '(') {
        throw std::runtime_error("Expected '(' after 'in' keyword in for statement");
    }
    lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", index_ - 1, index_));
    GetNextChar();
    SkipWhitespace();

    AnalyzeExpression();
    SkipWhitespace();

    if (ch_ != ',') {
        throw std::runtime_error("Expected ',' between range values, but get '" + std::string(1, ch_) + "'");
    }
    lexems_.emplace_back(Lexem(LexemType::OPERATOR, ",", index_ - 1, index_));
    GetNextChar();
    SkipWhitespace();

    AnalyzeExpression();
    SkipWhitespace();

    if (ch_ != ')') {
        throw std::runtime_error("Expected ')' after range values, but get '" + std::string(1, ch_) + "'");
    }
    lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", index_ - 1, index_));
    GetNextChar();
    SkipWhitespace();

    if (ch_ != ':') {
        throw std::runtime_error("Expected ':' after for statement, but get '" + std::string(1, ch_) + "'");
    }
    lexems_.emplace_back(Lexem(LexemType::EOS, ":", index_ - 1, index_));
    GetNextChar();
    SkipWhitespace();
}

// Analyze array declaration
void LexemAnalyzer::AnalyzeArrayDeclaration() {
    if (ch_ != '{') {
        throw std::runtime_error("Expected '{' at start of array declaration, but get '" + std::string(1, ch_) + "'");
    }

    lexems_.emplace_back(Lexem(LexemType::BRACKET, "{", index_ - 1, index_));

    SkipWhitespace();
    GetNextChar();
    while (true) {
        if (ch_ == '}') {
            break;
        }
        if (isdigit(ch_)) {
            AnalyzeNumber();
        } else if (isalpha(ch_) || ch_ == '_') {
            AnalyzeIdentifier();
        } else if (ch_ == '}') { 
            break;
        } else {
            throw std::runtime_error("Expected number or identifier in array, but get '" + std::string(1, ch_) + "'");
        }
        
        SkipWhitespace();
        
        if (ch_ == ',') {
            lexems_.emplace_back(Lexem(LexemType::OPERATOR, ",", index_ - 1, index_));
            GetNextChar();
            SkipWhitespace();
        } else if (ch_ != '}') {
            throw std::runtime_error("Expected ',' or '}' in array declaration, but get '" + std::string(1, ch_) + "'");
        }
    }
    
    lexems_.emplace_back(Lexem(LexemType::BRACKET, "}", index_ - 1, index_));
    GetNextChar();
    SkipWhitespace();
    
    if (ch_ != ';') {
        throw std::runtime_error("Expected ';' after array declaration, but get '" + std::string(1, ch_) + "'");
    }
    
    lexems_.emplace_back(Lexem(LexemType::EOS, ";", index_ - 1, index_));
    GetNextChar();
}

// Analyze assignments
void LexemAnalyzer::AnalyzeAssignment() {
    AnalyzeIdentifier();
    SkipWhitespace();
    if (ch_ == '(') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", index_ - 1, index_));
        GetNextChar();
        SkipWhitespace();
        while (ch_ != ')') {
            AnalyzeExpression();
            SkipWhitespace();
            
            if (ch_ == ',') {
                lexems_.emplace_back(Lexem(LexemType::OPERATOR, ",", index_ - 1, index_));
                GetNextChar();
                SkipWhitespace();
            } else if (ch_ != ')') {
                throw std::runtime_error("Expected ',' or ')' in function call, but get '" + std::string(1, ch_) + "'");
            }
        }
        
        lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", index_ - 1, index_));
        GetNextChar();
        SkipWhitespace();
        if (ch_ == ';') {
            lexems_.emplace_back(Lexem(LexemType::EOS, ";", index_ - 1, index_));
            GetNextChar();
        } else if (ch_ == '\n') {
            lexems_.emplace_back(Lexem(LexemType::NEWLINE, "\\n", index_ - 1, index_));
            GetNextChar();
        }
    }
    else if (ch_ == '=') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, "=", index_ - 1, index_));
        GetNextChar();
        // std::cout << ch_ << std::endl;
        AnalyzeExpression();
        SkipWhitespace();
        
        if (ch_ == ';') {
            lexems_.emplace_back(Lexem(LexemType::EOS, ";", index_ - 1, index_));
            GetNextChar();
        } else if (ch_ == '\n') {
            lexems_.emplace_back(Lexem(LexemType::NEWLINE, "\\n", index_ - 1, index_));
            GetNextChar();
        }
    } else {
        throw std::runtime_error("Expected '=' or '(' after identifier, but get '" + std::string(1, ch_) + "'");
    }
}

// Analyze expressions
void LexemAnalyzer::AnalyzeExpression() {
    SkipWhitespace();
    if (ch_ == '"') {
        GetNextChar();
        std::string temp = "";
        while (ch_ != '"'){
            temp += ch_;
            GetNextChar();
        }
        GetNextChar();
        lexems_.emplace_back(Lexem(LexemType::STRING, temp, index_ - temp.length(), index_));
        return;
    }
    while (isdigit(ch_) || isalpha(ch_) || ch_ == '_' || ch_ == '[' || ch_ == '+' || ch_ == '-' || ch_ == '*' || ch_ == '/' || ch_ == ';' || ch_ == '\n' || ch_ == ',' || ch_ == '\0' || ch_ == '(') {
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
            
            if (ch_ != ')') {
                throw std::runtime_error("Expected closing ')' after array size expression, but get '" + std::string(1, ch_) + "'");
            }
            // std::cout << lexems_.back().get_type() << std::endl;
            lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", index_ - 1, index_));
            GetNextChar();
        }
        else if (ch_ == '+' || ch_ == '-' || ch_ == '*' || ch_ == '/') {
            std::string op(1, ch_);
            lexems_.emplace_back(Lexem(LexemType::OPERATOR, op, index_ - 1, index_));
            GetNextChar();
        }
        else if (ch_ == ';' || ch_ == '\n' || ch_ == ',') {
            if (lexems_.back().get_type() == "OPERATOR") {
                throw std::runtime_error("Expected expression in expression, but get '" + lexems_.back().get_text() + "'");
            }
            break;
        }
        else if (ch_ == '\0') {
            throw std::runtime_error("Unexpected end of file while parsing expression, but get '" + std::string(1, ch_) + "'");
        }
        else {
            throw std::runtime_error("Unexpected character in expression: '" + std::string(1, ch_) + "'");
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
        if (lexems_.back().get_type() == "OPERATOR") {
            throw std::runtime_error("expected identifier");
        }

        if (ch_ == ')') {
            lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", index_ - 1, index_));
            GetNextChar();
        } else {
            throw std::runtime_error("Expected ')' after function parameters");
        }
    } else {
        throw std::runtime_error("Expected '(' after function name, but get '" + std::string(1, ch_) + "'");
    }

    SkipWhitespace();

    if (ch_ == ':') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, ":", index_ - 1, index_));
        GetNextChar();
    } else {
        throw std::runtime_error("Expected ':' after function declaration, but get '" + std::string(1, ch_) + "'");
    }
}

// Analyze function parameters
void LexemAnalyzer::AnalyzeParameters() {
    if (ch_ == ')') {
        return;
    }

    while (ch_ != ')' && ch_ != '\0' && (isdigit(ch_) || isalpha(ch_))) {
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
                throw std::runtime_error("Expected ']' after '[', but get '" + std::string(1, ch_) + "'");
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
    lexems_.emplace_back(Lexem(LexemType::KEYWORD, "if", index_ - 2, index_));
    SkipWhitespace();
    // GetNextChar();
    SkipWhitespace();
    if (ch_ == '(')
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", index_, index_ + 1));
    GetNextChar();
    AnalyzeExpression();
    if (lexems_.back().get_type() == "OPERATOR") {
        throw std::runtime_error("Expected expression in if condition, but get '" + lexems_.back().get_text() + "'");
    }
    if (ch_ == ')')
        lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", index_ - 1, index_));
    GetNextChar();
    SkipWhitespace();
    // std::cout << lexems_.back().get_type() << std::endl;
    if (ch_ == ':') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, ":", index_ - 1, index_));
        GetNextChar();
    }
    else {
        throw std::runtime_error("Expected ':' after if condition, but get '" + std::string(1, ch_) + "'");
    }
}

// Analyze while statements
void LexemAnalyzer::AnalyzeWhileStatement() {
    lexems_.emplace_back(Lexem(LexemType::KEYWORD, "while", index_ - 5, index_));
    SkipWhitespace();
    
    if (ch_ == '(') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", index_ - 1, index_));
        GetNextChar();
        SkipWhitespace();
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
        if (lexems_.back().get_type() == "BRACKET" ) {
            throw std::runtime_error("Expected expression in while condition, but get '" + lexems_.back().get_text() + "'");
        }
        if (ch_ == ')') {
            lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", index_ - 1, index_));
            GetNextChar();
        } else {
            throw std::runtime_error("Expected ')' after while condition, but get '" + std::string(1, ch_) + "'");
        }
    } else {
        throw std::runtime_error("Expected '(' after while keyword, but get '" + std::string(1, ch_) + "'");
    }

    SkipWhitespace();

    if (ch_ == ':') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, ":", index_ - 1, index_));
        GetNextChar();
    } else {
        throw std::runtime_error("Expected ':' after while condition, but get '" + std::string(1, ch_) + "'");
    }
}

// Analyze print statements
void LexemAnalyzer::AnalyzePrintStatement() {
    SkipWhitespace();
    lexems_.emplace_back(Lexem(LexemType::KEYWORD, "print", index_ - 5, index_));
    SkipWhitespace();
    if (ch_ == '(') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", index_ - 1, index_));
        GetNextChar();
        
        AnalyzeExpression();
        if (ch_ == ')') {
            lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", index_ - 1, index_));
            GetNextChar();
            SkipWhitespace();
            if (ch_ != ';'){
                throw std::runtime_error("Expected ';' after print statement, but get " + std::string(1, ch_) + "'");
            }
        }
        else {
            throw std::runtime_error("Expected ')' after print expression, but get '" + std::string(1, ch_) + "'");
        }
    }
    else {
        throw std::runtime_error("Expected '(' after print, but get '" + std::string(1, ch_) + "'");
    }

    SkipWhitespace();
    if (ch_ == ';') {
        lexems_.emplace_back(Lexem(LexemType::EOS, ";", index_ - 1, index_));
        GetNextChar();
    } else if (ch_ == '\n') {
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
        // simple warn чтобы не втыкал
        std::cout << "Warning: Expected ';' or newline after return statement but found '" << ch_ << "'" << std::endl;
        GetNextChar();
    }
}