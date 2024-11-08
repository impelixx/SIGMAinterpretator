#include <iostream>

#ifndef BACKEND_LEXEM_H
#define BACKEND_LEXEM_H

enum LexemType {
    KEYWORD,    // Ключевые слова (например, int, float, def, class)
    NUMBER,     // Числовые литералы
    REL_OP,     // Операторы сравнения (==, !=, >=, <=, >, <)
    OPERATOR,   // Арифметические и логические операторы (+, -, *, /, %, &&, ||, !, =, +=, -=, *=, /=, %=)
    STRING,     // Строковые литералы
    IDENTIFIER, // Идентификаторы (имена переменных и функций)
    NEWLINE,    // Перенос строки
    COMMENT,    // Комментарии
    DEDENT,     // Уменьшение отступа
    BRACKET,    // Скобки
    INDENT,     // Увеличение отступа
    EOS,        // End of string
    UNKNOWN     // Неизвестные токен
};


class Lexem {
public:
    Lexem(LexemType type, std::string text, int s, int e);
    // Getters
    std::string get_type() const;
    std::string get_text() const;
    // Setters
    void set_text(std::string text);
private:
    LexemType type_;
    std::string text_;
};


#endif //BACKEND_LEXEM_H
