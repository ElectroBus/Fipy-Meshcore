# Pycom FiPy + Expansion Board 3 — MeshCore

Unofficial test firmware. **Not MeshCore.** See [NOTICE.md](NOTICE.md). Custom pin maps and binaries were made **with AI assistance (Cursor)**.

Custom MeshCore companion for a **FiPy** seated on a **Pycom Expansion Board 3.0 / 3.1** (USB, 5 V power, and JST battery). This is not Pysense, Pytrack, or Pyscan.

The FiPy radio is an onboard **SX1272** (868 / 915 MHz, not 433). There is no official MeshCore target for this board.

## Hardware

- Seat the FiPy with the **module LED above the USB socket**.
- Connect a LoRa antenna to the **U.FL on the LED side** (LoRa / Sigfox). The other U.FL is LTE.
- Leave the **CTS and RTS jumpers off**. Those pins fight the LTE modem.
- Keep **TX, RX, BAT, and LED jumpers on** for USB serial, battery sense, and the user LED.
- **CHG on** = 450 mA charge current. Remove it to drop to 100 mA.
- Do **not** use **P12** (WiFi antenna select and Expansion Board safe-boot).

## Pin map

| Function | ESP32 GPIO | Notes |
|---|---|---|
| LoRa SCK / MOSI / MISO / NSS | 5 / 27 / 19 / 18 | Hard-wired on the FiPy |
| LoRa DIO0 + DIO1 | 23 | All DIOs are diode-OR'd to GPIO23 |
| LoRa RESET | unused | Pycom does not bring RESET out. Stuck-radio workaround is a deep sleep cycle. |
| USB serial TX / RX | 1 / 2 | Expansion Board 3 PIC, not GPIO3 |
| Battery enable | 15 | Driven high so the LiPo can power the module (`P8/G15`) |
| Battery ADC | 36 (P16) | On-board 1M / 1M divider. Firmware already applies ×2. |
| User LED | 16 | LED jumper |
| I2C SDA / SCL | 13 / 22 | P9 / P10. Full image looks for an SSD1306 here. |
| Optional UI button | 33 (P13) | Not on the expansion board. Wire to GND if you want OLED clicks. |

## Firmwares

| Image | Env | What it is |
|---|---|---|
| Bare | `FiPy_companion_radio_ble_bare` | MeshCore app over BLE. No OLED. |
| Full | `FiPy_companion_radio_ble` | BLE + SSD1306 on P9/P10 + I2C sensors |

BLE pairing PIN: `123456`

Default radio is MeshCore's compiled 869.618 MHz / 62.5 kHz / SF8. Change region in the MeshCore app if you are on 915.

The FiPy is a **4 MB, no-PSRAM** ESP32. These images use `min_spiffs` and a smaller contact list than the XIAO builds.

## Flash

Use the **MeshCore app**, not Meshtastic.

1. Plug the Expansion Board USB into a data cable. The PIC should put the FiPy into download mode.
2. If it does not, hold **Reset** on the FiPy, tap **P2 / GPIO0** to GND (or hold the module reset while GPIO0 is low), then release reset.
3. Flash the `*.factory.bin` at **0x0** with Chrome/Edge ([Adafruit ESP Web Flasher](https://adafruit.github.io/Adafruit_WebSerial_ESPTool/) or [esp.huhn.me](https://esp.huhn.me/)).

## Battery

Install the **BAT** jumper. Voltage is read on **P16 / GPIO36** through the 1M/1M divider. USB connected will read near 5 V. The jumper text that says "GPIO3" is wrong; Pycom's own battery example uses P16.
