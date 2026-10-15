#include "ft_traceroute.h"

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

        struct pollfd pfd = 
        {
            .fd = icmp_fd,
            .events = POLLIN
        };
        int ready = poll(&pfd, 1, remaining_ms);

        if (ready == 0)
            return TIMEOUT;
        if (ready == -1)
        {
            if (errno == EINTR)
                continue;
            return ERROR;
        }

        unsigned char packet[PACKET_REPLY_SIZE];
        struct sockaddr_in from = {0};
        socklen_t from_len = sizeof(from);

        ssize_t n = recvfrom(icmp_fd, packet, sizeof(packet), 0,
                             (struct sockaddr *)&from, &from_len);
        if (n == -1)
        {
            if (errno == EINTR)
                continue;
            return ERROR;
        }

        if ((size_t)n < sizeof(struct iphdr))
            continue;

        struct iphdr outer_ip;
        memcpy(&outer_ip, packet, sizeof(outer_ip));

        size_t outer_len = outer_ip.ihl * 4U;
        if (outer_ip.ihl < 5 ||
            (size_t)n < outer_len + sizeof(struct icmphdr))
            continue;

        struct icmphdr icmp;
        memcpy(&icmp, packet + outer_len, sizeof(icmp));

        if (icmp.type != ICMP_TIME_EXCEEDED &&
            icmp.type != ICMP_DEST_UNREACH)
            continue;
         
        size_t quote_offset = outer_len + sizeof(struct icmphdr);
        if ((size_t)n < quote_offset + sizeof(struct iphdr))
            continue;

        const unsigned char *quote = packet + quote_offset;
        struct iphdr original_ip;
        memcpy(&original_ip, quote, sizeof(original_ip));

        if (original_ip.ihl < 5 || original_ip.protocol != IPPROTO_UDP)
            continue;

        size_t original_ip_len = original_ip.ihl * 4U;
        if ((size_t)n < quote_offset + original_ip_len +
                        sizeof(struct udphdr))
            continue;

        struct udphdr original_udp;
        memcpy(&original_udp, quote + original_ip_len,
               sizeof(original_udp));

        if (original_ip.daddr != target->sin_addr.s_addr ||
            original_udp.dest != expected_port)
            continue;

        (*probe).router = from;
        (*probe).type = icmp.type;
        (*probe).code = icmp.code;
        return SUCCESS;
    }
}

void    send_probes(t_traceroute_data *data)
{
    unsigned char packet[data->packet_len - PACKET_SIZE_MIN];
    memset(packet, 0, data->packet_len - PACKET_SIZE_MIN);

    t_probes_info       probes[data->max_probes];
    struct timespec     send_at;
    struct timespec     received_at;

    int total_probes = 0;
    for (int ttl = 1; ttl <= MAX_HOPS; ttl++)
    {
        memset(probes, 0, data->max_probes * sizeof(*probes));

        for (size_t nb_probes = 0; nb_probes < data->max_probes; nb_probes++, total_probes++)
        {
            if (setsockopt(data->udp_fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) == -1)
                print_error("setsockopt IP_TTL");

            data->addr.sin_port = htons(FIRST_PORT + ttl + total_probes);
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
        print_response(probes, ttl, data->max_probes);
        if (probes[0].router.sin_addr.s_addr == data->addr.sin_addr.s_addr &&
            probes[0].type == ICMP_DEST_UNREACH &&
            probes[0].code == ICMP_PORT_UNREACH)
            break;
    }
}