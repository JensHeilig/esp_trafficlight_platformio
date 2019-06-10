/**@file parameters.cpp
 <PRE>
 |======================== COPYRIGHT INFORMATION ========================|
 |                                                                       |
 |    COPYRIGHT  (C)  2019,  eSystems MTG GmbH                           |
 |                                                                       |
 |    Bahnhofstr. 100   D 73240 Wendlingen                               |
 |                                                                       |
 |=======================================================================|
 |                                                                       |
 |  MODULNAME           :  parameters                                    |
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
 * Handles User-settable parameters, user-configuration.
 * This includes WiFi parameters (SSID, password), MQTT
 * connection parameters, etc.
 *
 <hr>
**************************************************************************/

#include "config.h"
#include <Preferences.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#define WIFI_MANAGER_USE_ASYNC_WEB_SERVER
#include <ESPAsyncWiFiManager.h>

#include "parameters.h"

AsyncWebServer server(80); /**< Webserver object used for WiFiManager */
DNSServer dns; /**< DNSServer object used for WiFiManager */
Preferences preferences;  /**< persistent data stored in "EEPROM" */

/** global variable is needed for interaction with callback function */
bool shouldSaveConfig = false;

/** Callback function for AsyncWebserver
 * This needs to be a function, since callbacks cannot be made
 * to class members
 */
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

/**
 * Parameters Class Constructor
 */
Parameters::Parameters()
{
  ;
}

/**
 * initializes the parameters module, handles WiFiManager setup
 * @return Error value (0 if no error)
 */
int Parameters::init()
{
  preferences.begin("trafficlight", false);
  getPrefPars();

  shouldSaveConfig = false;

  String tmpPort = String(mqttPort);
  String tmpTime = String(sleepTime);

  AsyncWiFiManagerParameter custom_friendly_name("name", "Device Name", deviceName.c_str(), 40);
  AsyncWiFiManagerParameter custom_mqtt_server("server", "MQTT Server", mqttBrokerAddr.c_str(), 40);
  AsyncWiFiManagerParameter custom_mqtt_port("port", "MQTT Port", tmpPort.c_str(), 6);
  AsyncWiFiManagerParameter custom_mqtt_user("user", "MQTT Username (if required)", mqttUser.c_str(), 12);
  AsyncWiFiManagerParameter custom_mqtt_pwd("password", "MQTT Password (if required)", mqttPwd.c_str(), 12);
  AsyncWiFiManagerParameter custom_mqtt_topic("topic", "MQTT Topic", mqttTopic.c_str(), MQTT_MAX_TOPIC_LEN);
  AsyncWiFiManagerParameter custom_sleeptime("sleeptime", "Sleep Time bewtween light updates (seconds)", tmpTime.c_str(), 4);

  AsyncWiFiManager wifiManager(&server,&dns);
  //wifiManager.resetSettings();

  wifiManager.addParameter(&custom_friendly_name);
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_pwd);
  wifiManager.addParameter(&custom_mqtt_topic);
  wifiManager.addParameter(&custom_sleeptime);

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  // set max. time trying to connect to WiFi, otherwise we drain too much battery
  wifiManager.setConfigPortalTimeout(300000);

  if(!wifiManager.autoConnect(deviceName.c_str())) {
    Serial.println("failed to connect to SSID and hit timeout");
    delay(100);
    return -1;
  }
  Serial.println("connected to WiFi");
  Serial.println("local ip");
  Serial.println(WiFi.localIP());

  if (shouldSaveConfig) {
    Serial.println("Saving config parameters");
    deviceName = custom_friendly_name.getValue();
    mqttBrokerAddr = custom_mqtt_server.getValue();
    mqttPort = atoi(custom_mqtt_port.getValue());
    mqttUser = custom_mqtt_user.getValue();
    mqttPwd = custom_mqtt_pwd.getValue();
    mqttTopic = custom_mqtt_topic.getValue();
    sleepTime = atoi(custom_sleeptime.getValue());
    putPrefPars();
  }
  return 0;
}

/**
 *  Get all parameters from EEPROM, set default value if not available from EEPROM
 */
void Parameters::getPrefPars()
{
  mqttBrokerAddr = preferences.getString("BrokerAddr", "");
  deviceName = preferences.getString("DeviceName", "ESP-trafficlight");
  mqttPort = preferences.getUShort("BrokerPort", 1883);
  mqttUser = preferences.getString("Username", "");
  mqttPwd = preferences.getString("Password", "");
  mqttTopic = preferences.getString("Topic", "ESP-trafficlight");
  sleepTime = preferences.getUInt("sleepTime", 20);
  Serial.print("BrokerAddr: "); Serial.println(mqttBrokerAddr);
  Serial.print("DeviceName: "); Serial.println(deviceName);
  Serial.print("BrokerPort: "); Serial.println(mqttPort);
  Serial.print("Username: "); Serial.println(mqttPwd);
  Serial.print("Topic: "); Serial.println(mqttTopic);
  Serial.print("sleepTime: "); Serial.println(sleepTime);
}

/**
 *  Store all parameters in EEPROM
 */
void Parameters::putPrefPars()
{
  preferences.putString("BrokerAddr", mqttBrokerAddr);
  preferences.putString("DeviceName", deviceName);
  preferences.putUShort("BrokerPort", mqttPort);
  preferences.putString("Username", mqttUser);
  preferences.putString("Password", mqttPwd);
  preferences.putString("Topic", mqttTopic);
  preferences.putUInt("sleepTime", sleepTime);
}

/**
 * Start the Configuration Portal to allow user to change parameters
 */
void Parameters::startConfigPortal()
{
  WiFi.disconnect(true,true);
  WiFi.setAutoConnect(false);
  WiFi.setAutoReconnect(false);
  
  delay(1000);
  ESP.restart();
  delay(1000);
}