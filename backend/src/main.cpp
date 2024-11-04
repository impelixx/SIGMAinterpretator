#include "Lexem.h"
#include <vector>
#include <fstream>

using namespace std;

vector<Lexem> lexems;

struct vertex {
    vertex* to[256] = {nullptr};
    bool terminal = false;
};

class Bor {
public:
    void add_string(string &s) {
        vertex* v = v0;
        for (char c: s) {
            if (!v->to[c]) {
                v->to[c] = new vertex;
            }
            v = v->to[c];
        }
        v->terminal = true;
    }
private:
    vertex* v0 = new vertex;
};

Lexem* processing(char*& symbol) {
    Lexem* lex = new Lexem;
    return lex;
}

int main() {
    Bor working_system;

    {
        std::ifstream file_code;
        file_code.open("/Users/sinicynaleksej/CLionProjects/untitled8/workwords");
        if (file_code.is_open()) {
            cout << "файл открылся" << endl;
            string ln;
            while (getline(file_code, ln)) {
                working_system.add_string(ln);
            }
            cout << "файл прочитан" << endl;
        }
        file_code.close();
    }

    char* code = new char;
    int size = 0;
    {
        std::ifstream file_code;
        file_code.open("/Users/sinicynaleksej/CLionProjects/untitled8/code.mop");
        if (file_code.is_open()) {
            cout << "файл открылся" << endl;
            string ln;
            while (getline(file_code, ln)) {
                for (char s: ln) {
                    code[size] = s;
                    ++size;
                }
                code[size] = '\n';
                ++size;
            }
            cout << "файл прочитан" << endl;
        }
        file_code.close();
    }

    for (int start = 0; start < size; ++start) {
        char* first = code + start;

        Lexem* lexem = processing(first);
    }

    return 0;
}