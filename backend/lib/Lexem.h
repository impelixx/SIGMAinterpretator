#include <iostream>

#ifndef BACKEND_LEXEM_H
#define BACKEND_LEXEM_H


class Lexem {
public:
    void set_type(std::string tp);
    void set_text(std::string txt);
    void set_s(int s);
    void set_e(int e);
    std::string get_type();
    std::string get_text();
    int get_s();
    int get_e();
private:
    std::string type_ = "";
    std::string text_ = "";
    int s_ = 0;
    int e_ = 0;
};


#endif //BACKEND_LEXEM_H
