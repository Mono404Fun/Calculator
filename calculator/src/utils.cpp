#include "utils.hpp"

namespace utils {
  constexpr bool is_number(std::string& token) noexcept {
    if (token.empty())
      return false;

    std::size_t start = 0;
    if (token[0] == '+' || token[0] == '-') {
      start = 1;
      if (token.length() == 1) return false;
    }

    auto isdigit = [](const char& c) { return c >= '0' && c <= '9'; };

    bool has_digit = false;
    bool has_dot = false;

    for (std::size_t i = start; i < token.length(); i++) {
      if (isdigit(token[i])) {
        has_digit = true;
      } else if (token[i] == '.') {
        if (has_dot)
          return false;
        has_dot = true;
      } else {
        return false;
      }
    }

    return has_digit;
  }

  constexpr bool is_letter(const char& c) noexcept {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
  }
}
