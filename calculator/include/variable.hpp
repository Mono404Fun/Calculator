#pragma once

namespace sya {
  struct Variable {
    std::string name;
    double value;
  };

  extern std::vector<Variable> constants;

  bool validate_variable_name(const std::string& name) noexcept;
  bool is_constant(const std::string& name) noexcept;
}