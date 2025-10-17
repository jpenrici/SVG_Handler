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

  // Converts vector<StringUtils::TagTuple> -> vector<TreeUtils::TagTuple>
  std::vector<TreeUtils::TagTuple> tuples;
  tuples.reserve(svg_tagTuple.size());

  for (const auto &t : svg_tagTuple) {
    std::string tag;
    StringUtils::Attributes s_attrs;
    StringUtils::TagType s_type;

    std::tie(tag, s_attrs, s_type) = t;
    TreeUtils::Attributes t_attrs = s_attrs;
    TreeUtils::TagType t_type = static_cast<TreeUtils::TagType>(s_type);

    tuples.emplace_back(std::move(tag), std::move(t_attrs), t_type);
  }

  // Check and process
  assert(TreeUtils::validate(tuples) == TreeUtils::Status::Success);
  return TreeUtils::process(tuples);
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
  std::println("[PIPELINE] SVG processing completed successfully.");
}

void test_svg_handler() {

  std::println("[TEST] Starting SVG handler test ...");

  // Integrated pipeline test
  SVG_HANDLER::SVG handler("resources/sample.svg", "sample.csv");
  handler.execute();

  std::println("[TEST] {} : test completed", __PRETTY_FUNCTION__);
}
