/**************************          MAZERUNNERS        **************************
 **************************          Created by:        **************************
 ************************** Tijmen Leemkuil    | 519136 **************************
 ************************** Sjoerd Quast       | 527411 **************************
 ************************** Pim van Haastrecht | 533136 **************************
 ************************** Kolinda Mastenbroek| 517186 **************************
 ************************** Release date     29-04-2016 **************************/

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <atomic>
#include <signal.h>
#include <csignal>
#include <curses.h>
#include <ncurses.h>
#include <time.h>
#include <chrono>

#include "appinfo.h"
#include "Client.hpp"
#include "CommandProcessor.h"
#include "Config.h"

using namespace std;

atomic<bool> receivedSIGINT {false};

void handleSIGINT(int /* s */)
{
    receivedSIGINT = true;
}

int main()
{
    signal(SIGINT, handleSIGINT);

    int major {0};
    int minor {0};
    int revision {0};
    cout << APPNAME_VERSION << std::endl;

    cout << "-- MQTT application: " << APPNAME_VERSION << "  ";
    mosqpp::lib_init();
    mosqpp::lib_version(&major, &minor, &revision);
    cout << "uses Mosquitto lib version "
         << major << '.' << minor << '.' << revision << endl;

    try{
        //Get PID in string style
        string str_pid = to_string(getpid());
        //Set start time
        string str_time = to_string(chrono::system_clock::now().time_since_epoch().count() % 10000);

        string playerID = str_pid + str_time;
        Client player("mazerunner", playerID, MQTT_BROKER, 1883);

        // Use curses.h functions to set up terminal
        initscr();                          // Initialize screen
        cbreak();                           // Set terminal in raw mode
        noecho();                           // Input is not shown in terminal
        nonl();                             // No enter or endl needed to analyse pressed key
        intrflush(stdscr, FALSE);           // Don't flush buffer
        keypad(stdscr, TRUE);               // Enable keyboard for interrupts
        curs_set(0);                        // Cursor invisible
        nodelay(stdscr, TRUE);

        auto clients = {static_cast<mosqpp::mosquittopp*>(&player)};
        while (!receivedSIGINT) {
            for (auto client: clients) {
                //Handle button presses
                player.ButtonPressed();
				//Handle MQTT data
                int rc = client->loop(200, 1);
                if (rc) {
                    cerr << "-- MQTT reconnect" << endl;
                    client->reconnect();
                }
            }
        }
    }
    catch (exception& e) {
        cerr << "Exception " << e.what() << endl;
    }
    catch (...) {
        cerr << "UNKNOWN EXCEPTION\n";
    }
    cout << "-- MQTT application: " << APPNAME_VERSION << " stopped " << endl << endl;
    mosqpp::lib_cleanup();

    std::cout << "Game ended" << std::endl;
    return 0;
}
