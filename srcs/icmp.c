#include "../ft_traceroute.h"

uint16_t get_checksum(const void *buf, size_t len) {
    uint32_t sum = 0;
    const uint16_t *packet = buf;

    while (len > 1) {
        sum += *packet++;
        len -= 2;
    }

    if (len > 0) {
        sum += *((const uint8_t *)packet);
    }

    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    return (uint16_t)(~sum);
}

void craft_icmp_packet(char *packet, t_trace trace) {
    struct icmphdr *icmp;
    icmp = (struct icmphdr *)(packet);

    icmp->type = 8;
    icmp->code = 0;
    icmp->un.echo.id = htons(trace.pid);
    icmp->un.echo.sequence = htons(trace.id);
    icmp->checksum = 0;


    icmp->checksum = get_checksum(icmp, 60);
}