#include <iostream>
#include <stdlib.h>
#include <fstream>
#include "gamemap.hpp"
#include "appinfo.h"
#include <tuple>

GameMap::GameMap(const std::string& appname,
                 const std::string& clientname,
                 const std::string& host,
                 int port)
    : CommandProcessor(appname, clientname, host, port),
    xEnd(0),
    yEnd(0)
{
    subscribeTo(admin);
    subscribeTo(keyHit);
    registerCommand("admin", bind(&GameMap::newPlayer, this, std::placeholders::_1));
    registerCommand("keyHit", bind(&GameMap::MoveCommand, this, std::placeholders::_1));
    std::cout << "GameMap created" << std::endl;
}

GameMap::~GameMap()
{
    std::stringstream message;
    message << "logOut" << std::endl;
    publish(nullptr, playField.c_str(), message.str().size(), message.str().c_str(), 0, true);
    publishTo(admin,"admin logOut playField");
    publishTo(fullPlayField, "Game ended because the gameserver application has been closed. Please restart the gameserver.");
    std::cout << "GameMap removed" << std::endl;
    system("beep -f 400");
}

void GameMap::loadMazeFromFile(const char* mazeFileName)
{
    initMaze();
    std::ifstream DataIn;
    DataIn.open(mazeFileName);
    int X, Y = 0;
    std::string line;

    if (DataIn.is_open())
    {
        while (DataIn.good())
        {
            getline(DataIn, line);
            X = 0;

            for (auto c : line)
            {
                if (c == 'S')
                {
                    startLocation = {X, Y};
                    maze[Y][X] = 'S';
                }
                else if (c == 'I')
                {
                    xEnd = X;
                    yEnd = Y;
                    maze[Y][X] = 'I';
                }
                else
                {
                    maze[Y][X] = c;
                }
                X++;
            }

            Y++;
        }

        DataIn.close();
    }
}

void GameMap::initMaze(void)
{
    int X, Y = 0;

    for (Y = SCRNMINY; Y <= SCRNMAXY; Y++)
    {
        for (X = SCRNMINX; X <= SCRNMAXX; X++)
        {
            maze[Y][X] = SPACE;
        }
    }
}

void GameMap::printMaze(void)
{
    screen = maze;

    if (!playerlocations.empty())
    {
        for (auto player : playerlocations)
        {
            screen[player.second.second][player.second.first] = 'S';
        }
    }

    if (!demonlocations.empty())
    {
        for (auto demon : demonlocations)
        {
            screen[demon.second.second][demon.second.first] = 'D';
        }
    }
    system("clear");

    std::cout << APPNAME_VERSION << std::endl;
    int X, Y = 0;

    for (Y = 0; Y < SCRNMAXY; Y++)   // SCRNMINY
    {
        for (X = 0; X < SCRNMAXX; X++)   //SCRNMINX
        {
            if (screen[Y][X] == 'S')
            {
                std::cout << GREEN << screen[Y][X] << RESET;
            }
            else if (screen[Y][X] == 'D')
            {
                std::cout << BLUE << screen[Y][X] << RESET;
            }
            else
            {
                std::cout << RED << screen[Y][X] << RESET;
            }
        }

        std::cout << std::endl;
    }

    std::cout << "---Playerlist---" << std::endl;

    if (!playerlocations.empty())
    {
        for (auto playerlist : playerlocations)
        {
            std::cout << "Player: " << playerlist.first << " x,y " << playerlist.second.first << "," << playerlist.second.second;
            std::cout << std::endl;
        }
    }
    std::cout << "---Demonlist---" << std::endl;

    if (!demonlocations.empty())
    {
        for (auto demonlist : demonlocations)
        {
            std::cout << "Demon: " << demonlist.first << " x,y " << demonlist.second.first << "," << demonlist.second.second << std::endl;
        }
    }
}

void GameMap::printMazeToMQTT(void)
{
    int X, Y = 0;

    std::stringstream map;
    map << "playField ";

    for (Y = 0; Y < SCRNMAXY; Y++)   // SCRNMINY
    {
        for (X = 0; X < SCRNMAXX; X++)   //SCRNMINX
        {
            if (maze[Y][X] == SPACE)
            {
                map << ".";
            }
            else if (maze[Y][X] == '\r')
            {
                map << "\n";
            }
            else
            {
                map << maze[Y][X];
            }
        }
    }

    publish(nullptr, playField.c_str(), map.str().size(), map.str().c_str(), 0, true);
    //publishTo(playField, map.str());
}

void GameMap::printFullMazeToMQTT(void)
{
    std::stringstream ret;
    screen = maze;

    if (!playerlocations.empty())
    {
        for (auto player : playerlocations)
        {
            screen[player.second.second][player.second.first] = 'S';
        }
    }

    if (!demonlocations.empty())
    {
        for (auto demon : demonlocations)
        {
            screen[demon.second.second][demon.second.first] = 'D';
        }
    }
    int X, Y = 0;

    for (Y = 0; Y < SCRNMAXY; Y++)   // SCRNMINY
    {
        for (X = 0; X < SCRNMAXX; X++)   //SCRNMINX
        {
            if (screen[Y][X] == 'S')
            {
                ret << screen[Y][X];
            }
            else if (screen[Y][X] == 'D')
            {
                ret << "<font color=\"blue\">" << screen[Y][X] << "</font>";
            }
            else if (screen[Y][X] == ' ')
            {
                ret << "<font color=\"white\">*</font>";
            }
            else
            {
                ret << "<font color=\"red\">" << screen[Y][X] << "</font>";
            }
        }

        ret << "<br>";
    }

    ret << "</p><p>";
    ret << "---Playerlist---<br>";

    if (!playerlocations.empty())
    {
        for (auto playerlist : playerlocations)
        {
            ret << "Player: " << playerlist.first << " x,y " << playerlist.second.first << "," << playerlist.second.second;
            ret << "<br>";
        }
    }
    std::cout << "---Demonlist---" << std::endl;

    if (!demonlocations.empty())
    {
        for (auto demonlist : demonlocations)
        {
            ret << "Demon: " << demonlist.first << " x,y " << demonlist.second.first << "," << demonlist.second.second;
            ret << "<br>";
        }
    }
    publishTo(fullPlayField, ret.str());
}

void GameMap::printPlayersToMQTT(void)   // posPlayer playerid id x y 0
{
    std::stringstream ret;
    ret << "posPlayer";
    int x, y = 0;

    if (!playerlocations.empty())
    {
        for (auto playerlist : playerlocations)
        {
            x = playerlist.second.second;
            y = playerlist.second.first;
            ret << " player" << playerlist.first << " " << playerlist.first
                << " " << x << " " << y;

            if (isSpaceEnd(x, y))
            {
                ret << " 1 ";
            }
            else
            {
                ret << " 0 ";
            }
        }

        publishTo(posPlayer, ret.str());
    }
    else
    {
        publishInfo("No Players!");
    }
}

void GameMap::printDemonsToMQTT(void)   // posDeamon demonid id x y
{
    std::stringstream ret;
    ret << "posDemon";

    if (!demonlocations.empty())
    {
        for (auto demonlist : demonlocations)
        {
            ret << " Demon" << demonlist.first << " " << demonlist.first
                << " " << demonlist.second.first << " " << demonlist.second.second;
        }

        // publishTo(posDemon,ret.str());
    }
    else
    {
        publishInfo("No demons!");
    }
    publishTo(posDemon, ret.str());
}

bool GameMap::isSpaceFree(int x, int y)
{
    if (screen[0][0] != '*')
    {
        std::cout << "SCREEN EMPTY!" << std::endl;

        if (maze[0][0] != '*')
        {
            std::cout << "MAZE EMPTY" << std::endl;
            return true;
        }
        else
        {
            screen = maze;
        }
    }

    if (screen[y][x] != SPACE)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool GameMap::isSpaceEnd(int x, int y)
{
   // std::cout << "end coordinates: x,y " << y << xEnd << " " << x << yEnd << std::endl;

    if (((y == xEnd) && (x == yEnd)) || ((y + 1 == xEnd) && (x == yEnd)) || ((y == xEnd) && (x + 1 == yEnd)) || ((y - 1 == xEnd) && (x == yEnd)) || ((y == xEnd) && (x - 1 == yEnd)))
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::string GameMap::getPlayerStartPos()
{
    std::stringstream ret;
    srand(time(NULL));
    int Randomx = startLocation.second + rand() % 3 + 0;
    srand(time(NULL));
    int Randomy = startLocation.first + rand() % 3 + 0;

    if (isSpaceFree(startLocation.second, startLocation.first))
    {
        ret << startLocation.first << "," << startLocation.second;
    }
    else
    {
        while (!isSpaceFree(Randomx, Randomy)) {
            Randomx = startLocation.second + (rand() % 5 + 0);
            Randomy = startLocation.first + (rand() % 3 + 0);
        }

        ret << Randomy << "," << Randomx;
    }
    return ret.str();
}

void GameMap::setPlayerStartPos(int Playerid)
{
    int Randomx = randomNumber(1, 5);
    int Randomy = randomNumber(1, 3);

    if (isSpaceFree(startLocation.second, startLocation.first))
    {
        playerlocations[Playerid] = std::make_pair(Randomx, Randomy);
    }
    else
    {
        while (!isSpaceFree(Randomx, Randomy)) {
            Randomx = randomNumber(1, 5);
            Randomy = randomNumber(1, 3);
        }

        playerlocations[Playerid] = std::make_pair(Randomx, Randomy);
    }
}

void GameMap::setDemonStartPos(int DemonId)
{
    int Randomx = randomNumber(1, 73);
    int Randomy = randomNumber(1, 22);

    if (isSpaceFree(startLocation.second, startLocation.first))
    {
        demonlocations[DemonId] = std::make_pair(Randomx, Randomy);
    }
    else
    {
        while (!isSpaceFree(Randomx, Randomy)) {
            Randomx = randomNumber(1, 73);
            Randomy = randomNumber(1, 22);
        }

        demonlocations[DemonId] = std::make_pair(Randomx, Randomy);
    }
}

void GameMap::changePlayerCoordinates(int playerid, int x, int y)
{
    if (isSpaceFree(x, y) )
    {
        playerlocations[playerid] = std::make_pair(x, y);
    }
}

bool GameMap::changeDemonCoordinates(int demonid, int x, int y)
{
    if (isSpaceFree(x, y))
    {
        demonlocations[demonid] = std::make_pair(x, y);
        return true;
    }
    else
    {
        return false;
    }
}

void GameMap::killDemon(int demonid)
{
    if (demonid == 666) {
        demonlocations.clear();
    } else {
        demonlocations.erase(demonid);
    }
}

void GameMap::killPlayer(int playerid)
{
    playerlocations.erase(playerid);
}

void GameMap::newPlayer(const std::vector<std::string>& commandParameters)
{
    // LOGIN/LOGOUT + playerID/demonID + ID
    std::string loginName, playerid;
    int ID;
    loginName = commandParameters[1];
    playerid = commandParameters[2];
    ID = stoi(playerid);

    if (commandParameters[0].find("logIn") == 0)   // login
    {
        if (loginName.find("Player") == 0)   // login player
        {
            setPlayerStartPos(ID);
            publishInfo("Player: " + loginName + " is successfully registered");
        }
        else      // login demon
        {
            setDemonStartPos(ID);
            publishInfo("Demon: " + loginName + " is successfully registered!");
        }
    }
    else if (commandParameters[0].find("logOut") == 0)     // logout
    {
        if (loginName.find("Player") == 0)   // logOut player
        {
            killPlayer(ID);
            publishInfo("Player: " + loginName + " is successfully logged out");
        }
        else      // login demon
        {
            killDemon(ID);
            publishInfo("Demon: " + loginName + " is successfully logged out!");
        }
    }
    else
    {
        publishInfo("login logout failed!");
    }
}

void GameMap::MoveCommand(const std::vector<std::string>& commandParameters)
{
    // PlayerID + ID + “UP”/”DOWN”/”LEFT”/”RIGHT”
    // demonID + ID + “UP”/”DOWN”/”LEFT”/”RIGHT”
    std::cout << "MOVECOMMAND RECEIVED!!!" << std::endl;
    std::cout << commandParameters[0] << commandParameters[1] << commandParameters[2] << std::endl;
    std::string demon = "demon";
    int ID = atoi(commandParameters[1].c_str());
    int x, y = 0;

    if (0 == commandParameters[0].find("Demon"))
    {
        // change demon
        if (!(demonlocations.find(ID) == demonlocations.end()))
        {
            auto coordinates = demonlocations[ID];
            y = coordinates.first;
            x = coordinates.second;

            if (commandParameters[2] == "RIGHT")
            {
                y++;
            }
            else if (commandParameters[2] == "LEFT")
            {
                y--;
            }
            else if (commandParameters[2] == "UP")
            {
                x--;
            }
            else if (commandParameters[2] == "DOWN")
            {
                x++;
            }
            std::cout << "Changed demon coordinates " << ID << " " << x << "," << y << std::endl;
        }

        if (isSpaceFree(y, x) )
        {
            demonlocations[ID] = std::make_pair(y, x);
        }
    }
    else
    {
        // change player
        if (!(playerlocations.find(ID) == playerlocations.end()))
        {
            auto coordinates = playerlocations[ID];
            y = coordinates.first;
            x = coordinates.second;

            if (commandParameters[2] == "RIGHT")
            {
                y++;
            }
            else if (commandParameters[2] == "LEFT")
            {
                y--;
            }
            else if (commandParameters[2] == "UP")
            {
                x--;
            }
            else if (commandParameters[2] == "DOWN")
            {
                x++;
            }
            std::cout << "Changed player coordinates " << ID << " " << x << "," << y << std::endl;

            if (isSpaceFree(y, x) )
            {
                playerlocations[ID] = std::make_pair(y, x);
            }
        }
    }
}

int GameMap::randomNumber(int min, int max)
{
    std::random_device Random;
    std::uniform_int_distribution<int> range(min, max);
    return std::floor(range(Random));
}

int GameMap::getNumberOfPlayers()
{
    if (playerlocations.size() == 0)
    {
        return 0;
    }
    else
    {
        return playerlocations.size();
    }
}