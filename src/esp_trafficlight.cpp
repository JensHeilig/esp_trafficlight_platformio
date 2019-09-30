#include <Arduino.h>

#include "config.h"
#include "parameters.h"
#include "mqtt.h"
#include "iLedTrafficLight.h"
#include "pl9823TrafficLight.h"

Parameters par;
pl9823TrafficLight light;


RTC_DATA_ATTR int bootCount = 0; ///< counts number of boots since last Power-On (stored in RTC memory, persistent over Deep Sleep)
unsigned int twait = 0; ///< measure time before going to sleep
unsigned int sleepflags = 0; ///< bitfield in which all components may signal that they are ready to go to sleep

/*
 * Print the reason by which ESP32
 * has been woken from sleep
*/
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

/**
 * Put ESP32 to sleep
 */
void goToSleep()
{
  /*
  First we configure the wake up source
  We set our ESP32 to wake up every par.sleepTime seconds
  */
  esp_sleep_enable_timer_wakeup(par.sleepTime * uS_TO_SECS);
  Serial.println("Setup ESP32 to sleep for " + String(par.sleepTime) + " Seconds");
  Serial.flush();
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  Serial.begin(115200);
  while(!Serial) {
    delay(100);
    Serial.print(". ");
  }

  if (0 != par.init()) {
    // Could not connect to WiFi ==> go to sleep
    goToSleep();
  }

  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number (since last Power-Off): " + String(bootCount));

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  mqttSetup();

  twait = millis();
}

void loop() {
  static unsigned long oldtime = 0;
  static unsigned long longpress = 0;
  static unsigned int flag_CfgPortalStarted = 0;

  mqttLoop();

  if ((SLEEP_READY_MASK & sleepflags) == SLEEP_READY_MASK) {
    // got to sleep if all flags are ready unless button is pressed
    if (digitalRead(BUTTON)) {
      goToSleep();
    }
  }

  // delay sleep if button is pressed
  if (! digitalRead(BUTTON))
  {
    twait = millis();
    oldtime = (millis() - twait) / MS_TO_SECS;

    // detect long press
    if (longpress)
    {
      if (((millis() - longpress) / MS_TO_SECS) > LONG_PRESS_TIME)
      {
        Serial.println("Long press Event dected: Erasing WLAN config, restarting in WLAN AP mode");
        if (!flag_CfgPortalStarted)
        {
          par.resetWifiConfig();
        }
        flag_CfgPortalStarted = 1;
        longpress=0;
      }
    } else
    {
      longpress = millis();
    }

  } else
  {
    longpress=0;
  }


  if ((unsigned int)((millis() - twait) / MS_TO_SECS) >= oldtime + 5)
  {
    oldtime = (millis() - twait) / MS_TO_SECS;
    Serial.print ("Going to sleep in ");
    Serial.print (TIME_UNTIL_SLEEP - (millis() - twait) / MS_TO_SECS);
    Serial.println(" seconds");
  }

  if (millis() - twait > (TIME_UNTIL_SLEEP * MS_TO_SECS))
  {
    // Go to sleep after TIME_UNTIL_SLEEP seconds if not inhibited via mqtt
    if (SLEEP_MQTT_NOT_INHIBITED == (sleepflags & SLEEP_MQTT_NOT_INHIBITED))
    {
      goToSleep();
    } else {
      Serial.println("Sleep inhibited. Not sleeping.");
      if (!flag_CfgPortalStarted)
      {
        Serial.println("Starting ConfigPortal...");
        par.startConfigPortal();
      }
      flag_CfgPortalStarted = 1;
      twait = millis();
      oldtime = (millis() - twait) / MS_TO_SECS;
    }
  }
  delay(1);
}
