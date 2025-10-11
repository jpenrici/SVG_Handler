#include "string_utils.hpp"

#include <print>

auto test_string_utils() -> bool {
  std::println("DEBUG: {}", __PRETTY_FUNCTION__);

  std::string svg{"<svg width=\"200\" height=\"200\" "
                  "xmlns=\"http://www.w3.org/2000/svg\">"
                  "<circle cx=\"55\" cy=\"55\" r=\"55\" stroke=\"red\" "
                  "stroke-width=\"4\" fill=\"yellow\" />"};

  std::println("{}", svg);

  // Receive SVG text
  // Validate SVG text
  // Process SVG text
  // Separate Tags, Attributes
  // Return prepared structure

  return true;
}
