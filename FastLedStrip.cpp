#include "FastLedStrip.h"

FastLedStrip::FastLedStrip(int num) {
  this->num_led = num;
  this->ledStrip = new CRGB[this->num_led];

  this->isOn = false;
  this->isRepeat = false;
  this->init();
}
void FastLedStrip::init() {
  this->isColdWhite = true;

  this->rainbowMove = false;
  this->rainbowHue = 0;
  //this->animHSV.setHSV(255, 255, 255);

  this->speed = 3;
  this->lum = 255;
  FastLED.setBrightness(this->lum);
}
////////////////////////////////////////////////////////////// Getters
CRGB* FastLedStrip::getLedStrip() {
  return this->ledStrip;
}
////////////////////////////////////////////////////////////// Setters
void FastLedStrip::setCommand(uint8_t command, bool repeat) {
  if (!repeat) this->prevCommand = this->command;
  this->command = command;
  this->isRepeat = repeat;
}
void FastLedStrip::setPrevCommand() {
  this->command = this->prevCommand;
  this->isRepeat = true;
}

bool FastLedStrip::power() {
  this->isOn = !this->isOn;
 
  Serial.print("power : ");
  Serial.println(this->isOn ? "on" : "off");

  if (this->isOn) {
    this->init();
    return this->setWhite();
  }
  else {
    return this->clear();
  }
}

bool FastLedStrip::setSpeed(bool isUp) {
  this->speed = constrain(this->speed - (isUp ? 1 : -1), 1, 20);
  this->setPrevCommand();
  Serial.print("Speed : ");
  Serial.println(this->speed);
  return true;
}
bool FastLedStrip::setLum(bool isUp) {
  this->lum = constrain(this->lum + (isUp ? 16 : -16), 0, 255);
  FastLED.setBrightness(this->lum);
  this->setPrevCommand();
  Serial.print("lum : ");
  Serial.println(this->lum);
  return true;
}

bool FastLedStrip::setColour(CRGB colour) {
  if (this->isRepeat) return false;
  this->isRepeat = true;
  for (int i = 0; i < this->num_led; i++) {
    //this->ledStrip[i].setColorCode(colour);
    this->ledStrip[i] = colour;
  }
  return true;
}
bool FastLedStrip::setColour(int r, int g, int b) {
  if (this->isRepeat) return false;
  this->isRepeat = true;
  for (int i = 0; i < this->num_led; i++) {
    this->ledStrip[i].setRGB(r, g, b);
  }
  return true;
}

bool FastLedStrip::setWhite() {
  if (this->isRepeat) return false;
  this->isColdWhite = !this->isColdWhite;

  Serial.print("white : ");
  Serial.println(this->isColdWhite ? "cold" : "warm");

  return this->isColdWhite ?
         this->setColour(Tungsten40W) :
         this->setColour(Candle);
}

bool FastLedStrip::setRainbow() {
  if (!this->isRepeat) {
    this->rainbowMove = !this->rainbowMove;
    Serial.print("Rainbow moving : ");
    Serial.println(this->rainbowMove ? "1" : "0");
    this->isRepeat = true;
  }
  this->rainbowHue = this->rainbowHue - this->speed;
  uint8_t tmp = this->rainbowHue;
  for (int i = 0; i < this->num_led; i++) {
    this->ledStrip[i].setHue(tmp);
    tmp += 10*this->rainbowMove;
  }
  return true;
}

bool FastLedStrip::setFire() {
  if (!this->isRepeat) {
    this->isRepeat = true;
    this->heat = new byte[this->num_led] {0};
    Serial.println("Fire : on");
  }

  bool gReverseDirection = false;
  int SPARKING = 200;
  int COOLING = 80;

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < this->num_led; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / this->num_led) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= this->num_led - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < this->num_led; j++) {
      int pixelnumber = gReverseDirection ? (this->num_led-1) - j : j;
      this->ledStrip[pixelnumber] = HeatColor(heat[j]);
    }

  return true;
}
////////////////////////////////////////////////////////////// Update
bool FastLedStrip::update() {
  if (this->command == 0x45 && !this->isRepeat) {
    return this->power();
  }

  if (!isOn) return false;

  switch (this->command) {
    case 0x46: // mode
      break;
    case 0x47: // mute
      break;
    case 0x44: // play/pause
      return this->setRainbow();
      break;
    case 0x40: // prev
      return this->setSpeed(false);
      break;
    case 0x43: // next
      return this->setSpeed(true);
      break;
    case 0x7: // EQ
      return this->setFire();
      break;
    case 0x15: // -
      return this->setLum(false);
      break;
    case 0x9: // +
      return this->setLum(true);
      break;
    case 0x19: // random
      /*CRGB random = static_cast<CRGB::HTMLColorCode>(rand() % 149);
      Serial.println(random);
      Serial.println(random, HEX);
      Serial.println();
      return this->setColour(random);*/
      CRGB c;
      c.setRGB(random(0, 256), random(0, 256), random(0, 256));
      //c.maximizeBrightness();
      return this->setColour(c);
      break;
    case 0xD: // u/sd warm/cold white
      return this->setWhite();
      break;
    case 0x16: // 0 red
      return this->setColour(255, 0, 0);
      break;
    case 0xc: // 1 rose
      return this->setColour(255, 0, 255);
      break;
    case 0x18: // 2 violet
      return this->setColour(128, 0, 255);
      break;
    case 0x5e: // 3 blue
      return this->setColour(0, 0, 255);
      break;
    case 0x8: // 4 azure
      return this->setColour(0, 128, 255);
      break;
    case 0x1c: // 5 cyan
      return this->setColour(0, 255, 255);
      break;
    case 0x5a: // 6 spring green
      return this->setColour(0, 255, 128);
      break;
    case 0x42: // 7 green
      return this->setColour(0, 255, 0);
      break;
    case 0x52: // 8 yellow
      return this->setColour(255, 255, 0);
      break;
    case 0x4a: // 9 orange
      return this->setColour(255, 128, 0);
      break;
    default:
      //this->draw();
      //Serial.println("Unknown button");
      return false;
  }
  return false;
}
////////////////////////////////////////////////////////////// Draw
void FastLedStrip::draw() {
  FastLED.show();
}

bool FastLedStrip::clear() {
  FastLED.clear();
  this->isRepeat = true;
  return true;
}
