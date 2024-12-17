#include "Semantic.h"
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

int Semantic::GetLine(int index) {
  int NumLine = 1;

  for (int i = 0; i <= index; ++i) {
    if (lex_[i].get_type() == "NEWLINE") {
      ++NumLine;
    }
  }

  return NumLine;
}

bool Semantic::Analyze() {
  std::vector<std::string> WorkWords = {
      "bool",  "int",      "float", "char", "void",  "return", "for", "while",
      "break", "continue", "if",    "else", "print", "array",  "def"};

  // Analyzing INDENT-DEDENT
  {
    int k1 = 0;
    int k2 = 0;

    bool WasNewFieldOfViewInThisLine = false;
    bool WasNewFieldOfViewInLastLine = false;

    for (int i = 0; i < lex_.size(); ++i) {
      if (i < lex_.size() - 1) {
        if (lex_[i].get_type() == "INDENT" &&
            lex_[i + 1].get_type() == "INDENT") {
          throw std::runtime_error(
              "A sharp increase in the number of INDENTS\nAt line: " +
              std::to_string(GetLine(i)));
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
        throw std::runtime_error(
            "Have INDENT after line without special keyword\nAt line: " +
            std::to_string(GetLine(i)));
        return false;
      }

      if (lex_[i].get_type() == "DEDENT") {
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
                 (lex_[i].get_text() == "if" || lex_[i].get_text() == "for" ||
                  lex_[i].get_text() == "while" ||
                  lex_[i].get_text() == "def" ||
                  lex_[i].get_text() == "else")) {
        WasNewFieldOfViewInThisLine = true;
        ++k2;
        ++k1;
      }

      if (k1 < 0 || k2 < 0) {
        throw std::runtime_error(
            "Different amount of INDENT and DEDENT\nAt line: " +
            std::to_string(GetLine(i)));
        return false;
      }
    }
  }

  // Analyzing variables (field of view)
  {
    std::vector<std::tuple<int, std::string, std::string>>
        variables;  // <field of view, type of the variable, name of the variable>

    int WordInLine = 1;
    int FieldOfViewNow = 0;

    std::string VariableType = "";

    bool was_pushed = false;

    for (int i = 0; i < lex_.size(); ++i) {
      if (lex_[i].get_type() == "DEDENT") {
        --FieldOfViewNow;
      }

      if (lex_[i].get_text() == "def" || lex_[i].get_text() == "if" ||
          lex_[i].get_text() == "for" || lex_[i].get_text() == "while" ||
          lex_[i].get_text() == "else") {
        ++FieldOfViewNow;
      }

      if ((was_pushed && lex_[i].get_text() == "=" ||
           lex_[i].get_text() == "==") &&
          get<1>(variables[variables.size() - 1]) == "USING") {
        variables[variables.size() - 1] = {
            get<0>(variables[variables.size() - 1]), "ASSIGNMENT",
            get<2>(variables[variables.size() - 1])};
      }

      was_pushed = false;

      bool MeetIf = false;

      if (lex_[i].get_text() == "bool" || lex_[i].get_text() == "char" ||
          lex_[i].get_text() == "string" || lex_[i].get_text() == "int" ||
          lex_[i].get_text() == "float" || lex_[i].get_text() == "def" ||
          lex_[i].get_text() == "for" || lex_[i].get_text() == "string") {
        WordInLine = 2;
        MeetIf = true;
        VariableType = lex_[i].get_text();
      } else if (WordInLine == 2) {
        if ((lex_[i].get_type() == "IDENTIFIER" ||
             lex_[i].get_type() == "KEYWORD") &&
            VariableType != "") {
          MeetIf = true;
          std::string WhatType = VariableType;
          if (VariableType == "for") {
            WhatType = "int";
          }

          if (VariableType == "def") {
            variables.push_back(
                {FieldOfViewNow - 1, WhatType, lex_[i].get_text()});
            was_pushed = true;
          } else {
            variables.push_back({FieldOfViewNow, WhatType, lex_[i].get_text()});
            was_pushed = true;
          }
        }
        WordInLine = 0;
      }

      if (!MeetIf) {
        if (lex_[i].get_type() == "IDENTIFIER" && !lex_[i].get_text().empty()) {
          // std::cout << "USING-1- " << lex_[i].get_text() << "\n";
          variables.push_back({FieldOfViewNow, "USING", lex_[i].get_text()});
          was_pushed = true;
        }
      }

      if (lex_[i].get_type() == "DEDENT") {
        variables.push_back({-1000000, "NONE_TYPE", "NONE"});
        was_pushed = true;
      }

      /*
            if (lex_[i].get_type() == "IDENTIFIER" && !lex_[i].get_text().empty()) {
                bool WasDeclared = false;
                for (int j = 0; j < variables.size(); ++j) {
                    if (get<2>(variables[j]) == lex_[i].get_text()) {
                        WasDeclared = true;
                        break;
                    } else {
                        // std::cout << get<2>(variables[j]) << "!=" << lex_[i].get_text() << "\n";
                    }
                }
                if (!WasDeclared) {
                    throw std::runtime_error("Using undeclared variable " + lex_[i].get_text());
                }
            }
            */
    }

    for (int i = 0; i < variables.size(); ++i) {
      while (get<2>(variables[i]) == "true" ||
             get<2>(variables[i]) == "false") {
        variables.erase(variables.begin() + i);
      }
    }

    for (int i = 0; i < variables.size(); ++i) {
      // std::cout << get<0>(variables[i]) << " " << get<1>(variables[i]) << " " << get<2>(variables[i]) << std::endl;
      for (int j = 0; j < WorkWords.size(); ++j) {
        if (get<2>(variables[i]) == WorkWords[j]) {
          throw std::runtime_error("Impossible variable name (KEYWORD): " +
                                   get<2>(variables[i]));
          return false;
        }
      }
    }

    std::map<std::string, bool> vised;

    for (int i = 0; i < variables.size(); ++i) {
      if (get<1>(variables[i]) == "USING" ||
          get<1>(variables[i]) == "ASSIGNMENT") {
        // std::cout << get<1>(variables[i]) << " " << get<2>(variables[i]) << "\n";
        if (!vised[get<2>(variables[i])]) {
          // std::cout << "err " << vised[get<2>(variables[i])] << std::endl;
          throw std::runtime_error("Using undeclared variable: " +
                                   get<2>(variables[i]));
        }
      } else {
        if (i > 0) {
          if (get<0>(variables[i]) < get<0>(variables[i - 1])) {
            for (int j = i - 1;
                 j >= 0 && get<0>(variables[j]) == get<0>(variables[i - 1]);
                 --j) {
              if (get<1>(variables[j]) != "ASSIGNMENT" &&
                  get<1>(variables[j]) != "USING" &&
                  get<1>(variables[j]) != "NONE_TYPE") {
                vised[get<2>(variables[j])] = false;
              }
            }
          }
        }
        if (vised[get<2>(variables[i])]) {
          // std::cout << "err " << vised[get<2>(variables[i])] << std::endl;
          throw std::runtime_error("Redeclaring an existing variable: " +
                                   get<2>(variables[i]));
        } else {
          // std::cout << "not vised " << get<2>(variables[i]) << std::endl;
        }
        if (get<0>(variables[i]) > -1000000) {
          vised[get<2>(variables[i])] = true;
        }
      }
    }

    for (int i = 0; i < variables.size(); ++i) {
      if (get<1>(variables[i]) == "ASSIGNMENT") {
        for (int j = 0; j < i; ++j) {
          if (get<2>(variables[i]) == get<2>(variables[j]) &&
              get<1>(variables[j]) == "def") {
            throw std::runtime_error(
                "You cannot equate the value of the function " +
                get<2>(variables[i]) + " to anything");
          }
        }
      }
    }
  }

  // Analyzing variables (connecting types)
  {
    std::vector<std::vector<Lexem>> LinesLexems = {{}};
    int LineNum = 0;
    for (int i = 0; i < lex_.size(); ++i) {
      if (lex_[i].get_type() == "NEWLINE") {
        LinesLexems.push_back({});
      } else {
        LinesLexems[LineNum].push_back(lex_[i]);
      }
      // LinesLexems.push_back();
    }
  }

  return true;
}