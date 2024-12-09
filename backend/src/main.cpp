#include <iostream>
#include <fstream>
#include <sstream>
#include "LexemAnalyzer.h"
#include <SyntaxAnalyzer.h>
#include <Semantic.h>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <workword_file>" << std::endl;
        return 1;
    }

    std::string code;
    std::string workwords;

    try {
        if (argc < 3) {
            std::ifstream codeFile("../test/code.us");
            std::ifstream workwordFile("../test/keywords");
            if (!codeFile.is_open()) {
                throw std::runtime_error("Failed to open code file '../test/code.us'");
            }
            if (!workwordFile.is_open()) {
                throw std::runtime_error("Failed to open workword file '../test/keywords'");
            }
            std::stringstream codeBuffer;
            codeBuffer << codeFile.rdbuf();
            code = codeBuffer.str();

        }
        std::ifstream codeFile(argv[1]);
        if (!codeFile.is_open()) {
            throw std::runtime_error(std::string("Failed to open code file '") + argv[1] + "'");
        }
        std::stringstream codeBuffer;
        codeBuffer << codeFile.rdbuf();
        code = codeBuffer.str();

        LexemAnalyzer analyzer(code, argv[2]);

        try {
            analyzer.Analyze();
            std::vector<Lexem> lexems = analyzer.GetLexems();
            try {
                Semantic semantic(lexems);
                semantic.Analyze();
            } catch (const std::exception& e) {
                std::cerr << "Semantic analysis error: " << e.what() << std::endl;
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Lexical analysis error: " << e.what() << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}