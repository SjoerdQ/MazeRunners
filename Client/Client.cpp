/**************************          MAZERUNNERS        **************************
 **************************          Created by:        **************************
 ************************** Tijmen Leemkuil    | 519136 **************************
 ************************** Sjoerd Quast       | 527411 **************************
 ************************** Pim van Haastrecht | 533136 **************************
 ************************** Kolinda Mastenbroek| 517186 **************************
 ************************** Release date     29-04-2016 **************************/

#include "Client.hpp"
//std functions
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
//Time
#include <time.h>
#include <chrono>
//input and output user
#include <curses.h>
#include <ncurses.h>

using namespace std;

Client::Client(const std::string& appname,
               const std::string& clientname,
               const std::string& host,
               int port)
    : CommandProcessor(appname, clientname, host, port)
{
	///Constructor Players class
	
    //Set start time
    starttime = std::chrono::system_clock::now();

    //Subscribe to MQTT topics
    subscribeTo(posPlayer);
    subscribeTo(posDemon);
    subscribeTo(playField);

    //Bind MQTT commands
    registerCommand("posPlayer", bind(&Client::getPlayers, this, std::placeholders::_1));
    registerCommand("posDemon", bind(&Client::getDemons, this, std::placeholders::_1));
    registerCommand("playField", bind(&Client::getField, this, std::placeholders::_1));
    registerCommand("logOut", bind(&Client::fieldGone, this, std::placeholders::_1));

    //Tell broker the client logged in (and add it to debug console)
    publishTo(admin, string("admin logIn Player ") + My_Client_id);
    //publishInfo(string("Player") + My_Client_id + string(" is trying to login..."));
}

Client::~Client()
{
    //*******************!!!Client destructor is not working, even as virtual function!!!*******************
    ///Destructor Players class
    publishTo(admin, string("admin logOut Player ") + My_Client_id);
    //publishInfo(string("Player") + My_Client_id + string(" is trying to logout..."));
    
    //Kill process
    endwin();
    exit(EXIT_SUCCESS);
}

void Client::getPlayers(const std::vector<std::string>& commandParameters)
{
    ///translates the data from MQTT topic 'posPlayer' into player locations printed in maze
    //publishInfo("Receiving player data...");

    //Enable colors, Use foreground GREEN and BLUE and background default colors
    start_color();
    assume_default_colors(-1, -1);
    init_pair(1, COLOR_GREEN, -1);
    init_pair(2, COLOR_BLUE, -1);

    // Set old values to empty space
    for (auto player : PlayerLocations)
    {
        int x, y;
        x = player.second.second;
        y = player.second.first;

        mvprintw(y, x, " ");
    }

    // Empty locations map (Players can be disconnected)
    PlayerLocations.clear();

    // Count nr of commands
    int i = 0;

    for (auto e: commandParameters)
    {
        i++;
    }

    attron(A_BOLD);

    // Set each parameter ( (name) | ID | x | y | finished in list)
    // commandParameters      1      2    3   4   5 etc.
    for (int j = 0; j < i; j += 5)
    {
        int x, y, id, end;
        id  = stoi(commandParameters[j + 1]);
        y   = stoi(commandParameters[j + 2]) + 1;       //+1 because first row is reserved for application name
        x   = stoi(commandParameters[j + 3]);
        end = stoi(commandParameters[j + 4]);
        PlayerLocations[id] = std::make_pair(y, x);
        
        // Set new positions; green for my player, Blue for others
        if (id == stoi(My_Client_id))
        {
            attron(COLOR_PAIR(1));      //green on
            mvprintw(y, x, "S");
            attroff(COLOR_PAIR(1));     //green off

            //check for player at end
            if (end == 1)
            {
                playerfinished = true;
            }
        }
        else
        {
            attron(COLOR_PAIR(2));      //blue on
            mvprintw(y, x, "S");
            attroff(COLOR_PAIR(2));     //blue off
        }
    }

    attroff(A_BOLD);

    //Once players have moved, set timeplayed parameter as well
    if (!playerfinished)
    {
        mvprintw(0, 21, "Player time: ");
        mvprintw(0, 21 + 13, Timeupdate().c_str());
    }
    else
    {
        mvprintw(0, 18, "Player done: ");
        mvprintw(0, 18 + 13, Timeupdate().c_str());
        mvprintw(0, 44, "Press BACKSPACE to end game..");
    }
    refresh();
}

void Client::getDemons(const std::vector<std::string>& commandParameters)
{
    ///translates the data from MQTT topic 'posDemon' into demon locations printed in maze
    //publishInfo("Receiving demon data...");

    //Enable colors, Use foreground RED and background default colors
    start_color();
    assume_default_colors(-1, -1);
    init_pair(3, COLOR_RED, -1);

    // Set old values to empty space
    for (auto demon : DemonLocations)
    {
        int x, y;
        x = demon.second.second;
        y = demon.second.first;
        mvprintw(y, x, " ");
    }

    // Empty locations map (Demons can be disconnected)
    DemonLocations.clear();

    // Count nr of commands
    int i = 0;

    for (auto e: commandParameters)
    {
        i++;
    }

    // Set each parameter ( (name) | ID | x | y in list)
    // Commandparameter:      1      2    3   4 etc.
    //Make letters red and bold
    attron(COLOR_PAIR(3));
    attron(A_BOLD);

    for (int j = 0; j < i; j += 4)
    {
        //cout << j << endl;
        int x, y, id;
        id = stoi(commandParameters[j + 1]);
        x  = stoi(commandParameters[j + 2]);
        y  = stoi(commandParameters[j + 3]) + 1;        //+1 for one row reserved for application name
        DemonLocations[id] = std::make_pair(y, x);
        // Set new positions
        mvprintw(y, x, "D");
    }

    //Don't make letters red and bold anymore
    attroff(COLOR_PAIR(3));
    attroff(A_BOLD);

    //refresh screen
    refresh();
}

void Client::getField(const std::vector<std::string>& commandParameters)
{
    ///translates the data from MQTT topic 'playField' into playfield data (maze walls)
    //publishInfo("Field received!");
    system("clear");

    //Print app name
    printw(APPNAME_VERSION);
    printw("\n");

    attron(A_DIM);

    for (int i = 0; i < SCRNMAXY; i++)
    {
        std::string playfield = commandParameters[i];

        //Replace all dots with spaces
        size_t found = playfield.find_first_of(".");
        while (found != string::npos) {
            playfield[found] = ' ';
            found = playfield.find_first_of(".", found + 1);
        }

        //print the whole row and go to the next one
        printw(playfield.c_str());
        printw("\n");
    }

    attroff(A_DIM);
    refresh();
}

string Client::Timeupdate()
{
    //If player haven't finished yet, update time
    if (!playerfinished)
    {
        activetime = chrono::duration_cast<chrono::seconds>(std::chrono::system_clock::now() - starttime).count();
    }

    return to_string(activetime) + " seconds         ";
}

void Client::ButtonPressed()
{
    ///Gets the input data from keyboard; Sends data via MQTT
    raw();

    switch (getch())
    {
        case KEY_UP:
            publishTo(keyHit, string("keyHit Player ") + My_Client_id + string(" UP"));
            break;

        case KEY_DOWN:
            publishTo(keyHit, string("keyHit Player ") + My_Client_id + string(" DOWN"));
            break;

        case KEY_RIGHT:
            publishTo(keyHit, string("keyHit Player ") + My_Client_id + string(" RIGHT"));
            break;

        case KEY_LEFT:
            publishTo(keyHit, string("keyHit Player ") + My_Client_id + string(" LEFT"));
            break;

        case KEY_BACKSPACE:
            //Kill process
            endwin();
            exit(EXIT_SUCCESS);
            break;

        case NOBUTTON:
            //publishInfo("No button pressed");
            break;

        default:
            //any other button
            publishError("Unknown direction player");
            break;
    }
}

void Client::fieldGone(const std::vector<std::string>& commandParameters)
{
	///Notify the user that there is no gamemap. The player will have to exit the game.
	//Enable colors, Use foreground RED and background default colors
    start_color();
    assume_default_colors(-1, -1);
    init_pair(3, COLOR_RED, -1);
    
    //Set Red and bold
    attron(COLOR_PAIR(3));
    attron(A_BOLD);
    
	mvprintw(11, 19, "Gamemap is gone!");
	mvprintw(12, 19, "Press backspace to close the game...");
	
	//set back to normal colours
	attroff(COLOR_PAIR(3));
    attroff(A_BOLD);
}
