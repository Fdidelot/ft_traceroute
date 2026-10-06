#include "ft_traceroute.h"

void	print_helper(void)
{
    static const char help_message[] =
    {
        "Usage:\n  ft_traceroute [options] host\n\
Options:\n\
  --help                      Read this help and exit\n\n\
Arguments:\n\
+     host          The host to traceroute to\n"
    };
	printf("%s", help_message);
    exit(EXIT_SUCCESS);
}

void    print_args_error(const char *wrong_arg, int index)
{
    fprintf(stderr, "Extra arg `%s' (position 3, argc %d)\n", wrong_arg, index);
    exit(WRONG_ARGUMENTS);
}

