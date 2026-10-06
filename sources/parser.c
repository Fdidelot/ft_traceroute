#include "ft_traceroute.h"

static int parse_options(int argc, char **argv)
{
    int         opt;
    const char  *options = "";
    extern int  optind;

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
            default:
                break;
        }
    }
    return optind;
}

void     parse_args(t_traceroute_data *data, int argc, char **argv)
{
    int nb_opts = parse_options(argc, argv);
    int nb_args = argc - nb_opts;

    if (nb_args > 2)
        print_args_error(*(argv + optind + 2), nb_opts + 2);

    (void)data;
}