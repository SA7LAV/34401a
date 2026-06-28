import sys
from PyQt5.QtWidgets import QApplication
from config import app_config
from translations import set_language
from ui.main_window import MainWindow


def main():
    app = QApplication(sys.argv)
    app.setStyle("Fusion")
    set_language(app_config.language)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
