#include "../ft_traceroute.h"

unsigned long getTimeStamp(void) {
    struct timeval tv;
    unsigned long time;
    
    gettimeofday(&tv, NULL);

    time = 1000000 * tv.tv_sec + tv.tv_usec;

    return time;
}