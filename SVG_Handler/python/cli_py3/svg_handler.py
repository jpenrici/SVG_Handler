# -*- coding: utf-8 -*-
"""
Python interface for the SVG Handler C++ library (via ctypes).

Usage:
    python3 svg_handler_py.py ../../resources/sample.svg ../output/sample_py.csv
"""

import sys
from ctypes import cdll, c_void_p, c_char_p
from pathlib import Path
from contextlib import contextmanager


def get_library_path() -> str:
    """Locate the shared library (.so) relative to this script."""
    lib_path = (Path(__file__).resolve().parent / "../../lib/libsvg_handler_py.so").resolve()
    if not lib_path.exists():
        print(f"[ERROR] Library not found: {lib_path}")
        sys.exit(1)
    return str(lib_path)


def load_library():
    """Load the shared C++ library and define function signatures."""
    try:
        lib = cdll.LoadLibrary(get_library_path())
    except OSError as e:
        print(f"[ERROR] Could not load library: {e}")
        sys.exit(1)

    # Define ctypes interface
    lib.svg_handler_create.restype = c_void_p
    lib.svg_handler_create.argtypes = [c_char_p, c_char_p]
    lib.svg_handler_execute.argtypes = [c_void_p]
    lib.svg_handler_destroy.argtypes = [c_void_p]
    return lib


@contextmanager
def svg_handler(lib, path_svg, path_csv):
    """Context manager to safely create and destroy the handler."""
    handler = lib.svg_handler_create(path_svg.encode("utf-8"), path_csv.encode("utf-8"))
    try:
        yield handler
    finally:
        lib.svg_handler_destroy(handler)


def execute(path_svg: str, path_csv: str = "output.csv") -> None:
    """Execute the SVG Handler pipeline (SVG → CSV)."""
    lib = load_library()
    try:
        with svg_handler(lib, path_svg, path_csv) as h:
            lib.svg_handler_execute(h)
        print(f"[INFO] SVG processed successfully → {path_csv}")
    except Exception as e:
        print(f"[ERROR] {e}")
        sys.exit(1)


def example():
    """Example execution for quick testing."""
    svg_path = "../../resources/sample.svg"
    csv_path = "../output/sample_py.csv"
    print("[INFO] Running example() ...")
    execute(svg_path, csv_path)
    print("[INFO] Example completed.")


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Python binding for SVG Handler C++ library")
    parser.add_argument("svg", nargs="?", help="Input SVG file path")
    parser.add_argument("csv", nargs="?", default="output.csv", help="Output CSV path (default: output.csv)")
    parser.add_argument("--example", action="store_true", help="Run built-in example with sample.svg")
    args = parser.parse_args()

    if args.example:
        example()
    elif args.svg:
        execute(args.svg, args.csv)
    else:
        print("Usage:")
        print("  python3 svg_handler_py.py <input.svg> [output.csv]")
        print("  python3 svg_handler_py.py --example")
        sys.exit(1)
