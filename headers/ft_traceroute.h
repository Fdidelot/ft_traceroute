#ifndef FT_TRACEROUTE_H
# define FT_TRACEROUTE_H

# include <stdlib.h>
# include <stdio.h>
# include <getopt.h>
# include <stdint.h>

# define WRONG_ARGUMENTS 2

enum
{
    OPT_HELP = 0x01,
};

typedef struct traceroute_data
{
    // uint8_t     options;
    char        **args;
} t_traceroute_data;

void    print_helper(void);
void    parse_args(t_traceroute_data *, int, char **);
void    print_args_error(const char *, int);


#endif