#ifndef LEXEM_ANALYZER_H
#define LEXEM_ANALYZER_H

#include "Lexem.h"
#include <string>
#include <vector>
#include "Bor.h"


class LexemAnalyzer {
public:
    LexemAnalyzer(const std::string& code);
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
    void AnalyzeWhileStatement();
    void AnalyzePrintStatement();
    void AnalyzeReturnStatement();
    void AnalyzeArrayDeclaration();
    void AnalyzeExpression();
    void AnalyzeIdentifier();
    void AnalyzeNumber();

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
};

#endif // LEXEM_ANALYZER_H