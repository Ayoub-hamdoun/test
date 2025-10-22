
NAME = ircserv
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = main.cpp commands/Server.cpp commands/Client.cpp commands/Channel.cpp commands/Commands.cpp \
      commands/BaseCommand.cpp \
      commands/PassCommand.cpp \
      commands/UserCommand.cpp \
      commands/JoinCommand.cpp \
      commands/PrivMsgCommand.cpp \
      commands/KickCommand.cpp \
      commands/InviteCommand.cpp \
      commands/TopicCommand.cpp \
      commands/ModeCommand.cpp \
      commands/QuitCommand.cpp \
      commands/PartCommand.cpp \
      commands/NickCommand.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp includes/Server.hpp includes/Client.hpp includes/Channel.hpp includes/Commands.hpp \
      includes/BaseCommand.hpp \
      includes/PassCommand.hpp \
      includes/UserCommand.hpp \
      includes/JoinCommand.hpp \
      includes/PrivMsgCommand.hpp \
      includes/KickCommand.hpp \
      includes/InviteCommand.hpp \
      includes/TopicCommand.hpp \
      includes/ModeCommand.hpp \
      includes/QuitCommand.hpp \
      includes/PartCommand.hpp \
      includes/NickCommand.hpp

	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME) a.out

re: fclean all

.PHONY: all clean fclean re
