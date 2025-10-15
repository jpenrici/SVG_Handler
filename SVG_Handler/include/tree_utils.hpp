/*
 * Organizes the data in the SVG file into a tree.
 */

#pragma once

#include <array>
#include <expected>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace TreeUtils {

    using Attribute = std::array<std::string, 2>;
    using Attributes = std::vector<Attribute>;
    using TagTuple = std::tuple<std::string, Attributes, bool>;

    struct Node {
        std::string tag;
        Attributes attributes;
        std::vector<std::unique_ptr<Node>> children;

        Node(std::string t, Attributes a = {})
            : tag(std::move(t)), attributes(std::move(a)) {}
    };

    struct Tree {
        std::unique_ptr<Node> root;
    };

    enum class Error {
        EmptyInput,
        InvalidRoot,
        UnbalancedTags,
        Unknown
    };

    using CsvRow = std::vector<std::string>;
    using CsvTable = std::vector<CsvRow>;
    using ExpectedTree = std::expected<Tree, Error>;


    // Syntax check
    auto validate(const std::vector<TagTuple>& svg_tagTuple) -> ExpectedTree;

    // Build the hierarchical tree
    auto process(const std::vector<TagTuple>& svg_tagTuple) -> Tree;

    // Convert Tree -> CSV table
    auto table(const Tree& tree) -> CsvTable;

    // Print hierarchy
    void view(const Node &node, int depth = 0);

} // namespace TreeUtils


/**
 * @brief Basic test.
 */
void test_tree_utils();
