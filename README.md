---

# SVG Handler

A modular C++ project for parsing, validating, visualizing, and exporting **SVG (Scalable Vector Graphics)** structures into **CSV** tables.
Designed for clarity, modularity, and minimal dependencies.

---

## Overview

The project is composed of several independent modules:

| Module           | Description                                                                 |
| ---------------- | --------------------------------------------------------------------------- |
| **svg_reader**   | Loads SVG files from disk.                                                  |
| **string_utils** | Cleans and tokenizes SVG text into structured tags.                         |
| **tree_utils**   | Builds and manages an internal hierarchical tree representation of the SVG. |
| **csv_exporter** | Converts the tree into a CSV table for data analysis.                       |
| **svg_handler**  | Orchestrates all modules into a single processing pipeline.                 |
| **cli**          | Command-line interface for pipeline execution or visualization.             |

---

## Requirements

* **C++ Standard:** C++23 or newer
* **CMake:** Version 3.25 or higher
* **Compiler:** GCC 15

---

## Build Instructions

```bash
# Clone repository
git clone <repository_url>
cd SVG_Handler

# Configure and build
cmake -B build
cmake --build build

# Run CLI
./build/svg_handler_cli resources/sample.svg output/sample.csv
```

---

## CLI Usage

### Full pipeline

```bash
./svg_handler_cli <input.svg> <output.csv>
```

### Validate only

```bash
./svg_handler_cli --validate <input.svg>
```

### View SVG hierarchy

```bash
./svg_handler_cli --view <input.svg>
```

---

## Testing

A unified test runner is provided:

```bash
# Run all module tests
./build/svg_handler_cli_test test=0
./build/svg_handler_cli_test test=all

# Or run individual modules
./build/svg_handler_cli_test test=3   # TreeUtils only
```

---

## Project Structure

```
project/
├── include/
│   ├── svg_core.hpp
│   ├── svg_reader.hpp
│   ├── string_utils.hpp
│   ├── tree_utils.hpp
│   ├── csv_exporter.hpp
│   └── svg_handler.hpp
├── src/
│   ├── svg_reader.cpp
│   ├── string_utils.cpp
│   ├── tree_utils.cpp
│   ├── csv_exporter.cpp
│   └── svg_handler.cpp
├── cli/
│   └── main.cpp
├── resources/
│   └── sample.svg
└── tests/
    └── test.cpp
```

---

## Learn More About SVG

* [W3C SVG Specification](https://www.w3.org/TR/SVG2/)
* [MDN Web Docs: SVG](https://developer.mozilla.org/en-US/docs/Web/SVG)
* [SVG Tutorial - W3Schools](https://www.w3schools.com/graphics/svg_intro.asp)

---

## License

This project is distributed for educational and research purposes under the MIT License.

---