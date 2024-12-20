#include <string>
#include <utility>
#include "Lexem.h"

/**
 * @brief A structure representing a vertex in a trie (prefix tree) data structure
 * 
 * Each vertex contains:
 * - An array of 256 pointers to other vertices, representing transitions for each possible byte value
 * - A boolean flag indicating whether this vertex represents the end of a pattern
 */
struct vertex {
  vertex* to[256] = {nullptr};
  bool terminal = false;
};

/**
 * @brief A trie (prefix tree) data structure implementation for string storage and retrieval
 * 
 * The bor class implements a trie data structure that efficiently stores and retrieves
 * strings. It supports operations such as adding new strings, checking for string
 * existence, and automatic memory management.
 * 
 * Key Features:
 * - Efficient string insertion and lookup
 * - Memory-safe implementation with proper cleanup
 * - Support for lexeme generation during string matching
 * 
 * Memory Management:
 * The class automatically manages memory for all vertices in the trie,
 * cleaning up allocated memory in its destructor.
 * 
 * Usage:
 * - Use add() to insert new strings into the trie
 * - Use has() to check if a string exists and get corresponding lexeme
 * - Use print() to traverse and display the trie structure
 * 
 * Time Complexity:
 * - Insertion: O(m) where m is the length of the string
 * - Lookup: O(m) where m is the length of the string
 * - Space Complexity: O(ALPHABET_SIZE * n) where n is the total length of all strings
 */
/**
 * @file Bor.h
 * @brief Implementation of a Trie (prefix tree) data structure
 * 
 * This file contains the implementation of a Trie data structure, also known as a prefix tree,
 * which is used for efficient storage and retrieval of strings. The name 'bor' comes from the
 * Russian transliteration of the word 'bore' (forest/tree).
 * 
 * The Trie structure supports:
 * - Adding strings
 * - Checking for string existence
 * - Printing the tree structure
 * 
 * Key Features:
 * - Efficient prefix-based string search
 * - Memory-efficient storage of strings with common prefixes
 * - Support for lexeme creation during string matching
 * 
 * Time Complexities:
 * - Insertion: O(m) where m is the length of the string
 * - Search: O(m) where m is the length of the string
 * 
 * Space Complexity:
 * - O(ALPHABET_SIZE * n) where n is the total length of all strings
 * 
 * @note This implementation uses dynamic memory allocation and requires proper cleanup
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