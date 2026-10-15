#include "ft_traceroute.h"

int reverse_dns_lookup(const struct sockaddr_in *addr,
                        char hostname[MAX_HOST_SIZE])
{
    return getnameinfo((const struct sockaddr *)addr,
                       sizeof(*addr),
                       hostname,
                       MAX_HOST_SIZE,
                       NULL,
                       0,
                       0);
}

void    get_ip_infos(t_traceroute_data *data)
{
    const char *node = data->ip_address;
    const char *service = NULL;
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_DGRAM,
        .ai_protocol = IPPROTO_UDP
    };
    struct addrinfo *res;

    int gai_error = getaddrinfo(node,
                                service,
                                &hints,
                                &res);
    if (gai_error != 0 || res == NULL)
    {
        fprintf(stderr, "%s: %s\n", node, gai_strerror(gai_error));
        print_args_error("Cannot handle \"host\" cmdline arg",
                        node);
        clean_exit(EXIT_FAILURE);
    }

    const struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
    if (inet_ntop(AF_INET, &addr->sin_addr, data->ip, sizeof(data->ip)) == NULL)
    {
        perror("inet_ntop");
        freeaddrinfo(res);
        clean_exit(EXIT_FAILURE);
    }
    data->addr = *addr;
    freeaddrinfo(res);
}