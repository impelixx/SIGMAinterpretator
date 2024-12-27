#ifndef CODEGEN_H
#define CODEGEN_H

#pragma once

#include <string>
#include <vector>
#include <cell.h>

class CodeGen {
public:
  CodeGen(std::string code, std::vector<RPNCell> rpn);

private:
  void run(std::string text);
  void repl(std::string& text, const std::string& was, const std::string& will);
  std::string runPythonScript(const std::string& scriptPath);
};

#endif // CODEGEN_H