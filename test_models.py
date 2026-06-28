import pytest
from models import MeasMode, MODES, format_value


def test_all_modes_defined():
    assert len(MODES) == 10


def test_vdc_scpi():
    assert MODES[MeasMode.VDC].conf_cmd == "CONF:VOLT:DC DEF,DEF"


def test_vdc_unit():
    assert MODES[MeasMode.VDC].unit == "VDC"


def test_vdc_has_ranges():
    assert "Auto" in MODES[MeasMode.VDC].ranges


def test_ohm2_has_eight_ranges():
    assert len(MODES[MeasMode.OHM2].ranges) == 8  # Auto + 7 Bereiche


def test_diode_no_ranges():
    assert MODES[MeasMode.DIODE].ranges == {}


def test_format_millivolts():
    val, prefix = format_value(0.012)
    assert prefix == "m"


def test_format_kiloohm():
    val, prefix = format_value(4700.0)
    assert prefix == "k"
