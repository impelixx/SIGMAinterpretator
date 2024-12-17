#include <Semantic.h>
#include <SyntaxAnalyzer.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include "LexemAnalyzer.h"

/**
 * @brief Entry point of the program.
 *
 * This function handles command-line arguments to determine the paths to the code file and the workwords file.
 * If no arguments are provided, it uses default test files. It then reads the code file, performs lexical analysis,
 * and then semantic analysis on the code. Errors encountered during these processes are caught and reported.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line arguments.
 *             - `argv[0]` is the program name.
 *             - `argv[1]` (optional) is the path to the code file.
 *             - `argv[2]` (optional) is the path to the workwords file.
 * @return An integer exit code:
 *         - `0` on successful execution.
 *         - `1` on usage error or lexical analysis error.
 *         - `2` on semantic analysis error.
 *         - `3` on other errors.
 */
int main(int argc, char* argv[]) {
  if (argc < 3) {
    if (argc != 1) {
      std::cerr << "Usage: " << argv[0]
                << " <path to code file> <path to workwords file>" << std::endl;
      return 1;
    }
  }

  std::string code = "";
  std::string workwords = "";

  try {
    std::ifstream codeFile;
    if (argc != 1) {
      codeFile = std::ifstream(std::string(argv[1]));
    } else {
      codeFile = std::ifstream("../test/code.us");
    }
    if (!codeFile.is_open()) {
      throw std::runtime_error(std::string("Failed to open code file '") +
                               argv[1] + "'");
    }
    std::stringstream codeBuffer;
    codeBuffer << codeFile.rdbuf();
    code = codeBuffer.str();
    std::ifstream workwordFile;
    if (argc != 1) {
      workwords = std::string(argv[2]);
    } else {
      workwords = "../test/workword";
    }
    LexemAnalyzer analyzer(code, workwords);

    try {
      analyzer.Analyze();
      std::vector<Lexem> lexems = analyzer.GetLexems();
      analyzer.PrintLexems();
      try {
        Semantic semantic(lexems);
        semantic.Analyze();
      } catch (const std::exception& e) {
        std::cerr << "Semantic analysis error: " << e.what() << std::endl;
        return 2;
      }
    } catch (const std::exception& e) {
      std::cerr << "Lexical analysis error: " << e.what() << std::endl;
      return 1;
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 3;
  }

  return 0;
}