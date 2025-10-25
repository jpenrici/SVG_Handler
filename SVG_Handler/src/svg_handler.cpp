#include "svg_handler.hpp"

#include <cassert>
#include <cstring>
#include <print>

using namespace SVG_HANDLER;

SVG_HANDLER::SVG::SVG(std::string_view file_path_svg,
                      std::string_view file_path_csv)
    : file_path_svg_(std::move(file_path_svg)),
      file_path_csv_(std::move(file_path_csv)) {}

auto SVG_HANDLER::SVG::load() -> std::string {
  return SvgReader::load(file_path_svg_);
}

auto SVG_HANDLER::SVG::prepare(std::string_view svg)
    -> std::vector<std::string> {
  return StringUtils::prepare(svg);
}

auto SVG_HANDLER::SVG::tokenize(const std::vector<std::string> &tags)
    -> std::vector<StringUtils::TagTuple> {
  std::vector<StringUtils::TagTuple> tokens;
  tokens.reserve(tags.size());
  for (const auto &tag : tags)
    tokens.push_back(StringUtils::process(tag));
  return tokens;
}

auto SVG_HANDLER::SVG::build(
    const std::vector<StringUtils::TagTuple> &svg_tagTuple) -> TreeUtils::Tree {
  assert(TreeUtils::validate(svg_tagTuple) == TreeUtils::Status::Success);
  return TreeUtils::process(svg_tagTuple);
}

auto SVG_HANDLER::SVG::to_csv(const TreeUtils::Tree &tree)
    -> TreeUtils::CsvTable {
  return TreeUtils::table(tree);
}

void SVG_HANDLER::SVG::export_csv(const TreeUtils::CsvTable &csvTable,
                                  std::string_view path) {
  CsvExporter::save(path, csvTable);
}

void SVG_HANDLER::SVG::execute() {
  auto content = load();
  auto prepared = prepare(content);
  auto tokens = tokenize(prepared);
  auto tree = build(tokens);
  auto table = to_csv(tree);
  export_csv(table, file_path_csv_);
  std::println("{}[PIPELINE]{} : SVG processing completed successfully.",
               color::blue, color::reset);
}

void test_svg_handler() {

  std::println("{}[TEST]{} : Starting SVG handler test ...", color::blue,
               color::reset);

  // Integrated pipeline test
  SVG_HANDLER::SVG handler("resources/sample.svg", "sample.csv");
  handler.execute();

  std::println("{}[TEST]{} : {} : test completed", color::green, color::reset,
               __PRETTY_FUNCTION__);
}

// C INTEROPERABILITY IMPLEMENTATION
SvgHandlerPtr svg_handler_create(const char *input_svg,
                                 const char *output_csv) {
  if (!input_svg || !output_csv)
    return nullptr;
  return new SVG(input_svg, output_csv);
}

void svg_handler_execute(SvgHandlerPtr handler) {
  if (!handler)
    return;
  static_cast<SVG *>(handler)->execute();
}

void svg_handler_destroy(SvgHandlerPtr handler) {
  if (handler)
    delete static_cast<SVG *>(handler);
}

char ***svg_handler_to_csv(SvgHandlerPtr handler, int *rows, int *cols) {
  if (!handler || !rows || !cols)
    return nullptr;

  auto *h = static_cast<SVG *>(handler);
  auto table = h->to_csv(h->build(h->tokenize(h->prepare(h->load()))));

  *rows = static_cast<int>(table.size());
  *cols = table.empty() ? 0 : static_cast<int>(table[0].size());

  // Allocate 2D C array
  char ***data = new char **[*rows];
  for (int i = 0; i < *rows; ++i) {
    data[i] = new char *[*cols];
    for (int j = 0; j < *cols; ++j) {
      const auto &cell = table[i][j];
      data[i][j] = new char[cell.size() + 1];
      std::strcpy(data[i][j], cell.c_str());
    }
  }
  return data;
}

void svg_handler_free_csv(char ***data, int rows, int cols) {
  if (!data)
    return;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j)
      delete[] data[i][j];
    delete[] data[i];
  }
  delete[] data;
}

// End of implementations for extern "C"
