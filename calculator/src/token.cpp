#include "utils.hpp"
#include "token.hpp"

#include <stdexcept> // for std::runtime_error, std::logical_error, std::out_of_range
#include <algorithm> // for std::swap

namespace sya {
  /************************\
  |      CONSTRUCTORS      |
  \************************/
  Token::Token(const std::string_view value, TokenType type) noexcept
  : ItClasses(value), value(value), m_type(type) {} // main ctor

  /************************\
  |        OPERATORS       |
  \************************/
  bool Token::operator== (const Token& t) const noexcept { return m_type == t.m_type && value == t.value; }
  auto Token::operator<=>(const Token& t) const {
    if (m_type != t.m_type)
      throw std::logic_error("Cannot compare tokens of different types.");
    return value <=> t.value;
  }

  /************************\
  |         METHODS        |
  \************************/
  bool Token::is_empty() const noexcept { return value.empty(); } // if token is empty
  [[nodiscard]] const std::string& Token::get() const noexcept { return value; }         // get token's vaue
  void Token::set(const std::string& nv) { value = nv; } // set token value to an other
  [[nodiscard]] TokenType Token::type() const noexcept { return m_type; }        // get token's type

  void Token::clear() { value.clear(); }  // clear token's value

  void Token::swap(Token& t) noexcept {
    std::swap(value, t.value);   // swap values
    std::swap(m_type, t.m_type); // swap types
  }

  void swap(Token& t1, Token& t2) noexcept { t1.swap(t2); } // an outer func to swap current token's contents with another

  std::ostream& operator<<(std::ostream& os, const Token& t) // to make it streamable (printable)
  { return os << t.get(); }
} // namespace sya
