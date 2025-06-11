#include "../ft_traceroute.h"


char *get_ip_address(struct sockaddr *addr) {
    if (addr->sa_family == AF_INET) {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)addr;
        return ft_strdup(inet_ntoa(ipv4->sin_addr));
    }
	else {
        fprintf(stderr, "Unsupported address family\n");
        return NULL;
    }
}

char *get_ip_address_from_domain(char *address) {
    struct addrinfo hints, *result, *rp;
    char *res;
    int s;
    
    ft_memset(&hints, 0, sizeof(struct addrinfo));
    
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;

    s = getaddrinfo(address, NULL, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "%s: Temporary failure in name resolution\n", address);
        return NULL;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        res = get_ip_address(rp->ai_addr);
    }

    freeaddrinfo(result);
    return res;
}

unsigned long getTimeStamp(void) {
    struct timeval tv;
    unsigned long time;
    
    gettimeofday(&tv, NULL);

    time = 1000000 * tv.tv_sec + tv.tv_usec;

    return time;
}

bool is_num(char *str) {
    while(*str) {
        if (*str < '0' || *str > '9') {
            return false;
        }
        str++;
    }
    return true;
}

void	*ft_memset(void *s, int c, size_t n)
{
	size_t			i;
	unsigned char	*p;

	p = s;
	i = 0;
	while (i < n)
	{
		p[i] = c;
		i++;
	}
	return (s);
}

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

char	*ft_strdup(const char *s)
{
	int		i;
	char	*str;

	i = 0;
	str = malloc(ft_strlen(s) + 1);
	if (str == NULL)
		return (NULL);
	while (s[i])
	{
		str[i] = s[i];
		i++;
	}
	str[i] = 0;
	return (str);
}

int	ft_atoi(const char *nptr)
{
	int	i;
	int	res;
	int	min;

	res = 0;
	i = 0;
	min = 0;
	while (nptr[i] == ' ' || (nptr[i] >= 9 && nptr[i] <= 13))
		i++;
	if (nptr[i] == '-')
	{
		min += 1;
		i++;
	}
	else if (nptr[i] == '+')
		i++;
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		res = res * 10 + (nptr[i] - 48);
		i++;
	}
	if (min > 0)
		return (-res);
	return (res);
}