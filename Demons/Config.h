/**************************          MAZERUNNERS        **************************
 **************************          Created by:        **************************
 ************************** Tijmen Leemkuil    | 519136 **************************
 ************************** Sjoerd Quast       | 527411 **************************
 ************************** Pim van Haastrecht | 533136 **************************
 ************************** Kolinda Mastenbroek| 517186 **************************
 ************************** Release date     29-04-2016 **************************/
 
#ifndef CONFIG_H
#define CONFIG_H

#include <string>

const char* getBBBid();

const std::string BBB_ID {getBBBid()};

const int MQTT_KEEP_ALIVE {60}; 
const int MQTT_QoS_0 {0};
const int MQTT_QoS_1 {1};
const int MQTT_QoS_2 {2};
const bool MQTT_RETAIN_OFF {false};
const bool MQTT_RETAIN_ON {true};
const std::string MQTT_BROKER {"iot.eclipse.org"};
const std::string MQTT_TOPIC_ROOT  {"ESEiot"};

#endif
