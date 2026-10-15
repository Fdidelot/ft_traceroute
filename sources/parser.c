#include "ft_traceroute.h"

static int parse_options(int argc, char **argv)
{
    int         opt;
    const char  *options = "";
    extern int  optind;
    extern int  optopt;
    extern int  opterr;

    opterr = 0;

    static struct option long_options[] = {
        {"help", no_argument, NULL,  OPT_HELP },
        {NULL,   0,           NULL,  0 }
    };

    while ((opt = getopt_long(argc, argv, options, long_options, NULL)) != -1)
    {
        switch (opt)
        {
            case OPT_HELP:
                print_helper();
                break ;
            default:
                print_args_error("Bad option", *(argv + optind) == NULL ? *(argv + optind - 1): *(argv + optind));
                break;
        }
    }
    return optind;
}

static size_t string_to_number(const char *str)
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
        fprintf(stderr, "Invalid packetlen: %s\n", str);
        clean_exit(EXIT_FAILURE);
    }
    return value;
}

void     parse_args(t_traceroute_data *data, int argc, char **argv)
{
    int nb_opts = parse_options(argc, argv);
    int nb_args = argc - nb_opts;

    if (nb_args > 2)
        print_args_error("Extra args", *(argv + optind + 2));

    data->ip_address = *(argv + optind);
    if (nb_args == 2)
    {
        size_t value = string_to_number(*(argv + optind + 1));
        if (value > PACKET_SIZE_MAX)
        {
            fprintf(stderr, "too big packetlen %s specified\n", *(argv + optind + 1));
            clean_exit(EXIT_FAILURE);
        }
        data->packet_len = value < PACKET_SIZE_MIN ? PACKET_SIZE_MIN: value;
    }
}