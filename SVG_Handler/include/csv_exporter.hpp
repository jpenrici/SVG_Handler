/*
 * Exports data in CSV format.
 */

#pragma once

#include <expected>
#include <filesystem>
#include <fstream>
#include <memory>
#include <vector>

namespace CsvExporter {

    using FileHandle = std::unique_ptr<std::ofstream>;
    using ExpectedFile = std::expected<FileHandle, std::error_code>;

    using CsvRow = std::vector<std::string>;
    using CsvTable = std::vector<CsvRow>;

    /**
    * @brief Checks the existence of an CSV file.
    *
    * @param path
    * @return file or error
    */
    auto check(const std::filesystem::path &path) -> ExpectedFile;

    /**
    * @brief Save CSV file.
    *
    * @param path
    * @param vector 2D of text
    * @param delimiter char
    */
    void save(std::string_view path, const CsvTable &table, char delimiter = ',');
} // namespace CsvExporter

/**
 * @brief Basic test.
 */
void test_csv_exporter();
