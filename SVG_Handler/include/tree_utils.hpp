/*
 * Organizes the data in the SVG file into a tree.
 */

#pragma once

#include "svg_core.hpp"

#include <array>
#include <expected>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace TreeUtils {

    using svg_core::TagTuple; // {tag, attrs, tag type}
    using svg_core::TagType; // {Unknown, Open, Close, SelfClose}
    using svg_core::Attributes; // vector {"name", "value"}

    using svg_core::Status; // {Success, EmptyInput, InvalidRoot, UnbalancedTags}

    using svg_core::CsvRow;
    using svg_core::CsvTable;

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
     * root
     * |_tag1
     *   | attributes
     *   |_tag2
     *     | attributes
     *
     * @param tree Tree structure containing the parsed SVG hierarchy.
     */
    void view(Tree& tree);

} // namespace TreeUtils


/**
 * @brief Basic test.
 */
void test_tree_utils();
