#include "Semantic.h"
#include <vector>

bool Semantic::Analyze() {
    int k = 0;

    for (int i = 0; i < lex_.size(); ++i) {
        if (lex_[i].get_type() == "INDENT") {
            std::cout << "+tab" << std::endl;
            ++k;
        }
        else if (lex_[i].get_type() == "DEDENT") {
            std::cout << "-tab" << std::endl;
            --k;
        }

        if (k < 0) {
            //throw std::runtime_error("error in");
            return false;
        }
    }

    return true;
}