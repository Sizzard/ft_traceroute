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
#include <sys/types.h>
#include <netdb.h>
#include <getopt.h>

typedef struct s_trace {
    int pid;
    int id;
    long int timestamp;
    bool responded;
}   t_trace;

typedef struct s_response {
    int          id;
    int          type;
    char         *address;
}   t_response;

typedef struct s_times {
    float one;
    float two;
    float three;
}   t_times;

typedef struct s_args {
    size_t     first_ttl;
    size_t     max_ttl;
    size_t     tos;
}   t_args;

extern t_args           args;

bool                print_help();

bool                is_num(char *str);
void	            *ft_memset(void *s, int c, size_t n);
char	            *ft_strdup(const char *s);
size_t          	ft_strlen(const char *s);
int             	ft_atoi(const char *nptr);

void                print_icmp_header(void *packet, int bytes);
void                 dump_ip_header(void *packet);

char                *get_ip_address_from_domain(char *address);

void                craft_icmp_packet(char *packet, t_trace trace);
void                dump_packet(char *packet, size_t bytes);
unsigned long       getTimeStamp(void);
int                 ft_traceroute(char *real_address, char *address);