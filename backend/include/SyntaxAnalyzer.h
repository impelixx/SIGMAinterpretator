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
class SyntaxAnalyzer {
 public:
  SyntaxAnalyzer(std::vector<Lexem> lexems) { lex = lexems; };

  std::string NumLine();
  bool Analyze();

 private:
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

  void GetLexem();
  std::vector<Lexem> lex;
};

#endif  //BACKEND_SYNTAXANALYZER_H
