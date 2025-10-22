#pragma once

#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QString>
#include <QTreeWidget>

#include <memory>

#include "svg_handler.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

private:
  std::unique_ptr<Ui::MainWindow> ui_;
  std::unique_ptr<SVG_HANDLER::SVG> svg_handler_;

  QString currentSvgPath_;
  QString currentCsvPath_;

  void setupConnections();

  void openFile();
  void exportCSV();

  void loadSvgToTree(const TreeUtils::Tree &tree);
  void clearTreeView();
};
