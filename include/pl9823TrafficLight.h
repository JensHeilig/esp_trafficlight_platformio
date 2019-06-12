#pragma once

#include "iLedTrafficLight.h"

#include "NeoPixelBus.h"

class pl9823TrafficLight : public iLedTrafficLight
{

public:
  pl9823TrafficLight();
  pl9823TrafficLight(int pin);
  virtual ~pl9823TrafficLight(){}

  virtual bool activateGreen();
  virtual bool activateYellow();
  virtual bool activateRed();
  virtual bool turnOff();
  virtual bool dim();
  virtual bool bright();
  virtual bool show();

private:
NeoPixelBus <NeoRgbFeature, NeoEsp32I2s0800KbpsMethod> _pixel;
int _brightness;
RgbColor _blue;
RgbColor _green;
RgbColor _yellow;
RgbColor _red;
RgbColor _black; 
RgbColor *_activeColor;


};