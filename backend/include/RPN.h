#ifndef RPN_H
#define RPN_H

#include <vector>
#include <string>
#include <stack>
#include <map>
#include <iostream>
#include "Lexem.h"
#include <cell.h>



class RPN {
public:
    RPN(const std::vector<Lexem>& lexems);
    void buildRPN();
    void printRPN() const;
    std::vector<RPNCell> getRPN();

private:
    std::vector<Lexem> lexems_;
    std::vector <RPNCell> rpn_;
    std::stack<size_t> FuncCalls;
    std::map<std::string, int> labels;
    Lexem curLex_;
    size_t index_;
    // analyze functions
    void AnalyzeLexemsList();
    void buildFunctionRPN();
    void buildAssignmentRPN();
    void buildPrintRPN();
    void buildIfRPN();
    void buildWhileRPN();
    void buildForRPN();
    void buildFunctionCallRPN();
    void buildMathOperationRPN(std::string start = "");
    void buildGoToCellRPN();
    void buildReturnCellRPN();
    void buildCallCeilRPN();
    void buildConditionalJumpCellRPN();
    void buildLabelCellRPN();
    void buildRPNCell(const RPNCell& cell);
    // helper functions
    void getLexem();
    int getPrecedence(const std::string& op) const;
    bool isOperator(const std::string& token) const;
    bool isRelOp(const std::string& token) const;
    bool isMathOp(const std::string& token) const;
    bool isGoToCell(const std::string& token) const;
    bool isReturnCell(const std::string& token) const;
    bool isCallCeil(const std::string& token) const;
    bool isConditionalJumpCell(const std::string& token) const;
    bool isLabelCell(const std::string& token) const;
    bool isNumber(const std::string& token) const;

};

#endif // RPN_H