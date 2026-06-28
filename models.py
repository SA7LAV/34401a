from enum import Enum, auto
from dataclasses import dataclass, field


class MeasMode(Enum):
    VDC = auto()
    ADC = auto()
    VAC = auto()
    AAC = auto()
    OHM2 = auto()
    OHM4 = auto()
    FREQ = auto()
    PERIOD = auto()
    DIODE = auto()
    CONT = auto()


@dataclass
class ModeConfig:
    label: str
    conf_cmd: str
    unit: str
    ranges: dict = field(default_factory=dict)


MODES = {
    MeasMode.VDC: ModeConfig(
        label="VDC", conf_cmd="CONF:VOLT:DC DEF,DEF", unit="VDC",
        ranges={
            "Auto":   None,
            "100 mV": "VOLT:DC:RANG 0.1",
            "1 V":    "VOLT:DC:RANG 1",
            "10 V":   "VOLT:DC:RANG 10",
            "100 V":  "VOLT:DC:RANG 100",
            "1000 V": "VOLT:DC:RANG 1000",
        },
    ),
    MeasMode.ADC: ModeConfig(
        label="ADC", conf_cmd="CONF:CURR:DC DEF,DEF", unit="ADC",
        ranges={
            "Auto":   None,
            "100 mA": "CURR:DC:RANG 0.1",
            "1 A":    "CURR:DC:RANG 1",
            "3 A":    "CURR:DC:RANG 3",
        },
    ),
    MeasMode.VAC: ModeConfig(
        label="VAC", conf_cmd="CONF:VOLT:AC DEF,DEF", unit="VAC",
        ranges={
            "Auto":   None,
            "100 mV": "VOLT:AC:RANG 0.1",
            "1 V":    "VOLT:AC:RANG 1",
            "10 V":   "VOLT:AC:RANG 10",
            "100 V":  "VOLT:AC:RANG 100",
            "1000 V": "VOLT:AC:RANG 1000",
        },
    ),
    MeasMode.AAC: ModeConfig(
        label="AAC", conf_cmd="CONF:CURR:AC DEF,DEF", unit="AAC",
        ranges={
            "Auto": None,
            "1 A":  "CURR:AC:RANG 1",
            "3 A":  "CURR:AC:RANG 3",
        },
    ),
    MeasMode.OHM2: ModeConfig(
        label="2W Ω", conf_cmd="CONF:RES DEF,DEF", unit="Ω",
        ranges={
            "Auto":   None,
            "100 Ω":  "RES:RANG 100",
            "1 kΩ":   "RES:RANG 1000",
            "10 kΩ":  "RES:RANG 10000",
            "100 kΩ": "RES:RANG 100000",
            "1 MΩ":   "RES:RANG 1000000",
            "10 MΩ":  "RES:RANG 10000000",
            "100 MΩ": "RES:RANG 100000000",
        },
    ),
    MeasMode.OHM4: ModeConfig(
        label="4W Ω", conf_cmd="CONF:FRES DEF,DEF", unit="Ω",
        ranges={
            "Auto":   None,
            "100 Ω":  "FRES:RANG 100",
            "1 kΩ":   "FRES:RANG 1000",
            "10 kΩ":  "FRES:RANG 10000",
            "100 kΩ": "FRES:RANG 100000",
            "1 MΩ":   "FRES:RANG 1000000",
            "10 MΩ":  "FRES:RANG 10000000",
            "100 MΩ": "FRES:RANG 100000000",
        },
    ),
    MeasMode.FREQ: ModeConfig(
        label="FREQ", conf_cmd="CONF:FREQ DEF,DEF", unit="Hz",
        ranges={
            "Auto":   None,
            "100 mV": "FREQ:VOLT:RANG 0.1",
            "1 V":    "FREQ:VOLT:RANG 1",
            "10 V":   "FREQ:VOLT:RANG 10",
            "100 V":  "FREQ:VOLT:RANG 100",
            "1000 V": "FREQ:VOLT:RANG 1000",
        },
    ),
    MeasMode.PERIOD: ModeConfig(
        label="PERIOD", conf_cmd="CONF:PER DEF,DEF", unit="s",
        ranges={
            "Auto":   None,
            "100 mV": "PER:VOLT:RANG 0.1",
            "1 V":    "PER:VOLT:RANG 1",
            "10 V":   "PER:VOLT:RANG 10",
            "100 V":  "PER:VOLT:RANG 100",
            "1000 V": "PER:VOLT:RANG 1000",
        },
    ),
    MeasMode.DIODE: ModeConfig(
        label="DIODE", conf_cmd="CONF:DIOD", unit="VDC", ranges={},
    ),
    MeasMode.CONT: ModeConfig(
        label="CONT", conf_cmd="CONF:CONT", unit="Ω", ranges={},
    ),
}

SI_PREFIXES = [
    (1e12, "T"), (1e9, "G"), (1e6, "M"), (1e3, "k"),
    (1.0, ""), (1e-3, "m"), (1e-6, "µ"), (1e-9, "n"), (1e-12, "p"),
]


def format_value(value: float, decimals: int = 4) -> tuple:
    fmt = f"{{:.{decimals}f}}"
    abs_val = abs(value)
    for threshold, prefix in SI_PREFIXES:
        if abs_val >= threshold or threshold == 1e-12:
            scaled = value / threshold
            return fmt.format(scaled), prefix
    return f"{value:.6e}", ""
