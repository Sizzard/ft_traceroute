NAME = ft_traceroute
SRCS = main.c srcs/help.c srcs/ft_traceroute.c srcs/debug.c srcs/utils.c srcs/icmp.c
OBJ = $(SRCS:.c=.o)
CFLAGS = -Wall -Wextra -Werror -g3

all: $(NAME)

$(NAME): $(OBJ)
	cc $(CFLAGS) -o $(NAME) $(OBJ)

%.o:%.c ft_traceroute.h
	cc $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
