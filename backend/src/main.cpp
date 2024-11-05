#include <iostream>
#include "LexemAnalyzer.h"
#include <fstream>
#include <sstream>

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
            analyzer.PrintLexems();
        } catch (const std::exception& e) {
            std::cerr << "LexemAnalyzer error: " << e.what() << std::endl;
            std::cerr << "At position: " << analyzer.GetCurrentPosition() << std::endl;
            return 2;
        }
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}