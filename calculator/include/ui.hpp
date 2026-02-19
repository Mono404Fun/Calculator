#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iomanip>
#include <math.h>
#include <fmt/core.h>

#include "logic.hpp"

namespace console {
struct HistoryEntry {
  size_t id;
  std::string expression;
  std::string result;
};

class Table {
public:
  explicit Table(std::vector<std::string> headers)
    : headers_(std::move(headers)) {}

  void add_row(std::vector<std::string> row) {
    rows_.push_back(std::move(row));
  }

  void add_row(std::initializer_list<std::string> row) {
    rows_.emplace_back(row);
  }

  void print(std::ostream& os = std::cout) const {
    auto widths = compute_widths();

    print_separator(os, widths);
    print_row(os, headers_, widths);
    print_separator(os, widths);

    for (const auto& r : rows_)
      print_row(os, r, widths);

    print_separator(os, widths);
  }

  size_t row_count() { return rows_.size(); }

private:
  std::vector<std::string> headers_;
  std::vector<std::vector<std::string>> rows_;

  std::vector<std::size_t> compute_widths() const {
    std::vector<std::size_t> w(headers_.size(), 0);

    auto update = [&](const std::vector<std::string>& row) {
      for (std::size_t i = 0; i < row.size(); ++i)
        w[i] = std::max(w[i], row[i].size());
    };

    update(headers_);
    for (const auto& r : rows_)
      update(r);

    return w;
  }

  static void print_separator(std::ostream& os,
                              const std::vector<std::size_t>& w) {
    os << '+';
    for (auto width : w) {
      os << std::string(width + 2, '-') << '+';
    }
    os << '\n';
  }

  static void print_row(std::ostream& os,
                        const std::vector<std::string>& row,
                        const std::vector<std::size_t>& w) {
    os << '|';
    for (std::size_t i = 0; i < row.size(); ++i) {
      os << ' '
         << std::left << std::setw(w[i]) << row[i]
         << ' ' << '|';
    }
    os << '\n';
  }
};


class Interface {
public:
  Interface(std::unordered_map<std::string, std::size_t> functions)
    : m_functions(std::move(functions)), variables(sya::constants) {}

  void run() {
    print_banner();

    while (true) {
      std::string input = "";

      std::cout << "> ";
      if (!std::getline(std::cin, input)) break;
      
      if (input.empty()) continue;
      if (input[0] == ':') {
        if (!handle_command(input.substr(1))) break;
      }
      else handle_expression(input);

      m_expr.clear();
    }
  }

private:
  std::unordered_map<std::string, std::string> commands = {
    { ":help", "Show available commands" },
    { ":functions", "List supported functions" },
    { ":clear", "Clear the screen" },
    { ":quit",  "Exit program" },
    { ":history", "Show calculation history" },
    { ":variables", "Show defined variables" },
    { ":clear_history", "Clear calculation history" },
    { ":clear_vars", "Clear defined variables" },
    { ":clear_all", "Clear both history and variables" },
    { ":remove_variable", "Remove a specific variable by name" }
  };
  std::unordered_map<std::string, std::size_t> m_functions;
  std::vector<sya::Variable> variables;
  std::vector<HistoryEntry> history;
  sya::Expression m_expr;

  void print_banner() const {
    std::cout
      << "+--------------------------------------+\n"
      << "| ExprEval: A C++ inline calculator    |\n"
      << "+--------------------------------------+\n"
      << "Type :help for commands\n";
  }

  bool handle_command(std::string_view cmd) {
    if (cmd == "quit") return false;

    if (cmd == "help") print_help();
    else if (cmd == "functions") print_functions();
    else if (cmd == "clear") clear();
    else if (cmd == "history") {
      if (history.empty()) {
        std::cout << "No history available.\n";
        return true;
      }
      Table t({ "ID", "Expression", "Result" });
      for (const auto& entry : history)
        t.add_row({ std::to_string(entry.id), entry.expression, entry.result });
      t.print();
    }
    else if (cmd=="variables") {
      Table t({ "Name", "Value" });

      for (const auto& [name, value] : variables) {
        if (sya::is_constant(name)) continue; // skip constants in variable listing
        t.add_row({ name, std::to_string(value) });
      }

      if (t.row_count() == 0) {
        std::cout << "No variables defined.\n";
        return true;
      }

      t.print();
    }
    else if (cmd == "constants") {
      Table t({ "Name", "Value" });

      for (const auto& [name, value] : sya::constants)
        t.add_row({ name, std::to_string(value) });
      t.print();
    }
    else if (cmd=="clear_history") {
      history.clear();
      std::cout << "History cleared.\n";
    }
    else if (cmd=="clear_vars") {
      clear_variables();
      std::cout << "Variables cleared.\n";
    }
    else if (cmd=="clear_all") {
      clear_variables();
      history.clear();
      std::cout << "History and variables cleared.\n";
    }
    else if (cmd == "remove_variable") {
      std::string var_name;
      std::cout << "Enter variable name to remove: ";
      std::cin >> var_name;

      if (sya::is_constant(var_name)) {
        std::cout << fmt::format(
          "Error: Cannot remove variable: {}, it is a reserved constant.\n", var_name);
      } else {

        auto it = std::find_if(variables.begin(), variables.end(), [&](const sya::Variable var)
                                                                  { return var.name == var_name; });
        if (it != variables.end()) {
          variables.erase(it);
          std::cout << "Variable '" << var_name << "' removed.\n";
        } else {
          std::cout << "Variable '" << var_name << "' not found.\n";
        }
      }
    }

    else std::cout << "Unknown command. Use :help\n";
    return true;
  }

  void handle_expression(std::string_view expr) {
    try {
      m_expr.set_expression(expr);
      m_expr.tokenize();

      auto result = sya::evaluate_rpn(sya::to_rpn(m_expr), variables);
      if (result.has_value()) {
        history.push_back(HistoryEntry{ history.size() + 1, std::string(expr), std::to_string(result.value()) });
        std::cout << "=> " << result.value() << "\n";
      }
    }
    catch (const std::exception& e) {
      std::cout << "Error: " << e.what() << "\n";
    }
  }

  void print_help() const {
    Table t({ "Command", "Description" });
    for (const auto& [cmd, desc] : commands)
      t.add_row({ cmd, desc });
    t.print();
  }

  void print_functions() const {
    Table t({ "Function", "Args" });

    for (const auto& [name, argc] : m_functions)
      t.add_row({ name, std::to_string(argc) });

    t.print();
  }

  void clear_variables() {
    size_t len = variables.size()-sya::constants.size();
    std::vector<sya::Variable> temp;
    temp.reserve(len);

    for (const auto& var : variables)  {
      if (sya::is_constant(var.name)) temp.push_back(var);
      continue;
    }

    variables = temp;
  }

  static void clear() {
#if defined(_WIN32)
    std::system("cls");
#else
    std::system("clear");
#endif
  }
};

} // namespace console
