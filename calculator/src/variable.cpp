#include "variable.hpp"

#include <cmath>

namespace sya {
  std::vector<Variable> constants = {
    {"pi", 3.14159265358979323846},
    {"e",  2.71828182845904523536},
    {"phi", (1 + std::sqrt(5)) / 2},
    {"gamma", 0.57721566490153286060}
  };

  bool is_constant(const std::string& name) noexcept {
    if (name.empty()) return false;
    if (std::find_if(constants.begin(), constants.end(), [&name](const Variable& var)
      { return var.name == name; }) != constants.end()) return true;
    return false;
  }

  bool validate_variable_name(const std::string& name) noexcept {
    if (name.empty() || !std::isalpha(name[0]))
          return false; // variable name must start with a letter
    for (char c : name) {
      if (!std::isalnum(c) && c != '_') return false; // variable name can only contain letters, digits, and underscores
    }
    return true;
  }
}