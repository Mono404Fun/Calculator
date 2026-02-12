#pragma once

#include <unordered_map>

namespace sya {
  enum class OperatorPrec : uint8_t { ADD_SUB = 1, MUL_DIV, POW };
  extern std::unordered_map<std::string, OperatorPrec> operators;
  extern std::unordered_map<std::string, std::size_t> functions;

  bool is_function(const std::string& token) noexcept;
  bool is_operator(const std::string& op);
  bool is_operator(char op);
  bool is_unary(const std::string& op);
  bool is_unary(char op);
  bool is_right_associative(const std::string& op);
  bool is_right_associative(char op);
  OperatorPrec opprec(const std::string& op);

  [[nodiscard]] float apply_operator(const std::string& op, float left, float right);
  [[nodiscard]] float apply_function(const std::string& fn, const std::vector<float>& args);
}
