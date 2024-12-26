#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include "Lexem.h"

class SemanticAnalyzer {
public:
    SemanticAnalyzer(std::vector<Lexem>& lexems);

    void Analyze();    
    void PrintFunction();

private:
    Lexem curLex_;
    void GetLexem();
    size_t index = 0;
    std::vector<Lexem> lexems_;
    std::map <std::string, std::string> variables_;
    std::map<std::string, std::vector<std::string>> functionSignatures_;
    void CheckFunctionCall(const std::string& funcName,
                           const std::vector<std::string>& argTypes);
    void CheckIndentation(const std::vector<std::string>& lines);
    void AnalyzeProgram();
    void AnalyzeStatement();
    void AnalyzeFunction();
    void AnalyzeVariableDeclaration();
    void AnalyzeExpression();
    void AnalyzePrint();
};