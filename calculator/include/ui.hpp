#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iomanip>

#include "logic.hpp"

namespace console {

class Table {
public:
  explicit Table(std::vector<std::string> headers)
    : headers_(std::move(headers)) {}

  void add_row(std::vector<std::string> row) {
    rows_.push_back(std::move(row));
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
    : m_functions(std::move(functions)) {}

  void run() {
    print_banner();

    std::string input;
    while (true) {
      std::cout << "\n> ";
      if (!std::getline(std::cin, input))
        break;

      if (input.empty())
        continue;

      if (input[0] == ':') {
        if (!handle_command(input))
          break;
      } else {
        handle_expression(input);
      }
    }
  }

private:
  std::unordered_map<std::string, std::size_t> m_functions;
  sya::Expression m_expr;

  void print_banner() const {
    std::cout
      << "+--------------------------------------+\n"
      << "| ExprEval: A C++ inline calculator    |\n"
      << "+--------------------------------------+\n"
      << "Type :help for commands\n";
  }

  bool handle_command(std::string_view cmd) {
    if (cmd == ":quit") return false;

    if (cmd == ":help") print_help();
    else if (cmd == ":funcs") print_functions();
    else if (cmd == ":clear") clear();
    else std::cout << "Unknown command. Use :help\n";
    return true;
  }

  void handle_expression(std::string_view expr) {
    try {
      m_expr.set_expression(expr);
      m_expr.tokenize();
      auto result = sya::evaluate_rpn(sya::to_rpn(m_expr));

      std::cout << "Expression: " << result << "\n";
    }
    catch (const std::exception& e) {
      std::cout << "Error: " << e.what() << "\n";
    }
  }

  void print_help() const {
    Table t({ "Command", "Description" });
    t.add_row({ ":help",  "Show available commands" });
    t.add_row({ ":funcs", "List supported functions" });
    t.add_row({ ":clear", "Clear the screen" });
    t.add_row({ ":quit",  "Exit program" });
    t.print();
  }

  void print_functions() const {
    Table t({ "Function", "Args" });

    for (const auto& [name, argc] : m_functions)
      t.add_row({ name, std::to_string(argc) });

    t.print();
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
