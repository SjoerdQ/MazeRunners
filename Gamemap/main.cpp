#include <iostream>
#include <unistd.h>
//#include "KeepTime.h"
#include "gamemap.hpp"
#include "appinfo.h"
#include "CommandProcessor.h"
#include <atomic>
#include <csignal>
#include <iostream>
#include "Config.h"
#include <stdio.h>
#include <curses.h> // getch()
// #include "Demons.hpp"


using namespace std;

atomic<bool> receivedSIGINT {false};

void handleSIGINT(int /* s */)
{
  receivedSIGINT = true;
}

int main(int argc, char *argv[]) {
         if(argc != 2) {
       std::cout << "Usage " << APPNAME << ": "<< argv[0] << " <file name map>" << std::endl;
       exit(EXIT_FAILURE);
   }

  signal(SIGINT, handleSIGINT);

  int major {0};
  int minor {0};
  int revision {0};
  std::cout << APPNAME_VERSION << std::endl;

  cout << "-- MQTT application: " << APPNAME_VERSION << "  ";
  mosqpp::lib_init();
  mosqpp::lib_version(&major, &minor, &revision);
  cout << "uses Mosquitto lib version "
       << major << '.' << minor << '.' << revision << endl;

    try{
       // Demons demons("mazerunner", "demons", MQTT_BROKER, 1883);
        GameMap map("mazerunner", "map", MQTT_BROKER, 1883);
      // CommandProcessor commandProcessor("mazerunner", "cp", MQTT_BROKER, 1883);
        //KeepTime timer;
        map.loadMazeFromFile(argv[1]);
        std::cout << std::endl;   
        auto i = 0;
        auto j = 0; 
        map.printMaze();
        map.printMazeToMQTT();
        auto clients = {static_cast<mosqpp::mosquittopp*>(&map)};
        while (!receivedSIGINT)
        {
             i++;
             j++;
            if (i == 8) {
            map.printPlayersToMQTT();
            map.printDemonsToMQTT();
            if (j == 256) {
                map.printFullMazeToMQTT();
                j = 0;
            }
            if (map.getNumberOfPlayers() == 0) {
                system("clear");
                std::cout << "Info: No players in game!" << std::endl;
                sleep(1);
            }
            //std::cout << "printed stuff to mqtt" << std::endl;
            i=0;
            }
          
          //  map.printMaze();
          for (auto client: clients)
          {
         // 	map.printPlayersToMQTT();
         //   map.printDemonsToMQTT();
            //map.printMaze();
          //  if (i == (50*map.getNumberOfPlayers())) {
                // 
               // i=1;
            //} 
           // i++;
            int rc = client->loop(25,1);
            if (rc)
            {
              cerr << "-- MQTT reconnect" << endl;
              client->reconnect();
            }
          }
        }
    }
    catch(exception& e){
        cerr << "Exception " << e.what() << endl;
    }
    catch(...){
        cerr << "UNKNOWN EXCEPTION\n"; 
    }
    

    
    cout << "-- MQTT application: " << APPNAME_VERSION << " stopped " << endl << endl;
    mosqpp::lib_cleanup();

    std::cout << "Game ended, goodbye" << std::endl;
	return 0;
}
