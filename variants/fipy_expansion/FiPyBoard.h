#pragma once

#include <Arduino.h>
#include <helpers/ESP32Board.h>

#ifndef PIN_BATT_EN
#define PIN_BATT_EN 15
#endif

#ifndef PIN_SERIAL_RX
#define PIN_SERIAL_RX 2
#endif

#ifndef PIN_SERIAL_TX
#define PIN_SERIAL_TX 1
#endif

class FiPyBoard : public ESP32Board {
public:
  void begin() {
    ESP32Board::begin();

    // Expansion Board 3: FET that lets the LiPo power the module.
    pinMode(PIN_BATT_EN, OUTPUT);
    digitalWrite(PIN_BATT_EN, HIGH);

    // EB3 USB-serial PIC is jumpered to GPIO1 (TX) and GPIO2 (RX), not GPIO3.
    Serial.begin(115200, SERIAL_8N1, PIN_SERIAL_RX, PIN_SERIAL_TX);

#ifdef ENABLE_USB_INTERFACE
    setInhibitSleep(true);
#endif
  }

  uint32_t getIRQGpio() override {
    return P_LORA_DIO_0;
  }

  const char* getManufacturerName() const override {
    return "Pycom FiPy + Expansion Board 3";
  }
};
