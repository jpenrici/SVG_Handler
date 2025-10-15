#include "tree_utils.hpp"

#include <cassert>
#include <print>
#include <stack>

using namespace TreeUtils;

auto TreeUtils::validate(const std::vector<TagTuple> &svg_tagTuple) -> Status {
  if (svg_tagTuple.empty()) {
    std::println("[ERROR] : Empty SVG tag sequence.");
    return Status::EmptyInput;
  }

  std::stack<std::string> tag_stack;
  bool has_root{false};

  for (const auto &[tag, attrs, tag_type] : svg_tagTuple) {
    if (tag.empty())
      continue;

    switch (tag_type) {
    case TagType::Open:
      // Open tag (e.g. <tag>)
      if (tag_stack.empty()) {
        if (has_root) {
          std::println("[ERROR] : Multiple root elements detected.");
          return Status::InvalidRoot;
        }
        has_root = true;
      }
      tag_stack.push(tag);
      break;

    case TagType::SelfClose:
      // Self-closing tag (e.g. <tag />)
      break;

    case TagType::Close:
      // Closed tag (e.g. </tag>)
      if (tag_stack.empty()) {
        std::println("[ERROR] : Closing tag </{}> without opening.", tag);
        return Status::UnbalancedTags;
      }

      if (tag_stack.top() != tag) {
        std::println("[ERROR] : Tag mismatch: opened <{}> but closed </{}>.",
                     tag_stack.top(), tag);
        return Status::UnbalancedTags;
      }
      tag_stack.pop();
      break;
    case TagType::Unknown:
      break;
    }
  }

  if (!tag_stack.empty()) {
    std::println("[ERROR] : Unclosed tag(s) detected at end of file.");
    return Status::UnbalancedTags;
  }

  std::println("[INFO] : SVG structure validated successfully.");
  return Status::Success;
}

// void TreeUtils::view(const Node &node, int depth) {
//   std::println("{}{}", std::string(depth * 2, ' '), node.tag);
//   for (auto &child : node.children)
//     view(*child, depth + 1);
// }

void test_tree_utils() {

  using TreeUtils::Status;
  using TreeUtils::TagTuple;
  using TreeUtils::TagType;
  using TreeUtils::validate;

  // Empty SVG
  assert(validate({}) == Status::EmptyInput);

  // Missing open tag closure.
  std::vector<TagTuple> missing_close{{"svg", {}, TagType::Open},
                                      {"g", {}, TagType::Open},
                                      {"circle", {}, TagType::SelfClose},
                                      {"svg", {}, TagType::Close}};

  assert(validate(missing_close) == Status::UnbalancedTags);

  // Tag closure without opening.
  std::vector<TagTuple> bad_close{{"svg", {}, TagType::Open},
                                  {"circle", {}, TagType::SelfClose},
                                  {"g", {}, TagType::Open}};

  assert(validate(bad_close) == Status::UnbalancedTags);

  // Bad hierarchy
  std::vector<TagTuple> bad_hierarchy{{"svg", {}, TagType::Open},
                                      {"g", {}, TagType::Open},
                                      {"svg", {}, TagType::Close},
                                      {"g", {}, TagType::Close}};

  assert(validate(bad_hierarchy) == Status::UnbalancedTags);

  // Valid structure.
  std::vector<TagTuple> valid_svg1{{"svg", {}, TagType::Open},
                                   {"g", {}, TagType::Open},
                                   {"circle", {}, TagType::SelfClose},
                                   {"g", {}, TagType::Close},
                                   {"svg", {}, TagType::Close}};

  assert(validate(valid_svg1) == Status::Success);

  std::vector<TagTuple> valid_svg2{
      TagTuple{{"svg"},
               {{"width", "200"},
                {"height", "200"},
                {"xmlns", "http://www.w3.org/2000/svg"}},
               TagType::Open},
      TagTuple{"g", {{"id", "group1"}}, TagType::Open},
      TagTuple{"circle",
               {{"cx", "55"},
                {"cy", "55"},
                {"r", "55"},
                {"stroke", "red"},
                {"stroke-width", "4"},
                {"fill", "yellow"}},
               TagType::SelfClose},
      TagTuple{"g", {}, TagType::Close}, TagTuple{"svg", {}, TagType::Close}};

  assert(validate(valid_svg2) == Status::Success);

  std::println("[TEST] {} : test completed", __PRETTY_FUNCTION__);
}

#ifdef BUILD_TEST_EXE
auto main() -> int {

  test_tree_utils();

  return 0;
}
#endif
