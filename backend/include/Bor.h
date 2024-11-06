#include <utility>
#include <string>
#include "Lexem.h"

struct vertex {
    vertex* to[256] = {nullptr};
    bool terminal = false;
};

class bor {
public:
    void add(const std::string& s) {
        vertex* v = v0;
        for (char c: s) {
            if (!v->to[static_cast<unsigned char>(c)]) {
                v->to[static_cast<unsigned char>(c)] = new vertex;
            }
            v = v->to[static_cast<unsigned char>(c)];
        }
        v->terminal = true;
    }

    std::pair<bool, Lexem> has(const char* s, size_t size, size_t s_index) {
        vertex* cur = v0;
        std::string line;

        for(size_t i = 0; i < size; ++i) {
            unsigned char c = static_cast<unsigned char>(s[i]);
            if (!cur->to[c]) {
                return {false, Lexem(LexemType::KEYWORD, line, s_index, s_index + i)};
            }
            cur = cur->to[c];
            line.push_back(s[i]);
            
            if (cur->terminal && i == size - 1) {
                return {true, Lexem(LexemType::KEYWORD, line, s_index, s_index + i + 1)};
            }
        }

        return {false, Lexem(LexemType::KEYWORD, line, s_index, s_index + size)};
    }

    ~bor() {
        deleteVertex(v0);
    }

private:
    vertex* v0 = new vertex;

    void deleteVertex(vertex* v) {
        if (!v) return;
        for (auto& next : v->to) {
            if (next) {
                deleteVertex(next);
            }
        }
        delete v;
    }
};
