#include "../include/LexemAnalyzer.h"
#include "../include/Lexem.h"  // Include the header for Lexem
#include <cctype>
#include <stdexcept>
#include <iostream>
#include <stack>
#include <fstream>

/**
 * @brief Constructs a LexemAnalyzer object.
 *
 * Initializes the analyzer with the provided source code and loads keywords from the specified file.
 *
 * @param code The source code to be analyzed.
 * @param pathToKeywords The file path to the keywords list.
 * @throws std::runtime_error If the keywords file cannot be opened.
 */
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

/**
 * @brief Retrieves the next character from the code.
 *
 * This function increments the current index and updates the current character `ch_`.
 * If the current character is a newline, it increments the `curLine_` counter.
 * It also updates the `currentPosition_` to reflect the new index.
 * If the end of the code is reached, it sets `ch_` to the null character.
 */
void LexemAnalyzer::GetNextChar() {
    if (index_ < code_.length()) {
        if (ch_ == '\n') {
            curLine_++;
        }
        ch_ = code_[index_++];
        currentPosition_ = index_;
        // Debugging output
        std::cout << "GetNextChar: ch_='" << ch_ << "', index_=" << index_ << ", curLine_=" << curLine_ << std::endl;
    } else {
        ch_ = '\0';
        // Debugging output
        std::cout << "GetNextChar: End of file reached." << std::endl;
    }
}

/**
 * @brief Skips whitespace and comments in the source code.
 * 
 * This function advances the current character position by skipping over
 * any whitespace characters, single-line comments (`//`), and multi-line
 * comments. It also handles newlines by updating the line count
 * and managing indentation levels.
 */
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
 * @brief Handles the indentation levels based on the current indentation.
 * 
 * This function compares the current indentation level with the previous one
 * and updates the indentation stack accordingly. It adds INDENT or DEDENT
 * tokens based on whether the current indentation is greater or less than
 * the previous level. Throws an error if indentation levels are inconsistent.
 * 
 * @param currentIndent The current indentation level to handle.
 * @throws std::runtime_error if indentation is inconsistent or mismatched.
 */
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

/**
 * @brief Performs lexical analysis of the source code.
 * 
 * This method executes the main lexical analysis process by:
 * 1. Getting the first character from the source
 * 2. Analyzing the program structure
 * 3. Adding an end-of-code (EOC) lexeme at the end
 * 
 * The analysis results are stored in the lexems_ container.
 */
void LexemAnalyzer::Analyze() {
    GetNextChar();
    AnalyzeProgram();
    lexems_.emplace_back(Lexem(LexemType::EOC, std::string("eof"), index_, index_+1, curLine_));
}

/**
 * @brief Analyzes the entire program by processing statements sequentially.
 * 
 * This method iterates through the program text character by character,
 * skipping whitespace and analyzing each statement until the end of input is reached.
 * The analysis stops when the null terminator character '\0' is encountered.
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
 * This function processes statements by:
 * 1. Identifying keywords, identifiers and expressions
 * 2. Handling various statement types including:
 *    - Variable declarations (int, float, bool, string)
 *    - Function declarations
 *    - Control flow statements (if, elif, else, for, while)
 *    - Print statements
 *    - Return statements
 *    - Assignments
 *    - Function calls
 *    - General expressions
 * 
 * The function first skips whitespace and checks for end of input.
 * Then it processes alphabetic characters to form words that could be:
 * - Keywords (triggering specific analysis routines)
 * - Identifiers (possibly followed by function calls or assignments)
 * If no word is formed, it processes the input as a general expression.
 * 
 * @note The function advances the internal character pointer and populates
 *       the lexems_ vector with identified lexical tokens
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
        if (word.empty()) {
            GetNextChar();
            SkipWhitespace();
            return;
        }
        auto [isKeyword, lexem] = keywords_.has(word.c_str(), word.length(), startPos);
        if (isKeyword) {
            lexems_.emplace_back(Lexem(LexemType::KEYWORD, word, startPos, currentPosition_, curLine_));
            if (word == "int" || word == "float" || word == "bool" || word == "string") {
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
            lexems_.emplace_back(Lexem(LexemType::IDENTIFIER, word, startPos, currentPosition_, curLine_));
            SkipWhitespace();
            if (ch_ == '(') {
              lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", currentPosition_, currentPosition_ + 1, curLine_));
              AnalyzeFunctionDeclaration();
            } else if (ch_ == '=') {
                AnalyzeAssignment();
            }
            AnalyzeExpression();
        }
    } 
}

/**
 * @brief Analyzes an assignment operation within the lexem stream.
 *
 * This method skips any leading whitespace, checks for the assignment operator '='.
 * If the '=' operator is found, it adds it as a lexem and proceeds to analyze the
 * subsequent expression.
 */
void LexemAnalyzer::AnalyzeAssignment() {
    SkipWhitespace();
    if (ch_ == '=') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, "=", currentPosition_, currentPosition_ + 1, curLine_));
        GetNextChar();
        AnalyzeExpression();
    }
}

/**
 * @brief Prints all lexemes in the analyzer to standard output
 * 
 * For each lexeme, displays:
 * - Its type
 * - Its text value
 * - The line number where it appears
 * 
 * The output format is: "TYPE: <type> VALUE: <value> On line: <line_number>"
 */
void LexemAnalyzer::PrintLexems() const {
    for (const auto& lexem : lexems_) {
        std::cout << "TYPE: " << lexem.get_type() << " VALUE: " << lexem.get_text() << " On line: " << lexem.get_line() << std::endl;
    }
}

/**
 * @brief Analyzes variable declaration statements in the source code.
 * 
 * This method handles the analysis of variable declarations, including:
 * - Skipping leading whitespace
 * - Processing identifiers (variable names)
 * - Handling assignment operations if present
 * - Processing expressions following assignments
 * - Detecting statement terminators (semicolons)
 * 
 * The method expects variable declarations to follow the pattern:
 * identifier [= expression];
 * 
 * During analysis, it generates corresponding lexems and adds them to the lexems_ collection.
 * After processing a complete declaration, it proceeds to analyze the next statement.
 */
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

/**
 * @brief Analyzes and processes an identifier or keyword from the input source.
 * 
 * This method reads characters from the current position until it encounters a character
 * that is not alphanumeric and not an underscore. It then checks if the accumulated
 * string is a keyword or an identifier and adds the appropriate lexem to the lexems list.
 * 
 * The method performs the following steps:
 * 1. Accumulates characters while they are alphanumeric or underscore
 * 2. If no characters were accumulated, skips whitespace and returns
 * 3. Checks if the accumulated word is a keyword
 * 4. Creates either a KEYWORD or IDENTIFIER lexem and adds it to the lexems list
 * 
 * @note The method advances the current position pointer as it reads characters
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
    auto [isKeyword, lexem] = keywords_.has(word.c_str(), word.length(), startPos);
    if (isKeyword) {
        lexems_.emplace_back(Lexem(LexemType::KEYWORD, word, startPos, currentPosition_, curLine_));
    } else {
        lexems_.emplace_back(Lexem(LexemType::IDENTIFIER, word, startPos, currentPosition_, curLine_));
    }
}

/**
 * @brief Analyzes and tokenizes an expression in the input text.
 * 
 * This method processes characters until it encounters a semicolon, newline, or end of file.
 * It identifies and creates lexems for:
 * - Numbers
 * - Identifiers
 * - String literals
 * - Brackets (parentheses, curly braces, square brackets)
 * - Operators and punctuation marks
 * 
 * If a semicolon is encountered first, it creates a semicolon lexem and continues
 * with statement analysis.
 * 
 * Whitespace is automatically skipped during processing.
 */
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

/**
 * @brief Analyzes and processes a numeric literal in the input stream
 * 
 * This method processes both integer and floating-point numbers.
 * It accumulates digits until a non-digit character is encountered.
 * If a decimal point is found after the integer part, it continues
 * to process decimal digits for floating-point numbers.
 * 
 * The method creates a NUMBER lexem containing the complete numeric literal
 * and adds it to the lexems collection.
 * 
 * @details The number format supported is: [0-9]+(.[0-9]+)?
 * 
 * @note The method assumes the current character (ch_) is a digit when called
 * @note Updates the current position (currentPosition_) as it processes characters
 */
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

/**
 * @brief Analyzes and processes a string literal encountered in the source code.
 * 
 * This method handles string literals enclosed in double quotes. It reads characters
 * until it encounters the closing double quote, building the string content.
 * After processing, it creates a STRING lexeme with the accumulated characters.
 * 
 * The method assumes that the current character (ch_) is positioned at the opening
 * double quote when called. It advances past the closing quote upon completion.
 * 
 * @note The method does not handle escape sequences or malformed strings.
 */
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

/**
 * @brief Analyzes array declaration syntax in the form of {expr1, expr2, ..., exprN}
 * 
 * Processes array declarations by:
 * 1. Adding opening brace lexem
 * 2. Analyzing comma-separated expressions inside braces
 * 3. Adding comma lexems between expressions
 * 4. Adding closing brace lexem
 * 
 * If syntax is invalid (missing closing brace or invalid expression separator),
 * the analysis stops and returns early.
 * 
 * @note Position tracking and whitespace handling is managed internally
 */
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

/**
 * @brief Analyzes function declaration in the source code.
 * 
 * This method processes function declarations by performing the following steps:
 * 1. Analyzes the function identifier (name)
 * 2. Skips any whitespace
 * 3. If an opening parenthesis is found, creates a bracket lexem
 * 4. Processes function parameters
 * 
 * The method expects the current position to be at the start of a function declaration
 * and handles the parsing up until the parameter list.
 */
void LexemAnalyzer::AnalyzeFunctionDeclaration() {
    AnalyzeIdentifier();
    SkipWhitespace();
    if (ch_ == '(') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", currentPosition_, currentPosition_ + 1, curLine_));
        GetNextChar();
        AnalyzeParameters();
    }
}

/**
 * @brief Analyzes function parameters in the input source code.
 * 
 * This method processes function parameters between parentheses, handling:
 * - Empty parameter lists
 * - Single parameters
 * - Multiple parameters separated by commas
 * 
 * The method generates lexems for:
 * - Parameter identifiers
 * - Comma separators
 * - Closing parenthesis
 * 
 * If a syntax error is encountered, the method returns early.
 */
void LexemAnalyzer::AnalyzeParameters() {
    SkipWhitespace();
    if (ch_ == ')') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, ")", currentPosition_, currentPosition_ + 1, curLine_));
        GetNextChar();
        AnalyzeStatement();
    }
    while (ch_ != ')' && ch_ != '\0') {
        AnalyzeIdentifier();
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

/**
 * @brief Analyzes an if statement in the source code.
 * 
 * This method processes an if statement by:
 * 1. Moving to the next character after 'if'
 * 2. Skipping any whitespace
 * 3. If an opening parenthesis is found:
 *    - Adds a BRACKET lexem to the lexem list
 *    - Processes the condition expression inside the parentheses
 * 
 * @pre Current character position should be at 'if' keyword
 * @post Current character position will be at the end of the condition expression
 */
void LexemAnalyzer::AnalyzeIfStatement() {
    GetNextChar();
    SkipWhitespace();
    if (ch_ == '(') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", currentPosition_, currentPosition_ + 1, curLine_));
        GetNextChar();
        AnalyzeExpression();
    }
}

/**
 * @brief Analyzes the 'else' statement in the source code
 * 
 * Processes the 'else' clause by:
 * 1. Moving to the next character after 'else'
 * 2. Skipping any whitespace
 * 3. If a colon is found:
 *    - Creates a colon operator lexem
 *    - Processes the subsequent statement
 * 
 * Expected syntax: else: <statement>
 */
void LexemAnalyzer::AnalyzeElseStatement() {
    GetNextChar();
    SkipWhitespace();
    if (ch_ == ':') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, ":", currentPosition_, currentPosition_ + 1, curLine_));
        GetNextChar();
        AnalyzeStatement();
    }
}

/**
 * @brief Analyzes a 'for' statement in the source code
 * 
 * This method processes a for statement by following these steps:
 * 1. Skips leading whitespace
 * 2. Analyzes the identifier (loop variable)
 * 3. Checks for the 'in' keyword
 * 4. If 'in' is found, creates a keyword lexem and processes the array declaration
 * 
 * Expected format: for identifier in array
 * 
 * @throws May throw exceptions if the format is invalid or unexpected characters are encountered
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
        lexems_.emplace_back(Lexem(LexemType::KEYWORD, "in", currentPosition_ - 2, currentPosition_, curLine_));
        GetNextChar();
        AnalyzeArrayDeclaration();
    }
}

/**
 * @brief Analyzes a while statement in the source code.
 * 
 * This method processes the beginning of a while statement by:
 * 1. Skipping any leading whitespace
 * 2. Handling the opening parenthesis '('
 * 3. Creating a BRACKET lexem for the opening parenthesis
 * 4. Analyzing the condition expression inside the while statement
 * 
 * @note The method expects to be called when a 'while' keyword has been identified
 * @note The method processes only the opening part of the while statement up to the condition
 */
void LexemAnalyzer::AnalyzeWhileStatement() {
    SkipWhitespace();
    if (ch_ == '(') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", currentPosition_, currentPosition_ + 1, curLine_));
        GetNextChar();
        AnalyzeExpression();
    }
}

/**
 * @brief Analyzes a print statement in the source code.
 * 
 * This method handles the parsing of print statements, which have the following syntax:
 * print(expression)
 * 
 * The method skips any leading whitespace, validates the opening parenthesis,
 * creates a BRACKET lexem, and proceeds to analyze the expression within the print statement.
 * 
 * @note This method assumes that 'print' keyword has already been processed
 * @see AnalyzeExpression()
 */
void LexemAnalyzer::AnalyzePrintStatement() {
    SkipWhitespace();
    if (ch_ == '(') {
        lexems_.emplace_back(Lexem(LexemType::BRACKET, "(", currentPosition_, currentPosition_ + 1, curLine_));
        GetNextChar();
        AnalyzeExpression();
    }
}

/**
 * @brief Analyzes a return statement in the source code
 * 
 * Processes a return statement by skipping whitespace and checking for semicolon.
 * If semicolon is found, creates a new OPERATOR lexem and continues analysis
 * of the next statement.
 */
void LexemAnalyzer::AnalyzeReturnStatement() {
    SkipWhitespace();
    if (ch_ == ';') {
        lexems_.emplace_back(Lexem(LexemType::OPERATOR, ";", currentPosition_, currentPosition_ + 1, curLine_));
        GetNextChar();
        AnalyzeStatement();
    }
}