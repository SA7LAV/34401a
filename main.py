import sys
from PyQt5.QtWidgets import QApplication
from PyQt5.QtCore import qInstallMessageHandler, QtMsgType
from config import app_config
from translations import set_language
from ui.main_window import MainWindow

_SUPPRESSED = (
    "Invalid description",
    "Wayland does not support QWindow::requestActivate",
)

def _msg_handler(msg_type, context, message):
    if any(s in message for s in _SUPPRESSED):
        return
    if msg_type in (QtMsgType.QtWarningMsg, QtMsgType.QtCriticalMsg, QtMsgType.QtFatalMsg):
        print(message, file=sys.stderr)


def main():
    qInstallMessageHandler(_msg_handler)
    app = QApplication(sys.argv)
    app.setStyle("Fusion")
    set_language(app_config.language)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
