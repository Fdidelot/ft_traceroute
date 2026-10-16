#include "ft_traceroute.h"

static int poll_reply(int fd, int timeout_duration)
{
    struct pollfd pfd = 
    {
        .fd = fd,
        .events = POLLIN
    };
    return poll(&pfd, 1, timeout_duration);
}

static int parse_icmp_reply(size_t reply_size, unsigned char packet[PACKET_REPLY_SIZE],
                            struct icmphdr *icmp, struct sockaddr_in *target,
                            in_port_t expected_port)
{
    if ((size_t)reply_size < sizeof(struct iphdr))
        return INVALID_REPLY;

    struct iphdr outer_ip;
    memcpy(&outer_ip, packet, sizeof(outer_ip));

    size_t outer_len = outer_ip.ihl * 4U;
    if (outer_ip.ihl < 5 ||
        (size_t)reply_size < outer_len + sizeof(struct icmphdr))
        return INVALID_REPLY;

    memcpy(icmp, packet + outer_len, sizeof(*icmp));

    if (icmp->type != ICMP_TIME_EXCEEDED &&
        icmp->type != ICMP_DEST_UNREACH)
        return INVALID_REPLY;
        
    size_t quote_offset = outer_len + sizeof(struct icmphdr);
    if ((size_t)reply_size < quote_offset + sizeof(struct iphdr))
        return INVALID_REPLY;

    const unsigned char *quote = packet + quote_offset;
    struct iphdr original_ip;
    memcpy(&original_ip, quote, sizeof(original_ip));

    if (original_ip.ihl < 5 || original_ip.protocol != IPPROTO_UDP)
        return INVALID_REPLY;

    size_t original_ip_len = original_ip.ihl * 4U;
    if ((size_t)reply_size < quote_offset + original_ip_len +
                    sizeof(struct udphdr))
        return INVALID_REPLY;

    struct udphdr original_udp;
    memcpy(&original_udp, quote + original_ip_len,
            sizeof(original_udp));

    if (original_ip.daddr != target->sin_addr.s_addr ||
        original_udp.dest != expected_port)
        return INVALID_REPLY;
    return SUCCESS;
}

static int wait_for_reply(t_traceroute_data *data, t_probes_info *probe)
{
    struct timespec start;
    get_time(&start);

    int                 icmp_fd = data->icmp_fd;
    struct sockaddr_in  *target = &data->addr;
    in_port_t           expected_port = data->addr.sin_port;

    while (1)
    {
        struct timespec now;
        get_time(&now);

        long elapsed_ms = time_diff(start, now);

        int remaining_ms = PROBE_TIMEOUT_MS - (int)elapsed_ms;
        if (remaining_ms <= 0)
            return TIMEOUT;

        int ready = poll_reply(icmp_fd, remaining_ms);
        if (ready == TIMEOUT || ready == ERROR)
            return ready;

        unsigned char packet[PACKET_REPLY_SIZE];
        struct sockaddr_in from = {0};
        socklen_t from_len = sizeof(from);

        ssize_t n = recvfrom(icmp_fd, packet, sizeof(packet), 0,
                             (struct sockaddr *)&from, &from_len);
        if (n == -1)
            return ERROR;

        struct icmphdr icmp_header;
        if (parse_icmp_reply((size_t)n, packet, &icmp_header,
                                target, expected_port) == INVALID_REPLY)
            continue;

        probe->router = from;
        probe->type = icmp_header.type;
        probe->code = icmp_header.code;
        return SUCCESS;
    }
}

void    send_probes(t_traceroute_data *data)
{
    unsigned char packet[data->packet_len - PACKET_SIZE_MIN];
    memset(packet, 0, data->packet_len - PACKET_SIZE_MIN);

    t_probes_info       probes[data->probes_per_hops];
    struct timespec     send_at;
    struct timespec     received_at;
    unsigned int        port = data->port;

    int total_probes = 0;
    for (int ttl = 1; ttl <= data->max_hops; ttl++)
    {
        memset(probes, 0, data->probes_per_hops * sizeof(*probes));

        for (size_t nb_probes = 0; nb_probes < data->probes_per_hops; nb_probes++, total_probes++)
        {
            if (setsockopt(data->udp_fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) == -1)
                print_error("setsockopt IP_TTL");

            data->addr.sin_port = htons(port + ttl + total_probes);
            get_time(&send_at);
            
            if (sendto(data->udp_fd, packet, sizeof(packet), 0,
                        (struct sockaddr *)&data->addr, sizeof(data->addr)) == -1)
                print_error("sendto");

            int result = wait_for_reply(data, &probes[nb_probes]);
            if (result == ERROR)
                print_error("poll");
            if (result == TIMEOUT)
            {
                probes[nb_probes].timeout = true;
                continue ;
            }

            if (inet_ntop(AF_INET, &probes[nb_probes].router.sin_addr,
                            probes[nb_probes].router_ip,
                            sizeof(probes[nb_probes].router_ip)) == NULL)
                print_error("inet_ntop router");

            get_time(&received_at);
            probes[nb_probes].rtt_ms = time_diff(send_at, received_at);
        }
        print_response(probes, ttl, data->probes_per_hops);
        if (probes[0].router.sin_addr.s_addr == data->addr.sin_addr.s_addr &&
            probes[0].type == ICMP_DEST_UNREACH &&
            probes[0].code == ICMP_PORT_UNREACH)
            break;
    }
}