# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: cmariot <cmariot@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/09/30 11:15:47 by cmariot           #+#    #+#              #
#    Updated: 2023/12/11 11:19:04 by cmariot          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


# **************************************************************************** #
#                              EXECUTABLE'S NAME                               #
# **************************************************************************** #


NAME			 = libunit.a



# **************************************************************************** #
#                                 COMPILATION                                  #
# **************************************************************************** #


CXX ?= g++


CXXFLAGS ?= -Wall -Wextra -Werror -std=c++17 -pthread -O3
LFLAGS			 = -Wall -Wextra -Werror -std=c++17 -pthread -O3


INCLUDES		 = -I includes



# **************************************************************************** #
#                                SOURCE FILES                                  #
# **************************************************************************** #


SRC_ROOTDIR		= srcs/


SRC_FILES	    = load_test.cpp \
				  launch_tests.cpp \
				  clear_test_list.cpp \
				  create_log_file.cpp \
				  execute_test.cpp \
				  timeout.cpp \
				  print_test_output.cpp \
				  results.cpp


SRCS			= $(addprefix $(SRC_ROOTDIR), $(SRC_FILES))



# **************************************************************************** #
#                                OBJECT FILES                                  #
# **************************************************************************** #


OBJ_ROOTDIR		= objs/

OBJ_FILES		= $(SRC_FILES:.cpp=.o)

OBJ_DIR 		= $(shell find ./srcs -type d | sed s/".\/srcs"/".\/objs"/g)

OBJS			= $(addprefix $(OBJ_ROOTDIR), $(OBJ_FILES))

DEPENDS			:= $(OBJS:.o=.d)



# **************************************************************************** #
#                                  COLORS                                      #
# **************************************************************************** #


RED				= \033[31;1m
CYAN			= \033[36;1m
RESET			= \033[0m



# **************************************************************************** #
#                             MAKEFILE'S RULES                                 #
# **************************************************************************** #


.SILENT : 		all


all : 			header $(NAME)

# Logging helpers
PRINTF			 = @printf
INFO			 = $(PRINTF) "$(CYAN)%s$(RESET)\n"
STEP			 = $(PRINTF) "  - %s\n"
V ?= 0
RUN			 = $(if $(filter 1,$(V)), $(PRINTF) "  $ %s\n" "$(1)"; $(1), @$(1))




$(OBJ_ROOTDIR)%.o: $(SRC_ROOTDIR)%.cpp
		$(INFO) "Compiling $< -> $@"
		@mkdir -p $(dir $@)
		$(call RUN,$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@)
		@printf "\n"


$(NAME)	: 		$(OBJS)
			$(INFO) "Archiving $(NAME)"
			$(call RUN,ar rcs $(NAME) $(OBJS))
			@printf "\n"


leaks :			all
				valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./$(NAME)


test :			all
				./$(NAME)


clean :
				@rm -rf $(OBJ_ROOTDIR) $(DEPENDS)


fclean :
				@-rm -f $(NAME)
				@-rm -rf $(OBJ_ROOTDIR) $(DEPENDS)


re :			fclean all


header :
				@printf "$(CYAN)"
				@printf "\nLIBUNIT COMPILATION\n"
				@printf "$(RESET)"



-include $(DEPENDS)


.PHONY : 		all clean fclean bonus re
