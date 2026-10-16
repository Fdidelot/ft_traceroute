#include "ft_traceroute.h"

void	print_helper(void)
{
    static const char help_message[] =
    {
        "Usage:\n  ft_traceroute [options] host\n\
Options:\n\
  -p port  --port=port        Set the destination port to use.\n\
                              (incremented by each probe, default is 33434)\n\
  -m max_ttl  --max-hops=max_ttl\n\
                              Set the max number of hops (max TTL to be\n\
                              reached). Default is 30\n\
  -q nqueries  --queries=nqueries\n\
                              Set the number of probes per each hop. Default is\n\
                              3\n\
  --help                      Read this help and exit\n\n\
Arguments:\n\
+     host          The host to traceroute to\n\
      packetlen     The full packet length (default is the length of an IP\n\
                    header plus 40). Can be ignored or increased to a minimal\n\
                    allowed value\n"
    };
	printf("%s", help_message);
    exit(EXIT_SUCCESS);
}

void    print_args_error(const char *error_type, 
                        const char *arg) 
{
    fprintf(stderr, "%s `%s'\n",
            error_type, arg);
    clean_exit(WRONG_ARGUMENTS);
}

void    print_intro(const char *ip, const char *name, 
                    const int max_hops, size_t packet_len)
{
    printf("ft_traceroute to %s (%s), %d hops max, %zu byte packets\n",
            ip, name, max_hops, packet_len);
}

void    print_error(const char *msg)
{
    perror(msg);
    clean_exit(EXIT_FAILURE);
}

void    print_error_msg(const char *msg)
{
    fprintf(stderr, "%s",msg);
    clean_exit(EXIT_FAILURE);
}

void    print_response(t_probes_info *probes, int ttl, size_t max_probes)
{
    char *ip = NULL;

    printf("%2d ", ttl);
    for (size_t i = 0; i < max_probes; i++)
    {
        if (probes[i].timeout)
            printf(" *");
        else
        {
            if (!ip || strncmp(ip, probes[i].router_ip, INET_ADDRSTRLEN))
            {
                ip = probes[i].router_ip;
                char hostname[MAX_HOST_SIZE];
                reverse_dns_lookup(&probes[i].router, hostname);
                printf(" %s (%s)", hostname, ip);
            }
            printf("  %.3f ms", probes[i].rtt_ms);
        }
    }
    printf("\n");
}
