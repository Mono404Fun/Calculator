#pragma once

#include <iostream>      // for std::cout
#include <string>        // for std::string
#include <string_view>   // for std::string_view
#include <stdexcept>     // for std::runtime_error
#include <unordered_map> // for std::unordered_map
#include <charconv>

namespace utils {  
  inline bool is_number(std::string_view sv) noexcept {
      if (sv.empty()) return false;

      if (sv.front() == '+' || sv.front() == '-')
          sv.remove_prefix(1);

      if (sv.empty()) return false;

      bool has_digit = false;
      bool has_dot = false;

      size_t i = 0;

      for (; i < sv.size(); ++i) {
        char c = sv[i];

        if (std::isdigit(c)) {
          has_digit = true;
          continue;
        }
        if (c == '.' && !has_dot) {
          has_dot = true;
          continue;
        }

        break;
      }

      if (!has_digit) return false;

      if (i < sv.size() && (sv[i] == 'e' || sv[i] == 'E')) {
        ++i;
        if (i == sv.size()) return false;
        if (sv[i] == '+' || sv[i] == '-') ++i;
        if (i == sv.size()) return false;
        bool exp_digit = false;
        for (; i < sv.size(); ++i) {
            if (!std::isdigit(sv[i])) return false;
            exp_digit = true;
        }
        return exp_digit;
      }

    return i == sv.size();
  }

  constexpr inline bool is_letter(const char& c) noexcept {
    return std::isalpha(c) || c == '_';
  }
} // namespace utils
