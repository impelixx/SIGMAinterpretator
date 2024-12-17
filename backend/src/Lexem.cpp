#include "../include/Lexem.h"
#include <iostream>

void Lexem::set_text(std::string txt) {
  text_ = txt;
}

/**
 * @brief Returns the string representation of the lexem type.
 * 
 * @return std::string One of the following:
 *         - "KEYWORD" for keywords
 *         - "NUMBER" for numeric values
 *         - "EOS" for end of statement
 *         - "OPERATOR" for operators
 *         - "STRING" for string literals
 *         - "IDENTIFIER" for variable/function names
 *         - "NEWLINE" for line breaks
 *         - "COMMENT" for comments
 *         - "DEDENT" for decrease in indentation
 *         - "INDENT" for increase in indentation
 *         - "BRACKET" for brackets
 *         - "EOC" for end of code
 *         - "UNKNOWN" if type is not recognized
 */
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
}

/**
 * @brief Получает текст лексемы.
 */
std::string Lexem::get_text() const {
  return text_;
}

/**
 * @brief Получает номер строки лексемы.
 */
size_t Lexem::get_line() const {
  return line_;
}

/**
 * @brief Получает начальную позицию лексемы.
 */
size_t Lexem::get_start() const {
  return s_;
}

/**
 * @brief Получает конечную позицию лексемы.
 */
size_t Lexem::get_end() const {
  return e_;
}

