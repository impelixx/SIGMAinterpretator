#include "Semantic.h"
#include <iostream>
#include <vector>

bool Semantic::Analyze() {
    {
        int k1 = 0;
        int k2 = 0;

        bool WasNewFieldOfViewInThisLine = false;
        bool WasNewFieldOfViewInLastLine = false;

        for (int i = 0; i < lex_.size() - 1; ++i) {
            if (lex_[i].get_type() == "INDENT" && lex_[i + 1].get_type() == "INDENT") {
                throw std::runtime_error("A sharp increase in the number of INDENTS");
                return false;
            }

            if (WasNewFieldOfViewInLastLine) {
                if (lex_[i].get_type() == "INDENT") {
                    // wait
                    WasNewFieldOfViewInLastLine = false;
                } else {
                    std::cout << lex_[i].get_type() << std::endl;
                    throw std::runtime_error("Have no INDENT after special keyword");
                    return false;
                }
            } else {
                if (lex_[i].get_type() == "INDENT") {
                    throw std::runtime_error("Have INDENT after line without special keyword");
                    return false;
                }
            }

            if (lex_[i].get_type() == "INDENT") {
                // std::cout << "+tab" << std::endl;
                ++k1;
            } else if (lex_[i].get_type() == "DEDENT") {
                // std::cout << "-tab" << std::endl;
                --k1;
                --k2;
            } else if (lex_[i].get_type() == "NEWLINE") {
                if (WasNewFieldOfViewInThisLine == true) {
                    WasNewFieldOfViewInThisLine = false;
                    WasNewFieldOfViewInLastLine = true;
                } else {
                    WasNewFieldOfViewInLastLine = false;
                }
            } else if (lex_[i].get_type() == "KEYWORD" &&
                       (lex_[i].get_text() == "if" ||
                        lex_[i].get_text() == "for" ||
                        lex_[i].get_text() == "while" ||
                        lex_[i].get_text() == "def" ||
                        lex_[i].get_text() == "else")) {
                WasNewFieldOfViewInThisLine = true;
                ++k2;
            }

            if (k1 < 0 || k2 < 0) {
                throw std::runtime_error("Different amount of INDENT and DEDENT");
                return false;
            }
        }
    }

    return true;
}