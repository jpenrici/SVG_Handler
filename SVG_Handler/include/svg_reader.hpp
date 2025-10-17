/*
 * Loads SVG file type.
 */

#pragma once

#include <expected>
#include <filesystem>
#include <string>

namespace SvgReader {

    using FileHandle = std::unique_ptr<std::ifstream>;
    using ExpectedFile = std::expected<FileHandle, std::error_code>;

    /**
     * @brief Checks the existence of an SVG file.
     *
     * @param path
     * @return file or error
     */
    auto check(const std::filesystem::path &path) -> ExpectedFile;

    /**
     * @brief Load SVG file.
     *
     * @param path
     * @return SVG text
     */
    auto load(std::string_view path) -> std::string;

} // namespace SvgReader

/**
 * @brief Basic test.
 */
void test_svg_reader();
