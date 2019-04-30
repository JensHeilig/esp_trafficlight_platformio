/**@file config.h
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
 * Compile-time configuration Items
 *
 <hr>
**************************************************************************/
#ifndef CONFIG_H
#define CONFIG_H

/** @addtogroup HardwareConfig
 *  Hardware Configuration (GPIO-Pins, etc)
 * @{ */
#define LED 2 /**< This GPIO drives the on-board LED */
#define BUTTON 0 /**< A button is attached to this GPIO */
/** @} */

/** @addtogroup GeneralConfig
 *  General Configuration (Behaviour of Software)
 * @{ */
#define TIME_TO_SLEEP  10        /**< Time ESP32 will stay in sleep (in seconds) */
#define TIME_UNTIL_SLEEP  100    /**< Time ESP32 will stay awake until sleep (in seconds) */
/** @} */

/** @addtogroup Misc
 *  Miscellaneous Configuration items (used internally by Software)
 * @{ */
#define uS_TO_SECS 1000000  /**< Conversion factor for microseconds to seconds */
#define MS_TO_SECS 1000     /**< Conversion factor for milliseconds to seconds */

#define MQTT_MAX_PAYLOAD_LEN 50  /**< maximum length (in bytes)  for payload received from mqtt */
#define MQTT_MAX_TOPIC_LEN 50  /**< maximum length (in bytes)  for mqtt topic*/
#define MQTT_STATUS_INTERVAL 10000 /**< interval time in milliseconds after which the device status will be sent over MQTT */

#define LONG_PRESS_TIME 3   /**< time (in seconds) to detect a long button press event */
/** @} */

#endif /* ifndef CONFIG_H */
