#pragma once

#include "utils.hpp"

#include <string>
#include <string_view>
#include <cstdint>
#include <iostream>
#include <ranges>

namespace sya {
/**
  * @brief An enum for various possible token types.
  *
*/
enum class TokenType : uint8_t {
  LITERAL_NUMBER, FLOAT_NUMBER, UNARY_NUMBER,
  OPERATOR, OPEN_PARENT, CLOSE_PARENT,
  SEPARATOR, FUNCTION, UNKNOWN,
};

/**
  * @brief A class holding a token value paried with it's type inside a given a expression.
  * Note that token value must be a string.
*/
class Token : public utils::ItClasses<std::string_view> { // deriving at() and operator[] from ItClasses
private:
  std::string value;  // token value
  TokenType   m_type; // token type

public:
  /************************\
  |      CONSTRUCTORS      |
  \************************/
  explicit Token(const std::string_view value, TokenType m_type) noexcept; // main ctor
  Token(const Token& t)     = default; // move ctor
  Token()          noexcept = default; // default ctor
  Token(Token&& t) noexcept = default; // move ctor

  /************************\
  |        OPERATORS       |
  \************************/
  Token& operator=(const Token& t)     = default; // copy assignement
  Token& operator=(Token&& t) noexcept = default; // move assignement

  bool operator== (const Token& t) const noexcept; // equality operator
  auto operator<=>(const Token& t) const;          // three-way comparrision operator

  // const char& operator[](std::size_t i) const noexcept; // indexing operator
  friend std::ostream& operator<<(std::ostream& os, const Token& t); // streaming operator

  /************************\
  |         METHODS        |
  \************************/
  bool is_empty() const noexcept; // return if token is empty
  [[nodiscard]] const std::string& get()  const noexcept;     // get token value
  void set(const std::string& nv); // set token value to an other
  [[nodiscard]] TokenType type() const noexcept;     // get token type
  void clear(); // clear token value

  void swap(Token& t) noexcept; // swap this token's contents with another one.
  friend void swap(Token& t1, Token& t2) noexcept; // an overloaded function to swap between two given tokens.
};
} // namespace sya
