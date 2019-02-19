/**@file parameters.h
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
#ifndef PARAMETERS_H
#define PARAMETERS_H

class Parameters {

  public:
    Parameters();

    /** @addtogroup Parameters
     *  User-settable Parameters (MQTT Broker address, preferences, ...)
     * @{ */
    String deviceName; /**< User-given name for the device */
    String mqttBrokerAddr; /**< DNS name of MQTT broker to connect to */
    uint16_t mqttPort; /**< TCP port to use for MQTT Broker connection */
    String mqttUser; /**< username for connecting to MQTT Broker */
    String mqttPwd; /**< password for connecting to MQTT Broker */
    String mqttTopic; /**< topic name for subscribing and publishing to */
    uint16_t sleepTime; /**< Time (in seconds) to sleep between MQTT polling */
    /** @} */

    /**
     * parametersInit initializes the parameters module, handles WiFiManager setup
     */
    int init();

  private:
    void initPrefPars(); /**< Set up parameters module */
    void getPrefPars(); /**< retrieves all parameters from EEPROM (or sets to default if not available) */
    void putPrefPars(); /**< stores all parameters in EEPROM */
};

#endif /* #ifndef PARAMETERS_H */
