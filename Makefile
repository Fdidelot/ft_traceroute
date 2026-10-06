NAME = ft_traceroute

PATH_OBJS	=	./objects/
PATH_SRC	=	./sources/
PATH_HEAD	= ./headers/

SRCS	=	main.c \
			print.c \
			parser.c

vpath %.c $(PATH_SRC)

HEADERS = ft_traceroute.h
vpath %.h $(PATH_HEAD)

OBJS	=	$(addprefix $(PATH_OBJS),  $(SRCS:.c=.o))

RM	=	rm -rf

CC			=	gcc
CFLAGS		+=	-Wall -Wextra -Werror
CFLAGS		+=	-I $(PATH_HEAD)
# LDLIBS		=
all: $(NAME)

clean:
	$(RM) $(PATH_OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(PATH_OBJS)%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $< $(CFLAGS)

$(OBJS): Makefile $(HEADERS) | $(PATH_OBJS)

$(PATH_OBJS):
	mkdir $@

.PHONY: all clean fclean re test
