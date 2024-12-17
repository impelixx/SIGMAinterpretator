/**
 * @file Lexem.h
 * @brief Defines the Lexem class and LexemType enum for lexical analysis
 * 
 * @enum LexemType
 * Enumerates different types of lexical tokens:
 * - KEYWORD: Programming language keywords (e.g., int, float, def, class)
 * - NUMBER: Numeric literals
 * - REL_OP: Relational operators (==, !=, >=, <=, >, <)
 * - OPERATOR: Arithmetic and logical operators (+, -, *, /, %, &&, ||, !, =, +=, -=, *=, /=, %=)
 * - STRING: String literals
 * - IDENTIFIER: Variable and function names
 * - NEWLINE: Line break token
 * - COMMENT: Code comments
 * - DEDENT: Decrease in indentation level
 * - BRACKET: Parentheses and brackets
 * - INDENT: Increase in indentation level
 * - EOS: End of string marker
 * - UNKNOWN: Unrecognized tokens
 * - EOC: End of file marker
 *
 * @class Lexem
 * @brief Represents a lexical token with its properties
 *
 * This class stores information about a lexical token including its type,
 * text content, position in source code, and line number.
 */

/**
 * @brief Constructs a new Lexem object
 * @param type The type of the lexical token
 * @param text The actual text content of the token
 * @param s Starting position in the source code
 * @param e Ending position in the source code
 * @param l Line number where the token appears
 */

/**
 * @brief Get the line number of the token
 * @return size_t Line number
 */

/**
 * @brief Get the type of the token as string
 * @return std::string String representation of token type
 */

/**
 * @brief Get the text content of the token
 * @return std::string The actual text of the token
 */

/**
 * @brief Get the starting position of the token
 * @return size_t Starting position in source code
 */

/**
 * @brief Get the ending position of the token
 * @return size_t Ending position in source code
 */

/**
 * @brief Set the text content of the token
 * @param text New text content to set
 */
#include <iostream>

#ifndef BACKEND_LEXEM_H
#define BACKEND_LEXEM_H

enum LexemType {
  KEYWORD,
  NUMBER,
  REL_OP,
  OPERATOR,
  STRING,
  IDENTIFIER,
  NEWLINE,
  COMMENT,
  DEDENT,
  BRACKET,
  INDENT,
  EOS,
  UNKNOWN,
  EOC
};

class Lexem {
 public:
  Lexem(LexemType type, std::string text, int s, int e, int l)
      : type_(type), text_(text), s_(s), e_(e), line_(l){};
  // Getters
  size_t get_line() const;
  std::string get_type() const;
  std::string get_text() const;
  size_t get_start() const;
  size_t get_end() const;
  // Setters
  void set_text(std::string text);

 private:
  int s_, e_;
  LexemType type_;
  std::string text_;
  size_t line_;
};

#endif  //BACKEND_LEXEM_H
