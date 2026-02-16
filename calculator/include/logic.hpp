#pragma once

#include "expression.hpp"
#include "variable.hpp"

namespace sya {
  struct FunctionInfo {
    std::string name;
    size_t arg_count;
  };

  [[nodiscard]] Expression to_rpn(const Expression& expr);
  [[nodiscard]] std::optional<float> evaluate_rpn(const Expression& rpn_expr, std::vector<Variable>& variables);
}
