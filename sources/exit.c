#include "ft_traceroute.h"

extern t_traceroute_data data;

void clean_exit(int exit_code)
{
    if (data.udp_fd)
        close(data.udp_fd);
    if (data.icmp_fd)
        close(data.icmp_fd);
    exit(exit_code);
}