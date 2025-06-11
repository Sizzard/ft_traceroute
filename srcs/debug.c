#include "../ft_traceroute.h"

void dump_packet(char *packet, size_t bytes) {
    printf("Packet Dump:\n");
    for (size_t i = 0; i < bytes; i+=2) {
        printf(" %02x%02x", (unsigned char)packet[i], (unsigned char)packet[i + 1]);
    }
    printf("\n");
}

void dump_ip_header(void *packet) {
    struct iphdr *ip = (struct iphdr *)(packet);
    struct sockaddr_in src, dst;
    char *dst_addr;
    char *src_addr;
    // char *pack_char = packet;
    // int flag = ip->frag_off >> 5;
    // int offset = 3 << ip->frag_off;
    // offset = offset >> 3;

    printf("IP Hdr Dump:\n");
    // for (size_t i = 28; i < 48; i += 2) {
    //     printf(" %02x%02x", (unsigned char)pack_char[i], (unsigned char)pack_char[i + 1]);
    // }
    // printf("\n");
    // printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src	Dst	Data\n");
    // printf(" %x", ip->version);
    // printf(" %x", ip->ihl);
    // printf("   %02x", ip->tos);
    // printf(" %04x", htons(ip->tot_len));
    // printf(" %x", ip->id);
    // printf("   %x", flag);
    // printf(" %04x", offset);
    // printf("  %02x", ip->ttl);
    // printf("  %02x", ip->protocol);
    // printf(" %04x", ip->check);

    src.sin_addr.s_addr = ip->saddr;
    src_addr = inet_ntoa(src.sin_addr);
    printf(" %s", src_addr);

    dst.sin_addr.s_addr = ip->daddr;
    dst_addr = inet_ntoa(dst.sin_addr);
    printf("  %s\n", dst_addr);
}

void print_icmp_header(void *packet, int bytes) {
    struct icmphdr *icmp = packet;
    printf("ICMP: type %d, code %d, size %d, id 0x%04x, seq 0x%04x\n", icmp->type, icmp->code, bytes, ntohs(icmp->un.echo.id), ntohs(icmp->un.echo.sequence));
}