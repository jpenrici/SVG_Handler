#include "svg_handler.hpp"

#include <print>

auto main() -> int {
  std::println("SVG HANDLER");

  if (test_svg_handler()) {
    std::println("App is working!");
  } else {
    std::println("App is not working!");
  }

  return 0;
}
