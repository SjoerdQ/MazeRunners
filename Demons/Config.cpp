/**************************          MAZERUNNERS        **************************
 **************************          Created by:        **************************
 ************************** Tijmen Leemkuil    | 519136 **************************
 ************************** Sjoerd Quast       | 527411 **************************
 ************************** Pim van Haastrecht | 533136 **************************
 ************************** Kolinda Mastenbroek| 517186 **************************
 ************************** Release date     29-04-2016 **************************/
 
#include "Config.h"
#include <unistd.h>

static const int MAXSIZE_HOSTNAME {25};

const char*getBBBid()
{
    static char hostname[MAXSIZE_HOSTNAME] = {'\0'};

    gethostname(hostname, MAXSIZE_HOSTNAME);
    return hostname;
}