#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QFontDatabase>
#include <QStyleFactory>

#include <print>

auto main(int argc, char *argv[]) -> int {
  QApplication app(argc, argv);
  QApplication::setStyle(QStyleFactory::create("Fusion"));

  QPalette dark;
  dark.setColor(QPalette::Window, QColor(45, 45, 45));
  dark.setColor(QPalette::WindowText, Qt::white);
  dark.setColor(QPalette::Base, QColor(30, 30, 30));
  dark.setColor(QPalette::Text, Qt::white);
  app.setPalette(dark);

  MainWindow w;
  w.show();

  std::println("{}[INFO]{} : Qt GUI started successfully.", color::blue,
               color::reset);
  return app.exec();
}
