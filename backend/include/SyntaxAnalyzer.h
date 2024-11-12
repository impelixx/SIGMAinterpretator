#ifndef BACKEND_SYNTAXANALYZER_H
#define BACKEND_SYNTAXANALYZER_H
#include <vector>
#include "Lexem.h"

class SyntaxAnalyzer {
public:
    SyntaxAnalyzer(std::vector<Lexem> lexems) {lex = lexems;};
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
    bool AnalyzePrintStatement();
    bool AnalyzeReturnStatement();
    bool AnalyzeExpression();
    bool AnalyzeTerm();
    bool AnalyzeFactor();
    bool AnalyzeType();
    bool AnalyzeIdentifier();
    bool AnalyzeNumber();
    bool AnalyzeStatementTerminator();
    int line = 0;
    int LexemIndex = 0;
    std::vector<Lexem> lex;
};


#endif //BACKEND_SYNTAXANALYZER_H
