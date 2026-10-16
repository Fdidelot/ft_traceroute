#include "ft_traceroute.h"

static size_t string_to_number(const char *str, const char *error_msg)
{
    char *end;
    long value;

    value = strtol(str, &end, 10);

    // No digits were found
    if (end == str ||
        errno == ERANGE ||
        value < INT_MIN || 
        value > INT_MAX ||
        *end != '\0' ||
        value < 0)
    {
        fprintf(stderr, "%s `%s'\n", error_msg, str);
        clean_exit(EXIT_FAILURE);
    }
    return value;
}

static int parse_options(int argc, char **argv, t_traceroute_data *data)
{
    int         opt;
    const char  *options = "p:m:q:";
    extern int  optind;
    extern int  optopt;
    extern int  opterr;

    opterr = 0;

    static struct option long_options[] = {
        {"help", no_argument, NULL,  OPT_HELP },
        {"port", required_argument, NULL,  'p' },
        {"max-hops", required_argument, NULL,  'm' },
        {"queries", required_argument, NULL,  'q' },
        {NULL,   0,           NULL,  0 }
    };

    while ((opt = getopt_long(argc, argv, options, long_options, NULL)) != -1)
    {
        switch (opt)
        {
            case OPT_HELP:
                print_helper();
                break ;
            case 'p':
                data->port = string_to_number(optarg, "Cannot handle `-p' option with arg");
                break ;
            case 'm':
                size_t hops = string_to_number(optarg, "Cannot handle `-m' option with arg");
                if (hops > 255)
                    print_error_msg("max hops cannot be more than 255\n");
                if (hops == 0)
                    print_error_msg("first hop out of range\n");
                data->max_hops = hops;
                break ;
            case 'q':
                size_t probes = string_to_number(optarg, "Cannot handle `-q' option with arg");
                if (probes > 10)
                    print_error_msg("no more than 10 probes per hop\n");
                data->probes_per_hops = probes;
                break ;
            default:
                print_args_error("Bad option", *(argv + optind) == NULL ? *(argv + optind - 1): *(argv + optind));
                break;
        }
    }
    return optind;
}

void     parse_args(t_traceroute_data *data, int argc, char **argv)
{
    int nb_opts = parse_options(argc, argv, data);
    int nb_args = argc - nb_opts;

    if (nb_args > 2)
        print_args_error("Extra args", *(argv + optind + 2));

    data->ip_address = *(argv + optind);
    if (nb_args == 2)
    {
        size_t value = string_to_number(*(argv + optind + 1), "Invalid packetlen:");
        if (value > PACKET_SIZE_MAX)
        {
            fprintf(stderr, "too big packetlen %s specified\n", *(argv + optind + 1));
            clean_exit(EXIT_FAILURE);
        }
        data->packet_len = value < PACKET_SIZE_MIN ? PACKET_SIZE_MIN: value;
    }
}