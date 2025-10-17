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

auto TreeUtils::table(const Tree &tree) -> CsvTable {

  if (!tree.root) {
    std::println("[INFO] : Empty tree.");
    return {};
  }

  CsvTable table;
  CsvRow header{"ID", "ParentID", "Depth", "Tag", "Attribute", "Value"};
  table.push_back(header);

  size_t current_id = 0;

  std::function<void(const Node *, int, int)> traverse;

  traverse = [&](const Node *node, int depth, int parent_id) {
    if (!node)
      return;

    size_t node_id = current_id++;

    if (node->attributes.empty()) {
      table.push_back({std::to_string(node_id), std::to_string(parent_id),
                       std::to_string(depth), node->tag, "", ""});
    } else {
      for (const auto &[name, value] : node->attributes) {
        table.push_back({std::to_string(node_id), std::to_string(parent_id),
                         std::to_string(depth), node->tag, name, value});
      }
    }

    // Children
    for (const auto &child : node->children)
      traverse(child.get(), depth + 1, static_cast<int>(node_id)); // recursion
  };

  // Initialize
  traverse(tree.root.get(), 0, -1);

  return table;
}

void TreeUtils::view(Tree &tree) {

  if (!tree.root) {
    std::println("[INFO] : Empty tree.");
    return;
  }

  std::println("[INFO] : SVG Tree Structure\n");

  std::function<void(const Node *, int)> view_node;

  view_node = [&](const Node *node, int depth) {
    if (!node)
      return;

    // Prefix for indentation
    std::string indent(depth * 2, ' ');

    // View tag
    if (depth == 0)
      std::println("{}{}", indent, node->tag); // root
    else
      std::println("{}|_{}", std::string(depth * 2 - 2, ' '), node->tag);

    // View attributes
    if (!node->attributes.empty()) {
      for (const auto &[name, value] : node->attributes) {
        std::println("{}| {}=\"{}\"", indent, name, value);
      }
    }

    // View children
    for (const auto &child : node->children)
      view_node(child.get(), depth + 1); // recursion
  };

  // Initialize
  view_node(tree.root.get(), 0);
}

void test_tree_utils() {

  using TreeUtils::CsvRow;
  using TreeUtils::CsvTable;
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

  tree = process(std::vector<TagTuple>{{"svg", {}, TagType::Open},
                                       {"g", {}, TagType::Open},
                                       {"circle",
                                        {
                                            {

                                                {"attr1", "value1"},
                                                {"attr2", "value2"},
                                                {"attr3", "value3"},

                                            },
                                        },
                                        TagType::SelfClose},
                                       {"rect",
                                        {
                                            {

                                                {"attr1", "value1"},

                                            },
                                        },
                                        TagType::SelfClose},
                                       {"g", {}, TagType::Close},
                                       {"g", {}, TagType::Open},
                                       {"line", {}, TagType::SelfClose},
                                       {"g", {}, TagType::Open},
                                       {"circle",
                                        {
                                            {

                                                {"attr1", "value1"},
                                                {"attr2", "value2"},

                                            },
                                        },
                                        TagType::SelfClose},
                                       {"path", {}, TagType::SelfClose},
                                       {"g", {}, TagType::Close},
                                       {"g", {}, TagType::Close},
                                       {"svg", {}, TagType::Close}});
  view(tree);

  // Tree -> Csv Table
  CsvTable csvTable =
      table(process(std::vector<TagTuple>{{"svg", {}, TagType::Open},
                                          {"g", {}, TagType::Open},
                                          {"circle",
                                           {{

                                               {"attr1", "value1"},
                                               {"attr2", "value2"},

                                           }},
                                           TagType::SelfClose},
                                          {"g", {}, TagType::Close},
                                          {"svg", {}, TagType::Close}}));

  CsvTable csvTable_expected{
      {"ID", "ParentID", "Depth", "Tag", "Attribute", "Value"}, // Header
      {"0", "-1", "0", "svg", "", ""},                          // Root
      {"1", "0", "1", "g", "", ""},                             // Level 1
      {"2", "1", "2", "circle", "attr1", "value1"},             // Level 2
      {"2", "1", "2", "circle", "attr2", "value2"}};            // Level 2

  assert(!csvTable.empty());
  assert(csvTable.size() == 5);

  auto assert_csv_eq = [](const CsvTable &got, const CsvTable &expected) {
    assert(got.size() == expected.size() && "CSV size mismatch!");
    for (size_t i = 0; i < got.size(); ++i)
      assert(got[i] == expected[i] && "CSV row mismatch!");
  };

  assert_csv_eq(csvTable, csvTable_expected);

  std::println("[TEST] {} : test completed", __PRETTY_FUNCTION__);
}

#ifdef BUILD_TEST_EXE
auto main() -> int {

  test_tree_utils();

  return 0;
}
#endif
