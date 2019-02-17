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
  // Open Preferences with "trafficlight" namespace. Each application module, library, etc
  // has to use a namespace name to prevent key name collisions. We will open storage in
  // RW-mode (second parameter has to be false).
  // Note: Namespace name is limited to 15 chars.
  preferences.begin("trafficlight", false);

  /*AsyncWiFiManagerParameter custom_friendly_name("name", "Friendly Name", _friendly_name, 40);
  AsyncWiFiManagerParameter custom_mqtt_server("server", "MQTT Server", _mqtt_server, 40);
  AsyncWiFiManagerParameter custom_mqtt_port("port", "MQTT Port", _mqtt_port, 6);
*/
  AsyncWiFiManager wifiManager(&server,&dns);
  if(!wifiManager.autoConnect(this->deviceName.c_str())) {
    Serial.println("failed to connect to SSID and hit timeout");
    // XXXX TODO go to deep sleep for some (long) time or return error
    delay(1000);
  }

  return 0;
}

/**
 *  Get all parameters from EEPROM, set default value if not available from EEPROM
 */
void Parameters::initPrefPars()
{
  mqttBrokerAddr = preferences.getString("BrokerAddr", "");
  deviceName = preferences.getString("DeviceName", "ESP-trafficlight");
  mqttPort = preferences.getUShort("BrokerPort", 1883);
  mqttUser = preferences.getString("Username", "");
  mqttPwd = preferences.getString("Password", "");
  mqttTopic = preferences.getString("Topic", "ESP-trafficlight");
  sleepTime = preferences.getUInt("sleepTime", 20);
}
