#include "RPN.h"
#include <iostream>
#include <stdexcept>

RPN::RPN(const std::vector<Lexem>& lexems)
    : lexems_(lexems), curLex_(lexems[0]), index_(0) {}

void RPN::buildRPN() {
  rpn_.clear();
  labels.clear();
  while (curLex_.get_type() != "EOC") {
    AnalyzeLexemsList();
  }
}

void RPN::AnalyzeLexemsList() {
  if (curLex_.get_type() == "DEDENT") {
    getLexem();
  }
  if (curLex_.get_type() == "EOC") {
    return;
  }
  if (curLex_.get_type() == "INDENT") {
    getLexem();
  }
  if (curLex_.get_type() == "KEYWORD") {
    if (curLex_.get_text() == "def") {
      buildFunctionRPN();
    } else if (curLex_.get_text() == "if") {
      buildIfRPN();
    } else if (curLex_.get_text() == "while") {
      buildWhileRPN();
    } else if (curLex_.get_text() == "for") {
      buildForRPN();
    } else if (curLex_.get_text() == "print") {
      buildPrintRPN();
    } else if (curLex_.get_text() == "return") {
      buildReturnCellRPN();
    } else if (curLex_.get_text() == "int" || curLex_.get_text() == "float" ||
               curLex_.get_text() == "string") {
      buildAssignmentRPN();
    } else if (isOperator(curLex_.get_text()) || isRelOp(curLex_.get_text())) {
      buildMathOperationRPN();
    } else {
      throw std::runtime_error("Unexpected keyword: " + curLex_.get_text());
    }
  } else if (curLex_.get_type() == "IDENTIFIER") {
    auto name = curLex_.get_text();
    getLexem();
    if (curLex_.get_text() == "=") {
      buildRPNCell(RPNCell(CellType::VarCell, name));
      getLexem();
      buildMathOperationRPN();
      buildRPNCell(RPNCell(CellType::MathCell, "="));
    } else if (curLex_.get_text() == "(") {
      buildRPNCell(RPNCell(CellType::CallCeil, name));
      buildRPNCell(RPNCell(CellType::GoToCell, std::to_string(labels[name])));
      getLexem();
      getLexem();
    }
  }
  if (curLex_.get_type() == "NEWLINE") {
    getLexem();
  }
}

void RPN::printRPN() const {
  size_t index = 0;
  for (const auto& cell : rpn_) {
    std::cout << "[" << index++ << "] [Type: " << cell.GetTypeAsString()
              << ", Value: " << cell.value << "]" << std::endl;
  }
}

void RPN::buildMathOperationRPN(std::string start) {
  std::string expression = start + curLex_.get_text();
  while (curLex_.get_type() != "NEWLINE") {
    getLexem();
    if (curLex_.get_text() == ";" || curLex_.get_text() == ":") {
      break;
    }
    expression += curLex_.get_text();
  }
  if (expression.empty()) {
    throw std::runtime_error("Empty expression");
  }
  if (expression[expression.size() - 1] == 'n' and
      expression[expression.size() - 2] == '\\') {
    expression.pop_back();
    expression.pop_back();
  }
  std::stack<std::string> operators;
  std::string result;
  std::string current;

  for (char c : expression) {
    if (isspace(c))
      continue;

    if (isalnum(c) || c == '_' || c == '.') {
      current += c;
    } else {
      if (!current.empty()) {
        result += "[" + current + "]";
        current.clear();
      }

      if (c == '(') {
        operators.push("(");
      } else if (c == ')') {
        while (!operators.empty() && operators.top() != "(") {
          result += operators.top();
          operators.pop();
        }
        if (!operators.empty()) {
          operators.pop();
        }
      } else {
        std::string op(1, c);
        while (!operators.empty() && operators.top() != "(" &&
               getPrecedence(operators.top()) >= getPrecedence(op)) {
          result += operators.top();
          operators.pop();
        }
        operators.push(op);
      }
    }
  }

  if (!current.empty()) {
    result += "[" + current + "]";
  }

  while (!operators.empty()) {
    if (operators.top() != "(") {
      result += operators.top();
    }
    operators.pop();
  }
  buildRPNCell(RPNCell(CellType::MathCell, result));
  getLexem();
}

void RPN::buildFunctionRPN() {
  while (curLex_.get_type() != "IDENTIFIER") {
    getLexem();
  }
  RPNCell funcCell(CellType::FunctionCell, curLex_.get_text());
  labels[curLex_.get_text()] = rpn_.size();
  buildRPNCell(funcCell);
  getLexem();
  getLexem();
  getLexem();
  getLexem();
  while (curLex_.get_type() != "DEDENT" && curLex_.get_type() != "EOC") {
    AnalyzeLexemsList();
  }
  buildRPNCell(RPNCell(CellType::LabelCell, "end_func"));
}

void RPN::buildAssignmentRPN() {
  while (curLex_.get_type() != "IDENTIFIER") {
    getLexem();
  }
  buildRPNCell(RPNCell(CellType::VarCell, curLex_.get_text()));
  getLexem();
  if (curLex_.get_text() == "=") {
    getLexem();
    buildMathOperationRPN();
  }
  buildRPNCell(RPNCell(CellType::MathCell, "="));
}

void RPN::buildPrintRPN() {
  getLexem();
  buildMathOperationRPN();
  buildRPNCell(RPNCell(CellType::FunctionCell, "print"));
}

void RPN::buildIfRPN() {
  getLexem();
  buildMathOperationRPN();
  RPNCell ifCell(CellType::LabelCell, "if_false");
  buildRPNCell(RPNCell(CellType::ConditionalJumpCell, "if_false"));
  getLexem();
  getLexem();
  while (curLex_.get_type() != "DEDENT" && curLex_.get_type() != "EOC") {
    AnalyzeLexemsList();
  }
  buildRPNCell(RPNCell(CellType::GoToCell, "if_end"));
  getLexem();
  getLexem();
  buildRPNCell(ifCell);
  labels["if_false"] = rpn_.size();
  if (curLex_.get_text() == "else") {
    getLexem();
    getLexem();
    while (curLex_.get_type() != "DEDENT" && curLex_.get_type() != "EOC") {
      AnalyzeLexemsList();
    }
  }
  buildRPNCell(RPNCell(CellType::LabelCell, "if_end"));
  labels["if_end"] = rpn_.size();
}

void RPN::buildWhileRPN() {
  buildRPNCell(RPNCell(CellType::LabelCell, "while_start"));
  labels["while_start"] = rpn_.size();
  getLexem();
  buildMathOperationRPN();
  RPNCell jumpCell(CellType::ConditionalJumpCell, "while_false");
  buildRPNCell(jumpCell);
  size_t condIndex = rpn_.size() - 1;
  while (curLex_.get_type() != "DEDENT" && curLex_.get_type() != "EOC") {
    AnalyzeLexemsList();
  }
  buildRPNCell(RPNCell(CellType::GoToCell, "while_start"));
  labels["while_false"] = rpn_.size();
  buildRPNCell(RPNCell(CellType::LabelCell, "while_false"));
}

void RPN::buildForRPN() {
  getLexem();
  auto identifier = curLex_.get_text();
  buildRPNCell(RPNCell(CellType::LabelCell, "start_for"));
  labels["start_for"] = rpn_.size();
  getLexem();
  getLexem();
  getLexem();
  getLexem();
  buildMathOperationRPN(identifier + "<");
  buildRPNCell(RPNCell(CellType::ConditionalJumpCell, "end_for"));
  while (curLex_.get_type() != "DEDENT" && curLex_.get_type() != "EOC") {
    AnalyzeLexemsList();
  }
  buildRPNCell(RPNCell(CellType::GoToCell, "start_for"));
  buildRPNCell(RPNCell(CellType::LabelCell, "end_for"));
}

void RPN::buildFunctionCallRPN() {
  getLexem();
  std::string funcName = curLex_.get_text();
  buildRPNCell(RPNCell(CellType::FunctionCell, funcName));
  getLexem();
  getLexem();
  while (curLex_.get_text() != ")" && curLex_.get_type() != "EOC") {
    buildMathOperationRPN();
    if (curLex_.get_text() == ",") {
      getLexem();
    }
  }
  buildRPNCell(RPNCell(CellType::GoToCell, funcName));
  getLexem();
}

void RPN::buildReturnCellRPN() {
  RPNCell returnCell(CellType::ReturnCell, "return");
  getLexem();
  buildMathOperationRPN();
  buildRPNCell(returnCell);
  buildRPNCell(RPNCell(CellType::GoToCell, "end_func"));
}

void RPN::buildCallCeilRPN() {
  RPNCell callCell(CellType::CallCeil, "call_ceil");
  buildRPNCell(callCell);
  FuncCalls.push(rpn_.size());
  while (curLex_.get_text() != ")" && curLex_.get_type() != "NEWLINE") {
    getLexem();
    buildMathOperationRPN();
  }
}

void RPN::buildConditionalJumpCellRPN() {
  // Используется в if, while
  RPNCell condJump(CellType::ConditionalJumpCell, "cond_jump");
  buildRPNCell(condJump);
}

void RPN::buildLabelCellRPN() {
  // Устанавливаем метку для переходов
  RPNCell labelCell(CellType::LabelCell, "label");
  buildRPNCell(labelCell);
}

void RPN::buildRPNCell(const RPNCell& cell) {
  rpn_.push_back(cell);
}

void RPN::getLexem() {
  static size_t index = 0;
  if (index < lexems_.size()) {
    curLex_ = lexems_[index];
    index++;
  }
}

int RPN::getPrecedence(const std::string& op) const {
  if (op == "+" || op == "-")
    return 1;
  if (op == "*" || op == "/")
    return 2;
  return 0;
}

std::vector<RPNCell> RPN::getRPN() {
  return rpn_;
}

bool RPN::isOperator(const std::string& token) const {
  return (token == "+" || token == "-" || token == "*" || token == "/");
}

bool RPN::isRelOp(const std::string& token) const {
  return (token == "==" || token == "!=" || token == ">=" || token == "<=" ||
          token == ">" || token == "<");
}

bool RPN::isMathOp(const std::string& token) const {
  return (token == "+" || token == "-" || token == "*" || token == "/");
}

bool RPN::isGoToCell(const std::string& token) const {
  return (token == "goto");
}

bool RPN::isReturnCell(const std::string& token) const {
  return (token == "return");
}

bool RPN::isCallCeil(const std::string& token) const {
  return (token == "call");
}

bool RPN::isConditionalJumpCell(const std::string& token) const {
  return (token == "if_goto" || token == "ifnot_goto");
}

bool RPN::isLabelCell(const std::string& token) const {
  return (token == "label");
}

bool RPN::isNumber(const std::string& token) const {
  if (token.empty())
    return false;
  for (char c : token) {
    if (!isdigit(c) && c != '.')
      return false;
  }
  return true;
}