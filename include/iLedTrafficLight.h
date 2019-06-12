#pragma once
/*

LED Interface definition

*/

class iLedTrafficLight {
  virtual bool activateGreen()=0;
  virtual bool activateYellow()=0;
  virtual bool activateRed()=0;
  virtual bool turnOff()=0;
};