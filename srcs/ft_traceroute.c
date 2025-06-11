#include "../ft_traceroute.h"


char *get_dest_address(struct iphdr *ip) {
    struct sockaddr_in src;
    char *src_addr;

    src.sin_addr.s_addr = ip->saddr;
    src_addr = inet_ntoa(src.sin_addr);
    
    return ft_strdup(src_addr);
}

t_response parse_response(void *buf, int bytes) {
    struct iphdr *ip = buf;
    struct icmphdr* icmp = buf + 20;
    t_response response = {0};
    (void)bytes;

    response.type = icmp->type;
    response.address = get_dest_address(ip);
    if (response.type != 0) {
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
    if (setsockopt(sock, IPPROTO_IP, IP_TOS, &args.tos, sizeof(args.tos)) == -1) {
        fprintf(stderr, "ft_traceroute: Failed to change tos value\n");
        return -1;   
    }
    return 0;
}

void init_traces(t_trace *traces) {
    int pid = getpid();

    for (size_t i = 0; i < 3; i++) {
        traces[i].pid = pid;
        traces[i].id = i;
        traces[i].responded = false;
    }
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

int triple_sent(t_trace *traces, struct sockaddr_in addr, int sock, char *packet) {
    int bytes_sent;

    for (size_t i = 0; i < 3; i++) {
        craft_icmp_packet(packet, traces[i]);
        traces[i].timestamp = getTimeStamp();
        bytes_sent = sendto(sock, packet, 60, 0, (const struct sockaddr *)&addr, sizeof(addr));
        if (bytes_sent == -1) {
            fprintf(stderr, "ft_traceroute: sending packet failed : %d\n", bytes_sent);
            return -1;
        }
    }
    return 0;
}

bool are_three_ok(t_trace *traces) {
    // printf("CHECK IS %d\n", traces[0].responded && traces[1].responded && traces[2].responded);
    return (traces[0].responded && traces[1].responded && traces[2].responded);
}

t_times get_time_diff(t_trace *traces) {
    t_times times;

    unsigned long int actual_time = getTimeStamp();
    unsigned long int time_diff = actual_time - traces[0].timestamp;
    times.one = (float)time_diff  / 1000;

    actual_time = getTimeStamp();
    time_diff = actual_time - traces[1].timestamp;
    times.two = (float)time_diff  / 1000;

    actual_time = getTimeStamp();
    time_diff = actual_time - traces[2].timestamp;
    times.three = (float)time_diff  / 1000;

    return times;
}

int recv_loop(char *recv_buf, t_trace *traces, int sock, int ttl) {
    fd_set rfds;
    struct timeval tv;
    int retval;

    tv.tv_sec = 1;
    tv.tv_usec = 0;

    while (true) {
            FD_ZERO(&rfds);
            FD_SET(sock, &rfds);

            // printf("SELECT CALLED\n");
            retval = select(sock + 1, &rfds,  NULL, NULL, &tv);
            if (retval == -1) {
                fprintf(stderr, "select() failed\n");
                break;
            }
            else if (retval == 0) {
                printf("%2d * * *\n", ttl);
                break;
            }
            else {
                // printf("SECOND SELECT SUCCESSFULL\n");
                int bytes = recvfrom(sock, recv_buf, 1024, 0, 0, NULL);
                // printf("Received %d bytes\n", bytes);
                // dump_packet(recv_buf + 54, bytes);
                if (bytes > 0) {
                    // print_icmp_header(recv_buf + 20, bytes);
                    // dump_ip_header(recv_buf);
                    // dump_ip_header(recv_buf + 28);
                    // dump_packet(recv_buf, bytes);
                    t_response response = parse_response(recv_buf, bytes);
                    // printf("RESPONSE IS TYPE : %d\n", response.type);
                    // printf("RES ID : %d --- PID : %d\n", response.id, trace.id);
                    if (response.id != traces[0].pid) {
                        // printf("OTHER ICMP PACKET\n");
                        free(response.address);
                        continue;
                    }

                    int sequence = get_packet_sequence(recv_buf, bytes);
                    traces[sequence].responded = true;
                    if (response.type == 0 && are_three_ok(traces) == true) {
                        t_times times = get_time_diff(traces);
                        printf("%2d %s (%s)  %.3f ms  %.3f ms  %.3f ms\n", ttl, response.address, response.address, times.one, times.two, times.three);
                        free(response.address);
                        return 1;
                    }
                    if (are_three_ok(traces) == true) {
                        t_times times = get_time_diff(traces);
                        printf("%2d %s (%s)  %.3f ms  %.3f ms  %.3f ms\n", ttl, response.address, response.address, times.one, times.two, times.three);
                        free(response.address);
                        break;
                    }
                    free(response.address);
                }
                else {
                    printf("PACKET SIZE TOO LITTLE, NOT PARSING IT\n");
                }
            }
    }
    return 0;
}

int ft_traceroute(char *real_address, char *address) {
    char packet[1024] = {0}; 
    char recv_buf[1024] = {0}; 
    struct sockaddr_in addr;
    t_trace traces[3];
    int sock;

    // printf("real address : %s ;address : %s\n", real_address, address);

    addr.sin_family = AF_INET;
    addr.sin_port = 0;

    if (inet_pton(AF_INET, address, &addr.sin_addr) != 1) {
        fprintf(stderr, "inet_pton() failed\n");
        return 1;
    }

    sock = create_socket();
    if (sock == -1) {
        return 1;
    }

    printf("traceroute to %s (%s), %ld hops max, 60 byte packets\n", real_address, address, args.max_ttl);
    
    for (size_t ttl = args.first_ttl; ttl <= args.max_ttl; ttl++) {

        init_traces(traces);

        if (set_ttl(sock, ttl)) {
            close(sock);
            return 1;
        }

        triple_sent(traces, addr, sock, packet);

        if (recv_loop(recv_buf, traces, sock, ttl) == 1) {
            close(sock);
            return 0;
        }
    }

    close(sock);
    return 0;
}

