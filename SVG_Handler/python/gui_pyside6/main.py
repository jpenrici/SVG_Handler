# -*- coding: utf-8 -*-

import sys

from PySide6.QtWidgets import QApplication
from mainwindow import MainWindow

def main(argv):
    app = QApplication(argv)
    win = MainWindow()
    win.show()
    return app.exec()

if __name__ == "__main__":
    sys.exit(main(sys.argv))
