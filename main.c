#include "ft_traceroute.h"



int main(int ac, char **av) {
    if (ac == 1) {
        return print_help();
    }
    return ft_traceroute(av[1], av[1]);
}