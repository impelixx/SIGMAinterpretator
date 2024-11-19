#include "Semantic.h"
#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <map>
#include <utility>

bool Semantic::Analyze() {
    std::vector<std::string> WorkWords = {"bool", "int", "float", "char", "void", "return", "for", "while", "break", "continue", "if", "else", "print", "array"};

    // Analyzing INDENT-DEDENT
    {
        int k1 = 0;
        int k2 = 0;

        bool WasNewFieldOfViewInThisLine = false;
        bool WasNewFieldOfViewInLastLine = false;

        for (int i = 0; i < lex_.size(); ++i) {
            if (i < lex_.size() - 1) {
                if (lex_[i].get_type() == "INDENT" && lex_[i + 1].get_type() == "INDENT") {
                    throw std::runtime_error("A sharp increase in the number of INDENTS");
                    return false;
                }
            }

            if (WasNewFieldOfViewInLastLine) {
                if (lex_[i].get_type() == "INDENT") {
                    // wait
                    WasNewFieldOfViewInLastLine = false;
                } else {
                    // std::cout << lex_[i].get_type() << std::endl;
                    // throw std::runtime_error("Have no INDENT after special keyword");
                    // return false;
                }
            } else if (lex_[i].get_type() == "INDENT") {
                throw std::runtime_error("Have INDENT after line without special keyword");
                return false;
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

    // Analyzing variables (field of view)
    {
        std::vector<std::tuple<int, std::string, std::string>> variables; // <field of view, type of the variable, name of the variable>

        int WordInLine = 1;
        int FieldOfViewNow = 0;

        std::string VariableType = "";

        for (int i = 0; i < lex_.size(); ++i) {
            if (lex_[i].get_type() == "INDENT") {
                ++FieldOfViewNow;
            } else if (lex_[i].get_type() == "DEDENT") {
                --FieldOfViewNow;
            }

            if (WordInLine == 1) {
                if (lex_[i].get_text() == "bool" ||
                        lex_[i].get_text() == "char" ||
                        lex_[i].get_text() == "string" ||
                        lex_[i].get_text() == "int" ||
                        lex_[i].get_text() == "float") {
                    VariableType = lex_[i].get_text();
                }
            } else if (WordInLine == 2) {
                if ((lex_[i].get_type() == "IDENTIFIER" || lex_[i].get_type() == "KEYWORD") && VariableType != "") {
                    variables.push_back({FieldOfViewNow, VariableType, lex_[i].get_text()});
                }
            }

            if (lex_[i].get_type() == "NEWLINE" || lex_[i].get_type() == "INDENT" || lex_[i].get_type() == "DEDENT") {
                WordInLine = 1;
                VariableType = "";
            } else {
                ++WordInLine;
            }
        }

        for (int i = 0; i < variables.size(); ++i) {
            // std::cout << get<0>(variables[i]) << " " << get<1>(variables[i]) << " " << get<2>(variables[i]) << std::endl;
            for (int j = 0; j < WorkWords.size(); ++j) {
                if (get<2>(variables[i]) == WorkWords[j]) {
                    throw std::runtime_error("Impossible variable name (KEYWORD)");
                    return false;
                }
            }
        }

        std::map<std::string, bool> vised;

        for (int i = 0; i < variables.size(); ++i) {
            if (i > 0) {
                if (get<0>(variables[i]) < get<0>(variables[i - 1])) {
                    for (int j = i - 1; j >= 0 && get<0>(variables[j]) == get<0>(variables[i - 1]); --j) {
                        vised[get<2>(variables[i])] = false;
                    }
                    continue;
                }
            }
            if (vised[get<2>(variables[i])]) {
                throw std::runtime_error("Redeclaring an existing variable");
            }
            vised[get<2>(variables[i])] = true;
        }
    }

    // Analyzing variables (connecting types)

    return true;
}