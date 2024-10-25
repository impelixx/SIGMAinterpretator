#include <iostream>
#include <vector>
#include <string>
#include <fstream>
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
    if (line[0] >= 'a' && line[0] <= 'z' || line[0] >= 'A' && line[0] <= 'Z' || line[0] == '_') {
        for (int i = 1; i < line.size(); ++i) {

        }
    }

    return false;
}

int main() {
    std::ifstream file_code;
    file_code.open("/Users/sinicynaleksej/SIGMAinterpretator/code.us");

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