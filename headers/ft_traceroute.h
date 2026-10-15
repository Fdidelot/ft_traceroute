#ifndef FT_TRACEROUTE_H
# define FT_TRACEROUTE_H

# include <stdlib.h>
# include <stdio.h>
# include <getopt.h>
# include <stdint.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <string.h>
# include <unistd.h>
# include <time.h>
# include <errno.h>
# include <poll.h>
# include <netinet/ip_icmp.h>
# include <netinet/udp.h>
# include <limits.h>
# include <stdbool.h>

# define MAX_HOPS 30
# define FIRST_PORT 33434
# define PROBE_TIMEOUT_MS 1000

# define PACKET_SIZE 60
# define PACKET_SIZE_MIN 28
# define PACKET_SIZE_MAX 65000
# define PACKET_REPLY_SIZE 2048
# define MAX_HOST_SIZE 256

# define DEFAULT_MAX_PROBES 3

# define WRONG_ARGUMENTS 2
# define ERROR -1
# define SUCCESS 1
# define TIMEOUT 0

enum
{
    OPT_HELP = 0x01,
};

typedef struct probes_info
{
    bool                timeout;
    double              rtt_ms;
    struct  sockaddr_in router;
    char                router_ip[INET_ADDRSTRLEN];
    unsigned char       type;
    unsigned char       code;
} t_probes_info;

typedef struct traceroute_data
{
    char                *ip_address;
    char                ip[INET_ADDRSTRLEN];
    struct sockaddr_in  addr;

    int icmp_fd;
    int udp_fd;

    size_t  packet_len;
    size_t  max_probes;
} t_traceroute_data;

void    print_helper(void);
void    print_args_error(const char *, 
                        const char *);
void    print_intro(const char *, const char *, 
                    const int, size_t);
void    print_error(const char *);
void    print_response(t_probes_info *, int , size_t );

void    parse_args(t_traceroute_data *, int, char **);

void    get_ip_infos(t_traceroute_data *);
int     reverse_dns_lookup(const struct sockaddr_in *, char hostname[MAX_HOST_SIZE]);
void    open_sockets(t_traceroute_data *);
void    send_probes(t_traceroute_data *);

void    get_time(struct timespec *);
double  time_diff(struct timespec, struct timespec);

void    clean_exit(int );

#endif