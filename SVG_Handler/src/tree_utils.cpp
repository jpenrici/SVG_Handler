#include "tree_utils.hpp"

#include <cassert>
#include <print>
#include <stack>

#include "tree_utils.hpp"
#include <print>
#include <stack>

auto TreeUtils::validate(const std::vector<TagTuple> &svg_tagTuple)
    -> ExpectedTree {

  if (svg_tagTuple.empty()) {
    std::println("[ERROR] : Empty SVG tag sequence.");
    return std::unexpected(Error::EmptyInput);
  }

  std::stack<std::string> tag_stack;
  bool has_root{false};

  for (const auto &[tag, attrs, is_closing] : svg_tagTuple) {
    if (tag.empty())
      continue;

    if (!is_closing) {
      if (tag_stack.empty()) {
        if (has_root) {
          std::println("[ERROR] : Multiple root elements detected.");
          return std::unexpected(Error::InvalidRoot);
        }
        has_root = true;
      }
      tag_stack.push(tag);
    } else {
      if (tag_stack.empty()) {
        std::println("[ERROR] : Closing tag </{}> without opening.", tag);
        return std::unexpected(Error::UnbalancedTags);
      }

      auto top = tag_stack.top();
      tag_stack.pop();

      if (top != tag) {
        std::println("[ERROR] : Tag mismatch: opened <{}> but closed </{}>.",
                     top, tag);
        return std::unexpected(Error::UnbalancedTags);
      }
    }
  }

  if (!tag_stack.empty()) {
    std::println("[ERROR] : Unclosed tag(s) detected at end of file.");
    return std::unexpected(Error::UnbalancedTags);
  }

  Tree tree{};
  std::println("[INFO] : SVG structure validated successfully.");
  return tree;
}

// void TreeUtils::view(const Node &node, int depth) {
//   std::println("{}{}", std::string(depth * 2, ' '), node.tag);
//   for (auto &child : node.children)
//     view(*child, depth + 1);
// }

void test_tree_utils() {

  using TreeUtils::Error;
  using TreeUtils::TagTuple;
  using TreeUtils::validate;

  std::vector<TagTuple> valid_svg{{"svg", {}, false},
                                  {"g", {}, false},
                                  {"circle", {}, true},
                                  {"g", {}, true},
                                  {"svg", {}, true}};

  auto result1 = validate(valid_svg);
  assert(result1.has_value());

  std::vector<TagTuple> invalid_svg1{{"svg", {}, false},
                                     {"g", {}, false},
                                     {"circle", {}, true},
                                     {"svg", {}, true}};

  auto result2 = validate(invalid_svg1);
  assert(!result2.has_value());
  assert(result2.error() == Error::UnbalancedTags);

  std::vector<TagTuple> invalid_svg2{
      {"svg", {}, false}, {"circle", {}, true}, {"g", {}, true}};

  auto result3 = validate(invalid_svg2);
  assert(!result3.has_value());

  std::vector<TagTuple> vec{TagTuple{{"svg"},
                                     {{"width", "200"},
                                      {"height", "200"},
                                      {"xmlns", "http://www.w3.org/2000/svg"}},
                                     false},
                            TagTuple{"g", {{"id", "group1"}}, false},
                            TagTuple{"circle",
                                     {{"cx", "55"},
                                      {"cy", "55"},
                                      {"r", "55"},
                                      {"stroke", "red"},
                                      {"stroke-width", "4"},
                                      {"fill", "yellow"}},
                                     true},
                            TagTuple{"g", {}, true}, TagTuple{"svg", {}, true}};

  // auto tree = TreeUtils::process(vec);
  // TreeUtils::view(tree);

  std::println("[TEST] {} : test completed", __PRETTY_FUNCTION__);
}

#ifdef BUILD_TEST_EXE
auto main() -> int {

  test_tree_utils();

  return 0;
}
#endif
