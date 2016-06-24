/**************************          MAZERUNNERS        **************************
 **************************          Created by:        **************************
 ************************** Tijmen Leemkuil    | 519136 **************************
 ************************** Sjoerd Quast       | 527411 **************************
 ************************** Pim van Haastrecht | 533136 **************************
 ************************** Kolinda Mastenbroek| 517186 **************************
 ************************** Release date     29-04-2016 **************************/

#include <iostream>
#include <unistd.h>
#include "appinfo.h"
#include "CommandProcessor.h"
#include <atomic>
#include <csignal>
#include <iostream>
#include "Config.h"
#include <stdio.h>
#include <curses.h> // getch()
#include "Demons.hpp"

using namespace std;

atomic<bool> receivedSIGINT {false};

void handleSIGINT(int /* s */)
{
    receivedSIGINT = true;
}

int main(int argc, char* argv[])
{
    signal(SIGINT, handleSIGINT);

    int major {0};
    int minor {0};
    int revision {0};
    std::cout << APPNAME_VERSION << std::endl;

    std::vector<mosqpp::mosquittopp*> vClients;

    cout << "-- MQTT application: " << APPNAME_VERSION << "  ";
    mosqpp::lib_init();
    mosqpp::lib_version(&major, &minor, &revision);
    cout << "uses Mosquitto lib version "
         << major << '.' << minor << '.' << revision << endl;

    try{
        Demons* demons = new Demons("mazerunner", "demons", MQTT_BROKER, &vClients, 1883);
        vClients.push_back(static_cast<mosqpp::mosquittopp*>(demons));
        while (!receivedSIGINT)
        {
            for (auto client: vClients)
            {
                int rc = client->loop((200), 1);

                if (rc)
                {
                    cerr << "-- MQTT reconnect error code:" << rc << endl;
                    client->reconnect();
                }
            }
        }

        delete demons;
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
