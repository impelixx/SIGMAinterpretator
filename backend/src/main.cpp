#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <utility>
#include "Lexem.h"

using namespace std;

/*
    список служебных слов:
    INT, CHAR, FOR,

*/

vector<string> stuff_words = {"    ", "\n", " ", "float", "int", "if", "else", "for", "while", "do", "break", "continue", ":", "+", "-", "/", "=", "and", "or", "not", ",", "void", "return"};

vector<Lexem> lexems;

struct Vertex {
    Vertex* next_[10];
    char sym_;
};

bool isLexem(string line) {
    // stuff words
    {
        for (string stuff: stuff_words) {
            if (line == stuff) {
                return true;
            }
        }
    }

    //identificator
    {
        if (line[0] >= 'a' && line[0] <= 'z' || line[0] >= 'A' && line[0] <= 'Z' || line[0] == '_') {
            bool is_ident = true;
            for (int i = 1; i < line.size(); ++i) {
                if (line[i] >= 'a' && line[i] <= 'z' ||
                    line[i] >= 'A' && line[i] <= 'Z' ||
                    line[i] == '_' ||
                    line[i] >= '0' && line[i] <= '9') {
                    continue;
                } else {
                    is_ident = false;
                }
            }

            if (is_ident) {
                return true;
            }
        }
    }

    //literal integer
    {
        if (line[0] >= '0' && line[0] <= '9') {
            bool is_int = true;
            for (int i = 1; i < line.size(); ++i) {
                if (line[i] >= '0' && line[i] <= '9') {
                    continue;
                } else {
                    is_int = false;
                }
            }

            if (is_int) {
                return true;
            }
        }
    }

    //literal float
    {
        if (line[0] >= '0' && line[0] <= '9') {
            bool is_float = true;
            int i = 1;

            for (; i < line.size(); ++i) {
                if (line[i] >= '0' && line[i] <= '9') {
                    continue;
                } else if (line[i] == '.') {
                    ++i;
                    break;
                } else {
                    is_float = false;
                }
            }

            for (; i < line.size(); ++i) {
                if (line[i] >= '0' && line[i] <= '9') {
                    continue;
                } else {
                    is_float = false;
                }
            }
            if (is_float) {
                return true;
            }
        }
    }

    //literal char
    {
        if (line[0] == '\'' && line[line.size() - 1] == '\'') {
            if (line.size() == 3 && (line[1] >= '(' && line[1] <= '}')) {
                return true;
            }
        }
    }

    return false;
}

int main() {
    std::ifstream file_code;
    file_code.open("/Users/sinicynaleksej/SIGMAinterpretator/used files/code.us");

    char* code = new char;
    int size = 0;

    if (file_code.is_open()) {
        cout << "файл открылся" << endl;
        string ln;
        while (getline(file_code, ln)) {
            for (char s : ln) {
                code[size] = s;
                ++size;
            }
            code[size] = '\n';
            ++size;
        }
        for (int i = 0; i < size; ++i) {
            cout << code[i];
        }
        cout << endl << "файл прочитан" << endl;
    }

    file_code.close();

    for (int start = 0; start < size; ++start) {
        string und_line = "";

        for (int i = start; i < size; ++i) {
            und_line.push_back(code[i]);
        }

        for (int sz = 0; sz < size - start; ++sz){
            if (isLexem(und_line)) {
                if (und_line == "\n") {
                    cout << endl;
                }
                else {
                    cout << "[" << und_line << "]";
                }

                {
                    Lexem *lex = new Lexem;
                    lex->set_type(und_line);
                    lex->set_text(und_line);
                    lex->set_s(start);
                    lex->set_e(start + und_line.size() - 1);
                    lexems.push_back(*lex);
                }

                start = start + und_line.size() - 1;

                break;
            }
            und_line.pop_back();
        }
    }
    return 0;
}