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
 * subscribe: .../set/red/{PWM-Wert 0-100% Default: 0}
 * subscribe:.../set/yellow/{PWM-Wert 0-100% Default: 0}
 * subscribe:.../set/green/{PWM-Wert 0-100% Default: 0}
 * subscribe:.../set/interval/{Zeit in Sekunden die das Gerät schläft, bevor es sich wieder aufweckt. Default: 20 Sekunden}
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

#include "mqtt.h"
#include <PubSubClient.h>

