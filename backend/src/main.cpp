#include <iostream>
#include <fstream>
#include <sstream>
#include "LexemAnalyzer.h"
#include <SyntaxAnalyzer.h>
#include <Semantic.h>

int GetLine(int index, std::vector<Lexem> lexems) {
    int line = 1;

    for (int i = 0; i < index; ++i) {
        if (lexems[i].get_type() == "NEWLINE") {
            ++line;
        }
    }

    return line;
}

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
            std::vector<Lexem> lexems = analyzer.GetLexems();
            SyntaxAnalyzer syntaxer(lexems);
            Semantic semantic(lexems);
            semantic.Analyze();
        } catch (const std::exception& e) {
            std::cout << "============================================" << std::endl;
            std::cerr << "error: " << e.what() << std::endl;
            std::cerr << "At line: " << GetLine((int) analyzer.GetCurrentPosition(), analyzer.GetLexems()) << std::endl;
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