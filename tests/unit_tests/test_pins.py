"""
Please Note:

These tests cover the process of identifying information about pins, they do not
check if the definition of MCUs and pins is correct.

"""
import pytest

from esphome.config_validation import Invalid
from esphome.core import EsphomeCore
from esphome import pins


MOCK_ESP8266_BOARD_ID = "_mock_esp8266"
MOCK_ESP8266_PINS = {'X0': 16, 'X1': 5, 'X2': 4, 'LED': 2}
MOCK_ESP8266_BOARD_ALIAS_ID = "_mock_esp8266_alias"
MOCK_ESP8266_FLASH_SIZE = pins.FLASH_SIZE_2_MB

MOCK_ESP32_BOARD_ID = "_mock_esp32"
MOCK_ESP32_PINS = {'Y0': 12, 'Y1': 8, 'Y2': 3, 'LED': 9, "A0": 8}
MOCK_ESP32_BOARD_ALIAS_ID = "_mock_esp32_alias"


@pytest.fixture
def mock_mcu(monkeypatch):
    """
    Add a mock MCU into the lists as a stable fixture
    """
    pins.ESP8266_BOARD_PINS[MOCK_ESP8266_BOARD_ID] = MOCK_ESP8266_PINS
    pins.ESP8266_FLASH_SIZES[MOCK_ESP8266_BOARD_ID] = MOCK_ESP8266_FLASH_SIZE
    pins.ESP8266_BOARD_PINS[MOCK_ESP8266_BOARD_ALIAS_ID] = MOCK_ESP8266_BOARD_ID
    pins.ESP8266_FLASH_SIZES[MOCK_ESP8266_BOARD_ALIAS_ID] = MOCK_ESP8266_FLASH_SIZE
    pins.ESP32_BOARD_PINS[MOCK_ESP32_BOARD_ID] = MOCK_ESP32_PINS
    pins.ESP32_BOARD_PINS[MOCK_ESP32_BOARD_ALIAS_ID] = MOCK_ESP32_BOARD_ID
    yield
    del pins.ESP8266_BOARD_PINS[MOCK_ESP8266_BOARD_ID]
    del pins.ESP8266_FLASH_SIZES[MOCK_ESP8266_BOARD_ID]
    del pins.ESP8266_BOARD_PINS[MOCK_ESP8266_BOARD_ALIAS_ID]
    del pins.ESP8266_FLASH_SIZES[MOCK_ESP8266_BOARD_ALIAS_ID]
    del pins.ESP32_BOARD_PINS[MOCK_ESP32_BOARD_ID]
    del pins.ESP32_BOARD_PINS[MOCK_ESP32_BOARD_ALIAS_ID]


@pytest.fixture
def core(monkeypatch, mock_mcu):
    core = EsphomeCore()
    monkeypatch.setattr(pins, "CORE", core)
    return core


@pytest.fixture
def core_esp8266(core):
    core.esp_platform = "ESP8266"
    core.board = MOCK_ESP8266_BOARD_ID
    return core


@pytest.fixture
def core_esp32(core):
    core.esp_platform = "ESP32"
    core.board = MOCK_ESP32_BOARD_ID
    return core


class Test_lookup_pin:
    @pytest.mark.parametrize("value, expected", (
            ("X1", 5),
            ("MOSI", 13),
    ))
    def test_valid_esp8266_pin(self, core_esp8266, value, expected):
        actual = pins._lookup_pin(value)

        assert actual == expected

    def test_valid_esp8266_pin_alias(self, core_esp8266):
        core_esp8266.board = MOCK_ESP8266_BOARD_ALIAS_ID

        actual = pins._lookup_pin("X2")

        assert actual == 4

    @pytest.mark.parametrize("value, expected", (
            ("Y1", 8),
            ("A0", 8),
            ("MOSI", 23),
    ))
    def test_valid_esp32_pin(self, core_esp32, value, expected):
        actual = pins._lookup_pin(value)

        assert actual == expected

    @pytest.mark.xfail(reason="This may be expected")
    def test_valid_32_pin_alias(self, core_esp32):
        core_esp32.board = MOCK_ESP32_BOARD_ALIAS_ID

        actual = pins._lookup_pin("Y2")

        assert actual == 3

    def test_invalid_pin(self, core_esp8266):
        with pytest.raises(Invalid, match="Cannot resolve pin name 'X42' for board _mock_esp8266."):
            pins._lookup_pin("X42")

    def test_unsupported_platform(self, core):
        core.esp_platform = "avr"

        with pytest.raises(NotImplementedError):
            pins._lookup_pin("TX")


class Test_translate_pin:
    @pytest.mark.parametrize("value, expected", (
            (2, 2),
            ("3", 3),
            ("GPIO4", 4),
            ("TX", 1),
            ("Y0", 12),
    ))
    def test_valid_values(self, core_esp32, value, expected):
        actual = pins._translate_pin(value)

        assert actual == expected
