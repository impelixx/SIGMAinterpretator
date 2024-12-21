#include <string>
#include <utility>
#include "Lexem.h"

/**
 * @brief Represents a vertex (node) in a trie data structure
 * 
 * A vertex in the trie contains:
 * - An array of pointers to child vertices (256 possible children for each ASCII character)
 * - A boolean flag indicating whether this vertex represents the end of a word
 */
struct vertex {
  vertex* to[256] = {nullptr};
  bool terminal = false;
};

/**
 * @file Bor.h
 * @brief Implementation of a trie (prefix tree) data structure for string matching
 * 
 * This file contains the implementation of a trie data structure used for efficient
 * string matching and storage. The trie enables quick insertion and lookup of strings,
 * making it particularly useful for keyword recognition in lexical analysis.
 * 
 * The trie supports:
 * - Adding new strings (O(m) time complexity, where m is string length)
 * - Checking for string existence (O(m) time complexity)
 * - String matching with lexeme generation
 * 
 * The implementation uses dynamic memory allocation for vertices and includes proper
 * cleanup through a destructor.
 * 
 * @note The implementation assumes ASCII character set (256 possible characters)
 * @see vertex class for the node structure used in the trie
 */
class bor {
 public:
  /**
   * @brief Adds a string to the trie data structure
   * 
   * This function traverses the trie character by character, creating new vertices
   * as needed, and marks the last vertex as terminal.
   * 
   * @param s The string to be added to the trie
   * 
   * Time Complexity: O(|s|) where |s| is the length of the input string
   * Space Complexity: O(|s|) in worst case when creating new vertices
   */
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

  /**
   * @brief Checks if a given string exists in the trie and returns the corresponding lexeme
   * @param s Pointer to the character array to check
   * @param size Length of the string to check
   * @param s_index Starting index of the string in the source code
   * @return std::pair<bool, Lexem> First element indicates if string was found,
   *         second element contains the constructed Lexem regardless of whether string was found
   * 
   * The function traverses the trie character by character checking if the given string
   * exists. If at any point a character is not found in the trie, returns false with
   * a partial lexeme. If the entire string is found and ends at a terminal node,
   * returns true with the complete lexeme. Otherwise returns false with the complete
   * but non-terminal lexeme.
   */
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

  /**
   * @brief Recursively traverses and prints the vertices of a trie data structure
   * 
   * This function performs a depth-first traversal of the trie starting from the given vertex.
   * It visits each child vertex and recursively processes their children.
   * 
   * @param v Pointer to the current vertex in the trie
   * @note If the input vertex is nullptr, the function returns without doing anything
   */
  void printVertex(vertex* v) {
    if (!v)
      return;
    for (auto& next : v->to) {
      if (next) {
        printVertex(next);
      }
    }
  }

  /**
   * @brief Recursively deletes a vertex and all its descendants in the trie
   * 
   * This function performs a post-order traversal of the trie starting from the given vertex,
   * deleting all child vertices before deleting the vertex itself. This ensures proper cleanup
   * of the entire subtree rooted at the given vertex.
   * 
   * @param v Pointer to the vertex to be deleted
   */
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