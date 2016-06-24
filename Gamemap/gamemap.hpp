#ifndef GAMEMAP_H
#define GAMEMAP_H

#define FALSE 0
#define TRUE 1

#define SCRNMINX 3
#define SCRNMAXX 74
#define SCRNMINY 2
#define SCRNMAXY 23

#define UP 1
#define DOWN 2
#define RIGHT 3
#define LEFT 4

#define SPACE ' '
#define DEMONS 8

//the following are UBUNTU/LINUX ONLY terminal color codes.
#define RESET "\033[0m"
#define BLACK "\033[30m"        /* Black */
#define RED "\033[31m"          /* Red */
#define GREEN "\033[32m"        /* Green */
#define YELLOW "\033[33m"       /* Yellow */
#define BLUE "\033[34m"         /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN "\033[36m"         /* Cyan */
#define WHITE "\033[37m"        /* White */

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <time.h>
#include <random>
// #include "Demon.hpp"
#include "CommandProcessor.h"

class Demon;

class Players {
};

class GameMap : public CommandProcessor {
public:
    GameMap(const std::string& appname,
            const std::string& clientname,
            const std::string& host,
            int port);
    ~GameMap();

    int randomNumber(int min, int max);

    void loadMazeFromFile(const char* mazeFileName);
    void printMaze(void);
    void printMazeToMQTT(void);
    void printFullMazeToMQTT(void);
    void printPlayersToMQTT(void);
    void printDemonsToMQTT(void);
    int getNumberOfPlayers();

private:
    bool isSpaceFree(int x, int y);
    bool isSpaceEnd(int x, int y);
    bool changeDemonCoordinates(int demonid, int x, int y);

    int xEnd;
    int yEnd;

    std::array<std::array<char, SCRNMAXX + 1>, SCRNMAXY + 1> maze;
    std::array<std::array<char, SCRNMAXX + 1>, SCRNMAXY + 1> screen;
    std::map<int, std::pair<int, int> > playerlocations;
    std::map<int, std::pair<int, int> > demonlocations;
    std::pair<int, int> startLocation;
    std::string getPlayerStartPos();

    void setPlayerStartPos(int Playerid);
    void setDemonStartPos(int DemonId);
    void changePlayerCoordinates(int playerid, int x, int y);
    void MoveCommand(const std::vector<std::string>& commandParameters);
    void killDemon(int demonid);
    void killPlayer(int playerid);
    void initMaze(void);
    void newPlayer(const std::vector<std::string>& commandParameters);
};

#endif