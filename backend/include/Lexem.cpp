//
// Created by Синицын Алексей on 21.10.2024.
//

#include "Lexem.h"
#include <iostream>
#include <httplib.h>
#include <json.h>

void Lexem::set_type(std::string tp) {
    type_ = tp;
}

void Lexem::set_text(std::string txt) {
    text_ = txt;
}

void Lexem::set_y(int y) {
    line_ = y;
}

void Lexem::set_x(int x) {
    pos_ = x;
}

std::string Lexem::get_type() {
    return type_;
}

std::string Lexem::get_text() {
    return text_;
}

int Lexem::get_y() {
    return line_;
}

int Lexem::get_x() {
    return pos_;
}