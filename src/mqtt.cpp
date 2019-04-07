/**@file mqtt.cpp
 <PRE>
 |======================== COPYRIGHT INFORMATION ========================|
 |                                                                       |
 |    COPYRIGHT  (C)  2019,  eSystems MTG GmbH                           |
 |                                                                       |
 |    Bahnhofstr. 100   D 73240 Wendlingen                               |
 |                                                                       |
 |=======================================================================|
 |                                                                       |
 |  MODULNAME           :  mqtt                                          |
 |                                                                       |
 |  PROJEKTTITEL        :  ESP32-Ampel                                   |
 |                                                                       |
 |  ZIELSYSTEM          :  ESP32 Arduino                                 |
 |                                                                       |
 |  PROZESSOR           :  Espressif ESP32                               |
 |                                                                       |
 |  COMPILER/IDE        :  Espressif IDF / PlatformIO (gcc)              |
 |                                                                       |
 |  BETRIEBSSYSTEM      :  Arduino                                       |
 |                                                                       |
 |=======================================================================|
 </PRE>
 *
 * @date   2019-02-16
 * @author Jens Heilig
 * @brief  
 *
 * Handles all things MQTT.
 * Connect to broker, subscribe to topics, handle callbacks,
 * publish to topics
 * 
 * verbindet sich automatisch zum konfigurierten Broker
 * subscribed zu einem konfigurierbaren Topic. Default: eSystems/Ampeln/${Gerätename oder Nummer}
 * Einstellbar ist der komplette String, "%s" im String wird automatisch durch den Gerätenamen ersetzt
 * subtopics:
 * subscribe:.../set/red {PWM-Wert 0-100% Default: 0}
 * subscribe:.../set/yellow {PWM-Wert 0-100% Default: 0}
 * subscribe:.../set/green {PWM-Wert 0-100% Default: 0}
 * subscribe:.../set/interval {Zeit in Sekunden die das Gerät schläft, bevor es sich wieder aufweckt. Default: 20 Sekunden}
 * publish: .../status/Name {Gerätename}
 * publish: .../status/nwakeup {wie oft ist das Gerät schon aufgewacht}
 * publish: .../status/interval {Welche Sleep-Time ist gerade eingestellt}
 * publish: .../status/red {Welches PWM ist gerade für rot eingestellt}
 * publish: .../status/yellow {Welches PWM ist gerade für gelb eingestellt}
 * publish: .../status/green {Welches PWM ist gerade für grün eingestellt}
 * publish: .../status/ip {Welche IP Adresse haben wir gerade}
 * publish: .../status/RSSI {signal strength}
 * publish: .../status/MAC {MAC address}
 <hr>
**************************************************************************/

#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"
#include "mqtt.h"
#include "parameters.h"

#define DBG_MQTT ///< enables debug output on serial port

typedef enum {
  ONOFF_TYPE,
  INT_TYPE
} TYPE_t;

typedef struct {
  char const * topic; ///< trailing part of topic (unique)
  TYPE_t type; ///< kind of item (int, on/off, ...)
  void (*callback) (char* message);
} SUBSCRIBE_DESCRIPTION_t;

void cb_setred(char* message);
void cb_setgreen(char* message);
void cb_setyellow(char* message);
void cb_setinterval(char* message);

const SUBSCRIBE_DESCRIPTION_t subscribtion_topics[] =
{
  {"set/red", ONOFF_TYPE, cb_setred},
  {"set/yellow", ONOFF_TYPE, cb_setyellow},
  {"set/green", ONOFF_TYPE, cb_setgreen},
  {"set/interval", INT_TYPE, cb_setinterval},
};

WiFiClient wifiClient;
PubSubClient mqttclient(wifiClient);
long lastMsg = 0;
char msg[MAX_PAYLOAD_LEN+1]; ///< holds payload received from MQTT Broker
int value = 0;

extern Parameters par;

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  if (length > MAX_PAYLOAD_LEN) {
    length = MAX_PAYLOAD_LEN;
  }

  for (int i = 0; i < length; i++) {
      msg[i] = (char)payload[i];
  }
  msg[length] = '\0';

#ifdef DBG_MQTT
  Serial.print("Received message [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(msg);
#endif
}

void cb_setred (char* message) {
  if(strcmp(message,"on")==0){
    ; // turn on red
  }
  else if(strcmp(message,"off")==0){
    ; // turn off red
  }
}

void cb_setyellow(char* message) {
  ;
}

void cb_setgreen  (char* message) {
  ;
}

void cb_setinterval (char* message) {
  int interval = 0;
  interval = atoi(message);
  if ((interval > 0) && (interval != par.sleepTime)) {
    par.sleepTime = interval;
    par.putPrefPars();
  }
}

void mqttSetup() {
  mqttclient.setServer(par.mqttBrokerAddr.c_str(), par.mqttPort);
  mqttclient.setCallback(mqttCallback);
}

