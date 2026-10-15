#include "ft_traceroute.h"

void    open_sockets(t_traceroute_data *data)
{
    data->icmp_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (data->icmp_fd == -1)
    {
        perror("socket ICMP");
        clean_exit(EXIT_FAILURE);
    }
    data->udp_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (data->udp_fd == -1)
    {
        perror("socket UDP");
        clean_exit(EXIT_FAILURE);
    }
}