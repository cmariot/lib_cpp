# **************************************************************************** #
#                                LIBRARY'S NAME                                #
# **************************************************************************** #


NAME			 = libftpp.a



# **************************************************************************** #
#                                 COMPILATION                                  #
# **************************************************************************** #


CC				 = c++

FLAGS			 = -Wall -Wextra -Werror
FLAGS            += --std=c++17


CFLAGS			 = $(FLAGS)
LFLAGS			 = $(FLAGS)


INCLUDES		 = -I includes



# **************************************************************************** #
#                                SOURCE FILES                                  #
# **************************************************************************** #


SRC_ROOTDIR		= sources/


SRC_FILES	    = data_structures/data_buffer.cpp \
				  iostream/thread_safe_iostream.cpp


SRCS			= $(addprefix $(SRC_ROOTDIR), $(SRC_FILES))



# **************************************************************************** #
#                                OBJECT FILES                                  #
# **************************************************************************** #


OBJ_ROOTDIR		= objects/

OBJ_FILES		= $(SRC_FILES:.cpp=.o)

OBJ_DIR 		= $(shell find ./sources -type d | sed s/".\/sources"/".\/objects"/g)

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


.PHONY: all clean fclean re help
.PHONY: help

all: header $(NAME)


# Compilation des fichiers objets
$(OBJ_ROOTDIR)%.o: $(SRC_ROOTDIR)%.cpp
				@mkdir -p $(OBJ_DIR)
				$(CC) $(CFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@


# Création de la librairie statique
$(NAME)	: 		$(OBJS)
				ar rcs $(NAME) $(OBJS)
				@printf "\n"


# Nettoyage des fichiers objets et dépendances
clean :
				@rm -rf $(OBJ_ROOTDIR) $(DEPENDS)


# Nettoyage complet (librairie + objets)
fclean: clean
				@rm -f $(NAME)


# Recompilation complète
re :			fclean all


# Affichage d'un en-tête coloré
header :
				@printf "$(CYAN)\n"
				@printf "LIFTPP"
				@printf "\n$(RESET)"


# Affiche l'aide sur les commandes disponibles
help:
	@echo "Commandes disponibles :"
	@echo "  all      - Compile la librairie"
	@echo "  clean    - Supprime les fichiers objets et dépendances"
	@echo "  fclean   - Supprime la librairie, les objets et dépendances"
	@echo "  re       - Recompile tout"
	@echo "  help     - Affiche ce message d'aide"


# Inclusion automatique des fichiers de dépendances générés par -MMD
-include $(DEPENDS)

.SILENT : 		all
