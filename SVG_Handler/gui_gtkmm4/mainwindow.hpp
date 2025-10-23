#pragma once

#include <gtkmm.h>

#include <memory>

#include "svg_handler.hpp"

class MainWindow : public Gtk::ApplicationWindow {
public:
  MainWindow();
  ~MainWindow() override = default;

private:
  // Layout
  Gtk::Box mainBox_{Gtk::Orientation::VERTICAL};
  Gtk::HeaderBar headerBar_;
  Gtk::Button btnOpen_{"Open SVG"};
  Gtk::Button btnExport_{"Export CSV"};
  Gtk::ScrolledWindow scroll_;
  Gtk::TreeView treeView_;
  Gtk::Statusbar statusBar_;

  // Tree model
  class ModelColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    ModelColumns() {
      add(col_tag);
      add(col_attrs);
    }
    Gtk::TreeModelColumn<Glib::ustring> col_tag;
    Gtk::TreeModelColumn<Glib::ustring> col_attrs;
  };

  ModelColumns columns_;
  Glib::RefPtr<Gtk::TreeStore> treeModel_;

  // Backend
  std::unique_ptr<SVG_HANDLER::SVG> svgHandler_;

  // Methods
  void on_open_clicked();
  void on_export_clicked();
  void load_svg_to_tree(const TreeUtils::Tree &tree);
  void clear_tree();
};
