#pragma once

#include <iostream>     // for std::cout
#include <string>       // for std::string
#include <ranges>       // for std::ranges::range
#include <string_view>  // for std::string_view
#include <stdexcept>    // for std::runtime_erorr

using namespace std::string_literals;

namespace utils {
  /**
   * @brief A helper class used to avoid duplication of at() and operator[] in major iterable custom classes.
   *
   * @tparam Iter The iterable type.
   */
  template <typename Iter> requires std::ranges::range<Iter> // only allow iterable types
  class ItClasses {
    private:
    Iter v; // value

    public:
      ItClasses(Iter v):v(v) {}

      [[nodiscard]] const auto& value() { return value; } // value getter
      [[nodiscard]] const char& operator[](std::size_t i) const { return v[i]; } // indexing operator
      [[nodiscard]] constexpr inline const char& at(size_t i) const { // a safer alternative func to indexing operator
        if (i >= v.size()) // if it's an out of range
        throw std::out_of_range("Index is out of range"); // throw an exception
        return v[i]; // return item/elem at index i
      }
  };

  template <typename T> // generic type
  concept is_iterable = std::ranges::range<T> &&
                        !std::same_as<std::remove_cvref_t<T>, std::string> &&
                        !std::same_as<std::remove_cvref_t<T>, std::string_view> &&
                        !std::same_as<std::remove_cv_t<T>, char *>; // if it's an array/stl container
  template <typename T>
  /*
  *  Prints a single value with a proper handling for format and
  *  performance, it supports generic types + STL containers.
  */

  void print_v(T &&value) {
    if constexpr (is_iterable<T>) { // check if the used type is an array/STLc at compile time
      bool is_first = true; // check if current element is the first while iterating over it

      // [a, b, c]

      std::cout << '[';        // for proper format
      for (auto &&elem : value) { // iterating over it's elements
        if (!is_first) // if it's not the first element
          std::cout << ", "; // firstly print a comma
        print_v(elem); // print that element
        is_first = false; // we are no longer at the first element in the array/STLc
      }

      std::cout << ']'; // close it
      return;
    } else if constexpr (std::is_same_v<std::remove_cvref_t<T>, bool>) { // handling bool values for better printing (0->"false", 1->"true")
      if (value == true) // if it's true
      std::cout << "true";
      else // otherwise
      std::cout << "false";
      return;
    }
    // otherwise (if it's any other type including std::string like types)
    // if it's not printable, such as some types/custom classes that don't have the << operator
    static_assert(requires { std::cout << std::declval<T>(); }, "ERROR: Type is not printable.");
    std::cout << value; // print it
  }

  inline void print(std::string_view fmt) { print_v(fmt); } // an overloaded function used in recursion later if no args are specfied (print format only)

  /*
  * @brief Prints to console a lot of args according to the given format for better & faster printing process.
  *
  * @param fmt  It must be a string, the format of the text to be printable. Use '{}' and fill them with the wanted args after it
  * @param first First arg (Replaces the first placeholder in the format string)
  * @param rest The rest of args, according to the number of placeholders used in the format
  * If args number exceeds number of placeholders in format, an exception will be thrown, and if it's the opposite, {} will be printed normally.
  *
  * Examples:
  *
  * utils::print("Hello world {}, u will be '{}' today.", 2025, "lucky") -> output: Hello world 2025, u will be 'lucky' today
  * utils::print("My name is {}, I'm {13} years old, my height is {} meters.", "Monoal", 1.7)
  */
  template <typename T, typename... Rest>
  inline constexpr void print(std::string_view fmt, T &&first, Rest &&...rest)  {
    size_t ph_count = 0; // track number of placeholders given in format while iterating over it

    for (size_t i = 0; i < fmt.size(); i++) { // iterate over fmt char by char
      if (fmt[i] == '{' && i + 1 < fmt.size() && fmt[i + 1] == '}') { // if we found "{}"
        if (ph_count <= 1 + sizeof...(Rest)) { // if the count of placeholders is less or equal to number of args given to function
          print_v(first); // print the first arg
          print(fmt.substr(i + 2), std::forward<Rest>(rest)...); // repeat the process (recursion) until the for loop is finished
          return;
        } else
          std::cout << "{}"; //  if a placeholder is available but no arg is given, print "{}" normally
        ph_count++; // increase placeholder count
        continue;
      }

      std::cout << fmt[i]; // if it's another character, or only { or } is found print it directly
    }

    // since real placholder count will be when the loop is finished
    if (1 + sizeof...(Rest) > ph_count) // check if args count is bigger than ph count
      throw std::runtime_error("Args count exceeds placeholder count"); // runtime erorr since ph_count can't be evaluated at compile time
  }

    /*
  * @brief Prints to console a lot of args according to the given format for better & faster printing process, with a new line taken.
  *
  * @param fmt  It must be a string, the format of the text to be printable. Use '{}' and fill them with the wanted args after it
  * @param args The args for the format, according to the number of placeholders used in the format
  * If args number exceeds number of placeholders in format, an exception will be thrown, if it's the opposite "{}" will be printed normally.
  *
  * Examples:
  *
  * utils::println("Hello world {}, u will be '{}' today.", 2025, "lucky") -> output: Hello world 2025, u will be 'lucky' today\n
  * utils::println("My name is {}, I'm {13} years old, my height is {} meters.", "Monoal", 1.7) -> output: My name is Monoal, I'm {13} years old, my height is 1.7 meters.\n
  */
  template <typename... Args>
  void println(std::string_view fmt, Args &&...args) {
    print(fmt, std::forward<Args>(args)...); // use the inline version function and forwarding parameters since it only requires rvalue references
                                                                                  // to prevent larger code and repetition
    std::cout << "\n"; // take a new line (println)
  }

  constexpr bool is_number(std::string& token) noexcept {
    if (token.empty())
      return false;

    std::size_t start = 0;
    if (token[0] == '+' || token[0] == '-') {
      start = 1;
      if (token.length() == 1) return false;
    }

    auto isdigit = [](const char& c) { return c >= '0' && c <= '9'; };

    bool has_digit = false;
    bool has_dot = false;

    for (std::size_t i = start; i < token.length(); i++) {
      if (isdigit(token[i])) {
        has_digit = true;
      } else if (token[i] == '.') {
        if (has_dot)
          return false;
        has_dot = true;
      } else {
        return false;
      }
    }

    return has_digit;
  }

  constexpr bool is_letter(const char& c) noexcept {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
  }
} // namespace utils
