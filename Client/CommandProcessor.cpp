/**************************          MAZERUNNERS        **************************
 **************************          Created by:        **************************
 ************************** Tijmen Leemkuil    | 519136 **************************
 ************************** Sjoerd Quast       | 527411 **************************
 ************************** Pim van Haastrecht | 533136 **************************
 ************************** Kolinda Mastenbroek| 517186 **************************
 ************************** Release date     29-04-2016 **************************/
 
#include "CommandProcessor.h"
#include "Config.h"
#include "Tokenizer.h"
#include "Topic.h"
#include <iostream>

#include <string>
#include <time.h>
#include <chrono>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

CommandProcessor::CommandProcessor(const std::string& appname,
                                   const std::string& clientname,
                                   const std::string& host,
                                   int port)
    :  mosqpp::mosquittopp{(BBB_ID + appname + clientname).c_str()}
    , _appname{appname}
    , _clientname{clientname}
    , control{MQTT_TOPIC_ROOT}
    , admin{MQTT_TOPIC_ROOT}
    , posPlayer{MQTT_TOPIC_ROOT}
    , posDemon{MQTT_TOPIC_ROOT}
    , playField{MQTT_TOPIC_ROOT}
    , keyHit{MQTT_TOPIC_ROOT}
    , _commands{}
{
    control.add(_appname).add("control");
    admin.add(_appname).add("admin");
    posPlayer.add(_appname).add("posPlayer");
    posDemon.add(_appname).add("posDemon");
    playField.add(_appname).add("playField");
    keyHit.add(_appname).add("keyHit");

    // Get PID in string style
    std::string str_pid = std::to_string(getpid());
    // Set start time
    std::string str_time = std::to_string(std::chrono::system_clock::now().time_since_epoch().count() % 10000);

    My_Client_id = str_pid + str_time;
    // Set last will before connecting
    setWill(admin, "admin logOut Player " + My_Client_id);
    connect(host.c_str(), port, MQTT_KEEP_ALIVE);

    subscribe(nullptr, control.c_str());

    registerCommand("ping", bind(&CommandProcessor::ping, this, std::placeholders::_1));
}

CommandProcessor::~CommandProcessor()
{
    disconnect();
}

void CommandProcessor::on_message(const mosquitto_message* message)
{
    executeCommand((char*)message->payload);
}

void CommandProcessor::registerCommand(const std::string& command,
                                       commandfunction cfunction)
{
    if (!commandIsRegistered(command))
    {
        _commands[command] = cfunction;
        publishInfo("commando: '" + command + "' registered");
    }
    else
    {
        publishWarning("commando: '" + command + "' already registered");
    }
}

void CommandProcessor::executeCommand(const std::string& command)
{
    std::vector<std::string> commandpars {split(command)};

    if (commandIsRegistered(commandpars[0]))
    {
        _commands[commandpars[0]](std::vector<std::string>(std::begin(commandpars) + 1,
                                                           std::end(commandpars)));
    }
    else
    {
        publishError("commando: '" + commandpars[0] + "' is unknown");
    }
}

void CommandProcessor::executeCommands(const std::vector<std::string>& commands)
{
    for (const auto & command : commands)
    {
        executeCommand(command);
    }
}

bool CommandProcessor::commandIsRegistered(const std::string& command) const
{
    return _commands.find(command) != std::end(_commands);
}

void CommandProcessor::publishReturn(const std::string& message)
{
    Topic topic {control};

    topic.add("return");
    publish(nullptr, topic.c_str(), message.size(), message.c_str());
}

void CommandProcessor::publishInfo(const std::string& message)
{
    Topic topic {control};

    topic.add("info");
    publish(nullptr, topic.c_str(), message.size(), message.c_str());
}

void CommandProcessor::publishWarning(const std::string& message)
{
    Topic topic {control};

    topic.add("warning");
    publish(nullptr, topic.c_str(), message.size(), message.c_str());
}

void CommandProcessor::publishError(const std::string& message)
{
    Topic topic {control};

    topic.add("error");
    publish(nullptr, topic.c_str(), message.size(), message.c_str());
}

void CommandProcessor::publishTo(Topic subtopic, const std::string& message)
{
    publish(nullptr, subtopic.c_str(), message.size(), message.c_str());
}

void CommandProcessor::subscribeTo(Topic subtopic)
{
    subscribe(nullptr, subtopic.c_str());
}

void CommandProcessor::ping(const std::vector<std::string> commandParameters)
{
    publishInfo("CommandProcessor pings back");
}

void CommandProcessor::setWill(Topic subtopic, const std::string& message)
{
    will_set(subtopic.c_str(), message.size(), message.c_str(), MQTT_QoS_0);
}