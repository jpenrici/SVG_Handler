# -*- coding: utf-8 -*-
from pathlib import Path
from typing import List

from PySide6.QtCore import Qt
from PySide6.QtGui import QAction
from PySide6.QtWidgets import (
    QMainWindow,
    QFileDialog,
    QMessageBox,
    QToolBar,
    QTreeView,
    QTableView,
    QSplitter,
    QWidget,
    QVBoxLayout,
    QStatusBar,
    QAbstractItemView,
)
from PySide6.QtGui import QStandardItemModel, QStandardItem

# import the Python adapter (svg_handler.py) from sibling directory cli_py3
# make sure cli_py3 is on sys.path (main.py already inserts it), but fallback:
try:
    from svg_handler import SvgHandlerAdapter
except Exception as e:
    raise ImportError("Could not import svg_handler.SvgHandlerAdapter. "
                      "Ensure cli_py3/svg_handler.py exists and is on PYTHONPATH.") from e


class MainWindow(QMainWindow):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("SVG Handler (PySide6 GUI)")
        self.resize(900, 600)

        # Backend adapter
        self.adapter = SvgHandlerAdapter()

        # Current file paths
        self.current_svg = None
        self.current_csv = None

        # UI: toolbar
        toolbar = QToolBar("Main")
        self.addToolBar(toolbar)

        act_open = QAction("Open", self)
        act_open.triggered.connect(self.on_open)
        toolbar.addAction(act_open)

        act_view = QAction("View", self)
        act_view.triggered.connect(self.on_view)
        toolbar.addAction(act_view)

        act_export = QAction("Export", self)
        act_export.triggered.connect(self.on_export)
        toolbar.addAction(act_export)

        # Central widget: splitter with TreeView (left) and TableView (right)
        central = QWidget(self)
        layout = QVBoxLayout(central)
        layout.setContentsMargins(0, 0, 0, 0)

        splitter = QSplitter(Qt.Horizontal, central)

        # Tree view for hierarchy
        self.tree_view = QTreeView(splitter)
        self.tree_model = QStandardItemModel()
        self.tree_model.setHorizontalHeaderLabels(["Tag / Node"])
        self.tree_view.setModel(self.tree_model)
        self.tree_view.setEditTriggers(QAbstractItemView.NoEditTriggers)
        self.tree_view.setHeaderHidden(False)

        # Table view for CSV-like table
        self.table_view = QTableView(splitter)
        self.table_model = QStandardItemModel()
        self.table_view.setModel(self.table_model)
        self.table_view.setEditTriggers(QAbstractItemView.NoEditTriggers)

        splitter.addWidget(self.tree_view)
        splitter.addWidget(self.table_view)
        splitter.setStretchFactor(0, 1)
        splitter.setStretchFactor(1, 2)

        layout.addWidget(splitter)
        self.setCentralWidget(central)

        # Status bar
        self.status = QStatusBar(self)
        self.setStatusBar(self.status)
        self.status.showMessage("Ready")

    # -------------
    # UI callbacks
    # -------------
    def on_open(self):
        path, _ = QFileDialog.getOpenFileName(self, "Open SVG", str(Path.cwd()), "SVG files (*.svg);;All Files (*)")
        if not path:
            return
        self.current_svg = path
        self.status.showMessage(f"Selected: {path}")
        self.setWindowTitle(f"SVG Handler — {Path(path).name}")

    def on_view(self):
        if not self.current_svg:
            QMessageBox.information(self, "No file", "Please open an SVG file first.")
            return
        try:
            table = self.adapter.extract_table(self.current_svg)
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to extract table:\n{e}")
            return

        if not table:
            QMessageBox.information(self, "Empty", "No data returned from adapter.")
            return

        # Populate table view
        self._populate_table_model(table)

        # Populate tree view (hierarchy)
        self._populate_tree_model_from_table(table)

        self.status.showMessage(f"Loaded structure from {Path(self.current_svg).name}")

    def on_export(self):
        if not self.current_svg:
            QMessageBox.information(self, "No file", "Please open an SVG file first.")
            return

        path, _ = QFileDialog.getSaveFileName(self, "Export CSV", str(Path.cwd() / "output.csv"), "CSV files (*.csv);;All Files (*)")
        if not path:
            return

        try:
            self.adapter.process_svg(self.current_svg, path)
            self.status.showMessage(f"Exported CSV → {path}")
            QMessageBox.information(self, "Exported", f"CSV exported to:\n{path}")
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to export CSV:\n{e}")

    # -------------------------
    # Model population helpers
    # -------------------------
    def _populate_table_model(self, table: List[List[str]]):
        # table[0] is header
        headers = table[0]
        self.table_model.clear()
        self.table_model.setHorizontalHeaderLabels(headers)

        for row in table[1:]:
            items = [QStandardItem(col) for col in row]
            self.table_model.appendRow(items)

        # resize columns to contents
        self.table_view.resizeColumnsToContents()

    def _populate_tree_model_from_table(self, table: List[List[str]]):
        """
        Build a hierarchical QStandardItemModel from the CSV-like table.
        Expected table columns: ID, ParentID, Depth, Tag, Attribute, Value
        We will create a node for each unique ID and attach as child to ParentID.
        If a node has attributes, they will be added as children (indented) or appended to the node label.
        """
        headers = table[0]
        # sanity check basic shape
        if len(headers) < 6:
            QMessageBox.warning(self, "Unexpected table", "Table doesn't contain expected columns.")
            return

        # Clear tree
        self.tree_model.clear()
        self.tree_model.setHorizontalHeaderLabels(["Tag / Node"])

        # Build nodes dict: id -> QStandardItem
        nodes = {}
        parents = {}
        # rows may contain multiple lines for same ID (one per attribute)
        for row in table[1:]:
            node_id, parent_id, *_rest, tag, attr, val = row
            # create node item if not exists
            if node_id not in nodes:
                node_item = QStandardItem(tag)
                node_item.setEditable(False)
                nodes[node_id] = node_item
                parents[node_id] = parent_id
            # attach attribute as child under node (optional)
            if attr:
                attr_item = QStandardItem(f'{attr} = "{val}"')
                attr_item.setEditable(False)
                nodes[node_id].appendRow([attr_item])

        # Now attach nodes according to parent relationships
        # First find roots (parent == -1)
        root_items = []
        for nid, parent_id in parents.items():
            if parent_id == "-1" or parent_id not in nodes:
                root_items.append(nid)

        # Attach recursively
        def attach_children(parent_item, parent_id):
            for nid, pid in parents.items():
                if pid == parent_id:
                    child_item = nodes[nid]
                    parent_item.appendRow([child_item])
                    attach_children(child_item, nid)

        # Add roots to model
        for rid in root_items:
            root_item = nodes[rid]
            self.tree_model.appendRow([root_item])
            attach_children(root_item, rid)

        self.tree_view.expandAll()

