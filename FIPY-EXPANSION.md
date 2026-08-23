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
| Bare BLE | `FiPy_companion_radio_ble_bare` | MeshCore app over Bluetooth. No OLED. |
| Full BLE | `FiPy_companion_radio_ble` | BLE + SSD1306 on P9/P10 + I2C sensors |
| USB | `FiPy_companion_radio_usb` | MeshCore web app over USB serial (RX on **P0 / GPIO3**) plus BLE PIN `123456`. |

BLE pairing PIN: `123456`. Official [flasher.meshcore.io](https://flasher.meshcore.io) will **not** list this board. Use [app.meshcore.nz](https://app.meshcore.nz) (USB or Bluetooth), not the flasher, after you have flashed a `.factory.bin` yourself.

Default radio is MeshCore's compiled 869.618 MHz / 62.5 kHz / SF8. Change region in the MeshCore app if you are on 915.

The FiPy is a **4 MB, no-PSRAM** ESP32. These images use `min_spiffs` and a smaller contact list than the XIAO builds.

## Flash

Use the **MeshCore app**, not Meshtastic. Flash a `*.factory.bin` at **0x0**. Chip is **ESP32** (not S3). Flash size **4MB**.

After flashing the USB image, remove the P2-to-GND boot jumper. Keep the **TX jumper** on. Keep your **PIC-RX-to-P0** flash wire (firmware RX is P0 / GPIO3, same as the bootloader). The stock **RX jumper (GPIO2)** is not used by the USB image. If the BAT jumper really ties GPIO3, take it off so it does not load the UART.

Then open [app.meshcore.nz](https://app.meshcore.nz) in Chrome/Edge → **USB** → Expansion Board COM port at **115200**. Or connect over **Bluetooth** with PIN `123456`. Do not use [flasher.meshcore.io](https://flasher.meshcore.io) to connect; that site only flashes official boards.

Expansion Board 3 will **not** auto-reset into the bootloader. Web flashers and esptool expect DTR/RTS; this PIC does not do that. The Safe Boot button is also the wrong control — that is MicroPython safe boot on P12, not ESP32 download mode.

### 1. Jumpers and cable

- Use a **data** USB cable, Chrome or Edge.
- **TX and RX jumpers on.** CTS and RTS **off**.
- In Windows Device Manager you should see a **COM port**. If the green USB LED on a 3.1 board is dark and there is no COM port, the PIC is stuck in DFU and needs a Pycom `dfu-util` update first.

### 2. Put the FiPy in download mode

Do this **before** you click Connect in the flasher.

1. Jumper **P2** (GPIO0, the RGB LED pin) to **GND**.
2. Press and release **Reset on the FiPy module** (not Safe Boot on the expansion board).
3. Leave P2 grounded until the flash finishes.

### 3. If the flasher still cannot sync

The ESP32 ROM bootloader listens on **P0 / GPIO3**. Expansion Board 3 routes USB RX to **GPIO2**, which is why a web flasher often opens the COM port and then never talks to the chip.

Add a jumper wire from the expansion board **RX** line to **P0**, keep P2 at GND, press FiPy Reset again, then flash.

Local flash (after the steps above). Replace `COM5` with your port:

```
esptool.py --chip esp32 --port COM5 --baud 115200 --before no-reset --after no-reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0x0 meshcore-fipy-expansion-ble-bare.factory.bin
```

`--before no-reset` is required. The tool must not toggle reset; you already entered download mode by hand.

## Battery

Install the **BAT** jumper. Voltage is read on **P16 / GPIO36** through the 1M/1M divider. USB connected will read near 5 V. The jumper text that says "GPIO3" is wrong; Pycom's own battery example uses P16.
