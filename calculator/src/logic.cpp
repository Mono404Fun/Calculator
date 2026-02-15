#include "logic.hpp"
#include "operator.hpp"

#include <fmt/core.h>
#include <vector>

namespace sya {
  [[nodiscard]] Expression to_rpn(const Expression& expr) { // convert expression to RPN using the shunting yard algorithm
      using tt = TokenType;

      Expression output; // the output expression in RPN form
      Token token;
      std::vector<Token> op_stack; // the operator stack which stores operators and functions during the conversion process
      std::vector<FunctionInfo> fs; // the function stack which stores function information (name and argument count) during the conversion process
      std::vector<std::string> stored_variable; // to store variable tokens to push them later in end of convertion process

      if (expr.empty()) throw std::runtime_error("Empty expression"); // handle empty expression case
      if ((expr.at(0).type() == tt::OPERATOR && !is_unary(expr.at(0).get()))
          || (expr.at(expr.size()-1).type() == tt::OPERATOR && !is_unary(expr.at(expr.size()-1).get()))) // handle invalid starting/ending operator case
        throw std::runtime_error("Invalid expression: unexpected operator at the start/end of the expression");

      op_stack.reserve(expr.size()); // reserve space for operators to avoid unnecessary reallocations later

      auto pop_operator = [&] { // pop operator from operator stack (op_stack) to output (rpn expression)
        output.push(op_stack.back());
        op_stack.pop_back();
      };

      // pop operators from operator stack to output until an open parenthesis
      // is encountered (used for handling parentheses and function argument separators)
      auto pop_until_open_parent = [&] {
        while (!op_stack.empty() && op_stack.back().type() != tt::OPEN_PARENT) pop_operator();
      };

      for (size_t i = 0; i < expr.size(); i++) { // iterate over tokens in the input expression
        token = expr[i]; // get the current token
        switch (token.type()) { // handle token based on its type
          case tt::NUMBER: { // if it's a number, push it directly to the output
            output.push(token);
            break;
          }
          case tt::OPERATOR: { // if it's an operator
            // pop operators from the operator stack to the output while the operator at the top
            // of the stack has greater precedence, or equal precedence and is left associative (for non-unary operators)
            if (token.get() == "=") continue;
            while (!op_stack.empty()
              && is_operator(op_stack.back().get())
              && (opprec(op_stack.back().get()) > opprec(token.get())
              || (opprec(op_stack.back().get()) == opprec(token.get()) && !is_right_associative(token.get())))) pop_operator();
             op_stack.push_back(token); // push the current operator to the operator stack
            break;
          }
          case tt::FUNCTION: { // if it's a function
            fs.push_back({token.get(), 0}); // push function info (name and initial argument count) to the function stack
            op_stack.push_back(token); // push the function token to the operator stack (functions are treated as operators during the conversion process)
            break;
          }
          case tt::VARIABLE : {
            if ((i + 1) < expr.size() && expr[i + 1].type() == tt::OPERATOR && expr[i + 1].get() == "=")
              stored_variable.push_back(token.get()); // store variable token
            else
              output.push(token); // push variable token directly to output for use in evaluation              
            break;
          }
          case tt::OPEN_PARENT: { // if it's an open parenthesiss
            op_stack.push_back(token); // push it to the operator stack
            break;
          }
          case tt::SEPARATOR: { // if it's a function argument separator (comma) like in "max(1, 2)"
            pop_until_open_parent();
            if (fs.empty()) // if there's no function in the function stack, it means the separator is outside of a function, which is invalid
              throw std::logic_error("Invalid function: separator outside function");
            fs.back().arg_count++; // increment the argument count for the current function being processed
            break;
          }
          case tt::CLOSE_PARENT: { // if it's a close parenthesis
            pop_until_open_parent();
            // if the operator stack is empty after popping until an open parenthesis,
            // it means there's a mismatched closing parenthesis
            if (op_stack.empty())
              throw std::logic_error("Invalid expression: mismatched parentheses");
            op_stack.pop_back(); // pop the open parenthesis from the operator stack
            
            if (!op_stack.empty() && op_stack.back().type() == tt::FUNCTION) { // in case of a function call
              // increment the argument count for current function as the last argument would be before this closing parenthesis
              fs.back().arg_count++;

              if (!op_stack.empty() && op_stack.back().type() == tt::FUNCTION) { // if we are at the function name
                auto fn = op_stack.back().get(); // get it's name
                auto ac = fs.back().arg_count; // and the argument count too

                // if the argument count doesn't match the expected count for this function,
                // it's an argument count mismatch error
                if (functions.at(fn) != ac)
                  throw std::logic_error(fmt::format(
                        "Invalid function: argument count mismatch for {}(). Expected {}, got {}",
                        fn, functions.at(fn), ac));
                
                pop_operator(); // pop the function token from the operator stack to the output
                fs.pop_back(); // pop the function info from the function stack as well since we're done processing this function
              }
            }
            break;
          }
          default: throw std::logic_error("Invalid token: unsupported token type"); // handle unsupported tokens
        }
      }

      // if there are still functions in the function stack after processing all tokens,
      // it means there's a mismatched opening parenthesis for a function call
      if (!fs.empty())
        throw std::logic_error("Invalid expression: mismatched parentheses");

      while (!op_stack.empty()) pop_operator(); // pop any remaining operators from the operator stack to the output
      
      if (!stored_variable.empty()) {
        bool has_assignment = false;
        for (const Token& t : expr) {
          if (t.type() == tt::OPERATOR && t.get() == "=") {
            has_assignment = true;
            break;
          }
        }

        if (has_assignment) {
          // Emit assignments in reverse order so right-most assignment happens first.
          for (auto it = stored_variable.rbegin(); it != stored_variable.rend(); ++it) {
            output.push({*it, tt::VARIABLE});
            output.push({"=", tt::OPERATOR});
          }
        } else {
          for (const auto& var : stored_variable)
            output.push({var, tt::VARIABLE});
        }
      }

      return output; // rpn expression
  }

  [[nodiscard]] std::optional<float> evaluate_rpn(const Expression& rpn_expr, std::vector<Variable>& variables) {
    using tt = TokenType;
    Token token;
    std::vector<float> stack; // evaluation stack for evaluating the RPN expression
    bool is_assignement = false; // flag to indicate if the expression contains an assignment operator

    stack.reserve(rpn_expr.size());    

    for (size_t i = 0; i < rpn_expr.size(); i++) { // iterate over tokens in the RPN expression
      token = rpn_expr[i]; // get the current token
      switch (token.type()) { // handle token based on its type
        case tt::NUMBER: { // if it's a number, push its value to the evaluation stack
          stack.push_back(std::stof(token.get()));
          break;
        }
        case tt::OPERATOR: { // if it's an operator, pop the required number of operands from the stack and apply the operator
          auto op = token.get();
          if (op == "=") continue;
          if (stack.size() < 2) throw std::logic_error("Invalid expression: insufficient operands for binary operator");
          float right = stack.back(); stack.pop_back();
          float left = stack.back(); stack.pop_back();
          stack.push_back(apply_operator(op, left, right)); // apply the binary operator and push the result back to the stack
          break;
        }
        case tt::FUNCTION: { // if it's a function, pop the required number of arguments from the stack and apply the function
          auto fn = token.get();
          auto arg_count = functions.at(fn); // get the expected argument count for this function
          if (stack.size() < arg_count) throw std::logic_error(fmt::format("Invalid expression: insufficient arguments for function {}()", fn));
          
          std::vector<float> args(arg_count); // vector to hold function arguments
          for (int i = arg_count - 1; i >= 0; i--) { // pop arguments in reverse order since they were pushed in order during evaluation
            args[i] = stack.back();
            stack.pop_back();
          }
          stack.push_back(apply_function(fn, args)); // apply the function with the popped arguments and push the result back to the stack
          break;
        }
        case tt::VARIABLE: {
          auto var_name = token.get();

          if ((i + 1) < rpn_expr.size() && rpn_expr[i + 1].type() == tt::OPERATOR && rpn_expr[i + 1].get() == "=") {
            if (stack.empty())
              throw std::logic_error(fmt::format(
                "Invalid expression: missing value for variable assignment to '{}'", var_name));
            
            float var_value = stack.back(); stack.pop_back(); // get the value to be assigned to the variable from the top of the stack
            auto it = std::find_if(variables.begin(), variables.end(), [&](const Variable& v) { return v.name == var_name; });
            if (it != variables.end())
              it->value = var_value; // if variable already exists, update its value
            else variables.push_back({var_name, var_value}); // otherwise, create a new variable with this name and value
            // Push the assigned value back to the stack so chained assignments keep the value available
            stack.push_back(var_value);
            is_assignement = true;
          } else {
            auto it = std::find_if(variables.begin(), variables.end(), [&](const Variable& v)
              { return v.name == var_name; });
            if (it == variables.end())
              throw std::logic_error(fmt::format("Undefined variable: '{}'", var_name)); // if variable is not defined, it's an undefined variable error
            stack.push_back(it->value); // push the value of the variable to the stack for use in evaluation
          }
          break;
        }
        default: throw std::logic_error("Invalid token: unsupported token type during evaluation"); // handle unsupported tokens during evaluation
      }
    }

   // if the expression contains an assignment operator, we return std::nullopt
   // to indicate that there's no result to return, since the purpose of an assignment
   // expression is to assign a value to a variable rather than produce a value itself
    if (is_assignement)
      return std::nullopt;

    if (stack.size() > 1) throw std::logic_error("Invalid expression: too many operands left after evaluation"); // after evaluating the entire RPN expression, there should be exactly one value left on the stack, which is the final result
    else if (stack.empty()) return std::nullopt; // if the stack is empty, it means there was no result to return (e.g., in case of an expression that only contains variable assignments without a final value), so we return std::nullopt to indicate the absence of a result
    return stack.back(); // return the final result of evaluating the RPN expression
  }
}