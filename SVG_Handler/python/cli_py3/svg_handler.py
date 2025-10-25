# -*- coding: utf-8 -*-
"""
svg_handler.py
--------------
Python adapter for the C++ SVG Handler core library.

Provides both a command-line interface and a reusable Python API
for GUIs or integrations (e.g. PySide6, Flask, etc.).
"""

import os
import sys
import argparse

from ctypes import cdll, c_void_p, c_char_p, POINTER, c_int


# --------------------------------
# Utility: Colored text for CLI
# --------------------------------
class Color:
    RESET = "\033[0m"
    BLUE = "\033[34m"
    GREEN = "\033[32m"
    YELLOW = "\033[33m"
    RED = "\033[31m"


# --------------
# Core adapter
# --------------
class SvgHandlerAdapter:
    def __init__(self, lib_path: str = "../../lib/libsvg_handler_py.so"):
        if not os.path.exists(lib_path):
            raise FileNotFoundError(f"Shared library not found: {lib_path}")

        self.lib = cdll.LoadLibrary(lib_path)

        # Function signatures
        self.lib.svg_handler_create.restype = c_void_p
        self.lib.svg_handler_create.argtypes = [c_char_p, c_char_p]

        self.lib.svg_handler_execute.argtypes = [c_void_p]
        self.lib.svg_handler_destroy.argtypes = [c_void_p]

        # Table export API
        self.lib.svg_handler_to_csv.restype = POINTER(POINTER(c_char_p))
        self.lib.svg_handler_to_csv.argtypes = [c_void_p,
                                                POINTER(c_int),
                                                POINTER(c_int)]
        self.lib.svg_handler_free_csv.argtypes = [POINTER(POINTER(c_char_p)),
                                                  c_int, c_int]

    def create(self, svg_path: str, csv_path: str = "output.csv") -> c_void_p:
        return self.lib.svg_handler_create(svg_path.encode("utf-8"),
                                           csv_path.encode("utf-8"))

    def execute(self, handler: c_void_p):
        self.lib.svg_handler_execute(handler)

    def destroy(self, handler: c_void_p):
        if handler:
            self.lib.svg_handler_destroy(handler)

    def to_csv_table(self, handler: c_void_p):
        rows, cols = c_int(), c_int()
        data = self.lib.svg_handler_to_csv(handler, rows, cols)
        table = [
            [data[i][j].decode("utf-8") for j in range(cols.value)]
            for i in range(rows.value)
        ]
        self.lib.svg_handler_free_csv(data, rows, cols)
        return table

    def process_svg(self, svg_path: str, csv_path: str = "output.csv"):
        handler = self.create(svg_path, csv_path)
        self.execute(handler)
        self.destroy(handler)
        print(f"{Color.GREEN}[INFO]{Color.RESET} Processed → {csv_path}")

    def extract_table(self, svg_path: str):
        handler = self.create(svg_path)
        table = self.to_csv_table(handler)
        self.destroy(handler)
        return table


# --------------------------------------
# Utility: Table viewer (pretty-print)
# --------------------------------------
def view_table(table: list[list[str]]):
    if not table:
        print(f"{Color.YELLOW}[WARN]{Color.RESET} Empty table.")
        return

    # Determine column widths
    col_widths = [max(len(row[i]) for row in table) for i in range(len(table[0]))]

    def fmt_row(row):
        return "  ".join(f"{col:<{col_widths[i]}}" for i, col in enumerate(row))

    print(f"{Color.BLUE}[INFO]{Color.RESET} Table view:\n")

    # Header
    header = fmt_row(table[0])
    print(f"{Color.GREEN}{header}{Color.RESET}")
    print("-" * len(header))

    # Rows
    for row in table[1:]:
        print(fmt_row(row))


# -----------------------------------------
# Utility: Hierarchical view (tree-style)
# -----------------------------------------
def view_hierarchy(table: list[list[str]]):
    if not table:
        print(f"{Color.YELLOW}[WARN]{Color.RESET} Empty table.")
        return

    print(f"{Color.BLUE}[INFO]{Color.RESET} SVG Hierarchy:\n")

    # nodes: id -> { tag, parent, attrs: [(name,value), ...] }
    nodes: dict[str, dict] = {}
    for row in table[1:]:  # skip header
        node_id, parent_id, *_rest, tag, attr, value = row
        # Ensure node exists
        if node_id not in nodes:
            nodes[node_id] = {"tag": tag, "parent": parent_id, "attrs": []}
        # Accumulate attribute if present (some rows have empty attr/value)
        if attr:
            nodes[node_id]["attrs"].append((attr, value))

    # Build children mapping parent_id -> [child_id, ...]
    children: dict[str, list[str]] = {}
    for nid, info in nodes.items():
        parent = info["parent"]
        children.setdefault(parent, []).append(nid)

    # Find roots (parent == "-1" or parent not in nodes)
    roots = children.get("-1", [])
    # also include nodes whose parent isn't listed (robustness)
    for nid, info in nodes.items():
        if info["parent"] not in nodes and info["parent"] != "-1":
            # attach as root if parent missing
            roots.append(nid)

    # Recursive printer
    def show(node_id: str, depth: int = 0):
        node = nodes.get(node_id)
        if not node:
            return
        indent = "  " * depth
        print(f"{indent}{Color.GREEN}{node['tag']}{Color.RESET}")
        # print attributes (each on its own indented line)
        for name, val in node["attrs"]:
            print(f"{indent}  {name}=\"{val}\"")
        # children
        for child_id in children.get(node_id, []):
            show(child_id, depth + 1)

    # Print all roots
    for root_id in roots:
        show(root_id)


# -----------------
# CLI entrypoint
# -----------------
def cli():
    parser = argparse.ArgumentParser(
        prog="svg_handler.py",
        description="Python interface for the C++ SVG Handler library.",
        formatter_class=argparse.RawTextHelpFormatter,
        epilog=(
            "Examples:\n"
            "  python3 svg_handler.py --example\n"
            "  python3 svg_handler.py input.svg output.csv\n"
            "  python3 svg_handler.py input.svg --table\n"
            "  python3 svg_handler.py input.svg --view\n"
        )
    )

    parser.add_argument("svg", nargs="?", help="Input SVG file path.")
    parser.add_argument("csv", nargs="?", default="output.csv",
                        help="Output CSV file path.")
    parser.add_argument("--table", action="store_true",
                        help="Displays the SVG structure as a formatted table.")
    parser.add_argument("--view", action="store_true",
                        help="Displays the SVG hierarchy in tree format.")
    parser.add_argument("--example", action="store_true",
                        help="Runs the built-in example (sample.svg).")

    args = parser.parse_args()
    adapter = SvgHandlerAdapter()

    if args.example:
        example()
        return

    if not args.svg:
        parser.print_help(sys.stderr)
        return

    if args.table or args.view:
        table = adapter.extract_table(args.svg)
        if args.table:
            view_table(table)
        if args.view:
            print()
            view_hierarchy(table)
    else:
        adapter.process_svg(args.svg, args.csv)


# ----------------
# Example usage
# ----------------
def example():
    adapter = SvgHandlerAdapter()
    svg_path = "../../resources/sample.svg"
    csv_path = "../output/sample_py.csv"

    print(f"{Color.BLUE}[INFO]{Color.RESET} Running example using {svg_path}")
    adapter.process_svg(svg_path, csv_path)

    # Table
    table = adapter.extract_table(svg_path)

    print()
    view_table(table)

    print()
    view_hierarchy(table)


# -------------
# Entry point
# -------------
if __name__ == "__main__":
    if len(sys.argv) == 1:
        print(f"{Color.BLUE}[INFO]{Color.RESET} SVG Handler Python adapter\n")
        print("Usage examples:")
        print("  python3 svg_handler.py --example")
        print("  python3 svg_handler.py <input.svg> [output.csv]")
        print("  python3 svg_handler.py <input.svg> --table")
        print("  python3 svg_handler.py <input.svg> --view")
        print("\nRun with -h or --help for details.\n")
    else:
        cli()
