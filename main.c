#include "ft_traceroute.h"



int main(int ac, char **av) {
    char *ip;

    if (ac == 1) {
        return print_help();
    }

    ip = get_ip_address_from_domain(av[1]);
    if (!ip) {
        fprintf(stderr, "Cannot handle \"host\" cmdline arg `%s' on position 1 (argc 1)\n", av[1]);
        return 1;
    }


    return ft_traceroute(av[1], ip);
}