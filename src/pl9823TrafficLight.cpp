#include "pl9823TrafficLight.h"

pl9823TrafficLight::pl9823TrafficLight():
_pixel(PIXEL_COUNT,PIXEL_PIN),
_brightness(DEFAULT_BRIGHTNESS),
_activeColor(_brightness,_brightness,_brightness)
{
  _pixel.Begin();
}

bool pl9823TrafficLight::turnOff(){
  _activeColor.R = 0;
  _activeColor.G = 0;
  _activeColor.B = 0;
  return show();
}

bool pl9823TrafficLight::activateGreen(){
  _activeColor.R = 0;
  _activeColor.G = _brightness;
  _activeColor.B = 0;
  return show();
}

bool pl9823TrafficLight::activateRed(){
  _activeColor.R = _brightness;
  _activeColor.G = 0;
  _activeColor.B = 0;
  return show();
}

bool pl9823TrafficLight::activateYellow(){
  _activeColor.R = _brightness;
  _activeColor.G = _brightness;
  _activeColor.B = 0;
  return show();
}

bool pl9823TrafficLight::setRgb(const int red, const int green, const int blue) {
  _activeColor.R = red;
  _activeColor.G = green;
  _activeColor.B = blue;
  return show();
}


bool pl9823TrafficLight::dim() {
  _activeColor.R -= 10;
  _activeColor.G -= 10;
  _activeColor.B -= 10;

  return show();
}

bool pl9823TrafficLight::bright() {
  _activeColor.R += 10;
  _activeColor.G += 10;
  _activeColor.B += 10;

  return show();
}

bool pl9823TrafficLight::show() {
  _pixel.SetPixelColor(0,_activeColor);
  _pixel.Show();

  return true;
}

/**
 * returns a JSON string with the RGB values in the provided buffer.
 * @param buf - buffer which will hold the returned string
 * @param sz - size of buffer in bytes
 */
bool pl9823TrafficLight::getRgbJson(char* buf, size_t sz)
{
  if (snprintf(buf, sz, "{\"red\":%d,\"green\":%d,\"blue\":%d}", _activeColor.R, _activeColor.G, _activeColor.B) <= sz)
  {
    return true;
  }
  else
  {
    return false;
  } 
}

/**
 * returns the currently active red color value
 */
 int pl9823TrafficLight::getRed()
 {
   return _activeColor.R;
 }

 /**
 * returns the currently active green color value
 */
 int pl9823TrafficLight::getGreen()
 {
   return _activeColor.G;
 }

 /**
 * returns the currently active blue color value
 */
 int pl9823TrafficLight::getBlue()
 {
   return _activeColor.B;
 }