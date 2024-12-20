#ifndef BACKEND_SEMANTIC_H
#define BACKEND_SEMANTIC_H
#include <vector>
#include "Lexem.h"

/**
 * @brief Class responsible for performing semantic analysis on lexems
 * 
 * The Semantic class takes a vector of lexems and provides functionality
 * to analyze them for semantic correctness according to the language rules.
 */
class Semantic {
 public:
  explicit Semantic(std::vector<Lexem> lexems) { lex_ = lexems; };

  bool Analyze();
  int GetLine(int index);

 private:
  std::vector<Lexem> lex_;
};

#endif  //BACKEND_SEMANTIC_H
