import pytest
from unittest.mock import MagicMock, patch
from instrument import Instrument


def test_not_connected_by_default():
    inst = Instrument()
    assert not inst.is_connected


def test_connect_opens_serial_port():
    inst = Instrument()
    with patch("instrument.serial.Serial") as mock_serial:
        mock_serial.return_value.is_open = True
        inst.connect("/dev/ttyUSB0", 9600, "N", 1)
        mock_serial.assert_called_once()
        assert inst.is_connected


def test_disconnect_closes_port():
    inst = Instrument()
    with patch("instrument.serial.Serial") as mock_serial:
        mock_port = MagicMock()
        mock_port.is_open = True
        mock_serial.return_value = mock_port
        inst.connect("/dev/ttyUSB0", 9600, "N", 1)
        inst.disconnect()
        mock_port.close.assert_called_once()
        assert not inst.is_connected


def test_send_raw_appends_crlf():
    inst = Instrument()
    with patch("instrument.serial.Serial") as mock_serial:
        mock_port = MagicMock()
        mock_port.is_open = True
        mock_serial.return_value = mock_port
        inst.connect("/dev/ttyUSB0", 9600, "N", 1)
        inst._send_raw("*IDN?")
        mock_port.write.assert_called_once_with(b"*IDN?\r\n")
