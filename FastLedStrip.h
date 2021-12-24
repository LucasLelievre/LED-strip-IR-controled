#include <FastLED.h>

class FastLedStrip {
  private:
    int num_led;
    CRGB * ledStrip;

    uint8_t command;
    uint8_t prevCommand;
    bool isRepeat;

    bool isOn;
    bool isColdWhite;
    float speed;
    uint8_t lum;
    bool rainbowMove;
    float rainbowHue;
    byte* heat;


    void init();
    bool power();
    void setPrevCommand();
    bool setSpeed(bool isUp);
    bool setLum(bool isUp);
    bool setColour(int r, int g, int b);
    bool setColour(CRGB colour);
    bool setWhite();
    bool setRainbow();
    bool setFire();

  public:
    FastLedStrip(int num);

    void setCommand(uint8_t command, bool repeat);
    bool clear();

    CRGB * getLedStrip();

    bool update();
    void draw();
};
