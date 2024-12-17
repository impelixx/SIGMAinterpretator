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
 * @param code The source code string to be analyzed
 * @param pathToKeywords Path to the file containing keywords
 * 
 * @details Initializes a LexemAnalyzer object by:
 *          - Setting initial state (code, current character, indices)
 *          - Pushing initial indent level (0) to indent stack
 *          - Loading keywords from specified file into keywords_ collection
 * 
 * @throws std::runtime_error If the keywords file cannot be opened
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
 * @brief Reads the next character from the input code and updates analyzer state
 * 
 * This method advances the lexical analyzer by one character in the input code.
 * If a newline character is encountered, it increments the line counter.
 * When the end of input is reached, it sets the current character to null terminator.
 * 
 * Updates:
 * - ch_: The current character being processed
 * - index_: Position in the input code 
 * - curLine_: Current line number (incremented on newlines)
 * - currentPosition_: Current position in the input
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
 * @brief Skips whitespace characters and handles special cases in the source code
 * 
 * This method processes:
 * - Regular whitespace characters
 * - Newlines (generating NEWLINE lexems and handling indentation)
 * - Single-line comments (// style)
 * - Multi-line comments (cpp style)
 * 
 * When a newline is encountered, it:
 * 1. Creates a NEWLINE lexem
 * 2. Increments the line counter
 * 3. Processes any indentation following the newline
 * 
 * Comments are completely skipped without generating lexems.
 * 
 * @note The method modifies the internal state by advancing the character pointer
 *       and potentially adding new lexems to the lexem collection
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
 * @brief Processes indentation changes in the source code and generates corresponding INDENT/DEDENT lexems.
 * 
 * This method compares the current indentation level with the previous one and:
 * - Creates an INDENT lexem when indentation increases
 * - Creates DEDENT lexems when indentation decreases
 * - Maintains a stack of indentation levels
 * 
 * @param currentIndent The number of spaces/tabs in the current line's indentation
 * @throws std::runtime_error If there's no matching indentation level or inconsistent indentation
 * 
 * @note The method expects indentStack_ to never be empty (should contain at least the base level 0)
 * @note Lexems are added to the lexems_ vector with appropriate position information
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
 * @brief Performs lexical analysis of the program text.
 * 
 * This method drives the lexical analysis process by:
 * 1. Getting the first character
 * 2. Analyzing the program text
 * 3. Adding an end-of-code (EOC) lexem at the end
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
 * @brief Analyzes the entire program by processing statements sequentially
 * 
 * This method iterates through the input program character by character,
 * skipping whitespace and analyzing individual statements until the end
 * of input (null character) is reached.
 * 
 * The analysis is performed by:
 * 1. Skipping any whitespace characters
 * 2. Breaking if end of input is reached
 * 3. Processing each statement using AnalyzeStatement()
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
 * @brief Analyzes and processes a single statement in the source code.
 * 
 * This method is responsible for analyzing different types of statements including:
 * - Variable declarations (int, float, bool, string)
 * - Function declarations
 * - Control flow statements (if, elif, else)
 * - Loop statements (for, while) 
 * - Print statements
 * - Return statements
 * - Assignments
 * - Expressions
 * 
 * The analysis process includes:
 * 1. Skipping whitespace
 * 2. Identifying keywords and identifiers
 * 3. Processing the statement based on its type
 * 4. Generating appropriate lexems
 * 
 * The method first checks if the current character is alphabetic to determine if it's
 * a keyword or identifier. If not, it treats the statement as an expression.
 * For keywords, it routes to specific analysis methods based on the keyword type.
 * For identifiers, it checks for function calls or assignments.
 * 
 * @note This is a core method of the lexical analyzer that handles the primary 
 *       statement processing logic
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
        GetNextChar();
        AnalyzeVariableDeclaration();
      }
      if (word == "def") {
        GetNextChar();
        AnalyzeFunctionDeclaration();
      }
      if (word == "if") {
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
      lexems_.emplace_back(Lexem(LexemType::IDENTIFIER, word, startPos,
                                 currentPosition_, curLine_));
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

/**
 * @brief Analyzes assignment operation in the input stream
 * 
 * This method processes the assignment operator ('=') and its right-hand expression.
 * It performs the following steps:
 * 1. Skips any leading whitespace
 * 2. If an equals sign is found:
 *    - Creates a new OPERATOR lexem for the '=' symbol
 *    - Advances to the next character
 *    - Analyzes the expression on the right side of the assignment
 * 
 * The method assumes that the current position is at the potential assignment operator.
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
 * @brief Prints all lexemes stored in the analyzer to standard output
 * 
 * Outputs each lexeme's type, text value, and line number in the format:
 * "TYPE: <type> VALUE: <value> On line: <line_number>"
 * 
 * This method is const and does not modify the internal state of the analyzer.
 */
void LexemAnalyzer::PrintLexems() const {
  for (const auto& lexem : lexems_) {
    std::cout << "TYPE: " << lexem.get_type() << " VALUE: " << lexem.get_text()
              << " On line: " << lexem.get_line() << std::endl;
  }
}

/**
 * @brief Analyzes variable declarations in the source code.
 * 
 * This method processes variable declarations following this pattern:
 * identifier [= expression];
 * 
 * The method:
 * 1. Skips leading whitespace
 * 2. Processes an identifier if it starts with a letter
 * 3. Handles optional assignment (=) followed by an expression
 * 4. Processes semicolon terminator
 * 5. Continues analyzing the next statement
 * 
 * Adds corresponding lexems to the lexems_ collection:
 * - Identifiers
 * - Assignment operator (=) if present
 * - Expression components if assignment exists
 * - Semicolon operator (;)
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
 * @brief Analyzes and processes an identifier or keyword token in the source code
 * 
 * This method reads characters from the current position as long as they form a valid
 * identifier (alphanumeric characters or underscore). It then checks if the formed word
 * is a keyword. Based on this check, it creates either a KEYWORD or IDENTIFIER lexem
 * and adds it to the lexems collection.
 * 
 * The method automatically advances the current position to the first character after
 * the identifier.
 * 
 * @note Assumes the current character (ch_) is valid start of an identifier
 * @see keywords_
 * @see lexems_
 */
void LexemAnalyzer::AnalyzeIdentifier() {
  size_t startPos = currentPosition_;
  std::string word;
  while (isalnum(ch_) || ch_ == '_') {
    word += ch_;
    GetNextChar();
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

/**
 * @brief Analyzes array declaration in the source code and generates corresponding lexems.
 * 
 * This method processes array declarations enclosed in curly braces, handling the following:
 * - Opening brace '{'
 * - Array elements (expressions) separated by commas
 * - Closing brace '}'
 * 
 * The method generates lexems for:
 * - Opening and closing braces (LexemType::BRACKET)
 * - Commas between elements (LexemType::OPERATOR)
 * - Array element expressions (via AnalyzeExpression())
 * 
 * @note Method advances the character pointer (ch_) and updates current position
 * @note Invalid array declarations may result in early return
 */
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
 * @brief Analyzes function declaration in the source code.
 * 
 * This method processes a function declaration by performing the following steps:
 * 1. Skips leading whitespace
 * 2. Analyzes the function identifier (name)
 * 3. Processes the opening parenthesis
 * 4. Analyzes function parameters if present
 * 
 * The method creates lexems for function identifier and opening bracket,
 * and delegates parameter analysis to AnalyzeParameters method.
 */
void LexemAnalyzer::AnalyzeFunctionDeclaration() {
  SkipWhitespace();
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
 * @brief Analyzes function parameters in the input source code.
 * 
 * This method processes function parameters within parentheses, tokenizing them into lexems.
 * It handles parameter lists of the following form: (param1, param2, ..., paramN)
 * 
 * The method:
 * - Processes closing parenthesis if it's the first character
 * - Analyzes comma-separated identifiers as parameters
 * - Creates lexems for parameters, commas, and closing parenthesis
 * - Advances the character pointer through the input
 * 
 * @note Assumes opening parenthesis has already been processed
 * @note Calls AnalyzeIdentifier() for each parameter
 * @note Stops on encountering EOF ('\0') or closing parenthesis
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
 * @brief Analyzes an 'if' statement in the source code and generates corresponding lexems
 * 
 * The function processes the conditional part of an if statement by:
 * 1. Moving past the 'if' keyword
 * 2. Handling any whitespace
 * 3. Processing the opening parenthesis
 * 4. Analyzing the condition expression
 * 
 * When an opening parenthesis is found, it creates a BRACKET lexem and proceeds
 * to analyze the expression within the if statement's condition.
 */
void LexemAnalyzer::AnalyzeIfStatement() {
  GetNextChar();
  SkipWhitespace();
  if (ch_ == '(') {
    lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", currentPosition_,
                               currentPosition_ + 1, curLine_));
    GetNextChar();
    AnalyzeExpression();
  }
}

/**
 * @brief Analyzes an else statement in the source code.
 * 
 * Processes the else branch of a conditional statement. After encountering the 'else' keyword,
 * it checks for a colon ':' delimiter and creates corresponding lexems. If found, proceeds
 * to analyze the statement following the else clause.
 * 
 * The function expects to be called after an 'else' keyword has been identified.
 * It advances the character pointer, skips any whitespace, and processes the statement structure.
 * 
 * @note This function modifies the internal state of the lexer by advancing the character pointer
 * and potentially adding new lexems to the lexem collection.
 */
void LexemAnalyzer::AnalyzeElseStatement() {
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
 * This function processes a 'for' loop statement by:
 * 1. Skipping leading whitespace
 * 2. Analyzing the loop variable identifier
 * 3. Checking for the 'in' keyword
 * 4. Processing the array declaration if 'in' keyword is found
 * 
 * Expected format: "for identifier in array"
 * 
 * @throws May throw exceptions from AnalyzeIdentifier() or AnalyzeArrayDeclaration()
 */
void LexemAnalyzer::AnalyzeForStatement() {
  SkipWhitespace();
  AnalyzeIdentifier();
  SkipWhitespace();
  std::string temp = "";
  for (int i = 0; i < 2; i++) {
    temp += ch_;
    GetNextChar();
  }
  if (temp.compare("in") == 0) {
    lexems_.emplace_back(Lexem(LexemType::KEYWORD, "in", currentPosition_ - 2,
                               currentPosition_, curLine_));
    GetNextChar();
    AnalyzeArrayDeclaration();
  }
}

/**
 * @brief Analyzes a 'while' statement in the source code
 * 
 * This method processes the syntax of a while loop statement. It first skips any whitespace,
 * then checks for and processes the opening parenthesis. After finding the opening parenthesis,
 * it creates a corresponding BRACKET lexem and proceeds to analyze the condition expression
 * inside the while statement.
 * 
 * The method expects to be called when the 'while' keyword has already been processed.
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
 * @brief Analyzes a print statement in the source code.
 * 
 * This method handles the parsing of print statements, which must follow the format:
 * print(expression). It processes the opening parenthesis and delegates the
 * expression analysis to AnalyzeExpression().
 * 
 * The method:
 * 1. Skips any leading whitespace
 * 2. Checks for and processes the opening parenthesis
 * 3. Creates a BRACKET lexem for the opening parenthesis
 * 4. Advances to the next character
 * 5. Analyzes the expression inside the print statement
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
 * @brief Analyzes a return statement in the source code.
 * 
 * Processes a return statement by checking for a semicolon terminator.
 * If a semicolon is found, creates a new operator lexem and continues
 * statement analysis.
 * 
 * @note This function assumes that 'return' keyword has already been processed
 * @note Uses internal state variables: ch_, lexems_, currentPosition_, curLine_
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