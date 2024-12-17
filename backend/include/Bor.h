#include <string>
#include <utility>
#include "Lexem.h"

struct vertex {
  vertex* to[256] = {nullptr};
  bool terminal = false;
};

class bor {
 public:
  void add(const std::string& s) {
    vertex* v = v0;
    for (char c : s) {
      unsigned char uc = static_cast<unsigned char>(c);
      if (!v->to[uc]) {
        v->to[uc] = new vertex;
      }
      v = v->to[uc];
    }
    v->terminal = true;
  }

  void print() { printVertex(v0); }

  std::pair<bool, Lexem> has(const char* s, size_t size, size_t s_index) {
    vertex* cur = v0;
    std::string line;

    for (size_t i = 0; i < size; ++i) {
      unsigned char c = static_cast<unsigned char>(s[i]);
      if (!cur->to[c]) {
        return {false,
                Lexem(LexemType::KEYWORD, line, s_index, s_index + i, 0)};
      }
      cur = cur->to[c];
      line.push_back(s[i]);

      if (cur->terminal && i == size - 1) {
        return {true,
                Lexem(LexemType::KEYWORD, line, s_index, s_index + i + 1, 0)};
      }
    }
    return {false, Lexem(LexemType::KEYWORD, line, s_index, s_index + size, 0)};
  }

  ~bor() { deleteVertex(v0); }

 private:
  vertex* v0 = new vertex;

  void printVertex(vertex* v) {
    if (!v)
      return;
    for (auto& next : v->to) {
      if (next) {
        printVertex(next);
      }
    }
  }

  void deleteVertex(vertex* v) {
    if (!v)
      return;
    for (auto& next : v->to) {
      if (next) {
        deleteVertex(next);
      }
    }
    delete v;
  }
};