# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: 42sh                                       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#                                                      #+#    #+#              #
#                                                     ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= 42sh

CC			= gcc
CFLAGS		= -Wall -Wextra -Werror -g3
DEPFLAGS	= -MMD -MP

# Directories
SRC_DIR		= src
INC_DIR		= include
OBJ_DIR		= obj
LIBFT_DIR	= libft

# Libraries
LIBFT		= $(LIBFT_DIR)/libft.a
LDFLAGS		= -L$(LIBFT_DIR)
LDLIBS		= -lft -lreadline -lncurses

# Includes
INCLUDES	= -I$(INC_DIR) -I$(LIBFT_DIR)

# Sources
SRC_FILES	= main.c \
			  init.c \
			  env.c \
			  signals.c \
			  lexer/lexer.c \
			  lexer/lexer_utils.c \
			  parser/parser.c \
			  parser/parser_utils.c \
			  expand/expand.c \
			  exec/executor.c \
			  exec/exec_cmd.c \
			  exec/exec_pipe.c \
			  exec/exec_redir.c \
			  exec/exec_path.c \
			  builtins/builtins.c \
			  builtins/builtin_cd.c \
			  builtins/builtin_echo.c \
			  builtins/builtin_exit.c \
			  builtins/builtin_type.c \
			  builtins/builtin_export.c \
			  builtins/builtin_unset.c \
			  builtins/builtin_set.c \
			  builtins/builtin_jobs.c \
			  builtins/builtin_fg.c \
			  builtins/builtin_bg.c \
			  vars/vars.c \
			  jobs/jobs.c

SRCS		= $(addprefix $(SRC_DIR)/, $(SRC_FILES))
OBJS		= $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS		= $(OBJS:.o=.d)

# Subdirectories for object files
OBJ_SUBDIRS	= $(OBJ_DIR)/lexer $(OBJ_DIR)/parser $(OBJ_DIR)/expand \
			  $(OBJ_DIR)/exec $(OBJ_DIR)/builtins $(OBJ_DIR)/vars \
			  $(OBJ_DIR)/jobs

# Rules
all: $(LIBFT) $(NAME)

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR) $(OBJ_SUBDIRS)
	$(CC) $(CFLAGS) $(DEPFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR) $(OBJ_SUBDIRS):
	mkdir -p $@

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

clean:
	$(MAKE) -C $(LIBFT_DIR) clean
	rm -rf $(OBJ_DIR)

fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean
	rm -f $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
