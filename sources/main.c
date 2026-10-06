#include "ft_traceroute.h"

int main(int argc, char **argv)
{
    t_traceroute_data data = {0};

    /* No arguments print the helper */
    if (argc == 1)
        print_helper();

    /* Parsing the arguments */
    parse_args(&data, argc, argv);

    return EXIT_SUCCESS;
}