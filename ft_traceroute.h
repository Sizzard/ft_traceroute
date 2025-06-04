#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>


typedef struct s_trace {
    int id;
}   t_trace;

typedef struct s_response {
    float        time;
    int          id;
    int          type;
    char         *address;
}   t_response;


bool                print_help();

void                craft_icmp_packet(char *packet, t_trace trace);
void                dump_packet(char *packet);
unsigned long       getTimeStamp(void);
int                 ft_traceroute(char *real_address, char *address);