/*
 * Manipulates string to separate Tags and attributes.
 * Simple lexical analysis.
 */

#pragma once

#include "svg_core.hpp"

#include <string_view>
#include <vector>

namespace StringUtils {

    using svg_core::TagTuple; // {tag, attrs, tag type}
    using svg_core::TagType; // {Unknown, Open, Close, SelfClose}
    using svg_core::Attributes; // vector {"name", "value"}

    /**
     * @brief Sanitize string SVG.
     *
     * @param str
     * @return String without special characters.
     */
    auto sanitize(std::string_view str) -> std::string;

    /**
     * @brief Validate SVG
     *
     * @param svg
     * @return True if SVG can be processed in this algorithm.
     */
    auto validate(std::string_view svg) -> bool;

    /**
    * @brief Prepare SVG text.
    * This function separates the possible tags from the SVG text.
    *
    * @param svg SVG text
    * @return Vector of rows '<content>'.
     */
    auto prepare(std::string_view svg) -> std::vector<std::string>;

    /**
     * @brief Processes SVG.
     * This function reads an SVG and separates the tag and attributes.
     *
     * @param svg
     * @return Tuple with {tag name, attribute array, tag type}.
     */
    auto process(std::string_view svg) -> TagTuple;

} // namespace StringUtils

/**
 * @brief Basic test.
 */
void test_string_utils();
