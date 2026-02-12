#pragma once

#include "token.hpp"
#include "utils.hpp"

#include <deque> // for std::deque
#include <string_view> // for std::string_view
#include <optional>

namespace sya {
  /**
   * @brief Making *Expression* objects creates a tokenized expression, from the given string.
   */
  class Expression { // deriving from ItClasses to make it iterable
    private:
    using t = std::deque<Token>; // to make things clean
    using it = decltype(std::begin(std::declval<t&>()));
    using cit = decltype(std::cbegin(std::declval<t&>()));

    std::string m_expr; // the string given expression
    t m_tokens; // the tokenized expression as a std::deque<Token>

    public:
    /************************\
    |      CONSTRUCTORS      |
    \************************/
    Expression() noexcept = default; // default ctor
    Expression(std::string_view expr);
    Expression(const Expression& expr) = default; // copying
    Expression(Expression&& expr) noexcept = default; // moving

    /************************\
    |        OPERATORS       |
    \************************/
    Expression& operator=(const Expression&  expr) = default; // copying
    Expression& operator=(Expression&& expr) noexcept = default; // moving

    /************************\
    |         METHODS        |
    \************************/
    /*Cconvert given string expression to valid ready to analyze tokens*/
    it begin() noexcept;
    it end() noexcept;

    cit begin() const noexcept;
    cit end() const noexcept;

    cit cbegin() const noexcept;
    cit cend() const noexcept;

    void tokenize();

    [[nodiscard]] const std::string& expression() const noexcept; // return string expression
    void set_expression(std::string_view expr);
    [[nodiscard]] t tokens() const noexcept; // return tokens

    void push(Token token); // push a new token to the expression (tokens)
    void pop(); // pop from expression
    [[nodiscard]] std::optional<Token> first() const; // return first token in the expression
    [[nodiscard]] std::optional<Token> last() const; // return last token in the expresion
    [[nodiscard]] std::string first_v() const; // return first token in the expression
    [[nodiscard]] std::string last_v() const; // return last token in the expresion
    [[nodiscard]] TokenType first_t() const; // return first token in the expression
    [[nodiscard]] TokenType last_t() const; // return last token in the expresion

    void clear() noexcept; // clear string expression and tokens as well
    [[nodiscard]] bool empty() const noexcept; // if expression is empty
    [[nodiscard]] size_t size() const noexcept; // return expression size

    friend std::ostream& operator<<(std::ostream& os, Expression expr) { return os << expr.m_expr; }
    Token& operator[](std::size_t i) noexcept;
    const Token& operator[](std::size_t i) const noexcept;
    const Token& at(std::size_t i) const;
  };
}
