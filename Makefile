# **************************************************************************** #
#                                LIBRARY'S NAME                                #
# **************************************************************************** #


NAME			 = libftpp.a



# **************************************************************************** #
#                                 COMPILATION                                  #
# **************************************************************************** #


CXX			 = c++

CXXFLAGS		 = -Wall -Wextra -Werror -std=c++23


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
				$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@


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
		@rm -rf $(TEST_BIN_DIR) $(TESTS_OBJ) tests/*.o tests/*/*.o
		@rm -f $(NAME)
		@rm -f tests/tests_launcher.cpp
		@echo "Invoking tests/framework fclean (if present)..."
		@if [ -d tests/framework ]; then $(MAKE) -C tests/framework fclean || true; fi
		@echo "Removing .log files..."
		@find . -type f -name "*.log" -print -delete || true


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
	@echo "  tests    - Compile et lance les tests unitaires"
	@echo "  test-only - Lance les tests sans recompiler la librairie"
	@echo "  tests-clean - Supprime les binaires/objets des tests"
	@echo "  help     - Affiche ce message d'aide"


# Inclusion automatique des fichiers de dépendances générés par -MMD
-include $(DEPENDS)

.SILENT : 		all


# **************************************************************************** #
#                                 TESTS                                       #
# **************************************************************************** #

TEST_BIN_DIR = bin
TESTS_SRC = tests/tests_launcher.cpp \
		tests/data_buffer/data_buffer.cpp \
		tests/pool/pool.cpp

TESTS_OBJ = $(patsubst %.cpp,%.o,$(TESTS_SRC))

.PHONY: tests run-tests

tests: $(TEST_BIN_DIR) tests/tests_launcher.cpp $(TESTS_OBJ) tests/framework/libunit.a $(NAME)
		$(CXX) $(CXXFLAGS) $(INCLUDES) -I tests/framework/includes -o $(TEST_BIN_DIR)/run_tests $(TESTS_OBJ) tests/framework/libunit.a $(NAME)
		@printf "\nTests binary created: $(TEST_BIN_DIR)/run_tests\n"
		$(TEST_BIN_DIR)/run_tests

run-tests: tests

.PHONY: tests-clean test-only

test-only:
		@if [ -x $(TEST_BIN_DIR)/run_tests ]; then $(TEST_BIN_DIR)/run_tests; else echo "No test binary found. Run 'make tests' first."; fi

tests-clean:
		@rm -f $(TEST_BIN_DIR)/run_tests $(TESTS_OBJ)

$(TEST_BIN_DIR):
		@mkdir -p $(TEST_BIN_DIR)


# Ensure the test framework archive is built when requested by top-level tests target
tests/framework/libunit.a:
		@if [ -d tests/framework ]; then $(MAKE) -C tests/framework all || true; else echo "No tests/framework directory"; fi

# Auto-generate tests/tests_launcher.cpp from Python script
tests/tests_launcher.cpp: tests/generate_launcher.py
		@echo "Generating tests/tests_launcher.cpp..."
		@python3 tests/generate_launcher.py

%.o: %.cpp
		$(CXX) $(CXXFLAGS) $(INCLUDES) -I tests/framework/includes -c $< -o $@
