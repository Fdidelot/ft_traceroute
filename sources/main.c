#include "ft_traceroute.h"

t_traceroute_data data =
{ 
    .packet_len = PACKET_SIZE,
    .probes_per_hops = DEFAULT_MAX_PROBES,
    .max_hops = DEFAULT_MAX_HOPS,
    .port = DEFAULT_PORT,
    .start_hop = DEFAULT_START_HOP
};

static void init(void)
{
    if (data.start_hop == 0 || data.start_hop > data.max_hops)
        print_error_msg("first hop out of range\n");
    get_ip_infos(&data);
    open_sockets(&data);
}

int main(int argc, char **argv)
{
    /* No arguments print the helper */
    if (argc == 1)
        print_helper();

    /* Parsing the arguments */
    parse_args(&data, argc, argv);

    init();

    print_intro(data.ip_address, data.ip, data.max_hops, data.packet_len);
    send_probes(&data);

    clean_exit(EXIT_SUCCESS);
}