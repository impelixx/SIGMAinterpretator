#include <iostream>
#include <fstream>
#include <sstream>
#include "LexemAnalyzer.h"
#include <SyntaxAnalyzer.h>
#include <Semantic.h>

int main() {
    try {
        std::ifstream file("../test/code.us");

        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file '../test/code.us'");
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string code = buffer.str();

        LexemAnalyzer analyzer(code);

        try {
            analyzer.Analyze();
            std::vector<Lexem> lexems = analyzer.GetLexems();
            try {
                Semantic semantic(lexems);
                semantic.Analyze();
            } catch (const std::exception& e) {
                std::cout << "============================================" << std::endl;
                std::cerr << "error: " << e.what() << std::endl;
                return 3;
            }
        } catch (const std::exception& e) {
            std::cout << "============================================" << std::endl;
            std::cerr << "error: " << e.what() << std::endl;
            std::cerr << "At line: " << analyzer.GetCurrentPosition() << std::endl;
            // std::cerr << "At position: " << analyzer.GetCurrentPosition() << std::endl;
            return 2;
        }

        std::cout << "============================================" << std::endl;
        std::cout << "No errors were found, the program is correct" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}