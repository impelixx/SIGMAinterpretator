#include "SyntaxAnalyzer.h"

bool SyntaxAnalyzer::Analyze() {
    AnalyzeProgram();
    return true;
}

bool SyntaxAnalyzer::AnalyzeProgram() {
    AnalyzeStatementList();
    return true;
}

bool SyntaxAnalyzer::AnalyzeStatementList() {
    while (AnalyzeStatement()) {}
    return true;
}

bool SyntaxAnalyzer::AnalyzeStatement() {
    return AnalyzeFunctionDeclaration()
        || AnalyzeVariableDeclaration()
        || AnalyzeAssignment()
        || AnalyzeIfStatement()
        || AnalyzeWhileStatement()
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

    try {
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
                                            ++LexemIndex;
                                            if (lex[LexemIndex].get_type() == "DEDENT") {
                                                ++LexemIndex;
                                                return true;
                                            }
                                            else {
                                                LexemIndex = LastLexemIndex;
                                                // throw
                                                return false;
                                            }
                                        }
                                        else {
                                            LexemIndex = LastLexemIndex;
                                            // throw
                                            return false;
                                        }
                                    }
                                    else {
                                        LexemIndex = LastLexemIndex;
                                        // throw
                                        return false;
                                    }
                                }
                                else {
                                    LexemIndex = LastLexemIndex;
                                    // throw
                                    return false;
                                }
                            }
                            else {
                                LexemIndex = LastLexemIndex;
                                // throw
                                return false;
                            }
                        }
                        else {
                            LexemIndex = LastLexemIndex;
                            // throw
                            return false;
                        }
                    }
                    else {
                        LexemIndex = LastLexemIndex;
                        // throw
                        return false;
                    }
                }
                else {
                    LexemIndex = LastLexemIndex;
                    // throw
                    return false;
                }
            }
            else {
                LexemIndex = LastLexemIndex;
                // throw
                return false;
            }
        }
        else {
            return false;
        }
    } catch (const std::exception& e) {
        LexemIndex = LastLexemIndex;
        return false;
    };
}

bool SyntaxAnalyzer::AnalyzeParameterList() {

}

bool SyntaxAnalyzer::AnalyzeVariableDeclaration() {

}

bool SyntaxAnalyzer::AnalyzeInitianalizerList() {

}

bool SyntaxAnalyzer::AnalyzeAssignment() {

}

bool SyntaxAnalyzer::AnalyzeIfStatement() {

}

bool SyntaxAnalyzer::AnalyzeWhileStatement() {

}

bool SyntaxAnalyzer::AnalyzePrintStatement() {

}

bool SyntaxAnalyzer::AnalyzeReturnStatement() {

}

bool SyntaxAnalyzer::AnalyzeExpression() {

}

bool SyntaxAnalyzer::AnalyzeTerm() {

}

bool SyntaxAnalyzer::AnalyzeFactor() {

}

bool SyntaxAnalyzer::AnalyzeType() {
    if (lex[LexemIndex].get_text() == "BOOL"
        || lex[LexemIndex].get_text() == "INT"
        || lex[LexemIndex].get_text() == "FLOAT"
        || lex[LexemIndex].get_text() == "CHAR"
        || lex[LexemIndex].get_text() == "STRING") {
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