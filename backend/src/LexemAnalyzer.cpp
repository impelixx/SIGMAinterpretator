#include "../include/LexemAnalyzer.h"
#include <cctype>
#include <stdexcept>
#include <iostream>
#include <stack>
#include <fstream>

LexemAnalyzer::LexemAnalyzer(const std::string& code, const std::string& pathToKeywords)
    : code_(code), ch_('\0'), index_(0), currentPosition_(0) {
    indentStack_.push_back(0);
    std::ifstream keywords(pathToKeywords);
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
                lexems_.emplace_back(Lexem(LexemType::NEWLINE, "\\n", index_ - 1, index_, curLine_));
                curLine_++;
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
        lexems_.emplace_back(Lexem(LexemType::INDENT, "INDENT", index_ - currentIndent, index_, curLine_));
    }
    else {
        while (currentIndent < previousIndent) {
            indentStack_.pop_back();
            lexems_.emplace_back(Lexem(LexemType::DEDENT, "DEDENT", index_, index_, curLine_));
            if (indentStack_.empty()) {
                throw std::runtime_error("Indentation error: No matching indentation level.\n On line: " + std::to_string(curLine_));
            }
            previousIndent = indentStack_.back();
        }
        if (currentIndent != previousIndent) {
            throw std::runtime_error("Indentation error: Inconsistent indentation.\n On line: " + std::to_string(curLine_));
        }
    }
}

void LexemAnalyzer::Analyze() {
    GetNextChar();
    AnalyzeProgram();
    lexems_.emplace_back(Lexem(LexemType::EOC, std::string("eof"), index_, index_+1, curLine_));
}

void LexemAnalyzer::AnalyzeProgram() {

    while (ch_ != '\0') {
        SkipWhitespace();
        if (ch_ == '\0') break;
        AnalyzeStatement();
    }
    while (indentStack_.size() > 1) {
        indentStack_.pop_back();
        lexems_.emplace_back(Lexem(LexemType::DEDENT, "DEDENT", index_, index_, curLine_));
    }
}

// просто выражение
void LexemAnalyzer::AnalyzeStatement() {
        size_t tempIndex = index_ - 1;
        std::string word;
        while (ch_ != '\0' && (isalnum(ch_) || ch_ == '_')) {
            word += ch_;
            GetNextChar();
        }
        std::cout << word << "<======="<< std::endl;
        if (word == "int" || word == "float" || word == "string" || word == "bool") {
            lexems_.emplace_back(Lexem(LexemType::KEYWORD, word, tempIndex, index_, curLine_));
            AnalyzeVariableDeclaration();
        } else if (word == "def") {
            AnalyzeFunctionDeclaration();
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
            lexems_.emplace_back(Lexem(LexemType::KEYWORD, word, index_ - word.length(), index_, curLine_));
        } else {
            if (word == "[" || word == "]" || word == "{" || word == "}" || word == "(" || word == ")") {
                lexems_.emplace_back(Lexem(LexemType::BRACKET, word, index_ - word.length(), index_, curLine_));
                GetNextChar();
                return;
            }
            if (word.empty() && word != " ") {
                GetNextChar();
                AnalyzeStatement();
                return;
            }
            //lexems_.emplace_back(Lexem(LexemType::IDENTIFIER, word, index_ - word.length(), index_, curLine_));
            GetNextChar();
            AnalyzeAssignment();
        }
}

void LexemAnalyzer::AnalyzeElseStatement() {
    if (ch_ == ':') {
        lexems_.emplace_back(Lexem(LexemType::KEYWORD, "else", index_ - 4, index_, curLine_));
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
        throw std::runtime_error("Expected variable name in variable declaration, but get empty string\n On line: " + std::to_string(curLine_));
    }

    // Handle array declarations
    if (ch_ == '[') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "[", index_ - 1, index_, curLine_));
        GetNextChar();
        SkipWhitespace();
        AnalyzeExpression();
        SkipWhitespace();

        if (ch_ == ']') {
            lexems_.emplace_back(Lexem(LexemType::BRACKET, "]", index_ - 1, index_, curLine_));
            GetNextChar();
            SkipWhitespace();
        } else {
            throw std::runtime_error("Expected ']' after array size expression, but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
        }
    }

    SkipWhitespace();
    
    if (ch_ == '=') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, "=", index_ - 1, index_, curLine_));
        SkipWhitespace();
        GetNextChar();
        SkipWhitespace();
        GetNextChar();
        if (ch_ == '{') {
            AnalyzeArrayDeclaration();
            return;
        }  else {
            AnalyzeExpression();
        }
        if (ch_ == '[') {
            lexems_.emplace_back(Lexem(LexemType::BRACKET, "[", index_ - 1, index_, curLine_));
            GetNextChar();
            AnalyzeExpression();
            if (ch_ != ']') {
                throw std::runtime_error("Expected ']' after array index, but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
            }
            lexems_.emplace_back(Lexem(LexemType::BRACKET, "]", index_ - 1, index_, curLine_));
            GetNextChar();
        }
        if (lexems_.back().get_type() != "NUMBER" && lexems_.back().get_type() != "IDENTIFIER" && lexems_.back().get_type() != "STRING" && lexems_.back().get_type() == "BRACKETS") {
            throw std::runtime_error("Expected number or identifier after '=' in variable declaration, but get '" + lexems_.back().get_text() + "'\n On line: " + std::to_string(curLine_));
        }
        SkipWhitespace();
        if (ch_ == ';') {
            lexems_.emplace_back(Lexem(LexemType::EOS, ";", index_ - 1, index_, curLine_));
            GetNextChar();
            AnalyzeStatement();
        
        } else if (ch_ == '(') {
            lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", index_ - 1, index_, curLine_));
            GetNextChar();
            SkipWhitespace();
            while (ch_ != ')') {
                AnalyzeExpression();
                SkipWhitespace();
                if (ch_ == ',') {
                    lexems_.emplace_back(Lexem(LexemType::OPERATOR, ",", index_ - 1, index_, curLine_));
                    GetNextChar();
                    SkipWhitespace();
                } else if (ch_ != ')') {
                    throw std::runtime_error("Expected ',' or ')' in function call, but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
                }
            }
            lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", index_ - 1, index_, curLine_));
            GetNextChar();
            SkipWhitespace();
            if (ch_ == ';') {
                lexems_.emplace_back(Lexem(LexemType::EOS, ";", index_ - 1, index_, curLine_));
                GetNextChar();
                return;
            } else if (ch_ == '\n') {
                lexems_.emplace_back(Lexem(LexemType::NEWLINE, "\\n", index_ - 1, index_, curLine_));
                curLine_++;
                GetNextChar();
                return;
            }
        }
    }
    else if (ch_ == ';') {
        lexems_.emplace_back(Lexem(LexemType::EOS, ";", index_ - 1, index_, curLine_));
        SkipWhitespace();
        GetNextChar();
        return;
    } else {
        throw std::runtime_error("Expected '=' or ';' after variable declaration, but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
    }
}

void LexemAnalyzer::AnalyzeForStatement() {
    lexems_.emplace_back(Lexem(LexemType::KEYWORD, "for", index_ - 1, index_ + 2, curLine_));
    SkipWhitespace();
    AnalyzeIdentifier();
    SkipWhitespace();
    std::string tmp = "";
    for (int i = 0; i < 2; ++i) {
        tmp += ch_;
        GetNextChar();
    }
    if (tmp != "in") {
        throw std::runtime_error("Expected 'in' after for loop variable, but get '" + tmp + "'\n On line: " + std::to_string(curLine_));
    }
    lexems_.emplace_back(Lexem(LexemType::KEYWORD, "in", index_ - 2, index_, curLine_));
    SkipWhitespace();
    if (ch_ != '(') {
        throw std::runtime_error("Expected '(' after 'in', but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
    }
    lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", index_ - 1, index_, curLine_));
    GetNextChar();
    AnalyzeExpression();
    SkipWhitespace();
    if (ch_ != ',') {
        throw std::runtime_error("Expected ',' after for loop expression, but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
    }
    lexems_.emplace_back(Lexem(LexemType::OPERATOR, ",", index_ - 1, index_, curLine_));
    GetNextChar();
    AnalyzeExpression();
    SkipWhitespace();
    if (ch_ != ')') {
        throw std::runtime_error("Expected ')' after for loop expression, but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
    }
    lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", index_ - 1, index_, curLine_));
    SkipWhitespace();
    GetNextChar();
    if (ch_ != ':') {
        throw std::runtime_error("Expected ':' after for loop expression, but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
    }
}

// Analyze array declaration
void LexemAnalyzer::AnalyzeArrayDeclaration() {
    if (ch_ != '{') {
        throw std::runtime_error("Expected '{' at start of array declaration, but get '" + std::string(1, ch_) + "'");
    }

    lexems_.emplace_back(Lexem(LexemType::BRACKET, "{", index_ - 1, index_, curLine_));

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
            throw std::runtime_error("Expected number or identifier in array, but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
        }
        
        SkipWhitespace();
        
        if (ch_ == ',') {
            lexems_.emplace_back(Lexem(LexemType::OPERATOR, ",", index_ - 1, index_, curLine_));
            GetNextChar();
            SkipWhitespace();
        } else if (ch_ != '}') {
            throw std::runtime_error("Expected ',' or '}' in array declaration, but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
        }
    }
    
    lexems_.emplace_back(Lexem(LexemType::BRACKET, "}", index_ - 1, index_, curLine_));
    GetNextChar();
    SkipWhitespace();
    
    if (ch_ != ';') {
        throw std::runtime_error("Expected ';' after array declaration, but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
    }
    
    lexems_.emplace_back(Lexem(LexemType::EOS, ";", index_ - 1, index_, curLine_));
    GetNextChar();
}

// Analyze assignments
void LexemAnalyzer::AnalyzeAssignment() {
    std::cout << "AnalyzeAssignment" << ch_  << curLine_<< std::endl;
    if (ch_ == ';') {
        lexems_.emplace_back(Lexem(LexemType::EOS, ";", index_ - 1, index_, curLine_));
        GetNextChar();
        return;
    }
    SkipWhitespace();
    AnalyzeIdentifier();
    SkipWhitespace();
    if (ch_ == '(') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", index_ - 1, index_, curLine_));
        GetNextChar();
        SkipWhitespace();
        while (ch_ != ')') {
            AnalyzeExpression();
            SkipWhitespace();
            
            if (ch_ == ',') {
                lexems_.emplace_back(Lexem(LexemType::OPERATOR, ",", index_ - 1, index_, curLine_));
                GetNextChar();
                SkipWhitespace();
            } else if (ch_ != ')') {
                throw std::runtime_error("Expected ',' or ')' in function call, but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
            }
        }
        
        lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", index_ - 1, index_, curLine_));
        GetNextChar();
        SkipWhitespace();
        if (ch_ == ';') {
            lexems_.emplace_back(Lexem(LexemType::EOS, ";", index_ - 1, index_, curLine_));
            GetNextChar();
            return;
        } else if (ch_ == '\n') {
            lexems_.emplace_back(Lexem(LexemType::NEWLINE, "\\n", index_ - 1, index_, curLine_));
            curLine_++;
            GetNextChar();
            return;
        }
    } else if (ch_ == '[') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "[", index_ - 1, index_, curLine_));
        GetNextChar();
        AnalyzeExpression();
        if (ch_ != ']') {
            throw std::runtime_error("Expected ']' after array index, but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
        }
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "]", index_ - 1, index_, curLine_));
        GetNextChar(); 
        SkipWhitespace();
        if (ch_ != '=') {
            throw std::runtime_error("Expected '=' after array index, but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
        }
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, "=", index_ - 1, index_, curLine_));
        GetNextChar();
        SkipWhitespace();
        AnalyzeExpression();
    } else if (ch_ == '=') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, "=", index_ - 1, index_, curLine_));
        GetNextChar();
        AnalyzeExpression();
        SkipWhitespace();
        if (ch_ == '[') {
            lexems_.emplace_back(Lexem(LexemType::BRACKET, "[", index_ - 1, index_, curLine_));
            GetNextChar();
            AnalyzeExpression();
            if (ch_ != ']') {
                throw std::runtime_error("Expected ']' after array index, but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
            }
            lexems_.emplace_back(Lexem(LexemType::BRACKET, "]", index_ - 1, index_, curLine_));
            GetNextChar();
        }
        if (ch_ == ';') {
            lexems_.emplace_back(Lexem(LexemType::EOS, ";", index_ - 1, index_, curLine_));
            GetNextChar();
            return;
        } else if (ch_ == '\n') {
            lexems_.emplace_back(Lexem(LexemType::NEWLINE, "\\n", index_ - 1, index_, curLine_));
            curLine_++;
            GetNextChar();
            return;
        }
    } else {
        throw std::runtime_error("Expected '=' or '(' after identifier, but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
    }
}

// Analyze expressions
void LexemAnalyzer::AnalyzeExpression() {
    if (ch_ == ';'){
        return;
    }
    if (ch_ == '"') {
        std::string str;
        size_t startPos = index_ - 1;
        GetNextChar();
        while (ch_ != '"' && ch_ != '\0') {
            str += ch_;
            GetNextChar();
        }
        lexems_.emplace_back(Lexem(LexemType::STRING, str, startPos, index_, curLine_));
        return;
    }
        SkipWhitespace();
        if (isdigit(ch_)) {
            AnalyzeNumber();
        }
        else if ((isalpha(ch_) || ch_ == '_') && ch_ != ';') {
            AnalyzeIdentifier();
        } else if (ch_ == '(') {
            lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", index_ - 1, index_, curLine_));
            GetNextChar();
            AnalyzeExpression();
            if (ch_ == ')'){
                lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", index_ - 1, index_, curLine_));
                GetNextChar();
            } else {
                throw std::runtime_error("Expected ')' after expression, but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
            }
        } else if (ch_ == '"') {
            std::string str;
            size_t startPos = index_ - 1;
            GetNextChar();
            while (ch_ != '"' && ch_ != '\0') {
                str += ch_;
                GetNextChar();
            }
            lexems_.emplace_back(Lexem(LexemType::STRING, str, startPos, index_, curLine_));
            GetNextChar();
            return; 
        } else {
            throw std::runtime_error("Unexpected character in expression: '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
        }

    SkipWhitespace();
    while (ch_ == '+' || ch_ == '-' || ch_ == '*' || ch_ == '/' || ch_ == '%') {
        std::string op(1, ch_);
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, op, index_ - 1, index_, curLine_));
        GetNextChar();
        SkipWhitespace();
        AnalyzeExpression();
    }
}

// Analyze identifiers
void LexemAnalyzer::AnalyzeIdentifier() {
    if (ch_ == ';'){
        return;
    }
    std::string identifier;
    size_t startPos = index_ - 1;

    while ((isalnum(ch_) || ch_ == '_') && ch_ != ';'){
        identifier += ch_;
        GetNextChar();
    }

    if (identifier.empty()) {
        return;
        throw std::runtime_error("Expected identifier, but get empty string\n On line: " + std::to_string(curLine_));
    }
    if (identifier.empty() || identifier == " " || identifier == "\n" || identifier == ";"){
        GetNextChar();
        AnalyzeStatement();
    }
    auto [isKeyword, lexem] = keywords_.has(identifier.c_str(), identifier.length(), startPos);
    if (isKeyword) {
        lexems_.emplace_back(Lexem(LexemType::KEYWORD, identifier, startPos, index_, curLine_));
    } else {
        lexems_.emplace_back(Lexem(LexemType::IDENTIFIER, identifier, startPos, index_, curLine_));
    }
}

// Analyze numbers
void LexemAnalyzer::AnalyzeNumber() {
    std::string number;
    size_t startPos = index_ - 1;

    bool point = false;

    while (isdigit(ch_) || (ch_ == '.' && !point)) {
        number += ch_;
        if (ch_ == '.') {
            point = true;
        }
        GetNextChar();
        if (point && ch_ == '.') {
            throw std::runtime_error("No second '.' in number\n On line: " + std::to_string(curLine_));
        }
    }

    lexems_.emplace_back(Lexem(LexemType::NUMBER, number, startPos, index_, curLine_));
}

void LexemAnalyzer::PrintLexems() const {
    for (auto& lexem : lexems_) {
        std::cout << "Type: " << lexem.get_type() << ", Text: " << lexem.get_text() << " On line: " << lexem.get_line() << std::endl;
    }
}

// Analyze function
void LexemAnalyzer::AnalyzeFunctionDeclaration() {

    lexems_.emplace_back(Lexem(LexemType::KEYWORD, "def", index_ - 3, index_, curLine_));

    SkipWhitespace();
    AnalyzeIdentifier();
    SkipWhitespace();

    if (ch_ == '(') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", index_ - 1, index_, curLine_));
        GetNextChar();
        AnalyzeParameters();
        if (lexems_.back().get_type() == "OPERATOR") {
            throw std::runtime_error("Expected identifier\n On line: " + std::to_string(curLine_));
        }

        if (ch_ == ')') {
            lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", index_ - 1, index_, curLine_));
            GetNextChar();
        } else {
            throw std::runtime_error("Expected ')' after function parameters \n On line: " + std::to_string(curLine_));
        }
    } else {
        throw std::runtime_error("Expected '(' after function name, but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
    }

    SkipWhitespace();

    if (ch_ == ':') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, ":", index_ - 1, index_, curLine_));
        AnalyzeStatement();
    } else {
        throw std::runtime_error("Expected ':' after function declaration, but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
    }
}

// Analyze function parameters
void LexemAnalyzer::AnalyzeParameters() {
    std::cout << "AnalyzeParameters" << ch_ << curLine_ << std::endl;
    SkipWhitespace();
    if (ch_ == ')') {
        return;
    }

    while (ch_ != ')' && ch_ != '\0') {
        std::string type;
        size_t startPos = index_ - 1;
        while (isalpha(ch_)) {
            type += ch_;
            GetNextChar();
        }
        if (!keywords_.has(type.c_str(), type.length(), startPos).first) {
            throw std::runtime_error("Expected type name in function parameter list, but get '" + type + "'\n On line: " + std::to_string(curLine_));
        }
        lexems_.emplace_back(Lexem(LexemType::KEYWORD, type, startPos, index_, curLine_));
        SkipWhitespace();

        AnalyzeIdentifier();
        SkipWhitespace();

        if (ch_ == '[') {
            lexems_.emplace_back(Lexem(LexemType::BRACKET, "[", index_ - 1, index_, curLine_));
            GetNextChar();
            if (ch_ == ']') {
                lexems_.emplace_back(Lexem(LexemType::BRACKET, "]", index_ - 1, index_, curLine_));
                GetNextChar();
            } else {
                throw std::runtime_error("Expected ']' after '[', but got '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
            }
            SkipWhitespace();
        }

        if (ch_ == ',') {
            lexems_.emplace_back(Lexem(LexemType::OPERATOR, ",", index_ - 1, index_, curLine_));
            GetNextChar();
            SkipWhitespace();
        } else if (ch_ != ')') {
            throw std::runtime_error("Expected ',' or ')' in function parameter list, but got '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
        }
    }
}

// Analyze if statements
void LexemAnalyzer::AnalyzeIfStatement() {
    lexems_.emplace_back(Lexem(LexemType::KEYWORD, "if", index_ - 2, index_, curLine_));
    SkipWhitespace();
    // GetNextChar();
    SkipWhitespace();
    if (ch_ == '(')
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", index_, index_ + 1, curLine_));
    GetNextChar();
    AnalyzeExpression();
    if (lexems_.back().get_type() == "OPERATOR") {
        throw std::runtime_error("Expected expression in if condition, but get '" + lexems_.back().get_text() + "'\n On line: " + std::to_string(curLine_));
    }
    if (ch_ == ')')
        lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", index_ - 1, index_, curLine_));
    GetNextChar();
    SkipWhitespace();
    if (ch_ == ':') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, ":", index_ - 1, index_, curLine_));
        GetNextChar();
    }
    else {
        throw std::runtime_error("Expected ':' after if condition, but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
    }
}

// Analyze while statements
void LexemAnalyzer::AnalyzeWhileStatement() {
    lexems_.emplace_back(Lexem(LexemType::KEYWORD, "while", index_ - 5, index_, curLine_));
    SkipWhitespace();
    
    if (ch_ == '(') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", index_ - 1, index_, curLine_));
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
                lexems_.emplace_back(Lexem(LexemType::OPERATOR, op, index_ - op.length(), index_, curLine_));
            }
            SkipWhitespace();
        }
        if (lexems_.back().get_type() == "BRACKET" ) {
            throw std::runtime_error("Expected expression in while condition, but get '" + lexems_.back().get_text() + "'\n On line: " + std::to_string(curLine_));
        }
        if (ch_ == ')') {
            lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", index_ - 1, index_, curLine_));
            GetNextChar();
        } else {
            throw std::runtime_error("Expected ')' after while condition, but get '" + std::string(1, ch_) + "'");
        }
    } else {
        throw std::runtime_error("Expected '(' after while keyword, but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
    }

    SkipWhitespace();

    if (ch_ == ':') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, ":", index_ - 1, index_, curLine_));
        GetNextChar();
    } else {
        throw std::runtime_error("Expected ':' after while condition, but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
    }
}

// Analyze print statements
void LexemAnalyzer::AnalyzePrintStatement() {
    SkipWhitespace();
    lexems_.emplace_back(Lexem(LexemType::KEYWORD, "print", index_ - 5, index_, curLine_));
    SkipWhitespace();
    if (ch_ == '(') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", index_ - 1, index_, curLine_));
        GetNextChar();
        
        AnalyzeExpression();
        if (ch_ == ')') {
            lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", index_ - 1, index_, curLine_));
            GetNextChar();
            SkipWhitespace();
            if (ch_ != ';'){
                throw std::runtime_error("Expected ';' after print statement, but get " + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
            }
        }
        else {
            throw std::runtime_error("Expected ')' after print expression, but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
        }
    }
    else {
        throw std::runtime_error("Expected '(' after print, but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
    }

    SkipWhitespace();
    if (ch_ == ';') {
        lexems_.emplace_back(Lexem(LexemType::EOS, ";", index_ - 1, index_, curLine_));
        GetNextChar();
        return;
    } else if (ch_ == '\n') {
        lexems_.emplace_back(Lexem(LexemType::NEWLINE, "\\n", index_ - 1, index_, curLine_));
        curLine_++;
        GetNextChar();
        return;
    }
    else {
        //  simple warning  чтобы не втыкал
        std::cout << "Warning: Expected ';' or newline after print statement but found '" << ch_ << "'" << std::endl;
        GetNextChar();
    }
}

// Analyze return 
void LexemAnalyzer::AnalyzeReturnStatement() {
    lexems_.emplace_back(Lexem(LexemType::KEYWORD, "return", index_ - 6, index_, curLine_));
    SkipWhitespace();
    GetNextChar();
    lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", index_ - 1, index_, curLine_));
    AnalyzeExpression();
    if (ch_ != ')') {
        throw std::runtime_error("Expected ')' after return expression, but get '" + std::string(1, ch_) + "'\n On line: " + std::to_string(curLine_));
    }
    lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", index_ - 1, index_, curLine_));
    GetNextChar();
    SkipWhitespace();
    GetNextChar();
    return;
}