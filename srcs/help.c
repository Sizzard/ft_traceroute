#include "../ft_traceroute.h"

bool print_help() {
    printf("Usage:\n  traceroute [host]\n  --help                      Read this help and exit\nArgument:\n+     host          The host to traceroute to\n");
    return 0;
}