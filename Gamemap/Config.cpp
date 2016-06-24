#include "Config.h"
#include <unistd.h>

static const int MAXSIZE_HOSTNAME {25};

const char*getBBBid()
{
    static char hostname[MAXSIZE_HOSTNAME] = {'\0'};

    gethostname(hostname, MAXSIZE_HOSTNAME);
    return hostname;
}