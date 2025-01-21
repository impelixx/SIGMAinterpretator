#include "Semantic.h"
#include <iostream>
#include <stdexcept>
#include "Lexem.h"

/**
 * @brief Constructs a SemanticAnalyzer object.
 * 
 * This constructor initializes the SemanticAnalyzer with a vector of lexems.
 * It sets the current lexem to the first element in the vector and initializes
 * the scope stack with an empty scope.
 * 
 * @param lexems A reference to a vector of Lexem objects to be analyzed.
 */
SemanticAnalyzer::SemanticAnalyzer(std::vector<Lexem>& lexems)
    : lexems_(lexems), curLex_(lexems[0]) {
  scopeStack_.push_back({});
}

/**
 * @brief Analyzes the semantic structure of the program.
 * 
 * This function initializes the analysis process by setting the index to 0,
 * calling the AnalyzeProgram function to perform the main analysis, and then
 * retrieving the next lexeme using the GetLexem function.
 */
void SemanticAnalyzer::Analyze() {
  index = 0;
  AnalyzeProgram();
  GetLexem();
}

/**
 * @brief Retrieves the next lexeme from the list of lexemes.
 *
 * This function updates the current lexeme (`curLex_`) to the next lexeme
 * in the `lexems_` list if the current index is within the bounds of the list.
 * The index is then incremented to point to the next lexeme for future calls.
 */
void SemanticAnalyzer::GetLexem() {
  if (index < lexems_.size()) {
    curLex_ = lexems_[index++];
  }
}

/**
 * @brief Enters a new scope by pushing an empty scope onto the scope stack.
 * 
 * This function is used to manage the scope stack within the semantic analyzer.
 * It creates a new, empty scope and adds it to the top of the stack, allowing
 * for the tracking of variables and other symbols within nested scopes.
 */
void SemanticAnalyzer::EnterScope() {
  scopeStack_.push_back({});
}

/**
 * @brief Exits the current scope by popping the top element from the scope stack.
 *
 * This function checks if the scope stack is not empty and then removes the 
 * top element, effectively exiting the current scope.
 */
void SemanticAnalyzer::ExitScope() {
  if (!scopeStack_.empty()) {
    scopeStack_.pop_back();
  }
}

/**
 * @brief Checks if a variable is defined in the current scope stack.
 *
 * This function iterates through the scope stack from the topmost scope to the bottommost scope
 * to determine if the specified variable is defined in any of the scopes.
 *
 * @param var The name of the variable to check.
 * @return true if the variable is defined in any scope; false otherwise.
 */
bool SemanticAnalyzer::IsVarDefined(const std::string& var) {
  for (int i = static_cast<int>(scopeStack_.size()) - 1; i >= 0; --i) {
    if (scopeStack_[i].find(var) != scopeStack_[i].end()) {
      return true;
    }
  }
  return false;
}

/**
 * @brief Retrieves the type of a given variable from the scope stack.
 *
 * This function searches through the scope stack from the most recent scope
 * to the oldest scope to find the type of the specified variable. If the
 * variable is found, its type is returned. If the variable is not found in
 * any scope, an empty string is returned.
 *
 * @param var The name of the variable whose type is to be retrieved.
 * @return The type of the variable as a string, or an empty string if the
 *         variable is not found in any scope.
 */
std::string SemanticAnalyzer::GetVarType(const std::string& var) {
  for (int i = static_cast<int>(scopeStack_.size()) - 1; i >= 0; --i) {
    if (scopeStack_[i].find(var) != scopeStack_[i].end()) {
      return scopeStack_[i][var];
    }
  }
  return "";
}

/**
 * @brief Adds a variable with its type to the current scope.
 * 
 * This function inserts a variable and its corresponding type into the 
 * most recent scope in the scope stack.
 * 
 * @param var The name of the variable to be added.
 * @param type The type of the variable to be added.
 */
void SemanticAnalyzer::AddVariable(const std::string& var,
                                   const std::string& type) {
  scopeStack_.back()[var] = type;
}

/**
 * @brief Analyzes the entire program by iterating through each statement.
 * 
 * This function continuously analyzes statements in the program until 
 * the end of the code (EOC) is reached. It relies on the AnalyzeStatement 
 * function to process each individual statement.
 */
void SemanticAnalyzer::AnalyzeProgram() {
  while (curLex_.get_type() != "EOC") {
    AnalyzeStatement();
  }
}

/**
 * @brief Prints the function signatures stored in the SemanticAnalyzer.
 *
 * This method outputs the names and parameter types of all functions
 * stored in the functionSignatures_ map to the standard output.
 * Each function signature is printed in the format:
 * functionName(type1, type2, ..., typeN)
 */
void SemanticAnalyzer::PrintFunction() {
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

/**
 * @brief Analyzes a single statement in the source code.
 * 
 * This function processes the current lexical token and determines the type of statement it represents.
 * Depending on the type of statement, it delegates the analysis to the appropriate function.
 * 
 * @throws std::runtime_error if an invalid keyword is encountered, or if there are issues with function calls or variable assignments.
 * 
 * The following types of statements are supported:
 * - Function definition (`def`)
 * - Variable declaration (`int`, `float`, `string`)
 * - Print statement (`print`)
 * - Return statement (`return`)
 * - While loop (`while`)
 * - If statement (`if`)
 * - For loop (`for`)
 * 
 * Additionally, it handles function calls and variable assignments for identifiers.
 * 
 * @note This function assumes that `curLex_` is properly initialized and points to the current lexical token.
 */
void SemanticAnalyzer::AnalyzeStatement() {
  if (curLex_.get_type() == "KEYWORD") {
    auto keyword = curLex_.get_text();
    if (keyword == "def") {
      GetLexem();
      AnalyzeFunction();
    } else if (keyword == "int" || keyword == "float" || keyword == "string") {
      AnalyzeVariableDeclaration();
    } else if (keyword == "print") {
      AnalyzePrint();
    } else if (keyword == "return") {
      GetLexem();
      AnalyzeExpression();
    } else if (keyword == "while") {
      AnalyzeWhile();
    } else if (keyword == "if") {
      AnalyzeIf();
    } else if (keyword == "for") {
      AnalyzeFor();
    } else {
      throw std::runtime_error("Invalid keyword: " + keyword + "\nOn line: " +
                               std::to_string(curLex_.get_line()));
    }
  }
  if (curLex_.get_type() == "IDENTIFIER") {
    auto name = curLex_.get_text();
    GetLexem();
    if (curLex_.get_text() == "(") {
      if (functionSignatures_.find(name) == functionSignatures_.end()) {
        throw std::runtime_error("Undefined function: " + name + "\nOn line: " +
                                 std::to_string(curLex_.get_line()));
      }
      auto argTypes = functionSignatures_[name];
      CheckFunctionCall(name);
    } else if (curLex_.get_text() == "=") {
      GetLexem();
      if (!IsVarDefined(name)) {
        throw std::runtime_error("Undefined variable: " + name + "\nOn line: " +
                                 std::to_string(curLex_.get_line()));
      }
      if (GetVarType(name) == "int" || GetVarType(name) == "float") {
        if (curLex_.get_type() == "STRING") {
          throw std::runtime_error(
              "Cannot assign string to " + GetVarType(name) + " variable: " +
              name + "\nOn line: " + std::to_string(curLex_.get_line()));
        }
      }
      if (GetVarType(name) == "string") {
        if (curLex_.get_type() == "NUMBER") {
          throw std::runtime_error(
              "Cannot assign number to string variable: " + name +
              "\nOn line: " + std::to_string(curLex_.get_line()));
        }
      }
      AnalyzeExpression();
    } else {
      throw std::runtime_error("Invalid statement: " + name + "\nOn line: " +
                               std::to_string(curLex_.get_line()));
    }
  }
  if (curLex_.get_type() == "NEWLINE" || curLex_.get_text() == ";" ||
      curLex_.get_text() == "}") {
    GetLexem();
  }
}

/**
 * @brief Checks the validity of a function call in the source code.
 *
 * This function verifies that the function being called is defined, that the
 * correct syntax is used for the function call, and that the arguments passed
 * to the function match the expected types and number of arguments.
 *
 * @param funcName The name of the function being called.
 *
 * @throws std::runtime_error If the function is undefined, if the syntax of
 * the function call is incorrect, if an undefined variable is used as an
 * argument, if the number of arguments does not match the function signature,
 * or if the types of the arguments do not match the expected types.
 */
void SemanticAnalyzer::CheckFunctionCall(const std::string& funcName) {
  if (functionSignatures_.find(funcName) == functionSignatures_.end()) {
    throw std::runtime_error("Undefined function: " + funcName);
  }
  if (curLex_.get_text() != "(") {
    throw std::runtime_error(
        "Expected '(' after function name: " + funcName +
        "\nOn line: " + std::to_string(curLex_.get_line()));
  }
  GetLexem();
  std::vector<std::string> argTypes;
  while (curLex_.get_text() != ")") {
    if (curLex_.get_type() == "NUMBER") {
      argTypes.push_back((curLex_.get_text().find('.') != std::string::npos)
                             ? "float"
                             : "int");
    } else if (curLex_.get_type() == "STRING") {
      argTypes.push_back("string");
    } else if (curLex_.get_type() == "IDENTIFIER") {
      if (!IsVarDefined(curLex_.get_text())) {
        throw std::runtime_error(
            "Undefined variable: " + curLex_.get_text() +
            "\nOn line: " + std::to_string(curLex_.get_line()));
      }
      argTypes.push_back(GetVarType(curLex_.get_text()));
    }
    AnalyzeExpression();
    if (curLex_.get_text() != "," && curLex_.get_text() != ")") {
      throw std::runtime_error(
          "Expected ',' or ')' after argument \nOn line: " +
          std::to_string(curLex_.get_line()));
    }
    if (curLex_.get_text() == ",") {
      GetLexem();
    }
  }
  if (argTypes.size() != functionSignatures_[funcName].size()) {
    throw std::runtime_error(
        "Invalid number of arguments for function: " + funcName +
        "\nOn line: " + std::to_string(curLex_.get_line()) +
        "\nExpected: " + std::to_string(functionSignatures_[funcName].size()) +
        "\nGot: " + std::to_string(argTypes.size()));
  }
  for (int i = 0; i < (int)argTypes.size(); i++) {
    if (argTypes[i] != functionSignatures_[funcName][i]) {
      throw std::runtime_error(
          "Invalid argument type for function: " + funcName +
          "\nOn line: " + std::to_string(curLex_.get_line()) + "\nExpected: " +
          functionSignatures_[funcName][i] + "\nGot: " + argTypes[i]);
    }
  }
  GetLexem();
}

/**
 * @brief Analyzes a function declaration and its body.
 * 
 * This function performs semantic analysis on a function declaration. It checks for the correct
 * syntax and structure of the function, including the function name, argument types, and the 
 * function body. It also manages the scope of the function and its parameters.
 * 
 * @throws std::runtime_error if any syntax or semantic errors are encountered during analysis.
 * 
 * The function performs the following steps:
 * 1. Skips any leading keywords.
 * 2. Validates the function name.
 * 3. Checks for the opening parenthesis '(' after the function name.
 * 4. Parses and validates the argument types.
 * 5. Checks for the closing parenthesis ')' after the argument list.
 * 6. Stores the function signature.
 * 7. Checks for the colon ':' after the function signature.
 * 8. Validates the presence of an indented block for the function body.
 * 9. Enters a new scope for the function.
 * 10. Adds the function parameters to the scope.
 * 11. Analyzes the statements within the function body.
 * 12. Exits the scope after the function body is analyzed.
 */
void SemanticAnalyzer::AnalyzeFunction() {
  while (curLex_.get_type() == "KEYWORD") {
    GetLexem();
  }
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
  EnterScope();
  for (int i = 0; i < (int)argTypes.size(); i++) {
    std::string paramName = "param" + std::to_string(i + 1);
    AddVariable(paramName, argTypes[i]);
  }
  GetLexem();
  while (curLex_.get_type() != "DEDENT" && curLex_.get_type() != "EOC") {
    AnalyzeStatement();
  }
  if (curLex_.get_type() == "DEDENT") {
    GetLexem();
  }
  ExitScope();
}

/**
 * @brief Analyzes a variable declaration in the source code.
 * 
 * This function processes the declaration of a variable, including its type,
 * name, and optional initialization. It performs several checks to ensure
 * the declaration is valid, such as verifying the variable name, checking for
 * redefinition, and ensuring proper syntax for array declarations and initializations.
 * 
 * @throws std::runtime_error if the variable name is invalid, if the variable
 *         is redefined, if there are syntax errors in the array declaration,
 *         or if there is a type mismatch during initialization.
 */
void SemanticAnalyzer::AnalyzeVariableDeclaration() {
  auto type = curLex_.get_text();
  while (curLex_.get_type() == "KEYWORD") {
    GetLexem();
  }
  if (curLex_.get_type() != "IDENTIFIER") {
    throw std::runtime_error(
        "Invalid variable name: " + curLex_.get_text() +
        "\nOn line: " + std::to_string(curLex_.get_line()));
  }
  std::string varName = curLex_.get_text();
  if (IsVarDefined(varName)) {
    throw std::runtime_error(
        "Variable redefinition: " + varName +
        "\nOn line: " + std::to_string(curLex_.get_line()));
  }
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
  AddVariable(varName, type);
}

/**
 * @brief Analyzes an expression for semantic correctness.
 * 
 * This function checks the types of variables and literals in an expression
 * to ensure they are used correctly. It throws runtime errors if it encounters
 * undefined variables or type mismatches in addition operations.
 * 
 * The function processes the expression until it encounters a newline, 
 * semicolon, closing parenthesis, comma, closing bracket, end of code, 
 * or closing brace.
 * 
 * @throws std::runtime_error if an undefined variable is encountered or 
 *         if there is a type mismatch in an addition operation.
 */
void SemanticAnalyzer::AnalyzeExpression() {
  std::string leftType;
  while (curLex_.get_type() != "NEWLINE" && curLex_.get_text() != ";" &&
         curLex_.get_text() != ")" && curLex_.get_text() != "," &&
         curLex_.get_text() != "]" && curLex_.get_type() != "EOC" &&
         curLex_.get_text() != "}") {
    if (curLex_.get_type() == "IDENTIFIER") {
      if (!IsVarDefined(curLex_.get_text())) {
        throw std::runtime_error(
            "Undefined variable: " + curLex_.get_text() +
            "\nOn line: " + std::to_string(curLex_.get_line()));
      }
      leftType = GetVarType(curLex_.get_text());
    } else if (curLex_.get_type() == "NUMBER") {
      leftType =
          (curLex_.get_text().find('.') != std::string::npos) ? "float" : "int";
    } else if (curLex_.get_type() == "STRING") {
      leftType = "string";
    }
    GetLexem();

    if (curLex_.get_text() == "+") {
      GetLexem();
      std::string rightType;
      if (curLex_.get_type() == "IDENTIFIER") {
        if (!IsVarDefined(curLex_.get_text())) {
          throw std::runtime_error(
              "Undefined variable: " + curLex_.get_text() +
              "\nOn line: " + std::to_string(curLex_.get_line()));
        }
        rightType = GetVarType(curLex_.get_text());
      } else if (curLex_.get_type() == "NUMBER") {
        rightType = (curLex_.get_text().find('.') != std::string::npos)
                        ? "float"
                        : "int";
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

/**
 * @brief Analyzes the syntax and semantics of a print statement.
 * 
 * This function processes a print statement by verifying the correct syntax
 * and types of its arguments. It expects the print statement to start with
 * a '(' and end with a ')'. The arguments within the parentheses must be
 * identifiers, numbers, or strings, separated by commas.
 * 
 * @throws std::runtime_error if the syntax is incorrect or if an invalid 
 * argument type is encountered.
 */
void SemanticAnalyzer::AnalyzePrint() {
  GetLexem();
  while (curLex_.get_type() == "KEYWORD") {
    GetLexem();
  }
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
  }
  GetLexem();
  GetLexem();
}

/**
 * @brief Analyzes a 'while' loop construct in the source code.
 * 
 * This function processes a 'while' loop by performing the following steps:
 * 1. Retrieves the next lexeme.
 * 2. Analyzes the loop's expression.
 * 3. Checks for the presence of a colon (':') after the expression.
 * 4. Ensures that the block following the 'while' loop is indented.
 * 5. Enters a new scope for the loop's body.
 * 6. Analyzes each statement within the loop's body until the end of the block.
 * 7. Exits the scope after processing the loop's body.
 * 
 * @throws std::runtime_error If the expected ':' is not found after the 'while' expression.
 * @throws std::runtime_error If the expected indented block is not found after the 'while' statement.
 */
void SemanticAnalyzer::AnalyzeWhile() {
  GetLexem();
  AnalyzeExpression();
  GetLexem();
  if (curLex_.get_text() != ":") {
    throw std::runtime_error(
        "Expected ':' after while expression \n On line: " +
        std::to_string(curLex_.get_line()));
  }
  GetLexem();
  GetLexem();
  if (curLex_.get_type() != "INDENT") {
    throw std::runtime_error("Expected indented block after while \nOn line: " +
                             std::to_string(curLex_.get_line()));
  }
  EnterScope();
  GetLexem();
  while (curLex_.get_type() != "DEDENT" && curLex_.get_type() != "EOC") {
    AnalyzeStatement();
  }
  if (curLex_.get_type() == "DEDENT") {
    GetLexem();
  }
  ExitScope();
}

/**
 * @brief Analyzes an 'if' statement in the source code.
 * 
 * This function processes the syntax and semantics of an 'if' statement,
 * ensuring that it follows the correct structure and rules. It handles
 * the conditional expression, the indented block of code that follows,
 * and optionally an 'else' block if present.
 * 
 * @throws std::runtime_error If the syntax of the 'if' statement is incorrect,
 * such as missing ':' after the condition or 'else', or missing indented blocks.
 */
void SemanticAnalyzer::AnalyzeIf() {
  GetLexem();
  AnalyzeExpression();
  GetLexem();
  if (curLex_.get_text() != ":") {
    throw std::runtime_error("Expected ':' after if expression \n On line: " +
                             std::to_string(curLex_.get_line()));
  }
  GetLexem();
  GetLexem();
  if (curLex_.get_type() != "INDENT") {
    throw std::runtime_error("Expected indented block after if \nOn line: " +
                             std::to_string(curLex_.get_line()));
  }
  EnterScope();
  GetLexem();
  while (curLex_.get_type() != "DEDENT" && curLex_.get_type() != "EOC") {
    AnalyzeStatement();
  }
  if (curLex_.get_type() == "DEDENT") {
    GetLexem();
  }
  ExitScope();
  while (curLex_.get_type() == "NEWLINE") {
    GetLexem();
  }
  if (curLex_.get_text() == "else") {
    GetLexem();
    if (curLex_.get_text() != ":") {
      throw std::runtime_error("Expected ':' after else \n On line: " +
                               std::to_string(curLex_.get_line()));
    }
    GetLexem();
    GetLexem();
    if (curLex_.get_type() != "INDENT") {
      throw std::runtime_error(
          "Expected indented block after else \nOn line: " +
          std::to_string(curLex_.get_line()));
    }
    EnterScope();
    GetLexem();
    while (curLex_.get_type() != "DEDENT" && curLex_.get_type() != "EOC") {
      AnalyzeStatement();
    }
    if (curLex_.get_type() == "DEDENT") {
      GetLexem();
    }
    ExitScope();
  }
}

/**
 * @brief Analyzes a 'for' loop in the source code.
 * 
 * This function processes the syntax and semantics of a 'for' loop, ensuring
 * that it follows the expected structure and semantics. The expected structure
 * is:
 * 
 * for <identifier> in range(<expression>):
 *     <indented block>
 * 
 * The function performs the following checks:
 * - The loop variable is a valid identifier.
 * - The keyword 'in' follows the variable name.
 * - The keyword 'range' follows 'in'.
 * - The range expression is enclosed in parentheses.
 * - A colon (:) follows the range expression.
 * - An indented block follows the colon.
 * 
 * If any of these checks fail, a runtime error is thrown with a descriptive
 * message indicating the nature of the error and the line number where it
 * occurred.
 * 
 * The function also manages the scope of the loop variable, adding it to the
 * current scope and removing it when the loop ends.
 * 
 * @throws std::runtime_error if the 'for' loop does not follow the expected
 *         structure or semantics.
 */
void SemanticAnalyzer::AnalyzeFor() {
  GetLexem();
  if (curLex_.get_type() != "IDENTIFIER") {
    throw std::runtime_error(
        "Invalid variable name: " + curLex_.get_text() +
        "\nOn line: " + std::to_string(curLex_.get_line()));
  }
  std::string varName = curLex_.get_text();

  GetLexem();
  if (curLex_.get_text() != "in") {
    throw std::runtime_error(
        "Expected 'in' after variable name, got: " + curLex_.get_text() +
        "\nOn line: " + std::to_string(curLex_.get_line()));
  }
  GetLexem();
  if (curLex_.get_text() != "range") {
    throw std::runtime_error(
        "Expected 'range', got: " + curLex_.get_text() +
        "\nOn line: " + std::to_string(curLex_.get_line()));
  }
  GetLexem();
  if (curLex_.get_text() != "(") {
    throw std::runtime_error(
        "Expected '(' after range, got: " + curLex_.get_text() +
        "\nOn line: " + std::to_string(curLex_.get_line()));
  }
  GetLexem();
  AnalyzeExpression();
  if (curLex_.get_text() != ")") {
    throw std::runtime_error(
        "Expected ')' after range value, got: " + curLex_.get_text() +
        "\nOn line: " + std::to_string(curLex_.get_line()));
  }
  GetLexem();
  if (curLex_.get_text() != ":") {
    throw std::runtime_error("Expected ':' after range expression \nOn line: " +
                             std::to_string(curLex_.get_line()));
  }
  GetLexem();
  GetLexem();
  if (curLex_.get_type() != "INDENT") {
    throw std::runtime_error(
        "Expected indented block after for loop \nOn line: " +
        std::to_string(curLex_.get_line()));
  }
  EnterScope();
  AddVariable(varName, "int");

  GetLexem();
  while (curLex_.get_type() != "DEDENT" && curLex_.get_type() != "EOC") {
    AnalyzeStatement();
  }
  if (curLex_.get_type() == "DEDENT") {
    GetLexem();
  }
  ExitScope();
}