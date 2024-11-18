#ifndef BACKEND_SEMANTIC_H
#define BACKEND_SEMANTIC_H
#include <vector>
#include "Lexem.h"

class Semantic {
public:
    explicit Semantic(std::vector<Lexem> lexems) {lex_ = lexems;};
    bool Analyze();
private:
    std::vector<Lexem> lex_;
};

#endif //BACKEND_SEMANTIC_H
