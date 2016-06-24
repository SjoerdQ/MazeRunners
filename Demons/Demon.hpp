/**************************          MAZERUNNERS        **************************
 **************************          Created by:        **************************
 ************************** Tijmen Leemkuil    | 519136 **************************
 ************************** Sjoerd Quast       | 527411 **************************
 ************************** Pim van Haastrecht | 533136 **************************
 ************************** Kolinda Mastenbroek| 517186 **************************
 ************************** Release date     29-04-2016 **************************/
 
#ifndef DEMON_H
#define DEMON_H

#include <tuple>
#include "CommandProcessor.h"
#include <map>
#include <cmath>
#include <string>
#include <vector>

#define MQTT_SEPCHAR (std::string)" "

#define POSITION_X		pPosition.first
#define POSITION_Y		pPosition.second

#define PYTHAGOR(l,w)		sqrt( (l*l) + (w*w) )
#define CALCLENGTH(i,j) 	(i<j) ? (j-i) : (i-j)

#define UP 1
#define DOWN 2
#define RIGHT 3
#define LEFT 4

class Demon: public CommandProcessor 
{
	public:
		Demon( const std::string& appname,
               const std::string& clientname,
               const std::string& host,
               const std::string name,
               int port);
		Demon();

		~Demon();

		std::string sID;

		void setPlayerPos(const std::vector<std::string>& args);
		void setOwnPos(const std::vector<std::string>& args);

	protected:


	private:
		bool bRun = true;

		std::pair<int,int> pPosition;

		std::map<std::string, std::pair<int,int>> mActPlayers;

		void RandMov();

		std::pair<int,int> getPlayerPos();

		int randNumb(int min, int max);

		void die();
};

#endif