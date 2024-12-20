#ifndef LEXEM_ANALYZER_H
#define LEXEM_ANALYZER_H

#include <string>
#include <vector>
#include "Bor.h"
#include "Lexem.h"

/**
 * @class LexemAnalyzer
 * @brief A lexical analyzer that breaks source code into lexical tokens (lexemes)
 *
 * The LexemAnalyzer class is responsible for performing lexical analysis on source code,
 * breaking it down into meaningful tokens called lexemes. It handles various language
 * constructs including variables, functions, control structures, and expressions.
 *
 * @details The analyzer supports:
 * - Variable and function declarations
 * - Control structures (if, for, while)
 * - Expressions and assignments
 * - Print and return statements
 * - Array declarations
 * - String and number literals
 * - Indentation-based scope management
 *
 * @param code The source code to be analyzed
 * @param pathToKeywords Path to the file containing language keywords
 */
/**
 * @file LexemAnalyzer.h
 * @brief Header file containing the LexemAnalyzer class for lexical analysis of code
 *
 * The LexemAnalyzer class provides functionality to break down source code into lexical tokens (lexemes).
 * It performs lexical analysis on input code by identifying and categorizing different programming language 
 * constructs like keywords, identifiers, literals, operators etc.
 *
 * Key features:
 * - Analyzes program structure and statements
 * - Handles variable and function declarations
 * - Processes control flow statements (if, for, while)
 * - Manages expressions and basic operations
 * - Tracks code indentation
 *
 * @author Anton Zykov
 */
class LexemAnalyzer {
 public:
  LexemAnalyzer(const std::string& code, const std::string& pathToKeywords);
  void Analyze();
  void PrintLexems() const;

  size_t GetCurrentPosition() const { return currentPosition_; }

  std::vector<Lexem> GetLexems() { return lexems_; }

 private:
  // Parsing func
  void AnalyzeProgram();
  void AnalyzeStatement();
  void AnalyzeVariableDeclaration();
  void AnalyzeFunctionDeclaration();
  void AnalyzeParameters();
  void AnalyzeElseStatement();
  void AnalyzeAssignment();
  void AnalyzeIfStatement();
  void AnalyzeForStatement();
  void AnalyzeWhileStatement();
  void AnalyzePrintStatement();
  void AnalyzeReturnStatement();
  void AnalyzeArrayDeclaration();
  void AnalyzeExpression();
  void AnalyzeIdentifier();
  void AnalyzeNumber();
  void AnalyzeString();
  void AnalyzeTerm();
  void AnalyzeFactor();

  // Util
  void GetNextChar();
  void SkipWhitespace();
  void HandleIndentation(int previousIndentation);

  // var
  char ch_;
  std::string code_;
  std::vector<Lexem> lexems_;
  std::vector<int> indentStack_;
  size_t index_;
  size_t currentPosition_;
  bor keywords_;
  size_t curLine_ = 0;
};

#endif  // LEXEM_ANALYZER_H