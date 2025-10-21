---

# SVG Handler

A modular C++ project for parsing, validating, visualizing, and exporting **SVG (Scalable Vector Graphics)** structures into **CSV** tables.
Includes both **CLI** and **Qt6 GUI** frontends.

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
| **gui_qt6**      | Qt6 desktop interface for visual exploration and CSV export.                |

---

## Requirements

* **C++ Standard:** C++23 or newer
* **CMake:** Version 3.25 or higher
* **Compiler:** GCC 15
* **Qt:** Qt6 Widgets module (required only for GUI build)

---

## Build Instructions

```bash
# Clone repository
git clone <repository_url>
cd SVG_Handler

# Configure and build
cmake -B build
cmake --build build
```

---

## Command-Line Interface (CLI)

### Full pipeline

```bash
./build/svg_handler_cli resources/sample.svg output/sample.csv
```

### Validate only

```bash
./build/svg_handler_cli --validate resources/sample.svg
```

### View SVG hierarchy

```bash
./build/svg_handler_cli --view resources/sample.svg
```

---

## Graphical Interface (Qt6 GUI)

### Run GUI

```bash
cmake --build build --target run_qtgui
```

or execute directly:

```bash
./build/svg_handler_qt
```

### Features

* Open and view SVG structure hierarchically
* Inspect attributes of each tag
* Export parsed data to CSV
* Clean, dark-themed Qt6 interface

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
├── gui_qt6/
│   ├── main.cpp
│   ├── mainwindow.h
│   ├── mainwindow.cpp
│   └── mainwindow.ui
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
* [QT - Documentation](https://doc.qt.io/)

---

## License

This project is distributed for educational and research purposes under the MIT License.

---