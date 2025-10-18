/*
 * Test Center
 * Runs individual or full module tests via CLI arguments.
 *
 * Usage:
 *   ./svg_handler_cli_test test=0   -> Run all tests
 *   ./svg_handler_cli_test test=1   -> Run SvgReader test
 *   ./svg_handler_cli_test test=2   -> Run StringUtils test
 *   ./svg_handler_cli_test test=3   -> Run TreeUtils test
 *   ./svg_handler_cli_test test=4   -> Run CsvExporter test
 *   ./svg_handler_cli_test test=5   -> Run SVG_Handler pipeline test
 */

#include "svg_handler.hpp"

#include <print>

namespace color {
constexpr auto reset = "\033[0m";
constexpr auto green = "\033[32m";
constexpr auto yellow = "\033[33m";
constexpr auto blue = "\033[34m";
constexpr auto red = "\033[31m";
} // namespace color

constexpr std::string_view USAGE =
    R"(Usage: ./svg_handler_cli_test test=<number>
        test=0  Run all tests
        test=1  SvgReader test
        test=2  StringUtils test
        test=3  TreeUtils test
        test=4  CsvExporter test
        test=5  SVG_Handler pipeline test)";

void select_test(char option) {
  switch (option) {
  case '0':
    [[fallthrough]];
  case '1':
    test_svg_reader();
    if (option != '0')
      break;
  case '2':
    test_string_utils();
    if (option != '0')
      break;
  case '3':
    test_tree_utils();
    if (option != '0')
      break;
  case '4':
    test_csv_exporter();
    if (option != '0')
      break;
  case '5':
    test_svg_handler();
    break;
  default:
    std::println("{}[ERROR]{} Invalid option '{}'.", color::red, color::reset,
                 option);
    std::println("{}{}{}", color::blue, USAGE, color::reset);
    std::exit(EXIT_FAILURE);
  }
}

auto main(int argc, char *argv[]) -> int {

  std::println("{}[INFO]{} SVG Handler Test Center", color::blue, color::reset);

  if (argc < 2) {
    std::println("{}{}{}", color::blue, USAGE, color::reset);
    return EXIT_FAILURE;
  }

  std::string arg = argv[1];
  constexpr std::string_view prefix = "test=";
  if (!arg.starts_with(prefix)) {
    std::println("{}[ERROR]{} Invalid argument '{}'. Expected format: test=<n>",
                 color::red, color::reset, arg);
    return EXIT_FAILURE;
  }

  std::string value = arg.substr(prefix.size());

  if (value == "all" || value == "0")
    select_test('0');
  else if (value.size() == 1 && std::isdigit(value[0]))
    select_test(value[0]);
  else {
    std::println("{}[ERROR]{} Unknown test option '{}'.", color::red,
                 color::reset, value);
    return EXIT_FAILURE;
  }

  std::println();
  std::println("{}[INFO]{} All requested tests completed successfully.",
               color::green, color::reset);

  return EXIT_SUCCESS;
}
