#pragma once

#include "iLedTrafficLight.h"

#include "NeoPixelBus.h"

#define PIXEL_COUNT 1
#define PIXEL_PIN 25 // pin 26for i2s1, pin 25 for i2s0
#define DEFAULT_BRIGHTNESS 30

class pl9823TrafficLight : public iLedTrafficLight
{

public:
  explicit pl9823TrafficLight();
  virtual ~pl9823TrafficLight(){}

  virtual bool activateGreen();
  virtual bool activateYellow();
  virtual bool activateRed();
  virtual bool setRgb(const int red, const int green, const int blue);
  virtual bool turnOff();
  virtual bool dim();
  virtual bool bright();
  virtual bool show();

private:
NeoPixelBus <NeoRgbFeature, NeoEsp32I2s0800KbpsMethod> _pixel;
int _brightness;
RgbColor _activeColor;


};