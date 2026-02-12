#include "ui.hpp"
#include "operator.hpp"

int main() {
  console::Interface ui(sya::functions);
  ui.run();

  return 0;
}
