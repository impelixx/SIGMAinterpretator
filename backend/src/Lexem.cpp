#include "../include/Lexem.h"
#include <iostream>

void Lexem::set_text(std::string txt) {
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
  if (type_ == LexemType::STRING) {
    return "STRING";
  }
  if (type_ == LexemType::EOC) {
    return "EOC";
  }
  return "UNKNOWN";

  std::string Lexem::get_text() const {
    return text_;
  }

  size_t Lexem::get_line() const {
    return line_;
  }

  size_t Lexem::get_start() const {
    return s_;
  }

  size_t Lexem::get_end() const {
    return e_;
  }