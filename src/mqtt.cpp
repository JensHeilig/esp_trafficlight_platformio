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
 * subscribe:.../set/red ON/OFF
 * subscribe:.../set/red/pwm {PWM-Wert 0-100% Default: 0}
 * subscribe:.../set/yellow ON/OFF
 * subscribe:.../set/yellow/pwm {PWM-Wert 0-100% Default: 0}
 * subscribe:.../set/green ON/OFF
 * subscribe:.../set/green/pwm {PWM-Wert 0-100% Default: 0}
 * subscribe:.../set/sleepinterval {Zeit in Sekunden die das Gerät schläft, bevor es sich wieder aufweckt. Default: 20 Sekunden}
 * subscribe:.../set/sleepInhibit ON/OFF to prevent device from going to sleep (set this as retained to ON to keep device awake)
 * publish: .../status/Devicename {Gerätename}
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
#include <ArduinoJson.hpp>

#include "config.h"
#include "mqtt.h"
#include "parameters.h"
#include "pl9823TrafficLight.h"

#define DBG_MQTT  ///< enables debug output on serial port

extern int bootCount;
extern pl9823TrafficLight light;

typedef enum {
  NONE_TYPE = 0,
  ONOFF_TYPE,
  INT_TYPE,
  STRING_TYPE,
  JSON_TYPE,
  JSON_ARRAY_TYPE
} TYPE_t;

typedef struct {
  char const * topic; ///< trailing part of topic (unique)
  TYPE_t type; ///< kind of item (int, on/off, ...)
  void (*callback) (char* message);
} SUBSCRIBE_DESCRIPTION_t;

void cb_setred(char* message);
void cb_setgreen(char* message);
void cb_setyellow(char* message);
void cb_setSleepInterval(char* message);
void cb_setredpwm(char* msg);
void cb_setyellowpwm(char* msg);
void cb_setgreenpwm(char* msg);
void cb_setSleepInhibit (char* message);
void cb_setLight(char* message);
void cb_setRgb(char* message);

const SUBSCRIBE_DESCRIPTION_t subscribtion_topics[] =
{
  {"set/red", ONOFF_TYPE, cb_setred},
  {"set/red/pwm", INT_TYPE, cb_setredpwm},
  {"set/yellow", ONOFF_TYPE, cb_setyellow},
  {"set/yellow/pwm", INT_TYPE, cb_setyellowpwm},
  {"set/green", ONOFF_TYPE, cb_setgreen},
  {"set/green/pwm", INT_TYPE, cb_setgreenpwm},
  {"set/sleepinterval", INT_TYPE, cb_setSleepInterval},
  {"set/sleepInhibit", ONOFF_TYPE, cb_setSleepInhibit},
  {"set/light", STRING_TYPE, cb_setLight},
  {"set/rgb", JSON_TYPE, cb_setRgb},
  {"",NONE_TYPE,nullptr}, ///< terminating item!
};

WiFiClient wifiClient;
PubSubClient mqttclient(wifiClient);

long lastMsg = 0;
char msg[MQTT_MAX_PAYLOAD_LEN+1]; ///< holds payload received from MQTT Broker
int value = 0;

extern Parameters par;

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  if (length > MQTT_MAX_PAYLOAD_LEN) {
    length = MQTT_MAX_PAYLOAD_LEN;
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

  for (int i=0; subscribtion_topics[i].topic[0] != 0; i++) {
    if (String(topic) == par.mqttTopic + "/" + String(subscribtion_topics[i].topic)) {
      Serial.println("Received payload for topic: " + String(subscribtion_topics[i].topic));
      subscribtion_topics[i].callback(msg);
    }
  }
}

void cb_setred (char* message) {
  if(strcasecmp(message,"on")==0) {
    // turn on red
    Serial.println("Red light ON");
    light.activateRed();
  }
  else if(strcasecmp(message,"off")==0) {
   // turn off red
    Serial.println("Red light OFF");
    light.turnOff();
  }
  sleepflags |= SLEEP_MQTT_RED;
}

void cb_setyellow(char* message) {
  if(strcasecmp(message,"on")==0) {
    // turn on red
    Serial.println("Yellow light ON");
    light.activateYellow();
  }
  else if(strcasecmp(message,"off")==0) {
   // turn off red
    Serial.println("Yellow light OFF");
    light.turnOff();
  }
  sleepflags |= SLEEP_MQTT_YELLOW;
}

void cb_setgreen  (char* message) {
  if(strcasecmp(message,"on")==0) {
    // turn on red
    Serial.println("Green light ON");
    light.activateGreen();
  }
  else if(strcasecmp(message,"off")==0) {
   // turn off red
    Serial.println("Green light OFF");
    light.turnOff();
  }
  sleepflags |= SLEEP_MQTT_GREEN;
}

void cb_setredpwm(char* msg) {
  int n = atoi(msg);
  // TODO set PWM output
  sleepflags |= SLEEP_MQTT_RED;
}

void cb_setyellowpwm(char* msg) {
  int n = atoi(msg);
  // TODO set PWM output
  sleepflags |= SLEEP_MQTT_YELLOW;
}

void cb_setgreenpwm(char* msg) {
  int n = atoi(msg);
  // TODO set PWM output
  sleepflags |= SLEEP_MQTT_GREEN;
}

void cb_setSleepInterval (char* message) {
  int interval = 0;
  interval = atoi(message);
  if ((interval > 0) && (interval != par.sleepTime)) {
    par.sleepTime = interval;
    par.putPrefPars();
  }
}

void cb_setSleepInhibit (char* message) {
  if(strcasecmp(message,"on")==0) {
    Serial.println("sleep inhibited");
    sleepflags &= ~SLEEP_MQTT_NOT_INHIBITED;
  }
  else if(strcasecmp(message,"off")==0) {
    Serial.println("sleep no longer inhibited");
    sleepflags |= SLEEP_MQTT_NOT_INHIBITED;
  }
}

void cb_setLight (char* message) {
  if(strcasecmp(message,"green")==0) {
    Serial.println("activate green");
    light.activateGreen();
  }
  else if(strcasecmp(message,"red")==0) {
    Serial.println("activate red");
    light.activateRed();
  }
  else if(strcasecmp(message,"yellow")==0) {
    Serial.println("activate yellow");
    light.activateYellow();
  }
    else if(strcasecmp(message,"off")==0) {
    Serial.println("turn off");
    light.turnOff();
  }
  else {
    Serial.printf("Unknown color: %s\n",message);
  }
}

void cb_setRgb (char* message) {
  //const size_t capacity = JSON_OBJECT_SIZE(3) + 30;
  //DynamicJsonBuffer jsonBuffer(capacity);
  ArduinoJson::DynamicJsonDocument jsonDoc(JSON_OBJECT_SIZE(3) + 30);
  
  if ( ArduinoJson::deserializeJson(jsonDoc, message).code() == ArduinoJson::DeserializationError::Ok) {
    int red = jsonDoc["red"]; // 255
    int green = jsonDoc["green"]; // 255
    int blue = jsonDoc["blue"]; // 255

    light.setRgb(red,green,blue);
  }
  else {
    Serial.printf("Failed to parse json from: %s\n", message);
  }

}

void mqttSetup() {
  mqttclient.setServer(par.mqttBrokerAddr.c_str(), par.mqttPort);
  mqttclient.setCallback(mqttCallback);
  sleepflags |= SLEEP_MQTT_NOT_INHIBITED;
}

void reconnect() {
  String topic = par.mqttTopic + "/set/#";
  // Loop until we're reconnected
  if (!mqttclient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttclient.connect(par.deviceName.c_str())) {
      Serial.println("connected");
      // Subscribe
      mqttclient.subscribe(topic.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttclient.state());
      // Wait some time to flush serial output
      delay(500);
    }
  }
}

void mqttLoop() {
  static unsigned int oldtime = 0;
  if (!mqttclient.connected()) {
    reconnect();
  } else {
    mqttclient.loop();
  }

  if ((0 == oldtime) || ((millis() - oldtime) > MQTT_STATUS_INTERVAL)) {
    oldtime = millis();
    String ipaddress = WiFi.localIP().toString();
    String rssi = String(WiFi.RSSI());
    String mac = String(WiFi.macAddress());
    mqttclient.publish(String(par.mqttTopic + "/status/Devicename").c_str(), par.deviceName.c_str());
    mqttclient.publish(String(par.mqttTopic + "/status/nwakeup").c_str(), String(bootCount).c_str());
    mqttclient.publish(String(par.mqttTopic + "/status/sleepinterval").c_str(), String(par.sleepTime).c_str());
    mqttclient.publish(String(par.mqttTopic + "/status/red").c_str(), String(10).c_str());
    mqttclient.publish(String(par.mqttTopic + "/status/yellow").c_str(), String(11).c_str());
    mqttclient.publish(String(par.mqttTopic + "/status/green").c_str(), String(12).c_str());
    mqttclient.publish(String(par.mqttTopic + "/status/ip").c_str(), ipaddress.c_str());
    mqttclient.publish(String(par.mqttTopic + "/status/RSSI").c_str(), rssi.c_str());
    mqttclient.publish(String(par.mqttTopic + "/status/MAC").c_str(), mac.c_str());
    sleepflags |= SLEEP_MQTT_STATUS_SENT;
  }
}
