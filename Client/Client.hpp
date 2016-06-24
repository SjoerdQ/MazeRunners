/**************************          MAZERUNNERS        **************************
 **************************          Created by:        **************************
 ************************** Tijmen Leemkuil    | 519136 **************************
 ************************** Sjoerd Quast       | 527411 **************************
 ************************** Pim van Haastrecht | 533136 **************************
 ************************** Kolinda Mastenbroek| 517186 **************************
 ************************** Release date     29-04-2016 **************************/
 
#ifndef CLIENT_H
#define CLIENT_H

#include "CommandProcessor.h"
#include "appinfo.h"

#include <iostream>
#include <chrono>
#include <vector>

//maze sizes
#define SCRNMAXX 74
#define SCRNMAXY 23

//key inputs
#define NOBUTTON -1

class Client : public CommandProcessor 
{ 
public: 
	Client(const std::string& appname, 
               const std::string& clientname, 
               const std::string& host, 
               int port);
	virtual ~Client();
	void ButtonPressed();												//Button signal handler	

private:
	// *********** Parameters Client ***********
	std::chrono::system_clock::time_point starttime;					//Starttime
	int activetime = 0;													//Total playtime
	std::map<int , std::pair<int,int> > PlayerLocations;				//player locations
    std::map<int , std::pair<int,int> > DemonLocations;   				//demon locations	
	bool playerfinished = false;										//Player is on now on ing
		
	// *********** MQTT Commands ***********
	void getPlayers(const std::vector<std::string>& commandParameters);	//Get player locations
	void getDemons(const std::vector<std::string>& commandParameters);	//Get demon locations
	void getField(const std::vector<std::string>& commandParameters);	//Get Wall data from maze
	void fieldGone(const std::vector<std::string>& commandParameters);	//Gamemap was terminated
	
	// *********** Functions Client ***********
	std::string Timeupdate();											//update time if neccesary
};

#endif
