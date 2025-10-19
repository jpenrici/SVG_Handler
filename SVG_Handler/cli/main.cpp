#include "svg_handler.hpp"

#include <cstdlib>
#include <filesystem>
#include <print>
#include <string_view>

namespace fs = std::filesystem;

constexpr std::string_view USAGE = R"(
Usage:
  ./svg_handler_cli <input.svg> <output.csv>

Description:
  Executes the SVG Handler pipeline:
    1. Reads the input SVG file.
    2. Parses and validates its structure.
    3. Builds an internal tree representation.
    4. Converts the tree into a CSV table.
    5. Saves the CSV to the specified output path.

Examples:
  ./svg_handler_cli resources/sample.svg output.csv
)";

auto main(int argc, char *argv[]) -> int {
  using namespace SVG_HANDLER;

  std::println("{}[INFO]{} : SVG Handler CLI", color::blue, color::reset);

  if (argc != 3) {
    std::println("{}[ERROR]{} : Invalid arguments!", color::red, color::reset);
    std::println("{}{}{}", color::green, USAGE, color::reset);
    return EXIT_FAILURE;
  }

  std::string_view input_path = argv[1];
  std::string_view output_path = argv[2];

  if (!fs::exists(input_path)) {
    std::println("{}[ERROR]{} : Input file not found: '{}'", color::red,
                 color::reset, input_path);
    return EXIT_FAILURE;
  }

  try {
    SVG handler(input_path, output_path);
    handler.execute();
  } catch (const std::exception &e) {
    std::println("{}[ERROR]{} : Exception: {}", color::red, color::reset,
                 e.what());
    return EXIT_FAILURE;
  }

  std::println("{}[INFO]{} : Output successfully saved to '{}'", color::green,
               color::reset, output_path);

  return EXIT_SUCCESS;
}
