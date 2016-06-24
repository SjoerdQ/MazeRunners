/**************************          MAZERUNNERS        **************************
 **************************          Created by:        **************************
 ************************** Tijmen Leemkuil    | 519136 **************************
 ************************** Sjoerd Quast       | 527411 **************************
 ************************** Pim van Haastrecht | 533136 **************************
 ************************** Kolinda Mastenbroek| 517186 **************************
 ************************** Release date     29-04-2016 **************************/
 
#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include "Topic.h"
#include "mosquittopp.h"
#include <functional>
#include <map>
#include <string>
#include <vector>

using commandfunction =
std::function<void(const std::vector<std::string>& commmandParameters)>;

/// CommandProcessor is able to connect a command name to a related function.
/// Topic (root) syntax: "MQTT_TOPIC_ROOT / <appname> / <clientname> / command"
/// The related command is part of the message. Every command can have >=0
/// parameters. The receiving client is responsible for checking the number
/// of parameters and the conversion of the strings to the correct type
/// (int, double, etc.).
class CommandProcessor: public mosqpp::mosquittopp
{
public:
  CommandProcessor(const std::string& appname,
                   const std::string& clientname,
                   const std::string& host,
                   int port);
  CommandProcessor(const CommandProcessor& other) = default;
  virtual ~CommandProcessor();
  void registerCommand(const std::string& command, commandfunction cfunction);
  void executeCommand(const std::string& command);
  void executeCommands(const std::vector<std::string>& commands);
  bool commandIsRegistered(const std::string& command) const;
  void subscribeTo(Topic suptopic);

protected:
  std::string _appname;
  std::string _clientname;
  Topic control;
  Topic admin;
  Topic posPlayer;
  Topic posDemon;
  Topic keyHit;
  Topic playField;
  virtual void on_message(const mosquitto_message* message) override;
  //virtual void on_disconnect(int rc) override {}
  //virtual void on_error() override;
  void publishReturn(const std::string& message);
  void publishInfo(const std::string& message);
  void publishWarning(const std::string& message);
  void publishError(const std::string& message);
  void publishTo(Topic subtopic, const std::string& message);
  void ping(const std::vector<std::string> commandParameters); 
  void setWill(Topic subtopic, const std::string& message);
  
private:
  std::map<std::string, commandfunction> _commands; 
};

#endif // COMMANDPROCCESSOR_H

