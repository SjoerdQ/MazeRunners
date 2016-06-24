#include "CommandProcessor.h"
#include "Config.h"
#include "Tokenizer.h"
#include "Topic.h"
#include <iostream>

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
    , fullPlayField{MQTT_TOPIC_ROOT}
    , keyHit{MQTT_TOPIC_ROOT}
    , _commands{}
{
    control.add(_appname).add("control");
    admin.add(_appname).add("admin");
    posPlayer.add(_appname).add("posPlayer");
    posDemon.add(_appname).add("posDemon");
    playField.add(_appname).add("playField");
    fullPlayField.add(_appname).add("fullPlayField");
    keyHit.add(_appname).add("keyHit");

    //Last will must be set before connect!
    setWill(playField, "playField logOut playField");
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

void CommandProcessor::on_connect(int rc)
{
    if (rc == 0)
    {
        //subscribe(nullptr, control.c_str());
    }
}

void CommandProcessor::on_log(int level, const char* str)
{
    //std::cerr << "---- # log CommandProcessor " << level << ": " << str
    //<< std::endl;
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
    publishInfo("ik ping terug");
}

void CommandProcessor::setWill(Topic subtopic, const std::string& message)
{
    will_set(subtopic.c_str(), message.size(), message.c_str(), MQTT_QoS_0);
}