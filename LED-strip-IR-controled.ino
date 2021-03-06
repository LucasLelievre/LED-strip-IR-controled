/*
    Based on IRRemote library example "Minimal receiver"
    Copyright (C) 2020-2021  Armin Joachimsmeyer

    Arduino libraries needed : FastLED, IRremote

    Modifications need to the IRremote library :
    - src/TinyIRReveiber.hpp :
      - line 98     void stopDrawing();     (next to the declaration of the handleReceivedIRData callback function)
      - line 112    stopDrawing();          (at the begining of IRPinChangeInterruptHandler())
*/

#define DEBUG

#define PIN_LEDS 11
#define NUM_LEDS 60
//#include "LedStrip.h"
#include "FastLedStrip.h"
//LedStrip strip(PIN_LEDS, NUM_LEDS);
FastLedStrip fstrip(NUM_LEDS);

#define IR_INPUT_PIN    2
#include "TinyIRReceiver.hpp"

bool isDrawing = true;

void setup() {
  Serial.begin(115200);

  // Init the IR receiver
#if defined(__AVR_ATmega32U4__) || defined(SERIAL_USB) || defined(SERIAL_PORT_USBVIRTUAL) || defined(ARDUINO_attiny3217)
  delay(4000); // To be able to connect Serial monitor after reset or power up and before first print out. Do not wait for an attached Serial Monitor!
#endif
  initPCIInterruptForTinyReceiver();
  Serial.print("Ready to receive NEC IR signals at pin ");
  Serial.println(IR_INPUT_PIN);

  // Init the LEDs
  FastLED.addLeds<WS2812, PIN_LEDS, GRB>(fstrip.getLedStrip(), NUM_LEDS).setCorrection(TypicalLEDStrip);
  fstrip.clear();
  fstrip.draw();
}

void loop() {
  EVERY_N_MILLIS(32){
    if (fstrip.update() && isDrawing) {
      fstrip.draw();
    }
  }
}

/*
   This is the function is called if a complete command was received
*/
#if defined(ESP8266)
ICACHE_RAM_ATTR
#elif defined(ESP32)
IRAM_ATTR
#endif
void handleReceivedTinyIRData(uint16_t aAddress, uint8_t aCommand, bool isRepeat) {
  // Print only very short output, since we are in an interrupt context
  // and do not want to miss the next interrupts of the repeats coming soon
#ifdef DEBUG
  Serial.print(F("A=0x"));
  Serial.print(aAddress, HEX);
  Serial.print(F(" C=0x"));
  Serial.print(aCommand, HEX);
  Serial.print(F(" R="));
  Serial.print(isRepeat);
  Serial.println();
#endif
  fstrip.setCommand(aCommand, isRepeat);
  isDrawing = true;
}

void stopDrawing() {
  isDrawing = false;
}