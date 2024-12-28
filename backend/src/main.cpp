#include <Codegen.h>
#include <RPN.h>
#include <SyntaxAnalyzer.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include "LexemAnalyzer.h"
#include "Semantic.h"

/**
 * @brief Main entry point of the SIGMA interpreter program
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @return 0 on successful execution, 1 on error
 *
 * The program expects two optional command line arguments:
 * 1. Path to the code file (defaults to "../test/code.us")
 * 2. Path to the workwords file (defaults to "../test/workword")
 *
 * Program flow:
 * 1. Validates command line arguments
 * 2. Opens and reads the code file
 * 3. Performs lexical analysis using LexemAnalyzer
 * 4. Performs semantic analysis using Semantic analyzer
 *
 * @throws std::runtime_error if code file cannot be opened
 * @throws Any exceptions from LexemAnalyzer or Semantic analysis
 */
int main(int argc, char* argv[]) {
  if (argc > 1 && argc < 3) {
    std::cerr << "Use: " << argv[0]
              << " <path to code file> <path to workwords file>" << std::endl;
    return 1;
  }

  std::string codePath = (argc != 1) ? argv[1] : "../test/code.us";
  std::string workwordsPath = (argc != 1) ? argv[2] : "../test/workword";

  try {
    std::ifstream codeFile(codePath);
    if (!codeFile.is_open()) {
      throw std::runtime_error("Failed to open code file '" + codePath + "'");
    }

    std::stringstream codeBuffer;
    codeBuffer << codeFile.rdbuf();
    std::string code = codeBuffer.str();

    LexemAnalyzer lexer(code, workwordsPath);
    lexer.Analyze();
    std::vector<Lexem> lexems = lexer.GetLexems();
    SyntaxAnalyzer syntaxer(lexems);
    syntaxer.Analyze();
    SemanticAnalyzer semantic(lexems);
    semantic.Analyze();
    RPN rpn(lexems);
    rpn.buildRPN();
    rpn.printRPN();
    std::cout << "Code analysis completed successfully!" << std::endl;
    CodeGen runner(code, rpn.getRPN());
    return 0;
  } catch (const std::exception& e) {
    std::cout << "Error: " << e.what() << std::endl;
    return 1;
  }
}
