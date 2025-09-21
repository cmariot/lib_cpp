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

# Small logging helpers used in rules (prints colored messages)
PRINTF			 = @printf
INFO			 = $(PRINTF) "$(CYAN)%s$(RESET)\n"
STEP			 = $(PRINTF) "  - %s\n"
RUN			 = $(PRINTF) "  $ %s\n" "$(1)"; $(1)



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
.PHONY: tests test-only tests-clean run-tests
.PHONY: docs

all: header $(NAME)


# Tooling
AR ?= ar


# Compilation des fichiers objets
$(OBJ_ROOTDIR)%.o: $(SRC_ROOTDIR)%.cpp
		$(INFO) "Compiling $< -> $@"
		@mkdir -p $(dir $@)
		$(call RUN,$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@)
		@printf "\n"


# Création de la librairie statique
$(NAME) 	: 		$(OBJS)
			$(INFO) "Archiving $(NAME)"
			$(call RUN,$(AR) rcs $(NAME) $(OBJS))
			@printf "\n"


# Nettoyage des fichiers objets et dépendances
clean:
	@rm -rf $(OBJ_ROOTDIR) $(DEPENDS)


# Nettoyage complet (librairie + objets)
fclean: clean
	@rm -f $(NAME)
	@rm -f $(TEST_BIN_DIR)/run_tests
	@rm -rf $(TEST_BIN_DIR)
	@rm -rf $(TESTS_OBJ_DIR) $(TEST_OBJS)
	@rm -f tests/tests_launcher.cpp
	@echo "Invoking tests/framework fclean (if present)..."
	@if [ -d tests/framework ]; then $(MAKE) -C tests/framework fclean || true; fi
	@echo "Removing .log files under tests/..."
	@find tests -type f -name "*.log" -print -delete || true
	@echo "Removing generated documentation (documentation/html) if present..."
	@rm -rf documentation/html || true
	@echo "Removing tests logs directory if present..."
	@rm -rf tests/logs || true


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

.PHONY: docs
docs:
	@echo "Generating documentation..."
	@if command -v doxygen >/dev/null 2>&1; then \
		if doxygen Doxyfile >/dev/null 2>&1; then \
			echo "Documentation generated in documentation/html"; \
			if [ -f documentation/html/index.html ]; then open documentation/html/index.html; else echo "Warning: documentation/html/index.html not found"; fi; \
		else \
			echo "Doxygen failed to generate documentation"; exit 1; \
		fi; \
	else \
		if [ -f documentation/html/index.html ]; then \
			echo "Doxygen not found — opening existing documentation/html/index.html"; open documentation/html/index.html; \
		else \
			echo "Doxygen not found. Install it to generate docs (brew install doxygen)"; exit 1; \
		fi; \
	fi


# Inclusion automatique des fichiers de dépendances générés par -MMD
-include $(DEPENDS)

.SILENT : 		all


# **************************************************************************** #
#                                 TESTS                                       #
# **************************************************************************** #

TEST_BIN_DIR = bin
TEST_SRCS = tests/tests_launcher.cpp \
        	tests/data_structures/data_buffer/data_buffer.cpp \
        	tests/data_structures/data_buffer/more_tests.cpp \
        	tests/data_structures/pool/pool.cpp \
        	tests/data_structures/pool/handle_test.cpp \
        	tests/design_patterns/memento.cpp \
        	tests/design_patterns/observer_test.cpp \
        	tests/design_patterns/singleton_test.cpp \
        	tests/design_patterns/state_machine_test.cpp

# All test cpp files under tests/ (used to trigger regeneration of the launcher)
TEST_SRCS_CPP := $(shell find tests -type f -name "*.cpp")

TESTS_OBJ_DIR = tests/objects

# Place test object files under $(TESTS_OBJ_DIR), preserving subpaths
TEST_OBJS = $(patsubst tests/%.cpp,$(TESTS_OBJ_DIR)/%.o,$(filter tests/%.cpp,$(TEST_SRCS)))

.PHONY: tests run-tests


tests: $(TEST_BIN_DIR) tests/tests_launcher.cpp $(TEST_OBJS) tests/framework/libunit.a $(NAME)
		$(INFO) "Building tests binary"
		$(STEP) "$(CXX) $(CXXFLAGS) $(INCLUDES) -I tests/framework/includes -o $(TEST_BIN_DIR)/run_tests $(TEST_OBJS) tests/framework/libunit.a $(NAME)"
		$(CXX) $(CXXFLAGS) $(INCLUDES) -I tests/framework/includes -o $(TEST_BIN_DIR)/run_tests $(TEST_OBJS) tests/framework/libunit.a $(NAME)
		@printf "\n"
		$(INFO) "Tests binary created: $(TEST_BIN_DIR)/run_tests"
		$(INFO) "Running tests..."
		$(TEST_BIN_DIR)/run_tests

run-tests: tests

.PHONY: tests-clean test-only

test-only:
		@if [ -x $(TEST_BIN_DIR)/run_tests ]; then $(TEST_BIN_DIR)/run_tests; else echo "No test binary found. Run 'make tests' first."; fi

tests-clean:
	@rm -f $(TEST_BIN_DIR)/run_tests $(TEST_OBJS)
	@rm -rf $(TESTS_OBJ_DIR)


$(TEST_BIN_DIR):
		@mkdir -p $(TEST_BIN_DIR)


# Ensure the test framework archive is built when requested by top-level tests target
tests/framework/libunit.a:
		@if [ -d tests/framework ]; then $(MAKE) -C tests/framework all || true; else echo "No tests/framework directory"; fi

# Auto-generate tests/tests_launcher.cpp from Python script
tests/tests_launcher.cpp: tests/generate_launcher.py $(TEST_SRCS_CPP)
	$(INFO) "Generating tests/tests_launcher.cpp..."
	$(STEP) "python3 tests/generate_launcher.py"
	@python3 tests/generate_launcher.py


# Rule to compile test .cpp files into tests/objs/... preserving directory structure
$(TESTS_OBJ_DIR)/%.o: tests/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -I tests/framework/includes -c $< -o $@

# Fallback generic rule for other .o (library objects)

%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -I tests/framework/includes -c $< -o $@
