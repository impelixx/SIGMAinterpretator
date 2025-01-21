#include <Codegen.h>
#include <cell.h>
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

string CodeGen::runPythonScript(const string& scriptPath) {
  string command = "python3 " + scriptPath;

  FILE* pipe = popen(command.c_str(), "r");

  if (!pipe) {
    return "";
  }

  string result = "";
  char buffer[128];

  while (!feof(pipe)) {
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
      result += buffer;
    }
  }

  int pclose_result = pclose(pipe);
  if (pclose_result == -1) {
    return "";
  }

  return "==============\n" + result;
}

void CodeGen::repl(std::string& text, const std::string& was,
                   const std::string& will) {
  size_t pos = text.find(was);
  while (pos != std::string::npos) {
    text.replace(pos, was.length(), will);
    pos = text.find(was, pos + will.length());
  }
}

void CodeGen::run(string text) {
  vector<string> symbols = {"(", ",", " ", "\n"};
  vector<string> types = {"int ", "float ", "char ", "string "};

  for (string tp : types) {
    for (string s : symbols) {
      repl(text, s + tp, s);
    }
  }

  ofstream MyFile("temporary.py");
  MyFile << text;
  MyFile.close();

  try {
    string output = runPythonScript("temporary.py");
    if (output.empty()) {
      cerr << "Не удалось получить вывод .us скрипта!" << endl;
    } else {
      cout << output << endl;
    }
  } catch (const exception& e) {
    cerr << "Ошибка: " << e.what() << endl;
  }
}

CodeGen::CodeGen(string code, vector<RPNCell> rpn) {
  run(code);
}