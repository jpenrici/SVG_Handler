#include "svg_handler.hpp"

#include <cassert>
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

// C Interoperability Layer
extern "C" {

void *svg_handler_create(const char *input_svg, const char *output_csv) {
  try {
    return new SVG_HANDLER::SVG(input_svg, output_csv);
  } catch (...) {
    return nullptr;
  }
}

void svg_handler_execute(void *handler) {
  if (!handler)
    return;
  auto *svg = static_cast<SVG_HANDLER::SVG *>(handler);
  svg->execute();
}

void svg_handler_destroy(void *handler) {
  delete static_cast<SVG_HANDLER::SVG *>(handler);
}

} // extern "C"
