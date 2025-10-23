#include "mainwindow.hpp"

#include <print>

MainWindow::MainWindow() {
  std::string title{"SVG Handler (GTK GUI)"};
  set_title(title);
  set_default_size(800, 600);

  // Header bar
  Gtk::Label label(title);
  headerBar_.set_title_widget(label);
  headerBar_.pack_start(btnOpen_);
  headerBar_.pack_end(btnExport_);
  set_titlebar(headerBar_);

  // Layout
  mainBox_.append(scroll_);
  mainBox_.append(statusBar_);
  set_child(mainBox_);

  // Tree setup
  treeModel_ = Gtk::TreeStore::create(columns_);
  treeView_.set_model(treeModel_);
  treeView_.append_column("Tag", columns_.col_tag);
  treeView_.append_column("Attributes", columns_.col_attrs);

  scroll_.set_child(treeView_);
  scroll_.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
  scroll_.set_expand(true);

  // Connections
  btnOpen_.signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::on_open_clicked));
  btnExport_.signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::on_export_clicked));

  statusBar_.push("Ready");
}

void MainWindow::on_open_clicked() {
  auto dialog =
      new Gtk::FileChooserDialog("Open SVG", Gtk::FileChooser::Action::OPEN);
  dialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
  dialog->add_button("_Open", Gtk::ResponseType::OK);
  dialog->set_modal(true);

  auto filter_svg = Gtk::FileFilter::create();
  filter_svg->set_name("SVG files");
  filter_svg->add_pattern("*.svg");
  dialog->add_filter(filter_svg);

  dialog->signal_response().connect([this, dialog](int response_id) {
    if (response_id == Gtk::ResponseType::OK) {
      auto file = dialog->get_file();
      if (!file)
        return;

      auto path = file->get_path();
      svgHandler_ = std::make_unique<SVG_HANDLER::SVG>(path, "output.csv");

      try {
        auto content = svgHandler_->load();
        auto prepared = svgHandler_->prepare(content);
        auto tokens = svgHandler_->tokenize(prepared);
        auto tree = svgHandler_->build(tokens);

        load_svg_to_tree(tree);

        std::println("{}[INFO]{} : Loaded SVG successfully: {}", color::blue,
                     color::reset, path);
        auto filename = std::filesystem::path(path).filename();
        statusBar_.push("Loaded: " + std::string{filename});

      } catch (const std::exception &e) {
        statusBar_.push("Error: " + std::string(e.what()));
        std::println("{}[ERROR]{} : {}", color::red, color::reset,
                     std::string(e.what()));
      }
    }
    dialog->hide();
  });

  dialog->present();
}

void MainWindow::on_export_clicked() {
  auto dialog =
      new Gtk::FileChooserDialog("Export CSV", Gtk::FileChooser::Action::SAVE);
  dialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
  dialog->add_button("_Save", Gtk::ResponseType::OK);
  dialog->set_modal(true);

  dialog->signal_response().connect([this, dialog](int response_id) {
    if (response_id == Gtk::ResponseType::OK) {
      auto file = dialog->get_file();
      if (!file)
        return;

      auto path = file->get_path();

      try {
        auto table = svgHandler_->to_csv(svgHandler_->build(
            svgHandler_->tokenize(svgHandler_->prepare(svgHandler_->load()))));
        svgHandler_->export_csv(table, path);
        std::println("{}[INFO]{} : Exported SVG successfully: {}", color::blue,
                     color::reset, path);
        statusBar_.push("Exported: " + path);
      } catch (const std::exception &e) {
        statusBar_.push("Export failed: " + std::string(e.what()));
        std::println("{}[ERROR]{} : {}", color::red, color::reset,
                     std::string(e.what()));
      }
    }
    dialog->hide();
  });

  dialog->present();
}

void MainWindow::load_svg_to_tree(const TreeUtils::Tree &tree) {
  clear_tree();

  if (!tree.root) {
    statusBar_.push("Empty SVG tree.");
    return;
  }

  std::function<void(const TreeUtils::Node *, Gtk::TreeModel::Row &)>
      add_node; // populate tree node

  add_node = [&](const TreeUtils::Node *node, Gtk::TreeModel::Row &parentRow) {
    if (!node)
      return;

    Gtk::TreeModel::Row row;
    if (parentRow)
      row = *(treeModel_->append(parentRow.children()));
    else
      row = *(treeModel_->append());

    row[columns_.col_tag] = node->tag;

    std::string attr_text;
    for (const auto &[key, value] : node->attributes) {
      attr_text += key + "=\"" + value + "\" ";
    }
    row[columns_.col_attrs] = Glib::ustring(attr_text);

    for (const auto &child : node->children)
      add_node(child.get(), row);
  };

  Gtk::TreeModel::Row root_row;
  add_node(tree.root.get(), root_row);

  treeView_.expand_all();
  statusBar_.push("Tree loaded successfully.");
}

void MainWindow::clear_tree() {
  if (treeModel_)
    treeModel_->clear();
}
