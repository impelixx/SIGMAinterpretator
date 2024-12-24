#ifndef BACKEND_SYNTAXANALYZER_H
#define BACKEND_SYNTAXANALYZER_H
#include <vector>
#include "Lexem.h"

/**
 * @brief A syntax analyzer for parsing and validating program syntax
 * 
 * This class implements a recursive descent parser that analyzes a sequence of lexemes
 * to verify syntactic correctness according to the language grammar rules.
 * 
 * @param lexems Vector of lexemes to be analyzed
 */
/**
 * @brief A class responsible for performing syntax analysis on a sequence of lexemes
 * 
 * The SyntaxAnalyzer class takes a vector of lexemes as input and performs recursive descent parsing
 * to validate the syntactic structure of the program according to the language grammar rules.
 * 
 * @class SyntaxAnalyzer
 * 
 * @details The analyzer includes methods for parsing different language constructs including:
 * - Program structure
 * - Variable declarations
 * - Function declarations
 * - Control flow statements (if, while, for)
 * - Print statements
 * - Return statements
 * - Expressions and assignments
 * 
 * The class maintains internal state tracking current lexeme position during analysis.
 * 
 * @param lexems Vector of lexemes to be analyzed, passed to constructor
 * 
 * @throws May throw syntax errors if invalid program structure is detected
 */
class SyntaxAnalyzer {
 public:
  SyntaxAnalyzer(std::vector<Lexem>& lexems)
      : lex(lexems), curLex_(lexems[0]) {}

  void Analyze();

 private:
  std::vector<Lexem> lex;
  Lexem curLex_;
  int index = 0;

  void GetLexem();
  void AnalyzeProgram();
  void AnalyzeStatement();
  void AnalyzeVariableDeclaration();
  void AnalyzeFunctionDeclaration();
  void AnalyzeParameterList();
  void AnalyzeIfStatement();
  void AnalyzeWhileStatement();
  void AnalyzeForStatement();
  void AnalyzePrintStatement();
  void AnalyzeElseStatement();
  void AnalyzeReturnStatement();
  bool IsType(const std::string& word);
  void AnalyzeAssignment();
  void AnalyzeExpression();
  void AnalyzeStatementTerminator();
  void AnalyzeBlock();
};

#endif  //BACKEND_SYNTAXANALYZER_H
