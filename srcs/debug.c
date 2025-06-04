#include "../ft_traceroute.h"

void dump_packet(char *packet) {
    printf("Packet Dump:\n");
    for (size_t i = 0; i < 1024; i+=2) {
        printf(" %02x%02x", (unsigned char)packet[i], (unsigned char)packet[i + 1]);
    }
    printf("\n");
}
