# -*- coding: utf-8 -*-

import sys
from pathlib import Path

root = Path(__file__).resolve().parents[1]
cli_py3 = root / "cli_py3"
if str(cli_py3) not in sys.path:
    sys.path.insert(0, str(cli_py3))

from PySide6.QtWidgets import QApplication
from mainwindow import MainWindow

def main(argv):
    app = QApplication(argv)
    win = MainWindow()
    win.show()
    return app.exec()

if __name__ == "__main__":
    sys.exit(main(sys.argv))
