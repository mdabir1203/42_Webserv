CC = g++
CFLAGS = -Wall -Wextra -Werror -std=c++98
NAME = webserv

all: $(NAME)

$(NAME): webserv.cpp config.cpp request.cpp response.cpp utils.cpp
    $(CC) $(CFLAGS) -o $(NAME) $^

clean:
    rm -f $(NAME)

fclean: clean
    rm -f *.o

re: fclean all