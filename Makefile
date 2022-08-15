# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/12/22 14:06:43 by lumenthi          #+#    #+#              #
#    Updated: 2022/08/15 14:45:10 by lumenthi         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

.PHONY : all clean fclean re

NAME = ft_ping

CC = gcc
FLAGS = -Wall -Werror -Wextra

GREEN = '\033[4;32m'
RED = '\033[4;31m'
BLANK = '\033[0m'
YELLOW = '\033[4;33m'
CYAN = '\033[4;38;5;51m'

TICK = '\033[1;32m~\033[0m'
CROSS = '\033[1;31mx\033[0m'

###### FOLDERS ######

LIBDIR = libft
SRCDIR = sources
HEADDIR = headers
OBJDIR = objs

#####################

###### LIBRARY ######

LIBFT = $(LIBDIR)/libft.a

#####################

###### HEADERS ######

HEADS = ping.h
HEADERS = $(addprefix $(HEADDIR)/, $(HEADS))

#####################

###### SOURCES ######

SRCS = ping.c \
	signal.c

SOURCES = $(addprefix $(SRCDIR)/, $(SRCS))

#####################

###### OBJECTS ######

OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.c=.o))

#####################

all: $(NAME)

###### FT_NM COMPILATION ######

$(NAME):  $(OBJS) ${HEADERS}
	@ make -sC $(LIBDIR)
	$(CC) $(OBJS) -o $(NAME) $(LIBFT)
	@ printf " %b | Compiled %b%b%b\n" $(TICK) $(GREEN) $(NAME) $(BLANK)

###############################

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(OBJDIR)
	$(CC) -c $(FLAGS) -I$(HEADDIR) -I$(LIBDIR) -o $@ $<

clean:
	@ make -sC $(LIBDIR) clean
	@ test -d $(OBJDIR) && \
	rm -rf $(OBJDIR) && \
	printf " %b | " $(TICK) && \
	printf "Removed %bobjects%b folders\n" $(YELLOW) $(BLANK) \
	|| (printf " %b | " $(CROSS) && \
	printf "No %bobjects%b folders\n" $(YELLOW) $(BLANK))

fclean: clean
	@ test -f $(LIBFT) && \
	rm -rf $(LIBFT) && \
	printf " %b | " $(TICK) && \
	printf "Removed %blibft%b library\n" $(RED) $(BLANK) \
	|| (printf " %b | " $(CROSS) && \
	printf "No %blibft%b library\n" $(RED) $(BLANK))
	@ test -f $(NAME) && \
	rm -rf $(NAME) && \
	printf " %b | " $(TICK) && \
	printf "Removed %b%b%b binary\n" $(RED) $(NAME) $(BLANK) \
	|| (printf " %b | " $(CROSS) && \
	printf "No %b%b%b binary\n" $(RED) $(NAME) $(BLANK))

re: fclean # Make -j support
	@ $(MAKE) all

run: all
	./$(NAME) localhost
