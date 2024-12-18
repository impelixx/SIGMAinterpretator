#include "../include/LexemAnalyzer.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <stack>
#include <stdexcept>
#include "../include/Lexem.h"

/**
 * @brief Constructor for the LexemAnalyzer class
 * 
 * @param code String containing the source code to be analyzed
 * @param pathToKeywords Path to the file containing keywords
 * 
 * @throws std::runtime_error If the keywords file cannot be opened
 * 
 * @details Initializes a new LexemAnalyzer instance with the given source code.
 * Sets up initial state with:
 * - Empty current character
 * - Zero index and position
 * - Initial indent level of 0
 * - Loads keywords from the specified file into keywords_ container
 */
LexemAnalyzer::LexemAnalyzer(const std::string& code,
                             const std::string& pathToKeywords)
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

/**
 * @brief Advances to the next character in the input code string
 * 
 * This method reads the next character from code_ and updates the current position.
 * If a newline character is encountered, the line counter is incremented.
 * When reaching end of input, sets character to null terminator.
 * 
 * @pre code_ contains the input string to analyze
 * @post ch_ contains the next character (or \0 if at end)
 * @post index_ is incremented
 * @post currentPosition_ is updated
 * @post curLine_ is incremented if newline was read
 */
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

/**
 * @brief Skips whitespace characters and handles indentation and comments in the source code
 * 
 * This method processes the input stream by:
 * 1. Skipping standard whitespace characters
 * 2. Handling newlines and subsequent indentation:
 *    - Creates a NEWLINE lexem
 *    - Increments line counter
 *    - Counts spaces after newline and processes indentation
 * 3. Processing comments:
 *    - Single-line comments (// style)
 *    - Multi-line comments (cpp style)
 * 
 * The method advances the character pointer (ch_) until it encounters
 * a non-whitespace, non-comment character.
 */
void LexemAnalyzer::SkipWhitespace() {
  while (true) {
    while (isspace(ch_)) {
      if (ch_ == '\n') {
        lexems_.emplace_back(
            Lexem(LexemType::NEWLINE, "\\n", index_ - 1, index_, curLine_));
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
    if (ch_ == '/' && index_ < code_.length() && code_[index_] == '/') {
      while (ch_ != '\n' && ch_ != '\0') {
        GetNextChar();
      }
      continue;
    }
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

/**
 * @brief Handles indentation levels in the source code and generates corresponding INDENT/DEDENT lexems.
 * 
 * This method manages the indentation stack and generates appropriate lexems based on the current
 * indentation level compared to the previous one. It follows these rules:
 * - If current indent is greater than previous, pushes INDENT lexem
 * - If current indent is less than previous, pushes DEDENT lexem(s) until matching level is found
 * - If indentation is inconsistent, throws runtime error
 * 
 * @param currentIndent The number of spaces/tabs at the current line
 * @throws std::runtime_error If there's no matching indentation level or inconsistent indentation
 */
void LexemAnalyzer::HandleIndentation(int currentIndent) {
  int previousIndent = indentStack_.back();
  if (currentIndent > previousIndent) {
    indentStack_.push_back(currentIndent);
    lexems_.emplace_back(Lexem(LexemType::INDENT, "INDENT",
                               index_ - currentIndent, index_, curLine_));
  } else {
    while (currentIndent < previousIndent) {
      indentStack_.pop_back();
      lexems_.emplace_back(
          Lexem(LexemType::DEDENT, "DEDENT", index_, index_, curLine_));
      if (indentStack_.empty()) {
        throw std::runtime_error(
            "Indentation error: No matching indentation level.\n On line: " +
            std::to_string(curLine_));
      }
      previousIndent = indentStack_.back();
    }
    if (currentIndent != previousIndent) {
      throw std::runtime_error(
          "Indentation error: Inconsistent indentation.\n On line: " +
          std::to_string(curLine_));
    }
  }
}

/**
 * @brief Performs lexical analysis of the source code.
 * 
 * This method executes the main lexical analysis process:
 * 1. Gets the first character from input
 * 2. Analyzes the program structure
 * 3. Adds an end-of-code (EOC) lexem at the end
 * 
 * The analysis results are stored in the lexems_ container.
 */
void LexemAnalyzer::Analyze() {
  GetNextChar();
  AnalyzeProgram();
  lexems_.emplace_back(
      Lexem(LexemType::EOC, std::string("eof"), index_, index_ + 1, curLine_));
}

/**
 * @brief Analyzes the entire program by processing statements until the end of input
 * 
 * Continuously processes statements in the input by:
 * 1. Skipping any whitespace characters
 * 2. Breaking if end of input is reached
 * 3. Analyzing individual statements
 * 
 * The analysis continues until a null terminator ('\0') is encountered,
 * indicating the end of the input stream.
 */
void LexemAnalyzer::AnalyzeProgram() {
  while (ch_ != '\0') {
    SkipWhitespace();
    if (ch_ == '\0') {
      break;
    }
    AnalyzeStatement();
  }
}

/**
 * @brief Analyzes a single statement in the source code and generates corresponding lexems
 * 
 * This method processes statements by:
 * 1. Handling keywords and their specific statements (if, while, for, etc.)
 * 2. Processing variable declarations
 * 3. Processing function declarations
 * 4. Processing identifiers and assignments
 * 5. Processing expressions
 * 
 * The analyzer skips whitespace and identifies tokens based on the current character.
 * For alphabetic characters, it builds words and checks if they are keywords.
 * For keywords, it calls the appropriate analysis method.
 * For identifiers, it handles function calls and assignments.
 * For other characters, it processes them as expressions.
 * 
 * @note The method advances the internal character pointer and updates the lexem collection
 */
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
    auto [isKeyword, lexem] =
        keywords_.has(word.c_str(), word.length(), startPos);
    if (isKeyword) {
      lexems_.emplace_back(Lexem(LexemType::KEYWORD, word, startPos,
                                 currentPosition_, curLine_));
      if (word == "int" || word == "float" || word == "bool" ||
          word == "string") {
        AnalyzeVariableDeclaration();
      }
      if (word == "def") {
        AnalyzeFunctionDeclaration();
      }
      if (word == "if") {
        AnalyzeIfStatement();
      }
      if (word == "elif") {
        AnalyzeIfStatement();
      }
      if (word == "else") {
        AnalyzeElseStatement();
      }
      if (word == "for") {
        AnalyzeForStatement();
      }
      if (word == "while") {
        AnalyzeWhileStatement();
      }
      if (word == "print") {
        AnalyzePrintStatement();
      }
      if (word == "return") {
        AnalyzeReturnStatement();
      }

    } else {
      if (word.empty()) {
        GetNextChar();
        SkipWhitespace();
        return;
      }
      lexems_.emplace_back(Lexem(LexemType::IDENTIFIER, word, startPos,
                                 currentPosition_, curLine_));
      SkipWhitespace();
      if (ch_ == '(') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", currentPosition_,
                                   currentPosition_ + 1, curLine_));
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

/**
 * @brief Analyzes assignment operation in the source code.
 * 
 * Processes an assignment operation by checking for the '=' operator and analyzing
 * the following expression. The method skips any leading whitespace before processing.
 * 
 * If an assignment operator is found:
 * 1. Creates a new OPERATOR lexem with '=' value
 * 2. Advances to the next character
 * 3. Processes the right-hand side expression
 */
void LexemAnalyzer::AnalyzeAssignment() {
  SkipWhitespace();
  if (ch_ == '=') {
    lexems_.emplace_back(Lexem(LexemType::OPERATOR, "=", currentPosition_,
                               currentPosition_ + 1, curLine_));
    GetNextChar();
    AnalyzeExpression();
  }
}

/**
 * @brief Prints all lexems stored in the analyzer to standard output
 * 
 * Iterates through the stored lexems and prints their type, text value, and line number
 * in the format "TYPE: <type> VALUE: <value> On line: <line_number>"
 * 
 * @note Output is sent to std::cout with each lexem on a new line
 */
void LexemAnalyzer::PrintLexems() const {
  for (const auto& lexem : lexems_) {
    std::cout << "TYPE: " << lexem.get_type() << " VALUE: " << lexem.get_text()
              << " On line: " << lexem.get_line() << std::endl;
  }
}

/**
 * @brief Analyzes variable declaration statements in the source code
 * 
 * This method handles the parsing of variable declarations, which can include:
 * - Variable identifier
 * - Optional assignment operation
 * - Expression following the assignment
 * - Statement termination with semicolon
 * 
 * The method processes the following grammar:
 * variable_declaration ::= identifier ['=' expression] ';'
 * 
 * @throws May throw exceptions if invalid syntax is encountered
 */
void LexemAnalyzer::AnalyzeVariableDeclaration() {
  SkipWhitespace();
  size_t startPos = currentPosition_;
  if (isalpha(ch_)) {
    AnalyzeIdentifier();
    SkipWhitespace();
    if (ch_ == '=') {
      lexems_.emplace_back(Lexem(LexemType::OPERATOR, "=", currentPosition_,
                                 currentPosition_ + 1, curLine_));
      GetNextChar();
      AnalyzeExpression();
    }
    if (ch_ == ';') {
      lexems_.emplace_back(Lexem(LexemType::OPERATOR, ";", currentPosition_,
                                 currentPosition_ + 1, curLine_));
      GetNextChar();
      AnalyzeStatement();
    }
  }
}

/**
 * @brief Analyzes and processes an identifier or keyword in the source code.
 * 
 * This method reads characters from the current position until it encounters a non-alphanumeric
 * character (excluding underscores). It then checks if the accumulated word is a keyword.
 * If the word is a keyword, it creates a KEYWORD lexem, otherwise creates an IDENTIFIER lexem.
 * Both types of lexems are added to the lexems_ collection.
 * 
 * If no valid identifier characters are found, the method skips whitespace and returns.
 * 
 * @note The method automatically advances the current position pointer and updates the current character.
 */
void LexemAnalyzer::AnalyzeIdentifier() {
  size_t startPos = currentPosition_;
  std::string word;
  while (isalnum(ch_) || ch_ == '_') {
    word += ch_;
    GetNextChar();
  }
  if (word.empty()) {
    GetNextChar();
    SkipWhitespace();
    return;
  }
  auto [isKeyword, lexem] =
      keywords_.has(word.c_str(), word.length(), startPos);
  if (isKeyword) {
    lexems_.emplace_back(
        Lexem(LexemType::KEYWORD, word, startPos, currentPosition_, curLine_));
  } else {
    lexems_.emplace_back(Lexem(LexemType::IDENTIFIER, word, startPos,
                               currentPosition_, curLine_));
  }
}

/**
 * @brief Analyzes and tokenizes an expression in the input source code
 * 
 * This method processes the current expression until it encounters a semicolon, newline, 
 * or end of file. During processing, it:
 * - Handles semicolons as statement terminators
 * - Skips whitespace characters
 * - Identifies and processes:
 *   - Numbers
 *   - Identifiers
 *   - String literals
 *   - Brackets (parentheses, curly braces, square brackets)
 *   - Operators and punctuation marks
 * 
 * For each recognized token, it creates a corresponding Lexem object and adds it 
 * to the lexems_ collection.
 */
void LexemAnalyzer::AnalyzeExpression() {
  if (ch_ == ';') {
    lexems_.emplace_back(Lexem(LexemType::OPERATOR, ";", currentPosition_,
                               currentPosition_ + 1, curLine_));
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
    } else if (ch_ == '(' || ch_ == ')' || ch_ == '{' || ch_ == '}' ||
               ch_ == '[' || ch_ == ']') {
      lexems_.emplace_back(Lexem(LexemType::BRACKET, std::string(1, ch_),
                                 currentPosition_, currentPosition_ + 1,
                                 curLine_));
      GetNextChar();
    } else if (ispunct(ch_)) {
      lexems_.emplace_back(Lexem(LexemType::OPERATOR, std::string(1, ch_),
                                 currentPosition_, currentPosition_ + 1,
                                 curLine_));
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
  lexems_.emplace_back(Lexem(LexemType::NUMBER, number,
                             currentPosition_ - number.length(),
                             currentPosition_, curLine_));
  return;
}

void LexemAnalyzer::AnalyzeString() {
  GetNextChar();
  std::string str;
  while (ch_ != '"') {
    str += ch_;
    GetNextChar();
  }
  lexems_.emplace_back(Lexem(LexemType::STRING, str,
                             currentPosition_ - str.length(), currentPosition_,
                             curLine_));
  GetNextChar();
}

void LexemAnalyzer::AnalyzeArrayDeclaration() {
  lexems_.emplace_back(Lexem(LexemType::BRACKET, "{", currentPosition_,
                             currentPosition_ + 1, curLine_));
  GetNextChar();
  SkipWhitespace();
  while (ch_ != '}' && ch_ != '\0') {
    AnalyzeExpression();
    SkipWhitespace();
    if (ch_ == ',') {
      lexems_.emplace_back(Lexem(LexemType::OPERATOR, ",", currentPosition_,
                                 currentPosition_ + 1, curLine_));
      GetNextChar();
      SkipWhitespace();
    } else if (ch_ != '}') {
      GetNextChar();
      return;
    }
  }
  if (ch_ == '}') {
    lexems_.emplace_back(Lexem(LexemType::BRACKET, "}", currentPosition_,
                               currentPosition_ + 1, curLine_));
    GetNextChar();
  } else {
    GetNextChar();
    return;
  }
}

/**
 * @brief Analyzes function declaration syntax in the source code
 * 
 * This method handles the parsing of function declarations. It processes:
 * 1. The function identifier/name
 * 2. Opening parenthesis
 * 3. Function parameters
 * 
 * The method expects the current position to be at the start of function identifier.
 * After execution, the position will be after the parameters list.
 * 
 * @throws LexicalError If the function declaration syntax is invalid
 */
void LexemAnalyzer::AnalyzeFunctionDeclaration() {
  AnalyzeIdentifier();
  SkipWhitespace();
  if (ch_ == '(') {
    lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", currentPosition_,
                               currentPosition_ + 1, curLine_));
    GetNextChar();
    AnalyzeParameters();
  }
}

/**
 * @brief Analyzes function parameters in the source code
 * 
 * This method processes function parameters within parentheses, handling:
 * - Empty parameter lists ()
 * - Multiple parameters separated by commas
 * - Individual parameter identifiers
 * 
 * The method advances through the source code, creating lexems for:
 * - Parameter identifiers
 * - Commas between parameters
 * - Closing parenthesis
 * 
 * The analysis continues until either:
 * - A closing parenthesis is encountered
 * - The end of input is reached
 * 
 * After processing parameters, it may proceed to analyze the subsequent statement.
 */
void LexemAnalyzer::AnalyzeParameters() {
  SkipWhitespace();
  if (ch_ == ')') {
    lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", currentPosition_,
                               currentPosition_ + 1, curLine_));
    GetNextChar();
    AnalyzeStatement();
  }
  while (ch_ != ')' && ch_ != '\0') {
    AnalyzeIdentifier();
    if (ch_ == ',') {
      lexems_.emplace_back(Lexem(LexemType::OPERATOR, ",", currentPosition_,
                                 currentPosition_ + 1, curLine_));
      GetNextChar();
    } else if (ch_ != ')') {
      GetNextChar();
      return;
    }
  }
  if (ch_ == ')') {
    lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", currentPosition_,
                               currentPosition_ + 1, curLine_));
    GetNextChar();
  } else {
    GetNextChar();
    return;
  }
}

/**
 * @brief Analyzes an 'if' statement in the source code.
 * 
 * This method handles the parsing of if statements by:
 * 1. Moving to the next character after 'if'
 * 2. Skipping any whitespace
 * 3. Processing the opening parenthesis if present
 * 4. Analyzing the condition expression inside the if statement
 * 
 * @note Expects to be called when current position is at the 'if' keyword
 * @throws May throw exceptions if the expression analysis fails
 */
GetNextChar();
SkipWhitespace();
if (ch_ == ':') {
  lexems_.emplace_back(Lexem(LexemType::OPERATOR, ":", currentPosition_,
                             currentPosition_ + 1, curLine_));
  GetNextChar();
  AnalyzeStatement();
}
}

/**
 * @brief Analyzes a 'for' statement in the source code.
 * 
 * This method processes a for loop construct by:
 * 1. Analyzing the loop variable identifier
 * 2. Checking for the 'in' keyword
 * 3. Processing the array declaration if the 'in' keyword is found
 * 
 * The method expects the format: "for identifier in array"
 * 
 * @pre Current position must be at the start of the for statement
 * @post Current position will be at the end of the array declaration
 * 
 * @throws May throw exceptions from AnalyzeIdentifier() or AnalyzeArrayDeclaration()
 */
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
    lexems_.emplace_back(Lexem(LexemType::KEYWORD, "in", currentPosition_ - 2,
                               currentPosition_, curLine_));
    GetNextChar();
    AnalyzeArrayDeclaration();
  }
}

/**
 * @brief Analyzes a while statement in the source code.
 * 
 * This method handles the parsing of while statement syntax. It processes:
 * 1. Any leading whitespace
 * 2. The opening parenthesis '('
 * 3. The conditional expression inside the while statement
 * 
 * The method creates appropriate lexems and stores them in the lexems_ collection.
 * After finding the opening bracket, it delegates to AnalyzeExpression() for
 * parsing the condition.
 */
void LexemAnalyzer::AnalyzeWhileStatement() {
  SkipWhitespace();
  if (ch_ == '(') {
    lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", currentPosition_,
                               currentPosition_ + 1, curLine_));
    GetNextChar();
    AnalyzeExpression();
  }
}

/**
 * @brief Analyzes print statement syntax in the input stream
 * 
 * This method processes print statement by analyzing its components:
 * - Skips any leading whitespace
 * - Handles opening parenthesis by creating corresponding lexem
 * - Processes the expression within print statement
 * 
 * Expected syntax: print(expression)
 * 
 * @note Assumes print keyword has already been processed before this method is called
 */
void LexemAnalyzer::AnalyzePrintStatement() {
  SkipWhitespace();
  if (ch_ == '(') {
    lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", currentPosition_,
                               currentPosition_ + 1, curLine_));
    GetNextChar();
    AnalyzeExpression();
  }
}

/**
 * @brief Analyzes a return statement in the source code
 * 
 * Processes a return statement by checking for semicolon and creating
 * corresponding lexem. After semicolon is processed, continues analysis
 * of the next statement.
 * 
 * The method skips any whitespace before processing the return statement
 * and expects a semicolon as the terminating character.
 */
void LexemAnalyzer::AnalyzeReturnStatement() {
  SkipWhitespace();
  if (ch_ == ';') {
    lexems_.emplace_back(Lexem(LexemType::OPERATOR, ";", currentPosition_,
                               currentPosition_ + 1, curLine_));
    GetNextChar();
    AnalyzeStatement();
  }
}