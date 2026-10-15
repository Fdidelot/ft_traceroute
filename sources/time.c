#include "ft_traceroute.h"

void    get_time(struct timespec *time_struct)
{
    if (clock_gettime(CLOCK_MONOTONIC, time_struct) == -1)
    {
        perror("clock_gettime");
        clean_exit(EXIT_FAILURE);
    }
}

double  time_diff(struct timespec from, struct timespec to)
{
    return (
        (to.tv_sec - from.tv_sec) * 1000.0 +
        (to.tv_nsec - from.tv_nsec) / 1000000.0
    );
}
