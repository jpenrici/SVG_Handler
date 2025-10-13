/*
 * Loads SVG file type.
 */

#pragma once

#include <expected>
#include <filesystem>
#include <string>

namespace SvgReader {

    /**
     * @brief Checks the existence of an SVG file.
     *
     * @param path
     * @return file or error
     */
    auto check(const std::filesystem::path &path) -> std::expected<std::ifstream, std::error_code>;

    /**
     * @brief Load SVG file.
     *
     * @param path
     * @return SVG text
     */
    auto load(std::string_view path) -> std::string;

}

/**
 * @brief Basic test.
 */
void test_svg_reader();
