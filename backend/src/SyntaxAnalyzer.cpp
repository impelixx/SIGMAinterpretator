#include "SyntaxAnalyzer.h"
#include <string>

std::string SyntaxAnalyzer::NumLine() {
    int index = LexemIndex;
    int s = 1;
    for (int i = 0; i < index; ++i) {
        if (lex[i].get_type() == "NEWLINE") {
            ++s;
        }
    }

    return std::to_string(s);
}

bool SyntaxAnalyzer::Analyze() {
    AnalyzeProgram();
    return true;
}

bool SyntaxAnalyzer::AnalyzeProgram() {
    AnalyzeStatementList();
    return true;
}

bool SyntaxAnalyzer::AnalyzeStatementList() {
    bool b = false;
    bool c = AnalyzeStatement();
    while (c) {
        b = b || c;
        c = AnalyzeStatement();
    }
    b = b || c;
    return b;
}

bool SyntaxAnalyzer::AnalyzeStatement() {
    return AnalyzeFunctionDeclaration()
        || AnalyzeVariableDeclaration()
        || AnalyzeAssignment()
        || AnalyzeIfStatement()
        || AnalyzeWhileStatement()
        || AnalyzeDoWhileStatement()
        || AnalyzeForStatement()
        || AnalyzePrintStatement()
        || AnalyzeReturnStatement()
        || AnalyzeEmptyStatement();
}

bool SyntaxAnalyzer::AnalyzeEmptyStatement() {
    if (lex[LexemIndex].get_text() == ";" || lex[LexemIndex].get_type() == "NEWLINE") {
        ++LexemIndex;
        return true;
    }
    return false;
}

bool SyntaxAnalyzer::AnalyzeFunctionDeclaration() {
    int LastLexemIndex = LexemIndex;

    if (lex[LexemIndex].get_text() == "def") {
        ++LexemIndex;
        if (AnalyzeIdentifier()) { // тут делаем ++LexemIndex
            if (lex[LexemIndex].get_text() == "(") {
                ++LexemIndex;
                if (AnalyzeParameterList()) { // тут делаем ++LexemIndex
                    if (lex[LexemIndex].get_text() == ")") {
                        ++LexemIndex;
                        if (lex[LexemIndex].get_text() == ":") {
                            ++LexemIndex;
                            if (lex[LexemIndex].get_type() == "NEWLINE") {
                                ++LexemIndex;
                                if (lex[LexemIndex].get_type() == "INDENT") {
                                    ++LexemIndex;
                                    if (AnalyzeStatementList()) {
                                        if (lex[LexemIndex].get_type() == "DEDENT") {
                                            ++LexemIndex;
                                            return true;
                                        }
                                        else {
                                            LexemIndex = LastLexemIndex;
                                            throw std::runtime_error( "Error in AnalyzeFunctionDeclaration() error in " + NumLine() + " line");
                                            return false;
                                        }
                                    }
                                    else {
                                        LexemIndex = LastLexemIndex;
                                        throw std::runtime_error( "Error in AnalyzeFunctionDeclaration() error in " + NumLine() + " line");
                                        return false;
                                    }
                                }
                                else {
                                    LexemIndex = LastLexemIndex;
                                    throw std::runtime_error( "Error in AnalyzeFunctionDeclaration() error in " + NumLine() + " line");
                                    return false;
                                }
                            }
                            else {
                                LexemIndex = LastLexemIndex;
                                throw std::runtime_error( "Error in AnalyzeFunctionDeclaration() error in " + NumLine() + " line");
                                return false;
                            }
                        }
                        else {
                            LexemIndex = LastLexemIndex;
                            throw std::runtime_error( "Error in AnalyzeFunctionDeclaration() error in " + NumLine() + " line");
                            return false;
                        }
                    }
                    else {
                        LexemIndex = LastLexemIndex;
                        throw std::runtime_error( "Error in AnalyzeFunctionDeclaration() error in " + NumLine() + " line");
                        return false;
                    }
                }
                else {
                    LexemIndex = LastLexemIndex;
                    throw std::runtime_error( "Error in AnalyzeFunctionDeclaration() error in " + NumLine() + " line");
                    return false;
                }
            }
            else {
                LexemIndex = LastLexemIndex;
                throw std::runtime_error( "Error in AnalyzeFunctionDeclaration() error in " + NumLine() + " line");
                return false;
            }
        }
        else {
            LexemIndex = LastLexemIndex;
            throw std::runtime_error( "Error in AnalyzeFunctionDeclaration() error in " + NumLine() + " line");
            return false;
        }
    }
    else {
        return false;
    }
}

bool SyntaxAnalyzer::AnalyzeParameterList() {
    int LastLexemIndex = LexemIndex;

    if (lex[LexemIndex].get_text() == ")") {
        return true;
    }

    if (lex[LexemIndex - 1].get_text() == "(") {
        if (AnalyzeIdentifier()) {
            while (lex[LexemIndex].get_text() == ",") {
                ++LexemIndex;
                if (!AnalyzeIdentifier()) {
                    LexemIndex = LastLexemIndex;
                    throw std::runtime_error( "Error in AnalyzeParameterList() error in " + NumLine() + " line");
                    return false;
                }
            }

            if (lex[LexemIndex].get_text() == ")") {
                return true;
            }
            else {
                LexemIndex = LastLexemIndex;
                throw std::runtime_error( "Error in AnalyzeParameterList() error in " + NumLine() + " line");
                return false;
            }
        }
        else {
            LexemIndex = LastLexemIndex;
            return false;
        }
    }

    return false;
}

bool SyntaxAnalyzer::AnalyzeVariableDeclaration() {
    int LastLexemIndex = LexemIndex;
    // Первый вариант: <Type> + <Identifier> + "=" + <Expression> + <StatementTerminator>
    if (AnalyzeType()) {
        if (AnalyzeIdentifier()) {
            if (lex[LexemIndex].get_text() == "=") {
                ++LexemIndex;
                if (AnalyzeExpression()) {
                    if (AnalyzeStatementTerminator()) {
                        return true;
                    }
                    else {
                        LexemIndex = LastLexemIndex;
                        throw std::runtime_error( "Error in AnalyzeVariableDeclaration() (Terminator) error in " + NumLine() + " line");
                        return false;
                    }
                }
                else {
                    LexemIndex = LastLexemIndex;
                    throw std::runtime_error( "Error in AnalyzeVariableDeclaration() (Expression) error in " + NumLine() + " line");
                    return false;
                }
            }
            else {
                LexemIndex = LastLexemIndex;
            }
        }
        else {
            LexemIndex = LastLexemIndex;
            return false;
        }
    }

    // Второй вариант: <Type> + <Identifier> + "[" + <Number> + "]" + "=" + <InitializerList> + <StatementTerminator>
    if (AnalyzeType()) {
        if (AnalyzeIdentifier()) {
            if (lex[LexemIndex].get_text() == "[") {
                ++LexemIndex;
                if (AnalyzeExpression()) {
                    if (lex[LexemIndex].get_text() == "]") {
                        ++LexemIndex;
                        if (lex[LexemIndex].get_text() == "=") {
                            ++LexemIndex;
                            if (AnalyzeInitianalizerList()) {
                                if (AnalyzeStatementTerminator()) {
                                    return true;
                                }
                                else {
                                    LexemIndex = LastLexemIndex;
                                    throw std::runtime_error( "Error in AnalyzeVariableDeclaration() (Terminator in []) error in " + NumLine() + " line");
                                    return false;
                                }
                            }
                            else {
                                LexemIndex = LastLexemIndex;
                                throw std::runtime_error( "Error in AnalyzeVariableDeclaration() (InitList {}) error in " + NumLine() + " line");
                                return false;
                            }
                        }
                        else {
                            LexemIndex = LastLexemIndex;
                            throw std::runtime_error( "Error in AnalyzeVariableDeclaration() (in =) error in " + NumLine() + " line");
                            return false;
                        }
                    }
                    else {
                        LexemIndex = LastLexemIndex;
                        throw std::runtime_error( "Error in AnalyzeVariableDeclaration() (]) error in " + NumLine() + " line");
                        return false;
                    }
                }
                else {
                    LexemIndex = LastLexemIndex;
                    throw std::runtime_error( "Error in AnalyzeVariableDeclaration() ({NUMBER}) error in " + NumLine() + " line");
                    return false;
                }
            }
            else {
                LexemIndex = LastLexemIndex;
                return false;
            }
        }
        else {
            LexemIndex = LastLexemIndex;
            return false;
        }
    }

    return false;
}

bool SyntaxAnalyzer::AnalyzeInitianalizerList() {
    int LastLexemIndex = LexemIndex;

    if (lex[LexemIndex].get_text() == "{") {
        ++LexemIndex;
        if (AnalyzeExpression()) {
            while (lex[LexemIndex].get_text() == ",") {
                ++LexemIndex;
                if (!AnalyzeExpression()) {
                    LexemIndex = LastLexemIndex;
                    throw std::runtime_error( "Error in AnalyzeInitianalizerList()(Cycle Expression) error in " + NumLine() + " line");
                    return false;
                }
            }
            if (lex[LexemIndex].get_text() == "}") {
                ++LexemIndex;
                return true;
            }
            else {
                LexemIndex = LastLexemIndex;
                throw std::runtime_error( "Error in AnalyzeInitianalizerList() (}) error in " + NumLine() + " line");
                return false;
            }
        }
        else {
            LexemIndex = LastLexemIndex;
            return false;
        }
    }

    return false;
}

bool SyntaxAnalyzer::AnalyzeAssignment() {
    int LastLexemIndex = LexemIndex;

    if (AnalyzeIdentifier()) {
        if (lex[LexemIndex].get_text() == "[") {
            ++LexemIndex;
            if (AnalyzeNumber()) {
                if (lex[LexemIndex].get_text() == "]") {
                    ++LexemIndex;
                }
                else {
                    LexemIndex = LastLexemIndex;
                    throw std::runtime_error( "Error in AnalyzeAssignment() error in " + NumLine() + " line");
                    return false;
                }
            }
            else {
                LexemIndex = LastLexemIndex;
                throw std::runtime_error( "Error in AnalyzeAssignment() error in " + NumLine() + " line");
                return false;
            }
        }
    }
    else {
        LexemIndex = LastLexemIndex;
        return false;
    }

    if (lex[LexemIndex].get_text() == "=") {
        ++LexemIndex;
    }
    else {
        LexemIndex = LastLexemIndex;
        throw std::runtime_error( "Error in AnalyzeAssignment() error in " + NumLine() + " line");
        return false;
    }

    if (!AnalyzeExpression()) {
        LexemIndex = LastLexemIndex;
        throw std::runtime_error( "Error in AnalyzeAssignment() error in " + NumLine() + " line");
        return false;
    }

    if (AnalyzeStatementTerminator()) {
        return true;
    }
    else {
        LexemIndex = LastLexemIndex;
        throw std::runtime_error( "Error in AnalyzeAssignment() error in " + NumLine() + " line");
        return false;
    }
}

bool SyntaxAnalyzer::AnalyzeIfStatement() {
    int LastLexemIndex = LexemIndex;  // Сохраняем текущий индекс лексем

    if (lex[LexemIndex].get_text() == "if") {
        ++LexemIndex;
        if (AnalyzeCaseExpression()) {
            if (AnalyzeFieldView()) {
                return true;
            }
            else {
                LexemIndex = LastLexemIndex;
                throw std::runtime_error( "Error in AnalyzeIfStatement() error in " + NumLine() + " line");
                return false;
            }
        }
        else {
            LexemIndex = LastLexemIndex;
            throw std::runtime_error( "Error in AnalyzeIfStatement() error in " + NumLine() + " line");
            return false;
        }
    }

    return false;
}

bool SyntaxAnalyzer::AnalyzeWhileStatement() {
    int LastLexemIndex = LexemIndex;

    if (lex[LexemIndex].get_text() == "while") {
        ++LexemIndex;
        if (AnalyzeCaseExpression()) {
            if (AnalyzeFieldView()) {
                return true;
            }
            else {
                LexemIndex = LastLexemIndex;
                throw std::runtime_error( "Error in AnalyzeWhileStatement() error in " + NumLine() + " line");
                return false;
            }
        }
        else {
            LexemIndex = LastLexemIndex;
            throw std::runtime_error( "Error in AnalyzeWhileStatement() error in " + NumLine() + " line");
            return false;
        }
    }
    return false;
}

bool SyntaxAnalyzer::AnalyzeDoWhileStatement() {
    int LastLexemIndex = LexemIndex;

    if (lex[LexemIndex].get_text() == "do") {
        ++LexemIndex;
        if (AnalyzeFieldView()) {
            if (lex[LexemIndex].get_text() == "while") {
                ++LexemIndex;
                if (AnalyzeCaseExpression()) {
                    if (AnalyzeStatementTerminator()) {
                        return true;
                    }
                    else {
                        LexemIndex = LastLexemIndex;
                        throw std::runtime_error( "Error in AnalyzeDoWhileStatement() error in " + NumLine() + " line");
                        return false;
                    }
                }
                else {
                    LexemIndex = LastLexemIndex;
                    throw std::runtime_error( "Error in AnalyzeDoWhileStatement() error in " + NumLine() + " line");
                    return false;
                }
            }
            else {
                LexemIndex = LastLexemIndex;
                throw std::runtime_error( "Error in AnalyzeDoWhileStatement() error in " + NumLine() + " line");
                return false;
            }
        }
        else {
            LexemIndex = LastLexemIndex;
            throw std::runtime_error( "Error in AnalyzeDoWhileStatement() error in " + NumLine() + " line");
            return false;
        }
    }
    return false;
}

bool SyntaxAnalyzer::AnalyzeForStatement() {
    int LastLexemIndex = LexemIndex;

    if (lex[LexemIndex].get_text() == "for") {
        ++LexemIndex;
        if (AnalyzeType()) {
            if (AnalyzeIdentifier()) {
                if (lex[LexemIndex].get_text() == "in") {
                    ++LexemIndex;
                    if (lex[LexemIndex].get_text() == "(") {
                        ++LexemIndex;
                        if (AnalyzeNumber()) {
                            if (lex[LexemIndex].get_text() == ",") {
                                ++LexemIndex;
                                if (AnalyzeNumber()) {
                                    if (lex[LexemIndex].get_text() == ")") {
                                        ++LexemIndex;
                                        if (AnalyzeFieldView()) {
                                            return true;
                                        }
                                        else {
                                            LexemIndex = LastLexemIndex;
                                            throw std::runtime_error( "Error in AnalyzeForStatement() error in " + NumLine() + " line");
                                            return false;
                                        }
                                    }
                                    else {
                                        LexemIndex = LastLexemIndex;
                                        throw std::runtime_error( "Error in AnalyzeForStatement() error in " + NumLine() + " line");
                                        return false;
                                    }
                                }
                                else {
                                    LexemIndex = LastLexemIndex;
                                    throw std::runtime_error( "Error in AnalyzeForStatement() error in " + NumLine() + " line");
                                    return false;
                                }
                            }
                            else {
                                LexemIndex = LastLexemIndex;
                                throw std::runtime_error( "Error in AnalyzeForStatement() error in " + NumLine() + " line");
                                return false;
                            }
                        }
                        else {
                            LexemIndex = LastLexemIndex;
                            throw std::runtime_error( "Error in AnalyzeForStatement() error in " + NumLine() + " line");
                            return false;
                        }
                    }
                    else {
                        LexemIndex = LastLexemIndex;
                        throw std::runtime_error( "Error in AnalyzeForStatement() error in " + NumLine() + " line");
                        return false;
                    }
                }
                else {
                    LexemIndex = LastLexemIndex;
                    throw std::runtime_error( "Error in AnalyzeForStatement() error in " + NumLine() + " line");
                    return false;
                }
            }
            else {
                LexemIndex = LastLexemIndex;
                throw std::runtime_error( "Error in AnalyzeForStatement() error in " + NumLine() + " line");
                return false;
            }
        }
        else {
            LexemIndex = LastLexemIndex;
            throw std::runtime_error( "Error in AnalyzeForStatement() error in " + NumLine() + " line");
            return false;
        }
    }

    return false;
}

bool SyntaxAnalyzer::AnalyzeFieldView() {
    int LastLexemIndex = LexemIndex;
    if (lex[LexemIndex].get_text() == ":") {
        ++LexemIndex;
        if (lex[LexemIndex].get_type() == "NEWLINE") {
            ++LexemIndex;
            if (lex[LexemIndex].get_type() == "INDENT") {
                ++LexemIndex;
                if (AnalyzeStatementList()) {
                    if (lex[LexemIndex].get_type() == "DEDENT") {
                        ++LexemIndex;
                        return true;
                    }
                    else {
                        LexemIndex = LastLexemIndex;
                        throw std::runtime_error( "Error in AnalyzeFieldView() error in " + NumLine() + " line");
                        return false;
                    }
                }
                else {
                    LexemIndex = LastLexemIndex;
                    throw std::runtime_error( "Error in AnalyzeFieldView() error in " + NumLine() + " line");
                    return false;
                }
            }
            else {
                LexemIndex = LastLexemIndex;
                throw std::runtime_error( "Error in AnalyzeFieldView() error in " + NumLine() + " line");
                return false;
            }
        }
        else {
            LexemIndex = LastLexemIndex;
            throw std::runtime_error( "Error in AnalyzeFieldView() error in " + NumLine() + " line");
            return false;
        }
    }

    return false;
}

bool SyntaxAnalyzer::AnalyzePrintStatement() {
    int LastLexemIndex = LexemIndex;

    if (lex[LexemIndex].get_text() == "print") {
        ++LexemIndex;
        if (lex[LexemIndex].get_text() == "(") {
            ++LexemIndex;
            if (AnalyzeExpression()) {
                if (lex[LexemIndex].get_text() == ")") {
                    ++LexemIndex;
                    if (AnalyzeStatementTerminator()) {
                        return true;
                    }
                    else {
                        LexemIndex = LastLexemIndex;
                        throw std::runtime_error( "Error in AnalyzePrintStatement() error in " + NumLine() + " line");
                        return false;
                    }
                } else {
                    LexemIndex = LastLexemIndex;
                    throw std::runtime_error( "Error in AnalyzePrintStatement() error in " + NumLine() + " line");
                    return false;
                }
            } else {
                LexemIndex = LastLexemIndex;
                throw std::runtime_error( "Error in AnalyzePrintStatement() error in " + NumLine() + " line");
                return false;
            }
        } else {
            LexemIndex = LastLexemIndex;
            throw std::runtime_error( "Error in AnalyzePrintStatement() error in " + NumLine() + " line");
            return false;
        }
    }

    return false;
}

bool SyntaxAnalyzer::AnalyzeReturnStatement() {
    int LastLexemIndex = LexemIndex;

    if (lex[LexemIndex].get_text() == "return") {
        ++LexemIndex;
        if (AnalyzeCaseExpression()) {
            if (AnalyzeStatementTerminator()) {
                return true;
            }
            else {
                LexemIndex = LastLexemIndex;
                throw std::runtime_error( "Error in AnalyzeReturnStatement() error in " + NumLine() + " line");
                return false;
            }
        }
        else {
            LexemIndex = LastLexemIndex;
            throw std::runtime_error( "Error in AnalyzeReturnStatement() error in " + NumLine() + " line");
            return false;
        }
    }
    return false;
}

bool SyntaxAnalyzer::AnalyzeCaseExpression() {
    int LastLexemIndex = LexemIndex;

    if (lex[LexemIndex].get_text() == "(") {
        ++LexemIndex;
        if (AnalyzeExpression()) {
            while (lex[LexemIndex].get_text() == "and" || lex[LexemIndex].get_text() == "or") {
                ++LexemIndex;
                if (!AnalyzeExpression()) {
                    LexemIndex = LastLexemIndex;
                    throw std::runtime_error( "Error in AnalyzeCaseExpression() error in " + NumLine() + " line");
                    return false;
                }
            }

            if (lex[LexemIndex].get_text() == ")") {
                ++LexemIndex;
                return true;
            }
            else {
                LexemIndex = LastLexemIndex;
                throw std::runtime_error( "Error in AnalyzeCaseExpression() error in " + NumLine() + " line");
                return false;
            }
        }
        else {
            LexemIndex = LastLexemIndex;
            throw std::runtime_error( "Error in AnalyzeCaseExpression() error in " + NumLine() + " line");
            return false;
        }
    }
    else {
        if (AnalyzeExpression()) {
            while (lex[LexemIndex].get_text() == "and" || lex[LexemIndex].get_text() == "or") {
                ++LexemIndex;
                if (!AnalyzeExpression()) {
                    LexemIndex = LastLexemIndex;
                    throw std::runtime_error( "Error in AnalyzeCaseExpression() error in " + NumLine() + " line");
                    return false;
                }
            }
        }
    }
    return false;
}

bool SyntaxAnalyzer::AnalyzeExpression() {
    int LastLexemIndex = LexemIndex;

    if (AnalyzeTerm()) {
        while (lex[LexemIndex].get_text() == "+" || lex[LexemIndex].get_text() == "-") {
            ++LexemIndex;
            if (!AnalyzeTerm()) {
                LexemIndex = LastLexemIndex;
                throw std::runtime_error( "Error in AnalyzeExpression() error in " + NumLine() + " line");
                return false;
            }
        }
        return true;
    }
    return false;
}

bool SyntaxAnalyzer::AnalyzeTerm() {
    int LastLexemIndex = LexemIndex;
    if (AnalyzeFactor()) {
        while (lex[LexemIndex].get_text() == "*" || lex[LexemIndex].get_text() == "/") {
            ++LexemIndex;

            if (!AnalyzeFactor()) {
                LexemIndex = LastLexemIndex;
                throw std::runtime_error( "Error in AnalyzeTerm() error in " + NumLine() + " line");
                return false;
            }
        }
        return true;
    }
    return false;
}

bool SyntaxAnalyzer::AnalyzeFactor() {
    int LastLexemIndex = LexemIndex;

        if (AnalyzeNumber()) {
            return true;
        }

        if (AnalyzeIdentifier()) {
            return true;
        }

        if (lex[LexemIndex].get_text() == "(") {
            ++LexemIndex;
            if (AnalyzeExpression()) {
                if (lex[LexemIndex].get_text() == ")") {
                    ++LexemIndex;
                    return true;
                }
                else {
                    LexemIndex = LastLexemIndex;
                    throw std::runtime_error( "Error in AnalyzeFactor() error in " + NumLine() + " line");
                    return false;
                }
            }
            else {
                LexemIndex = LastLexemIndex;
                throw std::runtime_error( "Error in AnalyzeFactor() error in " + NumLine() + " line");
                return false;
            }
        }
        return false;
}

bool SyntaxAnalyzer::AnalyzeType() {
    if (lex[LexemIndex].get_text() == "bool"
        || lex[LexemIndex].get_text() == "int"
        || lex[LexemIndex].get_text() == "float"
        || lex[LexemIndex].get_text() == "char"
        || lex[LexemIndex].get_text() == "string") {
        ++LexemIndex;
        return true;
    }
    return false;
}

bool SyntaxAnalyzer::AnalyzeIdentifier() {
    if (lex[LexemIndex].get_type() == "IDENTIFIER") {
        ++LexemIndex;
        return true;
    }
    return false;
}

bool SyntaxAnalyzer::AnalyzeNumber() {
    if (lex[LexemIndex].get_type() == "NUMBER") {
        ++LexemIndex;
        return true;
    }
    return false;
}

bool SyntaxAnalyzer::AnalyzeStatementTerminator() {
    if (lex[LexemIndex].get_text() == ";" || lex[LexemIndex].get_type() == "NEWLINE") {
        ++LexemIndex;
        return true;
    }
    return false;
}