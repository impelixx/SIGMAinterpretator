#include "Semantic.h"
#include <iostream>
#include <stdexcept>
#include "Lexem.h"

SemanticAnalyzer::SemanticAnalyzer(std::vector<Lexem>& lexems)
    : lexems_(lexems), curLex_(lexems[0]) {}

void SemanticAnalyzer::Analyze() {
  index = 0;
  GetLexem();
  AnalyzeProgram();
}

void SemanticAnalyzer::GetLexem() {
  if (index < lexems_.size()) {
    curLex_ = lexems_[index++];
  } else {
    throw std::runtime_error("Unexpected end of code");
  }
}

void SemanticAnalyzer::AnalyzeProgram() {
  while (curLex_.get_type() != "EOC") {
    AnalyzeStatement();
  }
}

void SemanticAnalyzer::PrintFunction() {
  std::cout << "End of code reached" << std::endl;
  std::cout << "Semantic function signatures:" << std::endl;
  for (const auto& [name, types] : functionSignatures_) {
    std::cout << name << "(";
    for (size_t i = 0; i < types.size(); i++) {
      std::cout << types[i];
      if (i < types.size() - 1) {
        std::cout << ", ";
      }
    }
    std::cout << ")" << std::endl;
  }
}

void SemanticAnalyzer::AnalyzeStatement() {
  if (curLex_.get_type() == "KEYWORD") {
    auto keyword = curLex_.get_text();
    if (keyword == "def") {
      AnalyzeFunction();
    } else if (keyword == "int" || keyword == "float" || keyword == "string") {
      AnalyzeVariableDeclaration();
    } else if (keyword == "print") {
      AnalyzePrint();
    } else if (keyword == "return") {
      GetLexem();
      AnalyzeExpression();
    } else {
      throw std::runtime_error("Invalid keyword: " + keyword + "\nOn line: " +
                               std::to_string(curLex_.get_line()));
    }
  }
  GetLexem();
}

void SemanticAnalyzer::AnalyzeFunction() {
  GetLexem();
  if (curLex_.get_type() != "IDENTIFIER") {
    throw std::runtime_error(
        "Invalid function name: " + curLex_.get_text() +
        "\nOn line: " + std::to_string(curLex_.get_line()));
  }
  std::string funcName = curLex_.get_text();
  GetLexem();
  if (curLex_.get_text() != "(") {
    throw std::runtime_error(
        "Expected '(' after function name, got: " + curLex_.get_text() +
        "\nOn line: " + std::to_string(curLex_.get_line()));
  }
  GetLexem();
  std::vector<std::string> argTypes;
  while (curLex_.get_text() != ")") {
    if (curLex_.get_type() != "KEYWORD") {
      throw std::runtime_error(
          "Invalid argument type: " + curLex_.get_text() +
          "\nOn line: " + std::to_string(curLex_.get_line()));
    }
    argTypes.push_back(curLex_.get_text());
    GetLexem();
    GetLexem();
    if (curLex_.get_text() != "," && curLex_.get_text() != ")") {
      throw std::runtime_error(
          "Expected ',' or ')' after argument type. \nOn line: " +
          std::to_string(curLex_.get_line()));
    }
    if (curLex_.get_text() == ",") {
      GetLexem();
    }
  }
  GetLexem();
  functionSignatures_[funcName] = argTypes;
  if (curLex_.get_text() != ":") {
    throw std::runtime_error(
        "Expected ':' after function signature \n On line: " +
        std::to_string(curLex_.get_line()));
  }
  GetLexem();
  GetLexem();
  if (curLex_.get_type() != "INDENT") {
    throw std::runtime_error(
        "Expected indented block after function declaration \nOn line: " +
        std::to_string(curLex_.get_line()));
  }
  GetLexem();

  while (curLex_.get_type() != "DEDENT" && curLex_.get_type() != "EOC") {
    AnalyzeStatement();
  }

  if (curLex_.get_type() == "DEDENT") {
    GetLexem();
  }
}

void SemanticAnalyzer::AnalyzeVariableDeclaration() {
  auto type = curLex_.get_text();
  GetLexem();
  if (curLex_.get_type() != "IDENTIFIER") {
    throw std::runtime_error(
        "Invalid variable name: " + curLex_.get_text() +
        "\nOn line: " + std::to_string(curLex_.get_line()));
  }
  if (variables_.find(curLex_.get_text()) != variables_.end()) {
    throw std::runtime_error(
        "Variable redefinition: " + curLex_.get_text() +
        "\nOn line: " + std::to_string(curLex_.get_line()));
  }
  std::string varName = curLex_.get_text();
  GetLexem();
  if (curLex_.get_text() == "[") {
    GetLexem();
    AnalyzeExpression();
    if (curLex_.get_text() != "]") {
      throw std::runtime_error("Expected ']' after array size \n On line: " +
                               std::to_string(curLex_.get_line()));
    }
    GetLexem();
  }
  if (curLex_.get_text() == "=") {
    GetLexem();
    if ((type == "int" || type == "float") && curLex_.get_type() == "STRING") {
      throw std::runtime_error(
          "Cannot assign string to " + type + " variable: " + varName +
          "\nOn line: " + std::to_string(curLex_.get_line()));
    }
    AnalyzeExpression();
  }
  variables_[varName] = type;
  GetLexem();
}

void SemanticAnalyzer::AnalyzeExpression() {
  std::string leftType;
  while (curLex_.get_type() != "NEWLINE" && curLex_.get_text() != ";" &&
         curLex_.get_text() != ")" && curLex_.get_text() != "," &&
         curLex_.get_text() != "]" && curLex_.get_type() != "EOC" &&
         curLex_.get_text() != "}") {

    if (curLex_.get_type() == "IDENTIFIER") {
      if (variables_.find(curLex_.get_text()) == variables_.end()) {
        throw std::runtime_error(
            "Undefined variable: " + curLex_.get_text() +
            "\nOn line: " + std::to_string(curLex_.get_line()));
      }
      leftType = variables_[curLex_.get_text()];
    } else if (curLex_.get_type() == "NUMBER") {
      leftType =
          curLex_.get_text().find('.') != std::string::npos ? "float" : "int";
    } else if (curLex_.get_type() == "STRING") {
      leftType = "string";
    }

    GetLexem();

    if (curLex_.get_text() == "+") {
      GetLexem();
      std::string rightType;

      if (curLex_.get_type() == "IDENTIFIER") {
        if (variables_.find(curLex_.get_text()) == variables_.end()) {
          throw std::runtime_error(
              "Undefined variable: " + curLex_.get_text() +
              "\nOn line: " + std::to_string(curLex_.get_line()));
        }
        rightType = variables_[curLex_.get_text()];
      } else if (curLex_.get_type() == "NUMBER") {
        rightType =
            curLex_.get_text().find('.') != std::string::npos ? "float" : "int";
      } else if (curLex_.get_type() == "STRING") {
        rightType = "string";
      }

      if (leftType != rightType &&
          !(leftType == "float" && rightType == "int") &&
          !(leftType == "int" && rightType == "float")) {
        throw std::runtime_error(
            "Type mismatch in addition: cannot add " + leftType + " and " +
            rightType + "\nOn line: " + std::to_string(curLex_.get_line()));
      }
    }
  }
}

void SemanticAnalyzer::AnalyzePrint() {
  GetLexem();
  if (curLex_.get_text() != "(") {
    throw std::runtime_error("Expected '(' after print \n On line: " +
                             std::to_string(curLex_.get_line()));
  }
  GetLexem();
  std::vector<std::string> argTypes;
  while (curLex_.get_text() != ")") {
    if (curLex_.get_type() != "IDENTIFIER" && curLex_.get_type() != "NUMBER" &&
        curLex_.get_type() != "STRING") {
      throw std::runtime_error(
          "Invalid argument for print: " + curLex_.get_text() +
          "\nOn line: " + std::to_string(curLex_.get_line()));
    }
    argTypes.push_back(curLex_.get_type());
    GetLexem();
    if (curLex_.get_text() != "," && curLex_.get_text() != ")") {
      throw std::runtime_error(
          "Expected ',' or ')' after argument \n On line: " +
          std::to_string(curLex_.get_line()));
    }
    if (curLex_.get_text() == ",") {
      GetLexem();
    }
  }
  GetLexem();
}
