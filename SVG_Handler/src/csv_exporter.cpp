#include "csv_exporter.hpp"

#include <cassert>
#include <print>

auto CsvExporter::check(const std::filesystem::path &path) -> ExpectedFile {
  std::error_code ec;

  // Check directory
  auto parent = path.parent_path();
  if (!parent.empty() && !std::filesystem::exists(parent, ec)) {
    std::filesystem::create_directories(parent, ec);
    if (ec) {
      return std::unexpected(ec);
    }
  }

  auto file =
      std::make_unique<std::ofstream>(path, std::ios::out | std::ios::trunc);
  if (file->is_open()) {
    return std::move(file);
  }

  return std::unexpected(std::make_error_code(std::errc::permission_denied));
}

void CsvExporter::save(std::string_view path, const CsvTable &table,
                       char delimiter) {

  if (path.empty()) {
    std::println("[ERROR] : Invalid CSV path (empty).");
    return;
  }

  std::filesystem::path fpath(path);

  auto file_expected = CsvExporter::check(fpath);
  if (!file_expected.has_value()) {
    std::error_code ec = file_expected.error();
    std::println("[ERROR] : Failed to open CSV file '{}' : {} ({})", path,
                 ec.message(), ec.value());
    return;
  }

  std::ofstream &file = *file_expected.value();

  // function to handle fields
  auto escape_csv_field = [](const std::string &field, char delimiter) {
    bool quotes = false;
    std::string result;
    result.reserve(field.size() + 2);

    for (char ch : field) {
      if (ch == '"' || ch == delimiter || ch == '\n' || ch == '\r') {
        quotes = true;
      }
      if (ch == '"') {
        result += "\"\"";
      } else {
        result.push_back(ch);
      }
    }

    if (quotes) {
      return "\"" + result + "\"";
    }

    return result;
  };

  // Write each line and field.
  for (const auto &row : table) {
    for (size_t i = 0; i < row.size(); ++i) {
      file << escape_csv_field(row[i], delimiter);
      if (i < row.size() - 1)
        file << delimiter;
    }
    file << '\n';
  }

  if (file.good()) {
    std::println("[INFO] : CSV file saved successfully at '{}'.", path);
  } else {
    std::println("[ERROR] : Writing error occurred for '{}'.", path);
  }
}

void test_csv_exporter() {

  // Simple test
  CsvExporter::CsvTable table{{"Line 1", "Value 1", "Description A,1"},
                              {"Line 2", "Value 2", "Description A,2"},
                              {"Line 3", "Value 3", "Description A,3"}};

  std::string output = "resources/test_output.csv";
  CsvExporter::save(output, table, ',');

  // Validates created file.
  std::error_code ec;
  assert(std::filesystem::exists(output, ec));
  assert(!ec);

  std::println("[TEST] {} : test completed", __PRETTY_FUNCTION__);
}

#ifdef BUILD_TEST_EXE
auto main() -> int {

  test_csv_exporter();

  return 0;
}
#endif
