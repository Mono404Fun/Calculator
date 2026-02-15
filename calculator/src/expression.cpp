#include "expression.hpp"
#include "operator.hpp"

#include <iostream>
#include <fmt/core.h>

namespace sya {
  Expression::Expression(std::string_view expr) : m_expr(expr) {} // construct an expression from a string and tokenize it

  [[nodiscard]] const std::string& Expression::expression() const noexcept { return m_expr; }
  void Expression::set_expression(std::string_view expr) { m_expr = expr; }
  [[nodiscard]] Expression::t Expression::tokens() const noexcept { return m_tokens; }

  // iterators and element access
  Expression::it Expression::begin() noexcept { return std::begin(m_tokens); }
  Expression::it Expression::end() noexcept { return std::end(m_tokens); }

  Expression::cit Expression::begin() const noexcept { return std::cbegin(m_tokens); }
  Expression::cit Expression::end() const noexcept { return std::cend(m_tokens); }

  Expression::cit Expression::cbegin() const noexcept { return std::cbegin(m_tokens); }
  Expression::cit Expression::cend() const noexcept { return std::cend(m_tokens); }

  Token& Expression::operator[](std::size_t i) noexcept { return m_tokens[i]; }
  const Token& Expression::operator[](std::size_t i) const noexcept { return m_tokens[i]; }
  const Token& Expression::at(std::size_t i) const {
    if (i >= m_tokens.size())
      throw std::out_of_range("Index is out of range");
    return m_tokens[i];
  }

  void Expression::clear() noexcept {
    m_tokens.clear();
    m_expr.clear();
  }
  [[nodiscard]] bool Expression::empty() const noexcept { return m_tokens.empty(); }
  [[nodiscard]] size_t Expression::size() const noexcept { return m_tokens.size(); }

  void Expression::push(Token token) { m_tokens.push_back(token); }
  void Expression::pop() { m_tokens.pop_back(); }
  [[nodiscard]] std::optional<Token> Expression::first() const { if (!empty()) return m_tokens.front(); else return std::nullopt; }
  [[nodiscard]] std::optional<Token> Expression::last() const {  if (!empty()) return m_tokens.back(); else return std::nullopt; }
  [[nodiscard]] std::string Expression::first_v() const {
    if (first().has_value())
      return first().value().get();
    return "";
  }
  [[nodiscard]] std::string Expression::last_v() const {
    if (last().has_value())
      return last().value().get();
    return "";
  }
  [[nodiscard]] TokenType Expression::first_t() const {
    if (first().has_value())
      return first().value().type();
    return TokenType::UNKNOWN;
  }
  [[nodiscard]] TokenType Expression::last_t() const {
    if (last().has_value())
      return last().value().type();
    return TokenType::UNKNOWN;
  }

  void Expression::tokenize() {
    using namespace utils;
    using tt = TokenType;
    using uc = unsigned char;

    m_tokens.clear(); // clear any existing tokens before tokenizing the new expression

    std::string ct; // current token being built
    int pb = 0; // parenthesis balance counter

    if (m_expr.empty()) throw std::runtime_error("Empty expression"); // handle empty expression case

    auto numlike  = [](uc c) -> bool { return std::isdigit(c) || c == '.'; }; // for handling numbers/decimals
    auto push_op  = [&](std::string_view ct_) { push({ct_, tt::OPERATOR}); }; // for pushing operators
    auto push_token = [&] { // for pushing the current token being built, if any
      if (ct.empty()) return; // nothing to push

      if (is_number(ct)) push({ct, tt::NUMBER}); // if it's a number, push as number token
      else if (is_function(ct)) push({ct, tt::FUNCTION}); // if it's a function, push as function token
      else if (validate_variable_name(ct)) { push({ct, tt::VARIABLE}); } // if it's a valid variable name, push as variable token
      else if (ct.size() == 1 && is_operator(ct.at(0))) push_op(ct); // if it's a single char operator, push as operator token
      else push({ct, tt::UNKNOWN}); // otherwise, push as unknown token

      ct.clear(); // clear the current token after pushing
    };

    for (size_t i = 0; i < m_expr.size(); ++i) {
      const uc c = m_expr[i]; const uc n = (i + 1 < m_expr.size()) ? m_expr.at(i+1) : '\0'; // current and next character (if any)
      size_t pos = i+1; // for error messages (1-based index)

      if (std::isspace(c)) { // skip whitespace, but check for invalid whitespace in numbers like "1 2" or "1. 2"
        if (!ct.empty() && (numlike(n) || is_letter(n)))
          throw std::runtime_error(fmt::format("Invalid expression: unexpected whitespace in number at position {}", pos));

        continue;
      }
      if (numlike(c)) {
        if (c == '.') { // handle decimal point
          if (ct.find('.') != std::string::npos || !std::isdigit(n)) // multiple decimal points or decimal point not followed by digit
            throw std::runtime_error(fmt::format("Invalid number: multipe decimal points at position {}", pos));

          if (ct.empty()) ct = "0"; // handle numbers like ".5" by treating them as "0.5"
          if (!ct.empty() && is_unary(ct.at(ct.size()-1))) ct = std::string(1, ct.at(0)) + "0"; // handle cases like "-.5" by treating them as "-0.5"
        }

        ct += c;
        if (is_letter(n)) { // handle cases like "1x" by treating them as "1*x"
          push_token(); // push the current number token before handling the implicit multiplication
          push_op("*"); // push the implicit multiplication operator
        }
        continue;
      }
      if (c == '(') {
        if (n == ')') // handle empty parentheses "()"
          throw std::runtime_error(fmt::format("Invalid expression: empty parentheses at position {}", pos));

        push_token(); // push any current token before handling the parenthesis

        if (is_number(last_v()) || last_t() == tt::VARIABLE ||
            last_t() == tt::CLOSE_PARENT) push_op("*"); // handle implicit multiplication like "2(3+4)" or "(1+2)(3+4)"

        push({"(", tt::OPEN_PARENT}); // push the open parenthesis token
        pb++; // increment parenthesis balance counter

        continue;
      }
      if (c == ')') {
        push_token(); // push any current token before handling the parenthesis
        push({")", tt::CLOSE_PARENT}); // push the close parenthesis token

        if (std::isdigit(n) || is_letter(n)) push_op("*"); // handle implicit multiplication like "(1+2)3"

        pb--; // decrement parenthesis balance counter
        if (pb < 0)
          throw std::runtime_error(fmt::format("Invalid expression: Unexpected closing parenthesis at position {}", pos));

        continue;
      }
      if (is_operator(c)) {
        push_token(); // push any current token before handling the operator

        if (is_unary(c) && ct.empty() &&
        (empty() || last_t() == tt::OPERATOR    ||
                    last_t() == tt::OPEN_PARENT ||
                    last_t() == tt::SEPARATOR)) { // handle unary operators at the start of the expression or after an operator/open parenthesis/separator
          if (n == ')' || n == ',' || std::isspace(n))
            throw std::runtime_error(fmt::format("Invalid expression: unexpected {} after unary operator at position {}", (std::isspace(n) ? "[SPACE]" : std::to_string(n)), pos));
          if (is_unary(n) && (n == c || n == '-' || n == '+')) // handle unary operator duplication
            throw std::runtime_error(fmt::format("Invalid expression: unexpected unary operator '{}' after unary operator at position {}", static_cast<char>(n), pos));

          ct = c; // start building the unary operator as part of the number token (e.g. "-5" or "+3.14")
          if (n == '(') ct += '1'; // handle cases like "-(3+4)" by treating them as "-1*(3+4)"
          
          continue;
        } else if (c == '=') {
          if (is_number(last_v()) || last_t() == tt::CLOSE_PARENT) // handle cases like "x=5" or "(1+2)=3" by treating them as "x=5" or "(1+2)=3"
            throw std::runtime_error(fmt::format("Invalid expression: unexpected assignment operator at position {}", pos));
          if (is_function(last_v())) // handle cases like "sin=5" by treating them as "sin=5"
            throw std::runtime_error(fmt::format("Invalid expression: unexpected assignment operator after function name at position {}", pos));
        }

        if (is_operator(n) && !is_unary(n)) // handle operator duplication
          throw std::runtime_error(fmt::format("Invalid expression: unexpected operator '{}' after operator at position {}", static_cast<char>(n), pos));

        char op[2] = {static_cast<char>(c), '\0'}; // convert operator char to string
        push_op(op); // push the operator token
        
        continue;
      }
      if (is_letter(c)) { 
        ct += c;

        continue;
      }
      if (c == ',') {
        push_token(); // push any current token before handling the separator

        if (last_t() == tt::OPEN_PARENT || n == ')' || pb == 0) // handle cases like "f(,)" or "f(x,)" where the separator is misplaced
          throw std::runtime_error(fmt::format("Invalid separator: unexpected separator at position {}", pos));

        push({",", tt::SEPARATOR}); // push the separator token
        continue;
      }

      throw std::runtime_error(fmt::format("Invalid character '{}' at position {}", static_cast<char>(c), pos));
    }

    push_token();

    if (pb != 0) // check for mismatched parentheses after processing the entire expression
      throw std::runtime_error(fmt::format("Mismatched parentheses: missing {} {} parenthesis", std::abs(pb), (pb > 0) ? "closing" : "opening"));
  }
}