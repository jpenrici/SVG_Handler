---

# SVG Handler

A modular **C++23+** project for parsing, validating, visualizing, and exporting **SVG (Scalable Vector Graphics)** structures into **CSV** tables.

Includes **CLI**, **Qt6**, **GTKmm4**, and **PySide6** graphical frontends — plus **Python interoperability** via shared library bindings.

Designed for clarity, modularity, and minimal dependencies.

---

## Overview

| Module / Component  | Description                                                                 |
| ------------------- | --------------------------------------------------------------------------- |
| **svg_reader**      | Loads SVG files from disk.                                                  |
| **string_utils**    | Cleans and tokenizes SVG text into structured tags.                         |
| **tree_utils**      | Builds and manages a hierarchical tree representation of the SVG.           |
| **csv_exporter**    | Converts the tree into a CSV table for data analysis.                       |
| **svg_handler**     | Orchestrates all modules into a single processing pipeline.                 |
| **cli**             | Command-line interface for running the full pipeline or partial validation. |
| **gui_qt6**         | Qt6 desktop interface for SVG visualization and CSV export.                 |
| **gui_gtkmm4**      | GTKmm4 GUI with hierarchical view and export support.                       |
| **gui_pyside6**     | Python-based GUI using PySide6 (Qt for Python).                             |
| **python bindings** | Shared C++ library for use via `ctypes` or future `pybind11`.               |

---

## Requirements

* **C++ Standard:** C++23 or newer
* **CMake:** Version 3.25 or higher
* **Compiler:** GCC 15 or Clang 18+
* **Optional GUI Frameworks:**

  * [Qt 6 Widgets](https://doc.qt.io/qt-6/qtwidgets-index.html)
  * [GTKmm 4](https://gnome.pages.gitlab.gnome.org/gtkmm-documentation/)
  * [PySide6 (Qt for Python)](https://doc.qt.io/qtforpython/)

### Install dependencies (Ubuntu/Debian)

```bash
sudo apt install qt6-base-dev libgtkmm-4.0-dev python3-pyside6
```

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
./build/cli/svg_handler_cli resources/sample.svg output/sample.csv
```

### Validate only

```bash
./build/cli/svg_handler_cli --validate resources/sample.svg
```

### View SVG hierarchy

```bash
./build/cli/svg_handler_cli --view resources/sample.svg
```

---

## GUI Interfaces

### GTKmm4 GUI

```bash
cmake --build build --target run_gtkmm
```

or run manually:

```bash
./build/gui_gtk/svg_handler_gtk
```

### Qt6 GUI

```bash
cmake --build build --target run_qtgui
```

or run manually:

```bash
./build/gui_qt/svg_handler_qt
```

### PySide6 GUI

Automatically packaged with shared library and resources:

```bash
cmake --build build --target run_pyside6
```

This creates:

```
gui_pyside6/
 ├── cli_py3/svg_handler.py
 ├── gui_pyside6/main.py
 ├── gui_pyside6/mainwindow.py
 ├── lib/libsvg_handler_py.so
 └── resources/sample.svg
```

---

## Python Interoperability

The shared library **`libsvg_handler_py.so`** provides direct access to C++ functions through Python’s `ctypes`.

You can use the adapter script:

```bash
python3 python/cli_py3/svg_handler.py --example
```

Or integrate it into your own Python GUI or web application.

---

## Testing

Run the unified test suite:

```bash
./build/tests/svg_handler_cli_test test=0
```

Or run individual module tests:

```bash
./build/tests/test_run_tree_utils
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
│   ├── mainwindow.cpp
│   ├── mainwindow.h
│   └── mainwindow.ui
├── gui_gtkmm4/
│   ├── main.cpp
│   ├── mainwindow.cpp
│   └── mainwindow.hpp
├── python/
│   ├── cli_py3/
│   │   └── svg_handler.py
│   └── gui_pyside6/
│       ├── main.py
│       └── mainwindow.py
├── lib/
│   ├── libsvg_handler.so
│   └── libsvg_handler_py.so
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

This project is distributed for educational and research purposes under the **MIT License**.

---

## Display

![display](https://github.com/jpenrici/SVG_Handler/blob/main/display/display.png)

