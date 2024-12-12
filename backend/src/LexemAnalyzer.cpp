#include "../include/LexemAnalyzer.h"
#include "../include/Lexem.h"  // Include the header for Lexem
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
        if (ch_ == '\n') {
            curLine_++;
        }
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
        if (ch_ == '\0') {
            break;
        }
        AnalyzeStatement();
    }
}

void LexemAnalyzer::AnalyzeStatement() {
    SkipWhitespace();
    if (ch_ == '\0') {
        return;
    }
    size_t startPos = currentPosition_;
    std::string word;
    if (isalpha(ch_)) {
        while (isalnum(ch_) || ch_ == '_') {
            word += ch_;
            GetNextChar();
        }
        auto [isKeyword, lexem] = keywords_.has(word.c_str(), word.length(), startPos);
        if (isKeyword) {
            lexems_.emplace_back(Lexem(LexemType::KEYWORD, word, startPos, currentPosition_, curLine_));
            if (word == "int" || word == "float" || word == "bool" || word == "string") {
                GetNextChar();
                AnalyzeVariableDeclaration();
            }
            if (word == "def") {
                //lexems_.emplace_back(Lexem(LexemType::KEYWORD, "def", currentPosition_ - 3, currentPosition_, curLine_));
                GetNextChar();
                AnalyzeFunctionDeclaration();
            }
            if (word == "if") {
                //lexems_.emplace_back(Lexem(LexemType::KEYWORD, "if", currentPosition_ - 2, currentPosition_, curLine_));
                GetNextChar();
                AnalyzeIfStatement();
            } 
            if (word == "elif") {
                GetNextChar();
                AnalyzeIfStatement();
            }
            if (word == "else") {
                GetNextChar();
                AnalyzeElseStatement();
            } 
            if (word == "for") {
                GetNextChar();
                AnalyzeForStatement();
            }
            if (word == "while") {
                GetNextChar();
                AnalyzeWhileStatement();
            }
            if (word == "print") {
                AnalyzePrintStatement();
            }
            if (word == "return") {
                AnalyzeReturnStatement();
            }
            
        } else {
            lexems_.emplace_back(Lexem(LexemType::IDENTIFIER, word, startPos, currentPosition_, curLine_));
            SkipWhitespace();
            if (ch_ == '(') {
                AnalyzeFunctionDeclaration();
            } else if (ch_ == '=') {
                AnalyzeAssignment();
            }
            AnalyzeExpression();
        }
    } else {
        AnalyzeExpression();
    }
}

void LexemAnalyzer::AnalyzeAssignment() {
    SkipWhitespace();
    if (ch_ == '=') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, "=", currentPosition_, currentPosition_ + 1, curLine_));
        GetNextChar();
        AnalyzeExpression();
    }
}

void LexemAnalyzer::PrintLexems() const {
    for (const auto& lexem : lexems_) {
        std::cout << "TYPE: " << lexem.get_type() << " VALUE: " << lexem.get_text() << " On line: " << lexem.get_line() << std::endl;
    }
}

void LexemAnalyzer::AnalyzeVariableDeclaration() {
    SkipWhitespace();
    size_t startPos = currentPosition_;
    if (isalpha(ch_)) {
        AnalyzeIdentifier();
        SkipWhitespace();
        if (ch_ == '=') {
            lexems_.emplace_back(Lexem(LexemType::OPERATOR, "=", currentPosition_, currentPosition_ + 1, curLine_));
            GetNextChar();
            AnalyzeExpression();
        }
        if (ch_ == ';') {
            lexems_.emplace_back(Lexem(LexemType::OPERATOR, ";", currentPosition_, currentPosition_ + 1, curLine_));
            GetNextChar();
            AnalyzeStatement();
        }
    }
}

void LexemAnalyzer::AnalyzeIdentifier() {
    size_t startPos = currentPosition_;
    std::string word;
    while (isalnum(ch_) || ch_ == '_') {
      word += ch_;
      GetNextChar();
    }
    auto [isKeyword, lexem] = keywords_.has(word.c_str(), word.length(), startPos);
    if (isKeyword) {
        lexems_.emplace_back(Lexem(LexemType::KEYWORD, word, startPos, currentPosition_, curLine_));
    } else {
        lexems_.emplace_back(Lexem(LexemType::IDENTIFIER, word, startPos, currentPosition_, curLine_));
    }
}

void LexemAnalyzer::AnalyzeExpression() {
    if (ch_ == ';') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, ";", currentPosition_, currentPosition_ + 1, curLine_));
        GetNextChar();
        AnalyzeStatement();
        return;
    }
        while (ch_ != '\0' && ch_ != '\n' && ch_ != ';') {
            SkipWhitespace();
            if (isdigit(ch_)) {
                AnalyzeNumber();
            } else if (isalpha(ch_)) {
                AnalyzeIdentifier();
            } else if (ch_ == '"') {
                AnalyzeString();
            } else if (ch_ == '(' || ch_ == ')' ||
                     ch_ == '{' || ch_ == '}' ||
                     ch_ == '[' || ch_ == ']') {
                lexems_.emplace_back(Lexem(LexemType::BRACKET, std::string(1, ch_), currentPosition_, currentPosition_ + 1, curLine_));
                GetNextChar();
            } else if (ispunct(ch_)) {
                lexems_.emplace_back(Lexem(LexemType::OPERATOR, std::string(1, ch_), currentPosition_, currentPosition_ + 1, curLine_));
                GetNextChar();
            } else {
                GetNextChar();
            }
        }
}

void LexemAnalyzer::AnalyzeNumber() {
    std::string number;
    while (isdigit(ch_)) {
        number += ch_;
        GetNextChar();
    }
    if (ch_ == '.') {
        number += ch_;
        GetNextChar();
        while (isdigit(ch_)) {
            number += ch_;
            GetNextChar();
        }
    }
    lexems_.emplace_back(Lexem(LexemType::NUMBER, number, currentPosition_ - number.length(), currentPosition_, curLine_));
    return;
}

void LexemAnalyzer::AnalyzeString() {
    GetNextChar();
    std::string str;
    while (ch_ != '"') {
        str += ch_;
        GetNextChar();
    }
    lexems_.emplace_back(Lexem(LexemType::STRING, str, currentPosition_ - str.length(), currentPosition_, curLine_));
    GetNextChar();
}

void LexemAnalyzer::AnalyzeArrayDeclaration() {
    lexems_.emplace_back(Lexem(LexemType::BRACKET, "{", currentPosition_, currentPosition_ + 1, curLine_));
    GetNextChar();
    SkipWhitespace();
    while (ch_ != '}' && ch_ != '\0') {
        AnalyzeExpression();
        SkipWhitespace();
        if (ch_ == ',') {
            lexems_.emplace_back(Lexem(LexemType::OPERATOR, ",", currentPosition_, currentPosition_ + 1, curLine_));
            GetNextChar();
            SkipWhitespace();
        } else if (ch_ != '}') {
            GetNextChar();
            return;
        }
    }
    if (ch_ == '}') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "}", currentPosition_, currentPosition_ + 1, curLine_));
        GetNextChar();
    } else {
        GetNextChar();
        return;
    }
}

void LexemAnalyzer::AnalyzeFunctionDeclaration() {
    //lexems_.emplace_back(Lexem(LexemType::KEYWORD, "def", currentPosition_ - 3, currentPosition_, curLine_));
    SkipWhitespace();
    AnalyzeIdentifier();
    SkipWhitespace();
    if (ch_ == '(') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", currentPosition_, currentPosition_ + 1, curLine_));
        GetNextChar();
        AnalyzeParameters();
    }
}

void LexemAnalyzer::AnalyzeParameters() {
    SkipWhitespace();
    if (ch_ == ')') {
        // Нет параметров
        lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", currentPosition_, currentPosition_ + 1, curLine_));
        GetNextChar();
        AnalyzeStatement();
    }
    while (ch_ != ')' && ch_ != '\0') {
        AnalyzeIdentifier();
        // GetNextChar();
        // SkipWhitespace();
        if (ch_ == ',') {
            lexems_.emplace_back(Lexem(LexemType::OPERATOR, ",", currentPosition_, currentPosition_ + 1, curLine_));
            GetNextChar();
        } else if (ch_ != ')') {
            GetNextChar();
            return;
        }
    }
    if (ch_ == ')') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", currentPosition_, currentPosition_ + 1, curLine_));
        GetNextChar();
    } else {
        GetNextChar();
        return;
    }
}

void LexemAnalyzer::AnalyzeIfStatement() {
    GetNextChar();
    SkipWhitespace();
    if (ch_ == '(') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", currentPosition_, currentPosition_ + 1, curLine_));
        GetNextChar();
        AnalyzeExpression();
    }
}

void LexemAnalyzer::AnalyzeElseStatement() {
    GetNextChar();
    SkipWhitespace();
    if (ch_ == ':') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, ":", currentPosition_, currentPosition_ + 1, curLine_));
        GetNextChar();
        AnalyzeStatement();
    }
}

void LexemAnalyzer::AnalyzeForStatement() {
    SkipWhitespace();
    AnalyzeIdentifier();
    SkipWhitespace();
    auto temp = "";
    for (int i = 0; i < 2; i++) {
        temp += ch_;
        GetNextChar();
    }
    if (temp == "in") {
        lexems_.emplace_back(Lexem(LexemType::KEYWORD, "in", currentPosition_ - 2, currentPosition_, curLine_));
        GetNextChar();
        AnalyzeArrayDeclaration();
    }
}

void LexemAnalyzer::AnalyzeWhileStatement() {
    SkipWhitespace();
    if (ch_ == '(') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", currentPosition_, currentPosition_ + 1, curLine_));
        GetNextChar();
        AnalyzeExpression();
    }
}

void LexemAnalyzer::AnalyzePrintStatement() {
    SkipWhitespace();
    if (ch_ == '(') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", currentPosition_, currentPosition_ + 1, curLine_));
        GetNextChar();
        AnalyzeExpression();
    }
}

void LexemAnalyzer::AnalyzeReturnStatement() {
    SkipWhitespace();
    if (ch_ == ';') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, ";", currentPosition_, currentPosition_ + 1, curLine_));
        GetNextChar();
        AnalyzeStatement();
    }
}