/**************************          MAZERUNNERS        **************************
 **************************          Created by:        **************************
 ************************** Tijmen Leemkuil    | 519136 **************************
 ************************** Sjoerd Quast       | 527411 **************************
 ************************** Pim van Haastrecht | 533136 **************************
 ************************** Kolinda Mastenbroek| 517186 **************************
 ************************** Release date     29-04-2016 **************************/
 
#include "Demon.hpp"
#include <iostream>
#include <algorithm>
#include <map>
#include <tuple>
#include <random>
#include <unistd.h>
#include <typeinfo>
#include "appinfo.h"

Demon::Demon(const std::string& appname,
             const std::string& clientname,
             const std::string& host,
             const std::string name,
             int port)
    : CommandProcessor(appname, clientname, host, port), sID(name)
{
    subscribeTo(posPlayer);
    subscribeTo(posDemon);

    registerCommand("posPlayer", bind(&Demon::setPlayerPos, this, std::placeholders::_1));
    registerCommand("posDemon", bind(&Demon::setOwnPos, this, std::placeholders::_1));

    std::string sMessage = "admin" + MQTT_SEPCHAR + "logIn" + MQTT_SEPCHAR;
    sMessage += sID + MQTT_SEPCHAR + sID.substr(5, 1);
    publishTo(admin, sMessage);
}

Demon::~Demon()
{

}

void Demon::die(){
    bRun = false;
}

void Demon::RandMov()
{
    int iReqDirection = 0;
    std::string sMessage;
    auto position = getPlayerPos();

    switch (randNumb(0, 6))
    {
        case 0:
            iReqDirection = randNumb(1, 4);
            break;

        case 1:     //match closest player x
            iReqDirection = (position.first < POSITION_X ? LEFT : (position.first > POSITION_X ? RIGHT : 0) );
            std::cout << "match x own pos (x,y) = " << POSITION_X << " , " << POSITION_Y << "   closest player = " << position.first << " , " << position.second << "    direction = " << iReqDirection << std::endl;
            break;

        case 2:     //match closed player y
            iReqDirection = (position.second < POSITION_Y ? UP : (position.second > POSITION_Y ? DOWN : 0) );
            std::cout << "match y own pos (x,y) = " << POSITION_X << " , " << POSITION_Y << "   closest player = " << position.first << " , " << position.second << "    direction = " << iReqDirection << std::endl;
            break;

        default:
            break;
    }

    sMessage = "keyHit" + MQTT_SEPCHAR + sID + MQTT_SEPCHAR + sID.substr(5) + MQTT_SEPCHAR;

    switch (iReqDirection)
    {
        case UP:
            sMessage += "UP";
            publishTo(keyHit, sMessage);
            break;

        case DOWN:
            sMessage += "DOWN";
            publishTo(keyHit, sMessage);
            break;

        case RIGHT:
            sMessage += "RIGHT";
            publishTo(keyHit, sMessage);
            break;

        case LEFT:
            sMessage += "LEFT";
            publishTo(keyHit, sMessage);
            break;

        default:
            sMessage += "NONE";
            break;
    }
}

std::pair<int, int> Demon::getPlayerPos()
{
    std::pair<int, int> position(1000, 1000);
    int iDistance = PYTHAGOR(((int)CALCLENGTH(POSITION_X, position.first)), ((int)CALCLENGTH(POSITION_Y, position.second)));

    for (auto player : mActPlayers)
    {
        auto tempPos = player.second;
        int iTempDis = PYTHAGOR(((int)CALCLENGTH(POSITION_X, tempPos.first)), ((int)CALCLENGTH(POSITION_Y, tempPos.second)));

        if (iTempDis < iDistance)
        {
            position = tempPos;
        }
    }

    return position;
}

int Demon::randNumb(int min, int max)
{
    std::random_device Random;
    std::uniform_int_distribution<int> range(min, max);
    return std::floor(range(Random));
}

void Demon::setPlayerPos(const std::vector<std::string>& args)
{
    int iCounter = 0;

    std::string id;
    std::pair<int, int> tempPos(0, 0);

    for (auto sTemp : args)
    {
        ++iCounter;

        switch (iCounter)
        {
            case 1:
                id = sTemp;
                break;

            case 3:
                tempPos.second = std::stoi(sTemp);
                break;

            case 4:
                tempPos.first = std::stoi(sTemp);
                mActPlayers[id] = tempPos;
                break;

            case 5:
                iCounter = 0;
                break;

            default:
                break;
        }
    }
}

void Demon::setOwnPos(const std::vector<std::string>& args)
{
    auto it = find(args.begin(), args.end(), sID);

    if (it != args.end())
    {
        ++it;
        POSITION_Y = std::stoi(*(++it));
        POSITION_X = std::stoi(*(++it));
        RandMov();
    }
}