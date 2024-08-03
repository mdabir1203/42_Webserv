CC = g++
CFLAGS = -Wall -Wextra -Werror -std=c++98
NAME = webserv

all: $(NAME)

$(NAME): src/main.cpp
	$(CC) $(CFLAGS) -o $(NAME) $^

clean:
	rm -f $(NAME)

fclean: clean
	rm -f *.o

re: fclean all