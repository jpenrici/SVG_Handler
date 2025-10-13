#include "string_utils.hpp"

#include <algorithm>
#include <array>
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

  if (count_smaller < 1 || count_greater < 1) {
    return false;
  }

  if (count_smaller != count_greater) {
    return false;
  }

  // Validate if not empty.
  return !bkp.empty();
}

auto StringUtils::prepare(std::string_view svg) -> std::vector<std::string> {

  if (!StringUtils::validate(svg)) {
    std::println("Error preparing SVG!");
    return {/* empty */};
  }

  // Sanitize
  std::string bkp{StringUtils::sanitize(svg)};

  // Prepare output
  std::vector<std::string> result{};
  std::string str{};
  bool flag{false};
  for (const auto &ch : bkp) {
    if (ch == '<') {
      flag = true;
    }
    if (flag) {
      str.push_back(ch);
    }
    if (ch == '>') {
      flag = false;
      result.push_back(str);
      str.clear();
    }
  }

  // Return prepared structure
  return result;
}

auto StringUtils::process(std::string_view svg)
    -> std::tuple<std::string,                             // tag name
                  std::vector<std::array<std::string, 2>>> // attributes
{
  if (!StringUtils::validate(svg)) {
    std::println("Error processing SVG!");
    return {/* empty */};
  }

  // Trim
  auto trim = [](std::string &str) {
    auto start = std::find_if_not(str.begin(), str.end(), ::isspace);
    auto end = std::find_if_not(str.rbegin(), str.rend(), ::isspace).base();
    return (start < end) ? std::string(start, end) : "";
  };

  // Split
  auto split = [](std::string &str, char ch) {
    auto vec = str | std::ranges::views::split(ch) |
               std::ranges::views::transform([](auto const &sr) {
                 return std::string_view(sr.begin(), sr.end());
               }) |
               std::ranges::to<std::vector<std::string>>();
    return vec;
  };

  // Sanitize
  std::string bkp{StringUtils::sanitize(svg)};
  bkp = trim(bkp);

  // Count < and >
  auto count_greater = std::count(bkp.begin(), bkp.end(), '>');
  auto count_smaller = std::count(bkp.begin(), bkp.end(), '<');

  if (count_smaller != 1 || count_greater != 1) {
    std::println("Invalid SVG!");
    return {/* empty */};
  }

  // Check position of < and >
  if (!bkp.starts_with("<") || !bkp.ends_with(">")) {
    std::println("Invalid SVG!");
    return {/* empty */};
  }

  // Ignore
  if (bkp.starts_with("<?") || bkp.starts_with("<!--")) {
    std::println("Ignore input!");
    return {/* empty */};
  }

  // Remove < and >
  bkp = bkp.substr(1, bkp.size() - 2);
  bkp = trim(bkp);
  if (bkp.ends_with("/")) {
    bkp = bkp.substr(0, bkp.size() - 2);
  }

  // Split
  auto elements = split(bkp, ' ');

  std::string tag = elements.front();
  std::vector<std::array<std::string, 2>> attributes;
  for (size_t i = 1; i < elements.size(); ++i) {
    auto e = elements.at(i);
    if (e.contains("=")) {
      auto arr = split(e, '=');
      if (arr.size() == 2) {
        auto attr = trim(arr.front());
        auto value = arr.back();
        if (value.starts_with("\"")) {
          value = value.substr(1, value.size() - 1);
        }
        if (value.ends_with("\"")) {
          value = value.substr(0, value.size() - 1);
        }
        attributes.push_back({attr, value});
      }
    }
  }

  return {tag, attributes};
}

void test_string_utils() {

  using std::string_view;
  using StringUtils::prepare;
  using StringUtils::process;
  using StringUtils::sanitize;
  using StringUtils::validate;

  // Sanitization test
  assert(sanitize(string_view{"\t\r\v\f\b\n\0"}) == std::string{""});
  assert(sanitize(string_view{" \t\r\v\f\b\n\0"}) == std::string{" "});

  // Valid test
  assert(validate(string_view{}) == false);
  assert(validate(string_view{" "}) == false);
  assert(validate(string_view{"tag"}) == false);
  assert(validate(string_view{"<tag"}) == false);
  assert(validate(string_view{"<tag>"}) == true);

  // Prepare test
  std::string svg{"<svg width=\"200\" height=\"200\" "
                  "xmlns=\"http://www.w3.org/2000/svg\">"
                  "<g id=\"group1\">"
                  "<circle cx=\"55\" cy=\"55\" r=\"55\" stroke=\"red\" "
                  "stroke-width=\"4\" fill=\"yellow\" />"
                  "</g>"
                  "</svg>"};

  std::vector<std::string> vec1{
      "<svg width=\"200\" height=\"200\" "
      "xmlns=\"http://www.w3.org/2000/svg\">",
      "<g id=\"group1\">",
      "<circle cx=\"55\" cy=\"55\" r=\"55\" stroke=\"red\" "
      "stroke-width=\"4\" fill=\"yellow\" />",
      "</g>", "</svg>"};

  using Vector = std::vector<std::array<std::string, 2>>;
  using Tuple = std::tuple<std::string, Vector>;

  std::vector<Tuple> vec2{Tuple{{"svg"},
                                {{"width", "200"},
                                 {"height", "200"},
                                 {"xmlns", "http://www.w3.org/2000/svg"}}},
                          Tuple{"g", {{"id", "group1"}}},
                          Tuple{"circle",
                                {{"cx", "55"},
                                 {"cy", "55"},
                                 {"r", "55"},
                                 {"stroke", "red"},
                                 {"stroke-width", "4"},
                                 {"fill", "yellow"}}},
                          Tuple{"/g", {}}, Tuple{"/svg", {}}};

  // Identify < content >
  auto result1 = prepare(svg);
  assert(result1 == vec1);

  // Process simple string
  std::string str = "< tag attr1=\"1\" attr2=\"2\" />";
  Tuple tp{"tag", {{"attr1", "1"}, {"attr2", "2"}}};
  auto result2 = process(str);
  assert(result2 == tp);

  // Process svg
  if (vec2.size() == result1.size()) {
    for (size_t i = 0; i < result1.size(); ++i) {
      auto result3 = process(result1.at(i));
      assert(result3 == vec2.at(i));
    }
  }

  std::println("[TEST] {} : test completed", __PRETTY_FUNCTION__);
}
