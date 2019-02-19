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

#include <Preferences.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#define WIFI_MANAGER_USE_ASYNC_WEB_SERVER 
#include <ESPAsyncWiFiManager.h>

#include "parameters.h"

AsyncWebServer server(80); /**< Webserver object used for WiFiManager */
DNSServer dns; /**< DNSServer object used for WiFiManager */
Preferences preferences;  /**< persistent data stored in "EEPROM" */

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

  String tmpPort = String(mqttPort);
  String tmpTime = String(sleepTime);
  
  AsyncWiFiManagerParameter custom_friendly_name("name", "Device Name", deviceName.c_str(), 40);
  AsyncWiFiManagerParameter custom_mqtt_server("server", "MQTT Server", mqttBrokerAddr.c_str(), 40);
  AsyncWiFiManagerParameter custom_mqtt_port("port", "MQTT Port", tmpPort.c_str(), 6);
  AsyncWiFiManagerParameter custom_mqtt_user("user", "MQTT Username (if required)", mqttUser.c_str(), 6);
  AsyncWiFiManagerParameter custom_mqtt_pwd("password", "MQTT Password (if required)", mqttPwd.c_str(), 6);
  AsyncWiFiManagerParameter custom_mqtt_topic("topic", "MQTT Topic", mqttTopic.c_str(), 60);
  AsyncWiFiManagerParameter custom_sleeptime("sleeptime", "Sleep Time bewtween light updates (seconds)", tmpTime.c_str(), 4);

  AsyncWiFiManager wifiManager(&server,&dns);
  if(!wifiManager.autoConnect(deviceName.c_str())) {
    Serial.println("failed to connect to SSID and hit timeout");
    // XXXX TODO go to deep sleep for some (long) time or return error
    delay(1000);
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