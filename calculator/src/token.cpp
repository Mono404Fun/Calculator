#include "utils.hpp"
#include "token.hpp"

#include <stdexcept> // for std::runtime_error, std::logical_error, std::out_of_range
#include <algorithm> // for std::swap

namespace sya {
  /************************\
  |      CONSTRUCTORS      |
  \************************/
  // Token::Token(std::string_view value) : ItClasses(m_value), m_value(value), m_type(TokenType::UNKNOWN) {}
  Token::Token(std::string_view value, TokenType type) noexcept
  : m_value(value), m_type(type) {} // main ctor

  /************************\
  |        OPERATORS       |
  \************************/
  bool Token::operator== (const Token& t) const noexcept { return m_type == t.m_type && m_value == t.m_value; }
  std::ostream& operator<<(std::ostream& os, const Token& t) { return os << t.m_value; }
  Token::operator std::string() const { return m_value; }

  /************************\
  |         METHODS        |
  \************************/
  [[nodiscard]] constexpr std::string_view Token::view() const noexcept { return m_value; }
  void Token::append(std::string_view value) { m_value.append(value); }
  bool Token::is_empty() const noexcept { return m_value.empty(); } // if token is empty
  [[nodiscard]] std::string Token::get() const noexcept { return m_value; }         // get token's vaue
  void Token::set(const std::string& nv) { m_value = nv; } // change token value
  [[nodiscard]] TokenType Token::type() const noexcept { return m_type; }        // get token's type
  void Token::clear() { m_value.clear(); }  // clear token's value

  void Token::swap(Token& t) noexcept {
    std::swap(m_value, t.m_value);   // swap values
    std::swap(m_type, t.m_type); // swap types
  }
  void swap(Token& t1, Token& t2) noexcept { t1.swap(t2); } // an outer func to swap current token's contents with another

} // namespace sya
