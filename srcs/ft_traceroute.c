#include "../ft_traceroute.h"

float parse_time_stamp(char *packet) {
    unsigned long time;
    float res = 0;    

    // for (size_t i = 28; i < 36; i++) {
    //     printf("%x ", (unsigned char)packet[i]);
    // }
    memcpy(&time, &packet[28], 8);

    // printf("time : %ld\n", time);
    
    res = getTimeStamp() - time;

    res /= 1000;
    
    // printf("res = %f\n", res);

    return res;
}

char *get_dest_address(struct iphdr *ip) {
    struct sockaddr_in src;
    char *src_addr;

    src.sin_addr.s_addr = ip->saddr;
    src_addr = inet_ntoa(src.sin_addr);
    
    return strdup(src_addr);
}

t_response parse_response(void *buf, int bytes) {
    struct iphdr *ip = buf;
    struct icmphdr* icmp = buf + 20;
    t_response response = {0};
    (void)bytes;

    response.time = -1;
    response.type = icmp->type;
    response.address = get_dest_address(ip);
    response.time = parse_time_stamp(buf);
    if (response.type != 0) {
        response.time = parse_time_stamp(buf + 28);
        icmp = buf + 48;
    }
    response.id = ntohs(icmp->un.echo.id);

    return response;
}

int create_socket() {
    int sock;

    sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock == -1) {
        fprintf(stderr, "ft_traceroute: Failed to create a Raw Socket (probably a right issue)\n");
        return -1;
    }

    return sock;
}

int set_ttl(int sock, int new_ttl) {
    if (setsockopt(sock, IPPROTO_IP, IP_TTL, &new_ttl, sizeof(new_ttl)) == -1) {
        fprintf(stderr, "ft_traceroute: Failed to change ttl value\n");
        return -1;   
    }
    return 0;
}

t_trace init_trace() {
    t_trace trace;

    trace.id = getpid();

    return trace;
}

int get_packet_sequence(char *packet, size_t bytes) {
    struct icmphdr *icmp;
    if (bytes < 54) {
        return -1;
    }
    icmp = (struct icmphdr*)(packet + 20);
    if (icmp->type != 0) {
        icmp = (struct icmphdr*)(packet + 48);
    }
    return ntohs(icmp->un.echo.sequence);
}

int ft_traceroute(char *real_address, char *address) {
    char packet[1024] = {0}; 
    char recv_buf[1024] = {0}; 
    struct sockaddr_in addr;
    t_trace trace;
    int sock = 0;
    int ttl = 1;
    int retval;
    fd_set rfds, wfds;
    struct timeval tv;

    (void)retval;
    (void)wfds;
    (void)tv;
    // printf("real address : %s ;address : %s\n", real_address, address);

    addr.sin_family = AF_INET;
    addr.sin_port = 0;
    addr.sin_addr.s_addr = inet_addr(address);

    
    trace = init_trace();

    sock = create_socket();
    if (sock == -1) {
        return 1;
    }

    printf("traceroute to %s (%s), 30 hops max, 60 byte packets\n", real_address, address);
    
    while (ttl < 31) {

        craft_icmp_packet(packet, trace, 170);

        set_ttl(sock, ttl);

        int bytes = 0;

        FD_ZERO(&rfds);
        FD_SET(sock, &rfds);
        FD_ZERO(&wfds);
        FD_SET(sock, &wfds);

        tv.tv_sec = 1;
        tv.tv_usec = 0;

        // printf("First select called\n");
        retval = select(sock + 1, NULL,  &wfds, NULL, &tv);
        if (retval == -1) {
            perror("select()");
            continue;
        }
        else if (retval == 0) {
            fprintf(stderr, "ft_traceroute: sending packet: failed sendto() returned : %d\n", bytes);
            return 1;
        }
        else {
            // printf("FIRST SELECT SUCCESSFULL\n");
            unsigned long int og_time = getTimeStamp();

            bytes = sendto(sock, packet, 60, 0, (const struct sockaddr *)&addr, sizeof(addr));
            // dump_packet(packet + 27, 60);
            // printf("Sended %d bytes\n", bytes);
            if (bytes <= 0) {
                fprintf(stderr, "ft_traceroute: sending packet: failed sendto() returned : %d\n", bytes);
                return 1;
            }

            while (true) {
                // printf("SECOND SELECT CALLED\n");
                retval = select(sock + 1, &rfds,  NULL, NULL, &tv);
                if (retval == -1) {
                    perror("select()");
                    break;
                }
                else if (retval == 0) {
                    printf("%2d *\n", ttl);
                    break;
                }
                else {
                    // printf("SECOND SELECT SUCCESSFULL\n");
                    bytes = recvfrom(sock, recv_buf, 1024, 0, 0, NULL);
                    // printf("Received %d bytes\n", bytes);
                    // dump_packet(recv_buf + 54, bytes);
                    // printf("Sequence : %d\n", get_packet_sequence(recv_buf, bytes));
                    if (bytes > 0) {
                        // print_icmp_header(recv_buf + 20, bytes);
                        t_response response = parse_response(recv_buf, bytes);
                        // printf("RESPONSE IS TYPE : %d\n", response.type);
                        // printf("RES ID : %d --- PID : %d\n", response.id, trace.id);
                        if (response.id != trace.id) {
                            printf("OTHER ICMP PACKET\n");
                            free(response.address);
                            continue;
                        }
                        unsigned long int actual_time = getTimeStamp();
                        unsigned long int time_diff = actual_time - og_time;
                        printf("%2d %s (%s) %.3f ms\n", ttl, response.address, response.address, (float)time_diff / 1000);
                        if (response.type == 0) {
                            return 0;
                        }
                        break;
                    }
                    else {
                        printf("PACKET SIZE TOO LITTLE, NOT PARSING IT\n");
                    }
                }
            }
        }
        ttl++;
    }

    return 0;
}

