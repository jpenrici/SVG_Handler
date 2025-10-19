#include "svg_reader.hpp"

#include <cassert>
#include <expected>
#include <filesystem>
#include <fstream>
#include <print>

auto SvgReader::check(const std::filesystem::path &path) -> ExpectedFile {
  std::error_code ec;

  if (!std::filesystem::is_regular_file(path, ec) || ec) {
    return std::unexpected(
        ec ? ec : std::make_error_code(std::errc::no_such_file_or_directory));
  }

  auto file = std::make_unique<std::ifstream>(path);
  if (file->is_open()) {
    return std::move(file);
  } else {
    return std::unexpected(std::make_error_code(std::errc::permission_denied));
  }
}

auto SvgReader::load(std::string_view path) -> std::string {

  if (path.empty()) {
    std::println("{}[ERROR]{} : Invalid path! Path cannot be empty.",
                 color::red, color::reset);
    return {/* empty */};
  }

  std::filesystem::path fpath(path);

  std::string ext = fpath.extension().string();
  std::transform(ext.begin(), ext.end(), ext.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  if (ext != ".svg") {
    std::println(
        "{}[ERROR]{} : Invalid file extension! Expected .svg, got '{}'.",
        color::red, color::reset, ext);
    return {/* empty */};
  }

  auto file_expected = SvgReader::check(fpath);
  if (file_expected.has_value()) {
    std::ifstream &file = *file_expected.value();
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();

    if (content.empty()) {
      std::println("{}[WARNING]{} : File is empty: '{}'.", color::yellow,
                   color::reset, path);
    }

    return content;
  }

  std::error_code ec = file_expected.error();
  std::println(
      "{}[ERROR]{} : Failed to open or read file '{}' . Error: {} ({}).",
      color::red, color::reset, path, ec.message(), ec.value());

  return {/* empty */};
}

void test_svg_reader() {

  std::string svg = SvgReader::load("resources/sample.svg");
  assert(!svg.empty());

  if (svg.empty()) {
    std::println(
        "{}[INFO]{} : Empty or invalid file! Check SVG file path and name.",
        color::blue, color::reset);
  }

  std::println("{}[TEST]{} : {} : test completed", color::green, color::reset,
               __PRETTY_FUNCTION__);
}

#ifdef BUILD_TEST_EXE
auto main() -> int {

  test_svg_reader();

  return 0;
}
#endif
