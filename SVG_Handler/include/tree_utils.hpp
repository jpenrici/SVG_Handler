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

    enum class TagType {Unknown, Open, Close, SelfClose};

    using Attribute = std::array<std::string, 2>; // {"name", "value"}
    using Attributes = std::vector<Attribute>;
    using TagTuple = std::tuple<std::string, Attributes, TagType>; // {tag, attrs, tag type}

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

    enum class Status {
        Success,
        EmptyInput,
        InvalidRoot,
        UnbalancedTags,
        Unknown
    };

    using CsvRow = std::vector<std::string>;
    using CsvTable = std::vector<CsvRow>;

    /**
     * @brief validate
     * Syntax check
     *
     * @param svg_tagTuple
     * @return enum class Status { Success, EmptyInput, InvalidRoot, UnbalancedTags, Unknown }
     */
    auto validate(const std::vector<TagTuple>& svg_tagTuple) -> Status;

    /**
     * @brief process
     * Build the hierarchical tree
     *
     * @param svg_tagTuple
     * @return Tree
     */
    auto process(const std::vector<TagTuple>& svg_tagTuple) -> Tree;

    /**
     * @brief table
     * Convert Tree -> CSV table
     *
     * @param tree
     * @return CSV table
     */
    auto table(const Tree& tree) -> CsvTable;

    /**
     * @brief view
     * Print hierarchy
     *
     * @param node
     * @param depth
     */
    void view(const Node &node, int depth = 0);

} // namespace TreeUtils


/**
 * @brief Basic test.
 */
void test_tree_utils();
