//
// Created by Синицын Алексей on 21.10.2024.
//
#include <iostream>
#include <httplib.h>
#include <json.h>

#ifndef BACKEND_LEXEM_H
#define BACKEND_LEXEM_H


class Lexem {
public:
    void set_type(std::string tp);
    void set_text(std::string txt);
    void set_y(int y);
    void set_x(int x);
    std::string get_type();
    std::string get_text();
    int get_y();
    int get_x();
private:
    std::string type_ = "";
    std::string text_ = "";
    int line_ = 0;
    int pos_ = 0;
};


#endif //BACKEND_LEXEM_H
