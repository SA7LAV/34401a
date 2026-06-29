import configparser
import os

CONFIG_FILE = os.path.expanduser("~/.config/hp34401a/serial_config.ini")

_DEFAULTS = {
    "serial": {
        "port":     "",
        "baudrate": "9600",
        "parity":   "N",
        "stopbits": "1",
        "bytesize": "8",
        "timeout":  "9.0",
    },
    "measurement": {
        "sampling_ms": "50",
        "nplc":        "1",
        "autozero":    "true",
    },
    "display": {
        "decimals":        "4",
        "show_stats":      "true",
        "overlay_enabled": "false",
    },
    "app": {
        "language": "de",
    },
}


class AppConfig:
    def __init__(self):
        self._cfg = configparser.ConfigParser()
        for section, values in _DEFAULTS.items():
            self._cfg[section] = values
        self._load()

    def _load(self):
        if os.path.exists(CONFIG_FILE):
            self._cfg.read(CONFIG_FILE)

    def save(self):
        os.makedirs(os.path.dirname(CONFIG_FILE), exist_ok=True)
        with open(CONFIG_FILE, "w") as f:
            self._cfg.write(f)

    def get(self, section: str, key: str) -> str:
        return self._cfg.get(section, key)

    def set(self, section: str, key: str, value: str):
        if not self._cfg.has_section(section):
            self._cfg.add_section(section)
        self._cfg.set(section, key, value)

    @property
    def language(self) -> str:
        return self.get("app", "language")

    @language.setter
    def language(self, v: str):
        self.set("app", "language", v)

    @property
    def sampling_ms(self) -> int:
        return int(self.get("measurement", "sampling_ms"))

    @property
    def nplc(self) -> str:
        return self.get("measurement", "nplc")

    @property
    def autozero(self) -> bool:
        return self.get("measurement", "autozero").lower() == "true"

    @property
    def decimals(self) -> int:
        return int(self.get("display", "decimals"))

    @property
    def show_stats(self) -> bool:
        return self.get("display", "show_stats").lower() == "true"

    @property
    def overlay_enabled(self) -> bool:
        return self.get("display", "overlay_enabled").lower() == "true"

    @property
    def serial_port(self) -> str:
        return self.get("serial", "port")

    @property
    def serial_baudrate(self) -> str:
        return self.get("serial", "baudrate")

    @property
    def serial_parity(self) -> str:
        return self.get("serial", "parity")

    @property
    def serial_stopbits(self) -> str:
        return self.get("serial", "stopbits")

    @property
    def serial_bytesize(self) -> str:
        return self.get("serial", "bytesize")

    @property
    def serial_timeout(self) -> str:
        return self.get("serial", "timeout")


app_config = AppConfig()
