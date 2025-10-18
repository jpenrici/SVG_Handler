/*
 * svg_handler.hpp
 *
 * Controls the execution of all processing stages:
 *  1. Load SVG file (svg_reader)
 *  2. Process SVG strings and extract tags (string_utils)
 *  3. Build the tree hierarchy (tree_utils)
 *  4. Export to CSV (csv_exporter)
 */

#pragma once

#include "csv_exporter.hpp"
#include "string_utils.hpp"
#include "svg_reader.hpp"
#include "tree_utils.hpp"

#include <string>

namespace SVG_HANDLER {

    class SVG {
    public:

        /**
         * @brief SVG
         * @param file_path_svg
         * @param file_path_csv
         */
        explicit SVG(std::string_view file_path_svg, std::string_view file_path_csv);

        /**
         * @brief execute
         * Main pipeline
         */
        void execute();

        /**
         * @brief load
         * Load SVG file (svg_reader)
         *
         * @return SVG text
         */
        auto load() -> std::string;

        /**
         * @brief prepare
         * Prepare SVG strings and extract tags (string_utils)
         *
         * @param svg
         * @return Vector of rows '<content>'.
         */
        auto prepare(std::string_view svg) -> std::vector<std::string>;

        /**
         * @brief tokenize
         * Process SVG prepared and separates tag and attributes (string_utils)
         *
         * @param Vector svg tags
         * @return Tuple with {tag name, attribute array}.
         */
        auto tokenize(const std::vector<std::string>& tags) -> std::vector<StringUtils::TagTuple>;

        /**
         * @brief build
         *
         * @param svg Tag Tuple
         * @return Tree
         */
        auto build(const std::vector<StringUtils::TagTuple>& svg_tagTuple) -> TreeUtils::Tree;

        /**
         * @brief to_csv
         * Export to CSV (csv_exporter)
         *
         * @param tree
         * @return CSV table
         */
        auto to_csv(const TreeUtils::Tree& tree) -> TreeUtils::CsvTable;

        /**
         * @brief export_csv
         *
         * @param CSV table
         * @param path
         */
        void export_csv(const TreeUtils::CsvTable& csvTable, std::string_view path);

    private:
        std::string file_path_svg_;
        std::string file_path_csv_;

    }; // class SVG

} // namespace SVG_HANDLER

void test_svg_handler();
