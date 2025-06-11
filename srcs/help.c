#include "../ft_traceroute.h"

bool print_help() {
    printf("Usage:\n  traceroute [host]\n  --help              Read this help and exit\n  -f first_ttl, --first=first_ttl\n                      Specifies with what TTL to start. Defaults to 1.\n  -m max_ttl, --max-hops=max_ttl\n                      Specifies  the  maximum  number of hops (max time-to-live value) traceroute will probe. The default is 30.\n  -t tos, --tos=tos\n                      For IPv4, set the Type of Service (TOS) and Precedence value.\nArgument:\n+     host          The host to traceroute to\n");
    return 0;
}