/*
 * SVG Handler CLI
 * ----------------
 * Executes the SVG processing pipeline or specific actions (view/validate).
 *
 * Usage:
 *   ./svg_handler_cli <input.svg> <output.csv>
 *   ./svg_handler_cli --view <input.svg>
 *   ./svg_handler_cli --validate <input.svg>
 *
 * Description:
 *   1. Reads the input SVG file.
 *   2. Parses and validates its structure.
 *   3. Builds an internal tree representation.
 *   4. Converts the tree into a CSV table.
 *   5. Saves the CSV to the specified output path.
 */

#include "svg_core.hpp"
#include "svg_handler.hpp"
#include "tree_utils.hpp" // TreeUtils::view and validate

#include <cstdlib>
#include <filesystem>
#include <print>
#include <string_view>

namespace fs = std::filesystem;

constexpr std::string_view USAGE = R"(
Usage:
  ./svg_handler_cli <input.svg> <output.csv>
  ./svg_handler_cli --view <input.svg>
  ./svg_handler_cli --validate <input.svg>

Description:
  Executes the SVG Handler pipeline or specific commands.

Examples:
  ./svg_handler_cli resources/sample.svg output/sample.csv
  ./svg_handler_cli --view resources/sample.svg
  ./svg_handler_cli --validate resources/sample.svg
)";

auto main(int argc, char *argv[]) -> int {
  using namespace SVG_HANDLER;
  using namespace color;

  std::println("{}[INFO]{} : SVG Handler CLI", blue, reset);

  // Validate CLI arguments
  if (argc < 2) {
    std::println("{}[ERROR]{} : Missing arguments!", red, reset);
    std::println("{}{}{}", green, USAGE, reset);
    return EXIT_FAILURE;
  }

  std::string_view arg1 = argv[1];

  // Handle special flags (--view / --validate)
  if (arg1 == "--view" || arg1 == "--validate") {
    if (argc != 3) {
      std::println("{}[ERROR]{} : Missing input file for '{}'", red, reset,
                   arg1);
      std::println("{}{}{}", green, USAGE, reset);
      return EXIT_FAILURE;
    }

    std::string_view input_path = argv[2];
    if (!fs::exists(input_path)) {
      std::println("{}[ERROR]{} : File not found: '{}'", red, reset,
                   input_path);
      return EXIT_FAILURE;
    }

    try {
      SVG handler(input_path, "temp.csv");
      auto content = handler.load();
      auto prepared = handler.prepare(content);
      auto tokens = handler.tokenize(prepared);

      if (arg1 == "--validate") {
        auto status = TreeUtils::validate(tokens);
        if (status == TreeUtils::Status::Success)
          std::println("{}[INFO]{} : SVG validation succeeded.", green, reset);
        else
          std::println("{}[WARN]{} : SVG validation failed.", yellow, reset);
      } else if (arg1 == "--view") {
        auto tree = handler.build(tokens);
        std::println("{}[INFO]{} : SVG structure", blue, reset);
        TreeUtils::view(tree);
      }

    } catch (const std::exception &e) {
      std::println("{}[ERROR]{} : Exception: {}", red, reset, e.what());
      return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
  }

  // Standard pipeline execution
  if (argc != 3) {
    std::println("{}[ERROR]{} : Invalid arguments!", red, reset);
    std::println("{}{}{}", green, USAGE, reset);
    return EXIT_FAILURE;
  }

  std::string_view input_path = argv[1];
  std::string_view output_path = argv[2];

  if (!fs::exists(input_path)) {
    std::println("{}[ERROR]{} : Input file not found: '{}'", red, reset,
                 input_path);
    return EXIT_FAILURE;
  }

  try {
    // Create destination directory if needed
    fs::create_directories(fs::path(output_path).parent_path());

    SVG handler(input_path, output_path);
    handler.execute();

    auto abs_output = fs::absolute(output_path).string();
    std::println("{}[INFO]{} : Output successfully saved to '{}'", green, reset,
                 abs_output);
  } catch (const std::exception &e) {
    std::println("{}[ERROR]{} : Exception: {}", red, reset, e.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
