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
#define TIME_UNTIL_SLEEP  10     /**< Time ESP32 will stay awake until sleep (in seconds) */
/** @} */

/** @addtogroup Misc
 *  Miscellaneous Configuration items (used internally by Software)
 * @{ */
#define uS_TO_SECS 1000000  /**< Conversion factor for microseconds to seconds */
#define MS_TO_SECS 1000     /**< Conversion factor for milliseconds to seconds */

#define MQTT_MAX_PAYLOAD_LEN 70  /**< maximum length (in bytes)  for payload received from mqtt */
#define MQTT_MAX_TOPIC_LEN 50  /**< maximum length (in bytes)  for mqtt topic*/
#define MQTT_STATUS_INTERVAL 10000 /**< interval time in milliseconds after which the device status will be sent over MQTT */

#define LONG_PRESS_TIME 3   /**< time (in seconds) to detect a long button press event */
/** @} */

/** @addtogroup flagsdone Flags for sending device to sleep
 *  Flags for sending the device to sleep as fast as possible
 * @{ */
extern unsigned int sleepflags;     /**< bitfield in which all components may signal that they are ready to go to sleep */
#define SLEEP_MQTT_RED 1            /**< mqtt message for setting red LED has arrived */
#define SLEEP_MQTT_YELLOW  2        /**< mqtt message for setting yellow LED has arrived */
#define SLEEP_MQTT_GREEN 4          /**< mqtt message for setting green LED has arrived */
#define SLEEP_MQTT_STATUS_SENT 8    /**< mqtt status messages have been sent */
#define SLEEP_MQTT_NOT_INHIBITED 16 /**< mqtt topic "sleepInhibit" has _not_ received "ON" */

#define SLEEP_READY_MASK (SLEEP_MQTT_RED | SLEEP_MQTT_YELLOW | SLEEP_MQTT_GREEN | SLEEP_MQTT_STATUS_SENT | SLEEP_MQTT_NOT_INHIBITED) /**< all required flags before going to sleep */
/** @} */

/** @addtogroup ledcolor LED Color definition
 *  Define RGB values for the fixed colors "Red", "Green" and "Yellow" (the traffic light colors)
 * @{ */
#define RGB_RED    255,0,0 /**< The RGB values for "Red" */
#define RGB_YELLOW 255,255,0 /**< The RGB values for "Yellow" */
#define RGB_GREEN  0,255,0 /**< The RGB values for "Green" */
/** @} */


#endif /* ifndef CONFIG_H */
