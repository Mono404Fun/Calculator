#pragma once

#include "expression.hpp"

namespace sya {
  struct FunctionInfo {
    std::string name;
    size_t arg_count;
  };

  struct Variable {
    std::string name;
    float value;
  };

  [[nodiscard]] Expression to_rpn(const Expression& expr);
  [[nodiscard]] std::optional<float> evaluate_rpn(const Expression& rpn_expr, std::vector<Variable>& variables);
}
