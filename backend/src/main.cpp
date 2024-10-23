#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

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

    return 0;
}