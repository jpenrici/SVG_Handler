/*
 * Reference:
 *    https://www.gtkmm.org
 *
 * Requeriment
 *    libgtkmm-4.0-dev
 */
#include "mainwindow.hpp"

#include <gtkmm.h>

int main(int argc, char *argv[]) {
  auto app = Gtk::Application::create("org.example.svg_handler");
  return app->make_window_and_run<MainWindow>(argc, argv);
}
