NAME = webserv

SRCS = main.cpp \
	Parser/config_file.cpp\
	Parser/server.cpp\
	Networking/socket.cpp \
	Parser/request.cpp\
	Response/response.cpp\
	Response/helpers.cpp\
	Networking/data_socket.cpp\
	Parser/chunk.cpp\

all: $(NAME)

$(NAME): $(SRCS)
	@c++ -std=c++98 -std=gnu++0x -fsanitize=address -Wall -Wextra -Werror $(SRCS) -o $(NAME)

clean:
	@rm -rf $(NAME)
	@rm -rf index.html
	@rm -rf tmp/*
	@rm -rf uploads/*

fclean: clean

re: fclean all

.PHONY: all clean fclean re