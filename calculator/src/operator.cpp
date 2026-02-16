#include "operator.hpp"

#include <math.h>
#include <fmt/core.h>

namespace sya {
  std::unordered_map<std::string, OperatorPrec> operators = {
    {"+", OperatorPrec::ADD_SUB},
    {"-", OperatorPrec::ADD_SUB},
    {"*", OperatorPrec::MUL_DIV},
    {"/", OperatorPrec::MUL_DIV},
    {"^", OperatorPrec::POW},
    {"=", OperatorPrec::ASSIGNEMENT},
  };

  std::unordered_map<std::string, std::size_t> functions = {
    {"sqrt",  1},
    {"pow",   2},
    {"cos",   1},
    {"sin",   1},
    {"max",   2},
    {"min",   2},
    {"abs",   1},
    {"exp",   1},
    {"log",   1},
    {"ln",    1},
    {"floor", 1},
    {"ceil",  1},
    {"round", 1},
    {"sign",  1},
    {"hypot", 2},
    {"atan2", 2},
    {"sinh",  1},
    {"cosh",  1},
    {"tanh",  1},
    {"asinh", 1},
    {"acosh", 1},
    {"atanh", 1}
  };

  bool is_function(const std::string& token) noexcept {
    return functions.find(token) != functions.end();
  }
  bool is_operator(const std::string& op) { return operators.find(op) != operators.end(); }
  bool is_operator(char op) { return is_operator(std::string(1, op)); }
  bool is_unary(const std::string& op) { return op == "-" || op == "+"; }
  bool is_unary(char op) { return is_unary(std::string(1, op)); }
  bool is_right_associative(char op) { return is_right_associative(std::string(1, op)); }
  OperatorPrec opprec(const std::string& op) { return operators[op]; }
  bool is_right_associative(const std::string& op) {
    if (op == "^" || op == "=") return true;
    return false;
  }
  [[nodiscard]] float apply_operator(const std::string& op, float left, float right) {
    if (op == "+") return left + right;
    if (op == "-") return left - right;
    if (op == "*") return left * right;
    if (op == "/") {
      if (right == 0) throw std::logic_error("Division by zero");
      return left / right;
    }
    if (op == "^") return std::pow(left, right);
    throw std::logic_error(fmt::format("Invalid operator: {}", op));
  }
  [[nodiscard]] float apply_function(const std::string& fn, const std::vector<float>& args) {
    if (fn == "sqrt") return std::sqrt(args[0]);
    if (fn == "pow") return std::pow(args[0], args[1]);
    if (fn == "cos") return std::cos(args[0]);
    if (fn == "sin") return std::sin(args[0]);
    if (fn == "max") return std::fmax(args[0], args[1]);
    if (fn == "min") return std::fmin(args[0], args[1]);
    if (fn == "abs") return std::fabs(args[0]);
    if (fn == "exp") return std::exp(args[0]);
    if (fn == "log" || fn == "ln") {
      if (args[0] <= 0) throw std::logic_error("Logarithm of non-positive number");
      return std::log(args[0]);
    }
    if (fn == "floor") return std::floor(args[0]);
    if (fn == "ceil") return std::ceil(args[0]);
    if (fn == "round") return std::round(args[0]);
    if (fn == "sign") return (args[0] > 0) - (args[0] < 0);
    if (fn == "hypot") return std::hypot(args[0], args[1]);
    if (fn == "atan2") return std::atan2(args[0], args[1]);
    if (fn == "sinh") return std::sinh(args[0]);
    if (fn == "cosh") return std::cosh(args[0]);
    if (fn == "tanh") return std::tanh(args[0]);
    if (fn == "asinh") return std::asinh(args[0]);
    if (fn == "acosh") {
      if (args[0] < 1) throw std::logic_error("Inverse hyperbolic cosine of number less than 1");
      return std::acosh(args[0]);
    }
    if (fn == "atanh") {
      if (args[0] <= -1 || args[0] >= 1) throw std::logic_error("Inverse hyperbolic tangent of number outside the range (-1, 1)");
      return std::atanh(args[0]);
    }
    throw std::logic_error(fmt::format("Invalid function: {}", fn));
  }
}
