/**************************          MAZERUNNERS        **************************
 **************************          Created by:        **************************
 ************************** Tijmen Leemkuil    | 519136 **************************
 ************************** Sjoerd Quast       | 527411 **************************
 ************************** Pim van Haastrecht | 533136 **************************
 ************************** Kolinda Mastenbroek| 517186 **************************
 ************************** Release date     29-04-2016 **************************/
 
#ifndef DEMONS_H
#define DEMONS_H

#include <tuple>
#include "CommandProcessor.h"
#include "Demon.hpp"
#include <map>
#include <iostream>
#include <string>
#include <vector>

class Demons : public CommandProcessor
{
public:
Demons(const std::string& appname,
       const std::string& clientname,
       const std::string& host,
       std::vector<mosqpp::mosquittopp*>* clients,
       int port);

~Demons();

void manageDemons(const std::vector<std::string>& args);
void fieldGone(const std::vector<std::string>& commandParameters);

protected:

private:
std::vector<Demon*> vDemon;
std::vector<mosqpp::mosquittopp*>* vClients;

std::string sAppname;
std::string sClientname;
std::string sHost;
int iPort;
};

#endif