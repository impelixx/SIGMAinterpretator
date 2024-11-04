#include "Lexem.h"
#include <iostream>

void Lexem::set_type(std::string tp) {
    type_ = tp;
}

void Lexem::set_text(std::string txt) {
    text_ = txt;
}

void Lexem::set_s(int s) {
    s_ = s;
}

void Lexem::set_e(int e) {
    e_ = e;
}

std::string Lexem::get_type() {
    return type_;
}

std::string Lexem::get_text() {
    return text_;
}

int Lexem::get_s() {
    return s_;
}

int Lexem::get_e() {
    return e_;
}