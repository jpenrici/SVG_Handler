#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTreeWidgetItem>
#include <print>

using namespace SVG_HANDLER;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui_(std::make_unique<Ui::MainWindow>()) {
  ui_->setupUi(this);
  setWindowTitle("SVG Handler (Qt6 GUI)");

  ui_->treeWidget->header()->setSectionResizeMode(
      QHeaderView::ResizeToContents);
}

MainWindow::~MainWindow() = default;

void MainWindow::on_actionOpen_triggered() {
  QString filePath = QFileDialog::getOpenFileName(this, "Open SVG File", ".",
                                                  "SVG Files (*.svg)");
  if (filePath.isEmpty())
    return;

  currentSvgPath_ = filePath;
  currentCsvPath_.clear();

  try {
    svg_handler_ =
        std::make_unique<SVG>(filePath.toStdString(), "temp_output.csv");

    auto svgContent = svg_handler_->load();
    auto prepared = svg_handler_->prepare(svgContent);
    auto tokens = svg_handler_->tokenize(prepared);
    auto tree = svg_handler_->build(tokens);

    clearTreeView();
    loadSvgToTree(tree);

    std::println("{}[INFO]{} : Loaded SVG successfully: {}", color::blue,
                 color::reset, filePath.toStdString());
  } catch (const std::exception &e) {
    QMessageBox::critical(this, "Error",
                          QString("Failed to load SVG:\n%1").arg(e.what()));
  }
}

void MainWindow::on_actionExportCSV_triggered() {
  if (!svg_handler_) {
    QMessageBox::warning(this, "No SVG Loaded",
                         "Please open an SVG file first.");
    return;
  }

  QString csvPath = QFileDialog::getSaveFileName(this, "Export CSV", ".",
                                                 "CSV Files (*.csv)");
  if (csvPath.isEmpty())
    return;

  try {
    auto svgContent = svg_handler_->load();
    auto prepared = svg_handler_->prepare(svgContent);
    auto tokens = svg_handler_->tokenize(prepared);
    auto tree = svg_handler_->build(tokens);
    auto table = svg_handler_->to_csv(tree);
    svg_handler_->export_csv(table, csvPath.toStdString());

    QMessageBox::information(this, "Success", "CSV exported successfully!");
    currentCsvPath_ = csvPath;
  } catch (const std::exception &e) {
    QMessageBox::critical(this, "Error",
                          QString("Export failed:\n%1").arg(e.what()));
  }
}

void MainWindow::on_actionViewTree_triggered() {
  if (!svg_handler_) {
    QMessageBox::warning(this, "No SVG Loaded",
                         "Please open an SVG file first.");
    return;
  }

  std::println("[DEBUG] ViewTree triggered.");

  on_actionOpen_triggered();
}

void MainWindow::loadSvgToTree(const TreeUtils::Tree &tree) {
  if (!tree.root)
    return;

  std::function<QTreeWidgetItem *(const TreeUtils::Node *, QTreeWidgetItem *)>
      addNode;

  addNode = [&](const TreeUtils::Node *node,
                QTreeWidgetItem *parent) -> QTreeWidgetItem * {
    auto *item = new QTreeWidgetItem();
    item->setText(0, QString::fromStdString(node->tag));

    QString attrText;
    for (const auto &[name, value] : node->attributes)
      attrText +=
          QString("%1=\"%2\" ")
              .arg(QString::fromStdString(name), QString::fromStdString(value));
    item->setText(1, attrText.trimmed());

    if (parent)
      parent->addChild(item);
    else
      ui_->treeWidget->addTopLevelItem(item);

    for (const auto &child : node->children)
      addNode(child.get(), item);

    return item;
  };

  addNode(tree.root.get(), nullptr);
  ui_->treeWidget->expandAll();
}

void MainWindow::clearTreeView() { ui_->treeWidget->clear(); }
