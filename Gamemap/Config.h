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
const std::string MQTT_BROKER {"iot.eclipse.org"}; //test.mosquitto.org
const std::string MQTT_TOPIC_ROOT  {"ESEiot"}; //mazerunner/" + BBB_ID

#endif
