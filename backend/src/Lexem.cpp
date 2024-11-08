#include "../include/Lexem.h"
#include <iostream>

void Lexem::set_text(std::string txt){
    text_ = txt;
}

std::string Lexem::get_type() const {
    if (type_ == LexemType::KEYWORD) {
        return "KEYWORD";
    }
    if (type_ == LexemType::NUMBER) {
        return "NUMBER";
    }
    if (type_ == LexemType::EOS) {
        return "EOS";
    }
    if (type_ == LexemType::OPERATOR) {
        return "OPERATOR";
    }
    if (type_ == LexemType::STRING) {
        return "STRING";
    }
    if (type_ == LexemType::IDENTIFIER) {
        return "IDENTIFIER";
    }
    if (type_ == LexemType::NEWLINE) {
        return "NEWLINE";
    }
    if (type_ == LexemType::COMMENT) {
        return "COMMENT";
    }
    if (type_ == LexemType::DEDENT) {
        return "DEDENT";
    }
    if (type_ == LexemType::INDENT) {
        return "INDENT";
    }
    if (type_ == LexemType::BRACKET) {
        return "BRACKET";
    }
    return "UNKNOWN";
}

std::string Lexem::get_text() const{
    return text_;
}

Lexem::Lexem(LexemType type, std::string text, int s, int e)
    : type_(type), text_(std::move(text)) {

}