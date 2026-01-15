#include "token.hpp"

auto main() -> int32_t {
  sya::Token token("Token1", sya::TokenType::UNKNOWN);
  utils::println("{}", token.at(2));

  return 0;
}
