#include "ft_traceroute.h"

t_args args;


void print_args(char *address) {
    printf("first_ttl : %ld\n", args.first_ttl);
    printf("max_ttl : %ld\n", args.max_ttl);
    printf("tos : %ld\n", args.tos);
    printf("Address is %s\n", address);
}

void init_args() {
    args.first_ttl  =  1;
    args.max_ttl    =  30;
    args.tos        =  0;
}

char  *parse_args(int ac, char **av) {
    int c;
    int option_index = 0;
    static struct option long_options[] =
        {
            {"help",        no_argument,        0,     'h'},
            {"first",       required_argument,  0,     'f'},
            {"max-hops",    required_argument,  0,     'm'},
            {"tos",         required_argument,  0,     't'},
            {0, 0, 0, 0}
        };

    while ( (c = getopt_long(ac, av, "hf:m:t:", long_options, &option_index)) != -1) {
        // printf("c value : %d : %c\n", c, c);
        switch (c) {
            case 'f' :
                // printf("option -s with value %s\n", optarg);
                if (!is_num(optarg)) {
                    fprintf(stderr, "./ft_traceroute: option with invalid argument\n");
                    return NULL;
                }
                args.first_ttl = ft_atoi(optarg);
                break;
            case 'm' :
                // printf("option -c with value %s\n", optarg);
                if (!is_num(optarg)) {
                    fprintf(stderr, "./ft_traceroute: option with invalid argument\n");
                    return NULL;
                }
                args.max_ttl = ft_atoi(optarg);
                if (args.max_ttl > 255) {
                    fprintf(stderr, "max hops cannot be more than 255\n");
                    return NULL;
                }
                break;
            case 't' :
                // printf("option -i with value %s\n", optarg);
                if (!is_num(optarg)) {
                    fprintf(stderr, "./ft_traceroute: option with invalid argument\n");
                    return NULL;
                }
                args.tos = ft_atoi(optarg);
                break;
            case 'h' :
                print_help();
                return NULL;
                break;
            default :
                return NULL;
        }
    }
    if (args.first_ttl > args.max_ttl || args.first_ttl == 0) {
        fprintf(stderr, "first hop out of range\n");
        return NULL;
    }
    if (av[optind] == NULL) {
        fprintf(stderr, "Specify \"host\" missing argument.\n");
    }
    return av[optind];
}

int main(int ac, char **av) {
    char *ip;
    int ret = 1;
    char *address;

    if (ac == 1) {
        return print_help();
    }

    init_args();

    address = parse_args(ac, av);

    if (address == NULL) {
        return 1;
    }
    // print_args(address);

    ip = get_ip_address_from_domain(address);
    if (!ip) {
        fprintf(stderr, "Cannot handle \"host\" cmdline arg `%s' on position 1 (argc 1)\n", address);
        return ret;
    }

    ret = ft_traceroute(address, ip);

    free(ip);

    return ret;
}