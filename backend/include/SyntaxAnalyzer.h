#ifndef BACKEND_SYNTAXANALYZER_H
#define BACKEND_SYNTAXANALYZER_H
#include <vector>
#include "Lexem.h"

class SyntaxAnalyzer {
 public:
  SyntaxAnalyzer(std::vector<Lexem> lexems) { lex = lexems; };

  std::string NumLine();
  bool Analyze();

 private:
  bool AnalyzeProgram();
  bool AnalyzeStatementList();
  bool AnalyzeStatement();
  bool AnalyzeEmptyStatement();
  bool AnalyzeFunctionDeclaration();
  bool AnalyzeParameterList();
  bool AnalyzeVariableDeclaration();
  bool AnalyzeInitianalizerList();
  bool AnalyzeAssignment();
  bool AnalyzeIfStatement();
  bool AnalyzeWhileStatement();
  bool AnalyzeDoWhileStatement();
  bool AnalyzeFieldView();
  bool AnalyzeForStatement();
  bool AnalyzePrintStatement();
  bool AnalyzeReturnStatement();
  bool AnalyzeCaseExpression();
  bool AnalyzeExpression();
  bool AnalyzeTerm();
  bool AnalyzeFactor();
  bool AnalyzeType();
  bool AnalyzeIdentifier();
  bool AnalyzeNumber();
  bool AnalyzeStatementTerminator();
  int LexemIndex = 0;
  std::vector<Lexem> lex;
};

#endif  //BACKEND_SYNTAXANALYZER_H
