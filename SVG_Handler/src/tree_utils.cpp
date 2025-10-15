#include "tree_utils.hpp"

#include <cassert>
#include <functional>
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

auto TreeUtils::process(const std::vector<TagTuple> &svg_tagTuple) -> Tree {
  Tree tree;

  if (svg_tagTuple.empty()) {
    std::println("[ERROR] : Empty SVG tag sequence. Tree not created.");
    return tree;
  }

  std::stack<Node *> node_stack;

  for (const auto &[tag, attrs, tag_type] : svg_tagTuple) {
    if (tag.empty())
      continue;

    switch (tag_type) {
    case TagType::Open: {
      auto new_node = std::make_unique<Node>(tag, attrs);

      if (node_stack.empty()) {
        // Root
        tree.root = std::move(new_node);
        node_stack.push(tree.root.get());
      } else {
        // Children
        Node *parent = node_stack.top();
        parent->children.push_back(std::move(new_node));
        node_stack.push(parent->children.back().get());
      }
      break;
    }

    case TagType::SelfClose: {
      auto new_node = std::make_unique<Node>(tag, attrs);

      if (!node_stack.empty()) {
        node_stack.top()->children.push_back(std::move(new_node));
      } else {
        // Standalone tag outside the root (rare case, but allowed)
        tree.root = std::move(new_node);
      }
      break;
    }

    case TagType::Close: {
      if (!node_stack.empty())
        node_stack.pop();
      else
        std::println("[WARN] : Unmatched closing tag </{}> ignored.", tag);
      break;
    }

    case TagType::Unknown:
      break;
    }
  }

  if (!node_stack.empty()) {
    std::println("[WARN] : Unbalanced tree ({} unclosed tag(s)).",
                 node_stack.size());
  }

  return tree;
}

void TreeUtils::view(Tree &tree) {

  if (!tree.root) {
    std::println("[INFO] : Empty tree.");
    return;
  }

  std::println("[INFO] : SVG Tree Structure");

  std::function<void(const TreeUtils::Node *, int)> view_node;

  view_node = [&](const Node *node, int depth) {
    if (!node)
      return;

    // View tag
    std::println("{}{}", std::string(depth * 2, ' '), node->tag);

    // View tag attributes
    if (!node->attributes.empty()) {
      for (const auto &[name, value] : node->attributes)
        std::println("{}|_ {}=\"{}\"", std::string(depth * 2, ' '), name,
                     value);
    }

    // View child tag
    if (!node->children.empty()) {
      for (const auto &child : node->children)
        view_node(child.get(), depth + 1); // recursion
    }
  };

  view_node(tree.root.get(), 0);
}

void test_tree_utils() {

  using TreeUtils::process;
  using TreeUtils::Status;
  using TreeUtils::TagTuple;
  using TreeUtils::TagType;
  using TreeUtils::validate;
  using TreeUtils::view;

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

  // Process and View Test
  Tree tree = process(valid_svg2);

  // Process Test : Main structure
  assert(tree.root);
  assert(tree.root->tag == "svg");
  assert(tree.root->children.size() == 1);

  Node *g_node = tree.root->children[0].get();
  assert(g_node);
  assert(g_node->tag == "g");
  assert(g_node->children.size() == 1);

  Node *circle_node = g_node->children[0].get();
  assert(circle_node);
  assert(circle_node->tag == "circle");
  assert(circle_node->children.empty());

  // Process Test : <svg> tag attributes
  const auto &svg_attrs = tree.root->attributes;
  assert(svg_attrs.size() == 3);
  assert(svg_attrs[0][0] == "width" && svg_attrs[0][1] == "200");
  assert(svg_attrs[1][0] == "height" && svg_attrs[1][1] == "200");
  assert(svg_attrs[2][0] == "xmlns" &&
         svg_attrs[2][1] == "http://www.w3.org/2000/svg");

  // Process Test : <g> tag attributes
  const auto &g_attrs = g_node->attributes;
  assert(g_attrs.size() == 1);
  assert(g_attrs[0][0] == "id" && g_attrs[0][1] == "group1");

  // Process Test : <circle /> tag attributes
  const auto &c_attrs = circle_node->attributes;
  assert(c_attrs.size() == 6);

  auto find_attr = [](const Attributes &attrs,
                      std::string_view key) -> std::string {
    for (auto &a : attrs)
      if (a[0] == key)
        return a[1];
    return {};
  };

  assert(find_attr(c_attrs, "cx") == "55");
  assert(find_attr(c_attrs, "cy") == "55");
  assert(find_attr(c_attrs, "r") == "55");
  assert(find_attr(c_attrs, "stroke") == "red");
  assert(find_attr(c_attrs, "stroke-width") == "4");
  assert(find_attr(c_attrs, "fill") == "yellow");

  // Hierarchy View
  view(tree);

  std::println("[TEST] {} : test completed", __PRETTY_FUNCTION__);
}

#ifdef BUILD_TEST_EXE
auto main() -> int {

  test_tree_utils();

  return 0;
}
#endif
