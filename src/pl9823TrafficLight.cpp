#include "pl9823TrafficLight.h"

pl9823TrafficLight::pl9823TrafficLight():
_pixel(1,25),
_brightness(25),
_blue(0,0,_brightness),
_green(0,_brightness,0),
_yellow(_brightness,_brightness,0),
_red(_brightness,0,0),
_black(0,0,0),
_activeColor(&_yellow)
{
  _pixel.Begin();
}

pl9823TrafficLight::pl9823TrafficLight(int pin):
_pixel(pin,1),
_brightness(122),
_blue(0,0,_brightness),
_yellow(_brightness,_brightness,0),
_red(_brightness,0,0),
_black(0,0,0),
_activeColor(&_blue)
{
  _pixel.SetPixelColor(0,_blue);
  _pixel.Show();
}

bool pl9823TrafficLight::turnOff(){
  _activeColor = &_black;
  show();
}

bool pl9823TrafficLight::activateGreen(){
  _activeColor = &_green;
  _pixel.SetPixelColor(0,_green);
  _pixel.Show();
}

bool pl9823TrafficLight::activateRed(){
  _activeColor = &_red;
  show();
}

bool pl9823TrafficLight::activateYellow(){
  _activeColor = &_yellow;
  show();
}

bool pl9823TrafficLight::dim() {
  _yellow.R = _yellow.R - 10;
  _yellow.G = _yellow.G - 10;

  _red.R -= 10;

  _green.G -= 10;

  _blue.B -= 10;

  show();
}

bool pl9823TrafficLight::bright() {
  _yellow.R = _yellow.R + 10;
  _yellow.G = _yellow.G + 10;

  _red.R += 10;

  _green.G += 10;

  _blue.B += 10;

  show();
}

bool pl9823TrafficLight::show() {
  _pixel.SetPixelColor(0,*_activeColor);
  _pixel.Show();
}