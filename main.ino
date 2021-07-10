/*
    Based on IRRemote library example "Minimal receiver"
    Copyright (C) 2020-2021  Armin Joachimsmeyer
*/

#define DEBUG

#define PIN_LEDS 11
#define NUM_LEDS 60
//#include "LedStrip.h"
#include "FastLedStrip.h"
//LedStrip strip(PIN_LEDS, NUM_LEDS);
FastLedStrip fstrip(NUM_LEDS);

#define IR_INPUT_PIN    2
#include "TinyIRReceiver.cpp.h"
uint8_t command = 0;

unsigned long count;
unsigned long lastTime;

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

  count = millis();
  lastTime = millis();
}

void loop() {
 
  if (millis() - lastTime > 64){
    lastTime = millis();
    //count = millis();
    if (fstrip.update()) {
      fstrip.draw();
      //delay(15);
      //Serial.println(millis() - count);
    }
    
  }
  //delay(32);
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
  //strip.setCommand(aCommand, isRepeat);
  fstrip.setCommand(aCommand, isRepeat);
}
