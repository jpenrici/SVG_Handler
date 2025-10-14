/*
 * Test Center
 */

#include "svg_handler.hpp"

#include <print>

void select_test(char option) {

  if (option == '1' or option == '0')
    test_svg_reader();
  if (option == '2' or option == '0')
    test_string_utils();
  if (option == '3' or option == '0')
    test_tree_utils();
  if (option == '4' or option == '0')
    test_csv_exporter();
  if (option == '5' or option == '0')
    test_svg_handler();
}

void testAll() {
  std::println("Test All ...");
  select_test('0');
}

auto main(int argc, char *argv[]) -> int {

  // Handle input arguments
  // Validate input
  // Select test option

  // Test
  // testAll();

  // Test string_utils.cpp
  select_test('4');

  return 0;
}
