#ifndef LEXEM_ANALYZER_H
#define LEXEM_ANALYZER_H

#include "Lexem.h"
#include <string>
#include <vector>

class LexemAnalyzer {
public:
    LexemAnalyzer(const std::string& code);
    void Analyze();
    void PrintLexems() const;
    size_t GetCurrentPosition() const { return currentPosition_; }

private:
    // Parsing func
    void AnalyzeProgram();
    void AnalyzeStatement();
    void AnalyzeVariableDeclaration();
    void AnalyzeFunctionDeclaration();
    void AnalyzeParameters();
    void AnalyzeAssignment();
    void AnalyzeIfStatement();
    void AnalyzeWhileStatement();
    void AnalyzePrintStatement();
    void AnalyzeReturnStatement();
    void AnalyzeBlock();
    void AnalyzeExpression();
    void AnalyzeIdentifier();
    void AnalyzeNumber();
    void AnalyzeType();

    // Util
    void GetNextChar();
    void SkipWhitespace();
    void HandleIndentation(int previousIndentation);

    // var
    std::string code_;
    std::vector<Lexem> lexems_;
    char ch_;
    size_t index_;
    size_t currentPosition_;
    std::vector<int> indentStack_;
};

#endif // LEXEM_ANALYZER_H