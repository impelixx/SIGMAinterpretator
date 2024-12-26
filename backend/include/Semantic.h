#pragma once

#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include "Lexem.h"

/**
 * @class SemanticAnalyzer
 * @brief Analyzes the semantics of a given set of lexems.
 *
 * The SemanticAnalyzer class is responsible for performing semantic analysis
 * on a vector of lexems. It checks for correct usage of variables, functions,
 * and other language constructs within the provided lexems.
 *
 * @var curLex_ The current lexem being analyzed.
 * @var index The current index in the lexem vector.
 * @var lexems_ The vector of lexems to be analyzed.
 * @var scopeStack_ A stack of scopes, each represented by a map of variable names to their types.
 * @var functionSignatures_ A map of function names to their parameter types.
 *
 * @fn SemanticAnalyzer(std::vector<Lexem>& lexems)
 * @brief Constructs a SemanticAnalyzer with the given vector of lexems.
 * @param lexems A vector of lexems to be analyzed.
 *
 * @fn void Analyze()
 * @brief Starts the semantic analysis process.
 *
 * @fn void PrintFunction()
 * @brief Prints the function signatures.
 *
 * @fn void GetLexem()
 * @brief Retrieves the next lexem from the vector.
 *
 * @fn void CheckFunctionCall(const std::string& funcName)
 * @brief Checks the validity of a function call.
 * @param funcName The name of the function being called.
 *
 * @fn void AnalyzeProgram()
 * @brief Analyzes the entire program.
 *
 * @fn void AnalyzeStatement()
 * @brief Analyzes a single statement.
 *
 * @fn void AnalyzeFunction()
 * @brief Analyzes a function definition.
 *
 * @fn void AnalyzeVariableDeclaration()
 * @brief Analyzes a variable declaration.
 *
 * @fn void AnalyzeExpression()
 * @brief Analyzes an expression.
 *
 * @fn void AnalyzePrint()
 * @brief Analyzes a print statement.
 *
 * @fn void AnalyzeWhile()
 * @brief Analyzes a while loop.
 *
 * @fn void AnalyzeIf()
 * @brief Analyzes an if statement.
 *
 * @fn void AnalyzeFor()
 * @brief Analyzes a for loop.
 *
 * @fn void EnterScope()
 * @brief Enters a new scope.
 *
 * @fn void ExitScope()
 * @brief Exits the current scope.
 *
 * @fn bool IsVarDefined(const std::string &var)
 * @brief Checks if a variable is defined in the current scope.
 * @param var The name of the variable.
 * @return True if the variable is defined, false otherwise.
 *
 * @fn std::string GetVarType(const std::string &var)
 * @brief Gets the type of a variable.
 * @param var The name of the variable.
 * @return The type of the variable.
 *
 * @fn void AddVariable(const std::string &var, const std::string &type)
 * @brief Adds a variable to the current scope.
 * @param var The name of the variable.
 * @param type The type of the variable.
 */
class SemanticAnalyzer {
public:
    SemanticAnalyzer(std::vector<Lexem>& lexems);
    void Analyze();
    void PrintFunction();

private:
    // vars
    Lexem curLex_;
    size_t index = 0;
    std::vector<Lexem> lexems_;
    std::vector<std::map<std::string, std::string>> scopeStack_; 
    std::map<std::string, std::vector<std::string>> functionSignatures_;

    // main analysis functions
    void GetLexem();
    void CheckFunctionCall(const std::string& funcName);
    void AnalyzeProgram();
    void AnalyzeStatement();
    void AnalyzeFunction();
    void AnalyzeVariableDeclaration();
    void AnalyzeExpression();
    void AnalyzePrint();
    void AnalyzeWhile();
    void AnalyzeIf();
    void AnalyzeFor();

    // utility functions
    void EnterScope();
    void ExitScope();
    bool IsVarDefined(const std::string &var);
    std::string GetVarType(const std::string &var);
    void AddVariable(const std::string &var, const std::string &type);
};