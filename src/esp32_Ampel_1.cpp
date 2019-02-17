#include <Arduino.h>

#include "config.h"
#include "parameters.h"
#include "mqtt.h"

Parameters par;


RTC_DATA_ATTR int bootCount = 0; ///< counts number of boots since last Power-On (stored in RTC memory, persistent over Deep Sleep)
unsigned int twait = 0; ///< measure time before going to sleep

/*
Method to print the reason by which ESP32
has been awaken from sleep
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

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  
  Serial.begin(115200);

  par.init();

  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number (since last Power-Off): " + String(bootCount));

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  /*
  First we configure the wake up source
  We set our ESP32 to wake up every 5 seconds
  */
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for " + String(TIME_TO_SLEEP) +
  " Seconds");

  twait = millis();
}

void loop() {
  
  // delay sleep if button is pressed
  if (! digitalRead(BUTTON))
  {
    twait = millis();
  }

  if (!((millis() - twait) % 1000))
  {
    Serial.print ("Going to sleep in ");
    Serial.print (TIME_UNTIL_SLEEP - (millis() - twait) / 1000);
    Serial.println(" seconds");
  }

  if (millis() - twait > (TIME_UNTIL_SLEEP*1000))
  {
    // Go to sleep after TIME_UNTIL_SLEEP seconds
    Serial.println("Going to sleep now");
    Serial.flush(); 
    esp_deep_sleep_start();
    Serial.println("This will never be printed");
  }
}
