#include "string_utils.hpp"

#include <algorithm>
#include <cassert>
#include <print>
#include <ranges>
#include <string>

auto StringUtils::sanitize(std::string_view str) -> std::string {

  std::string bkp(str);

  // Remove Non-printable/Control Characters
  std::array invisible{
      '\n', // Line Feed
      '\r', // Carriage Return
      '\t', // Horizontal Tab
      '\v', // Vertical Tab
      '\f', // Form Feed
      '\0', // NULL
      '\b', // Backspace
  };

  auto isInvisible = [&invisible](char ch) {
    return std::find(invisible.begin(), invisible.end(), ch) != invisible.end();
  };

  bkp.erase(std::remove_if(bkp.begin(), bkp.end(), isInvisible), bkp.end());

  // Replace '\>' to '>'
  size_t pos{0};
  while ((pos = bkp.find("/>", pos)) != std::string::npos) {
    bkp.replace(pos, bkp.length(), ">");
  }

  // Return sanitized string
  return bkp;
}

auto StringUtils::validate(std::string_view svg) -> bool {

  // Sanitize
  std::string bkp{StringUtils::sanitize(svg)};

  // Remove all spaces.
  bkp.erase(std::remove_if(bkp.begin(), bkp.end(), ::isspace), bkp.end());

  // Count < and >
  auto count_greater = std::count(bkp.begin(), bkp.end(), '>');
  auto count_smaller = std::count(bkp.begin(), bkp.end(), '<');

  if (count_smaller != count_greater) {
    return false;
  }

  // Validate if not empty.
  return !bkp.empty();
}

auto StringUtils::process(std::string_view svg) -> std::vector<std::string> {

  if (!StringUtils::validate(svg)) {
    std::println("Error!!!");
    return {/* empty */};
  }

  // Sanitize
  std::string bkp{StringUtils::sanitize(svg)};

  // Prepare output
  std::vector<std::string> result{};
  std::string str{};
  for (const auto &ch : bkp) {
    if (ch == '>' || ch == '<' || ch == ' ') {
      if (!str.empty()) {
        result.push_back(str);
        str.clear();
      }
      if (ch != ' ') {
        result.push_back(std::string{ch});
      }
    } else {
      str.push_back(ch);
    }
  }

  // Return prepared structure
  return result;
}

auto test_string_utils() -> bool {
  std::println("DEBUG: {}", __PRETTY_FUNCTION__);

  using std::string_view;
  using StringUtils::process;
  using StringUtils::sanitize;
  using StringUtils::validate;

  // Sanitization test
  assert(sanitize(string_view{"\t\r\v\f\b\n\0"}) == std::string{""});
  assert(sanitize(string_view{" \t\r\v\f\b\n\0"}) == std::string{" "});
  assert(sanitize(string_view{"<tag />"}) == std::string{"<tag >"});

  // Valid test
  assert(validate(string_view{}) == false);
  assert(validate(string_view{" "}) == false);
  assert(validate(string_view{"<tag"}) == false);
  assert(validate(string_view{" tag"}) == true);
  assert(validate(string_view{"<tag>"}) == true);

  // Process test
  std::string svg{"<svg width=\"200\" height=\"200\" "
                  "xmlns=\"http://www.w3.org/2000/svg\">"
                  "<circle cx=\"55\" cy=\"55\" r=\"55\" stroke=\"red\" "
                  "stroke-width=\"4\" fill=\"yellow\" />"};

  std::vector<std::string> vec{
      "<",
      "svg",
      "width=\"200\"",
      "height=\"200\"",
      "xmlns=\"http://www.w3.org/2000/svg\"",
      ">",
      "<",
      "circle",
      "cx=\"55\"",
      "cy=\"55\"",
      "r=\"55\"",
      "stroke=\"red\"",
      "stroke-width=\"4\"",
      "fill=\"yellow\"",
      ">",
  };

  assert(process(svg) == vec);

  return true;
}
