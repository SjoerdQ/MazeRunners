/**************************          MAZERUNNERS        **************************
 **************************          Created by:        **************************
 ************************** Tijmen Leemkuil    | 519136 **************************
 ************************** Sjoerd Quast       | 527411 **************************
 ************************** Pim van Haastrecht | 533136 **************************
 ************************** Kolinda Mastenbroek| 517186 **************************
 ************************** Release date     29-04-2016 **************************/
 
#include "Demons.hpp"
#include "Demon.hpp"
#include <iostream>
#include <exception>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <typeinfo>
#include <atomic>

#define DEMON_P_PLAYER 2

extern std::atomic<bool> receivedSIGINT;

Demons::Demons(const std::string& appname,
               const std::string& clientname,
               const std::string& host,
               std::vector<mosqpp::mosquittopp*>* clients,
               int port)
    : CommandProcessor(appname, clientname, host, port),
    sAppname(appname), sClientname(clientname), sHost(host), iPort(port), vClients(clients)
{
    subscribeTo(posPlayer);
    subscribeTo(playField);

    registerCommand("posPlayer", bind(&Demons::manageDemons, this, std::placeholders::_1));
    registerCommand("logOut", bind(&Demons::fieldGone, this, std::placeholders::_1));

    std::string sIDdemon = "Demon0";

    for (int i = 0; i < DEMON_P_PLAYER; i++)
    {
        sIDdemon.replace(5, 1, std::to_string(i + 1));
        vDemon.push_back(new Demon(appname, sIDdemon.substr(0, 6), host, sIDdemon, port));
        vClients->push_back(static_cast<mosqpp::mosquittopp*>(vDemon[i]));
    }
}

Demons::~Demons()
{
    for (auto demon : vDemon)
    {
        delete demon;
        usleep(200);
    }
}

void Demons::manageDemons(const std::vector<std::string>& args)
{
    std::string sIDdemon = "Demon0";
    bool bDone = false;
    
    while (!bDone) {
        if ((vDemon.size() < ((args.size() / 5) * DEMON_P_PLAYER)) 
                && (vDemon.size() < 9))
        {
            int iLoc = vDemon.size();
            sIDdemon.replace(5, 1, std::to_string(iLoc + 1));
            vDemon.push_back(new Demon(sAppname, sIDdemon.substr(0, 6), sHost, sIDdemon, iPort));
            vClients->push_back(static_cast<mosqpp::mosquittopp*>(vDemon[iLoc]));
            std::cout << "more players" << std::endl;
        }
        else if ((vDemon.size() > ((args.size() / 5) * DEMON_P_PLAYER)) 
                && ((args.size() / 5) > 0))
        {
            std::string sMessage = "admin" + MQTT_SEPCHAR + "logOut";
            sMessage += MQTT_SEPCHAR + vDemon.at(vDemon.size()-1)->sID + MQTT_SEPCHAR + 
                        vDemon.at(vDemon.size()-1)->sID.substr(5, 1);
            publishTo(admin, sMessage);
            vDemon.erase(--vDemon.end());
            vClients->erase(--vClients->end());
            std::cout << "less players" << std::endl;
        }
        else
        {
            bDone = true;
        }
    }
}

void Demons::fieldGone(const std::vector<std::string>& commandParameters)
{
	std::cout<<std::endl<<std::endl<<"playfield is gone killing program" <<std::endl<<std::endl;
	usleep(2000);
	receivedSIGINT = true;
}
