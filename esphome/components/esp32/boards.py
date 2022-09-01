from .const import VARIANT_ESP32, VARIANT_ESP32S2, VARIANT_ESP32C3

ESP32_BASE_PINS = {
    "TX": 1,
    "RX": 3,
    "SDA": 21,
    "SCL": 22,
    "SS": 5,
    "MOSI": 23,
    "MISO": 19,
    "SCK": 18,
    "A0": 36,
    "A3": 39,
    "A4": 32,
    "A5": 33,
    "A6": 34,
    "A7": 35,
    "A10": 4,
    "A11": 0,
    "A12": 2,
    "A13": 15,
    "A14": 13,
    "A15": 12,
    "A16": 14,
    "A17": 27,
    "A18": 25,
    "A19": 26,
    "T0": 4,
    "T1": 0,
    "T2": 2,
    "T3": 15,
    "T4": 13,
    "T5": 12,
    "T6": 14,
    "T7": 27,
    "T8": 33,
    "T9": 32,
    "DAC1": 25,
    "DAC2": 26,
    "SVP": 36,
    "SVN": 39,
}

ESP32_BOARD_PINS = {
    "alksesp32": {
        "A0": 32,
        "A1": 33,
        "A2": 25,
        "A3": 26,
        "A4": 27,
        "A5": 14,
        "A6": 12,
        "A7": 15,
        "D0": 40,
        "D1": 41,
        "D10": 19,
        "D11": 21,
        "D12": 22,
        "D13": 23,
        "D2": 15,
        "D3": 2,
        "D4": 0,
        "D5": 4,
        "D6": 16,
        "D7": 17,
        "D8": 5,
        "D9": 18,
        "DHT_PIN": 26,
        "LED": 23,
        "L_B": 5,
        "L_G": 17,
        "L_R": 22,
        "L_RGB_B": 16,
        "L_RGB_G": 21,
        "L_RGB_R": 4,
        "L_Y": 23,
        "MISO": 22,
        "MOSI": 21,
        "PHOTO": 25,
        "PIEZO1": 19,
        "PIEZO2": 18,
        "POT1": 32,
        "POT2": 33,
        "S1": 4,
        "S2": 16,
        "S3": 18,
        "S4": 19,
        "S5": 21,
        "SCK": 23,
        "SCL": 14,
        "SDA": 27,
        "SS": 19,
        "SW1": 15,
        "SW2": 2,
        "SW3": 0,
    },
    "az-delivery-devkit-v4": {},
    "bpi-bit": {
        "BUTTON_A": 35,
        "BUTTON_B": 27,
        "BUZZER": 25,
        "LIGHT_SENSOR1": 36,
        "LIGHT_SENSOR2": 39,
        "MPU9250_INT": 0,
        "P0": 25,
        "P1": 32,
        "P10": 26,
        "P11": 27,
        "P12": 2,
        "P13": 18,
        "P14": 19,
        "P15": 23,
        "P16": 5,
        "P19": 22,
        "P2": 33,
        "P20": 21,
        "P3": 13,
        "P4": 15,
        "P5": 35,
        "P6": 12,
        "P7": 14,
        "P8": 16,
        "P9": 17,
        "RGB_LED": 4,
        "TEMPERATURE_SENSOR": 34,
    },
    "briki_abc_esp32": {},
    "briki_mbc-wb_esp32": {},
    "d-duino-32": {
        "D1": 5,
        "D10": 1,
        "D2": 4,
        "D3": 0,
        "D4": 2,
        "D5": 14,
        "D6": 12,
        "D7": 13,
        "D8": 15,
        "D9": 3,
        "MISO": 12,
        "MOSI": 13,
        "SCK": 14,
        "SCL": 4,
        "SDA": 5,
        "SS": 15,
    },
    "esp-wrover-kit": {},
    "esp32-devkitlipo": {},
    "esp32-evb": {
        "BUTTON": 34,
        "MISO": 15,
        "MOSI": 2,
        "SCK": 14,
        "SCL": 16,
        "SDA": 13,
        "SS": 17,
    },
    "esp32-gateway": {"BUTTON": 34, "LED": 33, "SCL": 16, "SDA": 32},
    "esp32-poe-iso": {
        "BUTTON": 34,
        "MISO": 15,
        "MOSI": 2,
        "SCK": 14,
        "SCL": 16,
        "SDA": 13,
    },
    "esp32-poe": {"BUTTON": 34, "MISO": 15, "MOSI": 2, "SCK": 14, "SCL": 16, "SDA": 13},
    "esp32-pro": {
        "BUTTON": 34,
        "MISO": 15,
        "MOSI": 2,
        "SCK": 14,
        "SCL": 16,
        "SDA": 13,
        "SS": 17,
    },
    "esp320": {
        "LED": 5,
        "MISO": 12,
        "MOSI": 13,
        "SCK": 14,
        "SCL": 14,
        "SDA": 2,
        "SS": 15,
    },
    "esp32cam": {},
    "esp32dev": {},
    "esp32doit-devkit-v1": {"LED": 2},
    "esp32doit-espduino": {"TX0": 1, "RX0": 3, "CMD": 11, "CLK": 6, "SD0": 7, "SD1": 8},
    "esp32thing": {"BUTTON": 0, "LED": 5, "SS": 2},
    "esp32thing_plus": {
        "SDA": 23,
        "SCL": 22,
        "SS": 33,
        "MOSI": 18,
        "MISO": 19,
        "SCK": 5,
        "A0": 26,
        "A1": 25,
        "A2": 34,
        "A3": 39,
        "A4": 36,
        "A5": 4,
        "A6": 14,
        "A7": 32,
        "A8": 15,
        "A9": 33,
        "A10": 27,
        "A11": 12,
        "A12": 13,
    },
    "esp32vn-iot-uno": {},
    "espea32": {"BUTTON": 0, "LED": 5},
    "espectro32": {"LED": 15, "SD_SS": 33},
    "espino32": {"BUTTON": 0, "LED": 16},
    "etboard": {
        "LED_BUILTIN": 5,
        "TX": 34,
        "RX": 35,
        "SS": 29,
        "MOSI": 37,
        "MISO": 31,
        "SCK": 30,
        "A0": 36,
        "A1": 39,
        "A2": 32,
        "A3": 33,
        "A4": 34,
        "A5": 35,
        "A6": 25,
        "A7": 26,
        "D2": 27,
        "D3": 14,
        "D4": 12,
        "D5": 13,
        "D6": 15,
        "D7": 16,
        "D8": 17,
        "D9": 4,
    },
    "featheresp32": {
        "A0": 26,
        "A1": 25,
        "A10": 27,
        "A11": 12,
        "A12": 13,
        "A13": 35,
        "A2": 34,
        "A4": 36,
        "A5": 4,
        "A6": 14,
        "A7": 32,
        "A8": 15,
        "A9": 33,
        "Ax": 2,
        "LED": 13,
        "MOSI": 18,
        "RX": 16,
        "SCK": 5,
        "SDA": 23,
        "SS": 33,
        "TX": 17,
    },
    "featheresp32-s2": {
        "SDA": 3,
        "SCL": 4,
        "SS": 42,
        "MOSI": 35,
        "SCK": 36,
        "MISO": 37,
        "A0": 18,
        "A1": 17,
        "A10": 27,
        "A11": 12,
        "A12": 13,
        "A13": 35,
        "A2": 16,
        "A3": 15,
        "A4": 14,
        "A5": 8,
        "LED": 13,
        "TX": 39,
        "RX": 38,
        "T5": 5,
        "T8": 8,
        "T9": 9,
        "T10": 10,
        "T11": 11,
        "T12": 12,
        "T13": 13,
        "T14": 14,
        "DAC1": 17,
        "DAC2": 18,
    },
    "firebeetle32": {"LED": 2},
    "fm-devkit": {
        "D0": 34,
        "D1": 35,
        "D10": 0,
        "D2": 32,
        "D3": 33,
        "D4": 27,
        "D5": 14,
        "D6": 12,
        "D7": 13,
        "D8": 15,
        "D9": 23,
        "I2S_DOUT": 22,
        "I2S_LRCLK": 25,
        "I2S_MCLK": 2,
        "I2S_SCLK": 26,
        "LED": 5,
        "SCL": 17,
        "SDA": 16,
        "SW1": 4,
        "SW2": 18,
        "SW3": 19,
        "SW4": 21,
    },
    "frogboard": {},
    "healtypi4": {
        "KEY_BUILTIN": 17,
        "ADS1292_DRDY_PIN": 26,
        "ADS1292_CS_PIN": 13,
        "ADS1292_START_PIN": 14,
        "ADS1292_PWDN_PIN": 27,
        "AFE4490_CS_PIN": 21,
        "AFE4490_DRDY_PIN": 39,
        "AFE4490_PWDN_PIN": 4,
        "PUSH_BUTTON": 17,
        "SLIDE_SWITCH": 16,
    },
    "heltec_wifi_kit_32": {
        "A1": 37,
        "A2": 38,
        "BUTTON": 0,
        "LED": 25,
        "RST_OLED": 16,
        "SCL_OLED": 15,
        "SDA_OLED": 4,
        "Vext": 21,
    },
    "heltec_wifi_kit_32_v2": "heltec_wifi_kit_32",
    "heltec_wifi_lora_32": {
        "BUTTON": 0,
        "DIO0": 26,
        "DIO1": 33,
        "DIO2": 32,
        "LED": 25,
        "MOSI": 27,
        "RST_LoRa": 14,
        "RST_OLED": 16,
        "SCK": 5,
        "SCL_OLED": 15,
        "SDA_OLED": 4,
        "SS": 18,
        "Vext": 21,
    },
    "heltec_wifi_lora_32_V2": {
        "BUTTON": 0,
        "DIO0": 26,
        "DIO1": 35,
        "DIO2": 34,
        "LED": 25,
        "MOSI": 27,
        "RST_LoRa": 14,
        "RST_OLED": 16,
        "SCK": 5,
        "SCL_OLED": 15,
        "SDA_OLED": 4,
        "SS": 18,
        "Vext": 21,
    },
    "heltec_wireless_stick": {
        "BUTTON": 0,
        "DIO0": 26,
        "DIO1": 35,
        "DIO2": 34,
        "LED": 25,
        "MOSI": 27,
        "RST_LoRa": 14,
        "RST_OLED": 16,
        "SCK": 5,
        "SCL_OLED": 15,
        "SDA_OLED": 4,
        "SS": 18,
        "Vext": 21,
    },
    "heltec_wireless_stick_lite": {
        "LED_BUILTIN": 25,
        "KEY_BUILTIN": 0,
        "SS": 18,
        "MOSI": 27,
        "MISO": 19,
        "SCK": 5,
        "Vext": 21,
        "LED": 25,
        "RST_LoRa": 14,
        "DIO0": 26,
        "DIO1": 35,
        "DIO2": 34,
    },
    "honeylemon": {
        "LED_BUILTIN": 2,
        "BUILTIN_KEY": 0,
    },
    "hornbill32dev": {"BUTTON": 0, "LED": 13},
    "hornbill32minima": {"SS": 2},
    "imbrios-logsens-v1p1": {
        "LED_BUILTIN": 33,
        "UART2_TX": 17,
        "UART2_RX": 16,
        "UART2_RTS": 4,
        "CAN_TX": 17,
        "CAN_RX": 16,
        "CAN_TXDE": 4,
        "SS": 15,
        "MOSI": 13,
        "MISO": 12,
        "SCK": 14,
        "SPI_SS1": 23,
        "BUZZER_CTRL": 19,
        "SD_CARD_DETECT": 35,
        "SW2_BUILDIN": 0,
        "SW3_BUILDIN": 36,
        "SW4_BUILDIN": 34,
        "LED1_BUILDIN": 32,
        "LED2_BUILDIN": 33,
    },
    "inex_openkb": {
        "LED_BUILTIN": 16,
        "LDR_PIN": 36,
        "SW1": 16,
        "SW2": 14,
        "BT_LED": 17,
        "WIFI_LED": 2,
        "NTP_LED": 15,
        "IOT_LED": 12,
        "BUZZER": 13,
        "INPUT1": 32,
        "INPUT2": 33,
        "INPUT3": 34,
        "INPUT4": 35,
        "OUTPUT1": 26,
        "OUTPUT2": 27,
        "SDA0": 21,
        "SCL0": 22,
        "SDA1": 4,
        "SCL1": 5,
    },
    "intorobot": {
        "A1": 39,
        "A2": 35,
        "A3": 25,
        "A4": 26,
        "A5": 14,
        "A6": 12,
        "A7": 15,
        "A8": 13,
        "A9": 2,
        "BUTTON": 0,
        "D0": 19,
        "D1": 23,
        "D2": 18,
        "D3": 17,
        "D4": 16,
        "D5": 5,
        "D6": 4,
        "LED": 4,
        "MISO": 17,
        "MOSI": 16,
        "RGB_B_BUILTIN": 22,
        "RGB_G_BUILTIN": 21,
        "RGB_R_BUILTIN": 27,
        "SCL": 19,
        "SDA": 23,
        "T0": 19,
        "T1": 23,
        "T2": 18,
        "T3": 17,
        "T4": 16,
        "T5": 5,
        "T6": 4,
    },
    "iotaap_magnolia": {},
    "iotbusio": {},
    "iotbusproteus": {},
    "kits-edu": {},
    "labplus_mpython": {
        "SDA": 23,
        "SCL": 22,
        "P0": 33,
        "P1": 32,
        "P2": 35,
        "P3": 34,
        "P4": 39,
        "P5": 0,
        "P6": 16,
        "P7": 17,
        "P8": 26,
        "P9": 25,
        "P10": 36,
        "P11": 2,
        "P13": 18,
        "P14": 19,
        "P15": 21,
        "P16": 5,
        "P19": 22,
        "P20": 23,
        "P": 27,
        "Y": 14,
        "T": 12,
        "H": 13,
        "O": 15,
        "N": 4,
        "BTN_A": 0,
        "BTN_B": 2,
        "SOUND": 36,
        "LIGHT": 39,
        "BUZZER": 16,
    },
    "lolin32": {"LED": 5},
    "lolin32_lite": {"LED": 22},
    "lolin_c3_mini": {
        "TX": 21,
        "RX": 20,
        "SDA": 8,
        "SCL": 10,
        "SS": 5,
        "MOSI": 4,
        "MISO": 3,
        "SCK": 2,
        "A0": 0,
        "A1": 1,
        "A2": 2,
        "A3": 3,
        "A4": 4,
        "A5": 5,
        "D0": 1,
        "D1": 10,
        "D2": 8,
        "D3": 7,
        "D4": 6,
        "D5": 2,
        "D6": 3,
        "D7": 4,
        "D8": 5,
        "LED": 7,
        "BUTTON": 9,
    },
    "lolin_d32": {"LED": 5, "_VBAT": 35},
    "lolin_d32_pro": {"LED": 5, "_VBAT": 35},
    "lopy": {
        "A1": 37,
        "A2": 38,
        "LED": 0,
        "MISO": 37,
        "MOSI": 22,
        "SCK": 13,
        "SCL": 13,
        "SDA": 12,
        "SS": 17,
    },
    "lopy4": {
        "A1": 37,
        "A2": 38,
        "LED": 0,
        "MISO": 37,
        "MOSI": 22,
        "SCK": 13,
        "SCL": 13,
        "SDA": 12,
        "SS": 18,
    },
    "m5stack-atom": {
        "SDA": 26,
        "SCL": 32,
        "ADC1": 35,
        "ADC2": 36,
        "SS": 19,
        "MOSI": 33,
        "MISO": 23,
        "SCK": 22,
    },
    "m5stack-core-esp32": {
        "ADC1": 35,
        "ADC2": 36,
        "G0": 0,
        "G1": 1,
        "G12": 12,
        "G13": 13,
        "G15": 15,
        "G16": 16,
        "G17": 17,
        "G18": 18,
        "G19": 19,
        "G2": 2,
        "G21": 21,
        "G22": 22,
        "G23": 23,
        "G25": 25,
        "G26": 26,
        "G3": 3,
        "G34": 34,
        "G35": 35,
        "G36": 36,
        "G5": 5,
        "RXD2": 16,
        "TXD2": 17,
    },
    "m5stack-core2": {
        "SDA": 32,
        "SCL": 33,
        "SS": 5,
        "MOSI": 23,
        "MISO": 38,
        "SCK": 18,
        "ADC1": 35,
        "ADC2": 36,
    },
    "m5stack-coreink": {
        "SDA": 32,
        "SCL": 33,
        "SS": 9,
        "MOSI": 23,
        "MISO": 34,
        "SCK": 18,
        "ADC1": 35,
        "ADC2": 36,
    },
    "m5stack-fire": {
        "ADC1": 35,
        "ADC2": 36,
        "G0": 0,
        "G1": 1,
        "G12": 12,
        "G13": 13,
        "G15": 15,
        "G16": 16,
        "G17": 17,
        "G18": 18,
        "G19": 19,
        "G2": 2,
        "G21": 21,
        "G22": 22,
        "G23": 23,
        "G25": 25,
        "G26": 26,
        "G3": 3,
        "G34": 34,
        "G35": 35,
        "G36": 36,
        "G5": 5,
    },
    "m5stack-grey": {
        "ADC1": 35,
        "ADC2": 36,
        "G0": 0,
        "G1": 1,
        "G12": 12,
        "G13": 13,
        "G15": 15,
        "G16": 16,
        "G17": 17,
        "G18": 18,
        "G19": 19,
        "G2": 2,
        "G21": 21,
        "G22": 22,
        "G23": 23,
        "G25": 25,
        "G26": 26,
        "G3": 3,
        "G34": 34,
        "G35": 35,
        "G36": 36,
        "G5": 5,
        "RXD2": 16,
        "TXD2": 17,
    },
    "m5stack-timer-cam": {
        "LED_BUILTIN": 2,
        "SDA": 4,
        "SCL": 13,
        "SS": 5,
        "MOSI": 23,
        "MISO": 19,
        "SCK": 18,
        "ADC1": 35,
        "ADC2": 36,
    },
    "m5stick-c": {
        "ADC1": 35,
        "ADC2": 36,
        "G0": 0,
        "G10": 10,
        "G26": 26,
        "G32": 32,
        "G33": 33,
        "G36": 36,
        "G37": 37,
        "G39": 39,
        "G9": 9,
        "MISO": 36,
        "MOSI": 15,
        "SCK": 13,
        "SCL": 33,
        "SDA": 32,
    },
    "magicbit": {
        "BLUE_LED": 17,
        "BUZZER": 25,
        "GREEN_LED": 16,
        "LDR": 36,
        "LED": 16,
        "LEFT_BUTTON": 35,
        "MOTOR1A": 27,
        "MOTOR1B": 18,
        "MOTOR2A": 16,
        "MOTOR2B": 17,
        "POT": 39,
        "RED_LED": 27,
        "RIGHT_PUTTON": 34,
        "YELLOW_LED": 18,
    },
    "mgbot-iotik32a": {
        "LED_BUILTIN": 4,
        "TX2": 17,
        "RX2": 16,
    },
    "mgbot-iotik32b": {
        "LED_BUILTIN": 18,
        "IR": 27,
        "TX2": 17,
        "RX2": 16,
    },
    "mhetesp32devkit": {"LED": 2},
    "mhetesp32minikit": {"LED": 2},
    "microduino-core-esp32": {
        "A0": 12,
        "A1": 13,
        "A10": 25,
        "A11": 26,
        "A12": 27,
        "A13": 14,
        "A2": 15,
        "A3": 4,
        "A6": 38,
        "A7": 37,
        "A8": 32,
        "A9": 33,
        "D0": 3,
        "D1": 1,
        "D10": 5,
        "D11": 23,
        "D12": 19,
        "D13": 18,
        "D14": 12,
        "D15": 13,
        "D16": 15,
        "D17": 4,
        "D18": 22,
        "D19": 21,
        "D2": 16,
        "D20": 38,
        "D21": 37,
        "D3": 17,
        "D4": 32,
        "D5": 33,
        "D6": 25,
        "D7": 26,
        "D8": 27,
        "D9": 14,
        "SCL": 21,
        "SCL1": 13,
        "SDA": 22,
        "SDA1": 12,
    },
    "nano32": {"BUTTON": 0, "LED": 16},
    "nina_w10": {
        "D0": 3,
        "D1": 1,
        "D10": 5,
        "D11": 19,
        "D12": 23,
        "D13": 18,
        "D14": 13,
        "D15": 12,
        "D16": 32,
        "D17": 33,
        "D18": 21,
        "D19": 34,
        "D2": 26,
        "D20": 36,
        "D21": 39,
        "D3": 25,
        "D4": 35,
        "D5": 27,
        "D6": 22,
        "D7": 0,
        "D8": 15,
        "D9": 14,
        "LED_BLUE": 21,
        "LED_GREEN": 33,
        "LED_RED": 23,
        "SCL": 13,
        "SDA": 12,
        "SW1": 33,
        "SW2": 27,
    },
    "node32s": {},
    "nodemcu-32s": {"BUTTON": 0, "LED": 2},
    "nscreen-32": {},
    "odroid_esp32": {"ADC1": 35, "ADC2": 36, "LED": 2, "SCL": 4, "SDA": 15, "SS": 22},
    "onehorse32dev": {"A1": 37, "A2": 38, "BUTTON": 0, "LED": 5},
    "oroca_edubot": {
        "A0": 34,
        "A1": 39,
        "A2": 36,
        "A3": 33,
        "D0": 4,
        "D1": 16,
        "D2": 17,
        "D3": 22,
        "D4": 23,
        "D5": 5,
        "D6": 18,
        "D7": 19,
        "D8": 33,
        "LED": 13,
        "MOSI": 18,
        "RX": 16,
        "SCK": 5,
        "SDA": 23,
        "SS": 2,
        "TX": 17,
        "VBAT": 35,
    },
    "pico32": {},
    "piranha_esp32": {
        "LED_BUILTIN": 2,
        "KEY_BUILTIN": 0,
    },
    "pocket_32": {"LED": 16},
    "pycom_gpy": {
        "A1": 37,
        "A2": 38,
        "LED": 0,
        "MISO": 37,
        "MOSI": 22,
        "SCK": 13,
        "SCL": 13,
        "SDA": 12,
        "SS": 17,
    },
    "qchip": "heltec_wifi_kit_32",
    "quantum": {},
    "s_odi_ultra": {
        "LED_BUILTIN": 2,
        "LED_BUILTINB": 4,
    },
    "sensesiot_weizen": {},
    "sg-o_airMon": {},
    "sparkfun_lora_gateway_1-channel": {"MISO": 12, "MOSI": 13, "SCK": 14, "SS": 16},
    "tinypico": {},
    "ttgo-lora32-v1": {
        "A1": 37,
        "A2": 38,
        "BUTTON": 0,
        "LED": 2,
        "MOSI": 27,
        "SCK": 5,
        "SS": 18,
    },
    "ttgo-lora32-v2": {
        "LED_BUILTIN": 22,
        "KEY_BUILTIN": 0,
        "SS": 18,
        "MOSI": 27,
        "MISO": 19,
        "SCK": 5,
        "A1": 37,
        "A2": 38,
    },
    "ttgo-lora32-v21": {
        "LED_BUILTIN": 25,
        "KEY_BUILTIN": 0,
        "SS": 18,
        "MOSI": 27,
        "MISO": 19,
        "SCK": 5,
        "A1": 37,
        "A2": 38,
    },
    "ttgo-t-beam": {"BUTTON": 39, "LED": 14, "MOSI": 27, "SCK": 5, "SS": 18},
    "ttgo-t-watch": {"BUTTON": 36, "MISO": 2, "MOSI": 15, "SCK": 14, "SS": 13},
    "ttgo-t1": {"LED": 22, "MISO": 2, "MOSI": 15, "SCK": 14, "SCL": 23, "SS": 13},
    "ttgo-t7-v13-mini32": {"LED": 22},
    "ttgo-t7-v14-mini32": {"LED": 19},
    "turta_iot_node": {},
    "vintlabs-devkit-v1": {
        "LED": 2,
        "PWM0": 12,
        "PWM1": 13,
        "PWM2": 14,
        "PWM3": 15,
        "PWM4": 16,
        "PWM5": 17,
        "PWM6": 18,
        "PWM7": 19,
    },
    "wemos_d1_mini32": {
        "D0": 26,
        "D1": 22,
        "D2": 21,
        "D3": 17,
        "D4": 16,
        "D5": 18,
        "D6": 19,
        "D7": 23,
        "D8": 5,
        "LED": 2,
        "RXD": 3,
        "TXD": 1,
        "_VBAT": 35,
    },
    "wemosbat": {"LED": 16},
    "wesp32": {"MISO": 32, "SCL": 4, "SDA": 15},
    "widora-air": {
        "A1": 39,
        "A2": 35,
        "A3": 25,
        "A4": 26,
        "A5": 14,
        "A6": 12,
        "A7": 15,
        "A8": 13,
        "A9": 2,
        "BUTTON": 0,
        "D0": 19,
        "D1": 23,
        "D2": 18,
        "D3": 17,
        "D4": 16,
        "D5": 5,
        "D6": 4,
        "LED": 25,
        "MISO": 17,
        "MOSI": 16,
        "SCL": 19,
        "SDA": 23,
        "T0": 19,
        "T1": 23,
        "T2": 18,
        "T3": 17,
        "T4": 16,
        "T5": 5,
        "T6": 4,
    },
    "wifiduino32": {
        "LED_BUILTIN": 2,
        "KEY_BUILTIN": 0,
        "SDA": 5,
        "SCL": 16,
        "A0": 27,
        "A1": 14,
        "A2": 12,
        "A3": 35,
        "A4": 13,
        "A5": 4,
        "D0": 3,
        "D1": 1,
        "D2": 17,
        "D3": 15,
        "D4": 32,
        "D5": 33,
        "D6": 25,
        "D7": 26,
        "D8": 23,
        "D9": 22,
        "D10": 21,
        "D11": 19,
        "D12": 18,
        "D13": 2,
    },
    "xinabox_cw02": {"LED": 27},
}

"""
BOARD_TO_VARIANT generated with:

git clone https://github.com/platformio/platform-espressif32
for x in platform-espressif32/boards/*.json; do
  mcu=$(jq -r .build.mcu <"$x");
  fname=$(basename "$x")
  board="${fname%.*}"
  variant=$(echo "$mcu" | tr '[:lower:]' '[:upper:]')
  echo "    \"$board\": VARIANT_${variant},"
done | sort
"""

BOARD_TO_VARIANT = {
    "alksesp32": VARIANT_ESP32,
    "az-delivery-devkit-v4": VARIANT_ESP32,
    "bpi-bit": VARIANT_ESP32,
    "briki_abc_esp32": VARIANT_ESP32,
    "briki_mbc-wb_esp32": VARIANT_ESP32,
    "d-duino-32": VARIANT_ESP32,
    "esp320": VARIANT_ESP32,
    "esp32-c3-devkitm-1": VARIANT_ESP32C3,
    "esp32cam": VARIANT_ESP32,
    "esp32-devkitlipo": VARIANT_ESP32,
    "esp32dev": VARIANT_ESP32,
    "esp32doit-devkit-v1": VARIANT_ESP32,
    "esp32doit-espduino": VARIANT_ESP32,
    "esp32-evb": VARIANT_ESP32,
    "esp32-gateway": VARIANT_ESP32,
    "esp32-poe-iso": VARIANT_ESP32,
    "esp32-poe": VARIANT_ESP32,
    "esp32-pro": VARIANT_ESP32,
    "esp32-s2-kaluga-1": VARIANT_ESP32S2,
    "esp32-s2-saola-1": VARIANT_ESP32S2,
    "esp32thing_plus": VARIANT_ESP32,
    "esp32thing": VARIANT_ESP32,
    "esp32vn-iot-uno": VARIANT_ESP32,
    "espea32": VARIANT_ESP32,
    "espectro32": VARIANT_ESP32,
    "espino32": VARIANT_ESP32,
    "esp-wrover-kit": VARIANT_ESP32,
    "etboard": VARIANT_ESP32,
    "featheresp32-s2": VARIANT_ESP32S2,
    "featheresp32": VARIANT_ESP32,
    "firebeetle32": VARIANT_ESP32,
    "fm-devkit": VARIANT_ESP32,
    "frogboard": VARIANT_ESP32,
    "healthypi4": VARIANT_ESP32,
    "heltec_wifi_kit_32_v2": VARIANT_ESP32,
    "heltec_wifi_kit_32": VARIANT_ESP32,
    "heltec_wifi_lora_32_V2": VARIANT_ESP32,
    "heltec_wifi_lora_32": VARIANT_ESP32,
    "heltec_wireless_stick_lite": VARIANT_ESP32,
    "heltec_wireless_stick": VARIANT_ESP32,
    "honeylemon": VARIANT_ESP32,
    "hornbill32dev": VARIANT_ESP32,
    "hornbill32minima": VARIANT_ESP32,
    "imbrios-logsens-v1p1": VARIANT_ESP32,
    "inex_openkb": VARIANT_ESP32,
    "intorobot": VARIANT_ESP32,
    "iotaap_magnolia": VARIANT_ESP32,
    "iotbusio": VARIANT_ESP32,
    "iotbusproteus": VARIANT_ESP32,
    "kits-edu": VARIANT_ESP32,
    "labplus_mpython": VARIANT_ESP32,
    "lolin32_lite": VARIANT_ESP32,
    "lolin32": VARIANT_ESP32,
    "lolin_c3_mini": VARIANT_ESP32C3,
    "lolin_d32_pro": VARIANT_ESP32,
    "lolin_d32": VARIANT_ESP32,
    "lopy4": VARIANT_ESP32,
    "lopy": VARIANT_ESP32,
    "m5stack-atom": VARIANT_ESP32,
    "m5stack-core2": VARIANT_ESP32,
    "m5stack-core-esp32": VARIANT_ESP32,
    "m5stack-coreink": VARIANT_ESP32,
    "m5stack-fire": VARIANT_ESP32,
    "m5stack-grey": VARIANT_ESP32,
    "m5stack-timer-cam": VARIANT_ESP32,
    "m5stick-c": VARIANT_ESP32,
    "magicbit": VARIANT_ESP32,
    "mgbot-iotik32a": VARIANT_ESP32,
    "mgbot-iotik32b": VARIANT_ESP32,
    "mhetesp32devkit": VARIANT_ESP32,
    "mhetesp32minikit": VARIANT_ESP32,
    "microduino-core-esp32": VARIANT_ESP32,
    "nano32": VARIANT_ESP32,
    "nina_w10": VARIANT_ESP32,
    "node32s": VARIANT_ESP32,
    "nodemcu-32s": VARIANT_ESP32,
    "nscreen-32": VARIANT_ESP32,
    "odroid_esp32": VARIANT_ESP32,
    "onehorse32dev": VARIANT_ESP32,
    "oroca_edubot": VARIANT_ESP32,
    "pico32": VARIANT_ESP32,
    "piranha_esp32": VARIANT_ESP32,
    "pocket_32": VARIANT_ESP32,
    "pycom_gpy": VARIANT_ESP32,
    "qchip": VARIANT_ESP32,
    "quantum": VARIANT_ESP32,
    "sensesiot_weizen": VARIANT_ESP32,
    "sg-o_airMon": VARIANT_ESP32,
    "s_odi_ultra": VARIANT_ESP32,
    "sparkfun_lora_gateway_1-channel": VARIANT_ESP32,
    "tinypico": VARIANT_ESP32,
    "ttgo-lora32-v1": VARIANT_ESP32,
    "ttgo-lora32-v21": VARIANT_ESP32,
    "ttgo-lora32-v2": VARIANT_ESP32,
    "ttgo-t1": VARIANT_ESP32,
    "ttgo-t7-v13-mini32": VARIANT_ESP32,
    "ttgo-t7-v14-mini32": VARIANT_ESP32,
    "ttgo-t-beam": VARIANT_ESP32,
    "ttgo-t-watch": VARIANT_ESP32,
    "turta_iot_node": VARIANT_ESP32,
    "vintlabs-devkit-v1": VARIANT_ESP32,
    "wemosbat": VARIANT_ESP32,
    "wemos_d1_mini32": VARIANT_ESP32,
    "wesp32": VARIANT_ESP32,
    "widora-air": VARIANT_ESP32,
    "wifiduino32": VARIANT_ESP32,
    "xinabox_cw02": VARIANT_ESP32,
}
