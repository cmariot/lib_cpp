# **************************************************************************** #
#                                LIBRARY'S NAME                                #
# **************************************************************************** #

NAME			 = libftpp.a


# **************************************************************************** #
#                                 COMPILATION                                  #
# **************************************************************************** #

CXX			 	 = c++

CXXFLAGS		 = -Wall -Wextra -Werror -std=c++23

INCLUDES		 = -I includes

AR 				 ?= ar

# Inclusion automatique des fichiers de dépendances générés par -MMD
-include $(DEPENDS)


# **************************************************************************** #
#                                   LOGGING                                    #
# **************************************************************************** #

PRINTF			 = @printf
INFO			 = $(PRINTF) "$(CYAN)%s$(RESET)\n"
STEP			 = $(PRINTF) "  - %s\n"
V ?= 0

# RUN: helper that respects verbosity flag V. Use 'make V=1' for verbose.
# When V=1 we print the command then run it; otherwise run quietly.
RUN			 	 = $(if $(filter 1,$(V)), $(PRINTF) "  $ %s\n" "$(1)"; $(1), @$(1))


# **************************************************************************** #
#                                SOURCE FILES                                  #
# **************************************************************************** #


SRC_ROOTDIR		= sources/


SRC_FILES = \
data_structures/data_buffer.cpp \
iostream/thread_safe_iostream.cpp \
	networking/client.cpp \
	networking/server.cpp


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
#                                 TESTS                                       #
# **************************************************************************** #

TEST_BIN_DIR = bin

TEST_SRCS = \
tests/tests_launcher.cpp \
tests/data_structures/data_buffer/data_buffer.cpp \
tests/data_structures/data_buffer/more_tests.cpp \
tests/data_structures/pool/pool.cpp \
tests/data_structures/pool/handle_test.cpp \
tests/design_patterns/memento.cpp \
tests/design_patterns/observer_test.cpp \
tests/design_patterns/singleton_test.cpp \
tests/design_patterns/state_machine_test.cpp \
tests/networking/loopback_test.cpp \
tests/networking/message_test.cpp \
tests/networking/message_helpers_test.cpp \
tests/networking/broadcast_test.cpp

# All test cpp files under tests/ (used to trigger regeneration of the launcher)
# Exclude the generated launcher itself to avoid a circular dependency
TEST_SRCS_CPP := $(filter-out tests/tests_launcher.cpp,$(shell find tests -type f -name "*.cpp"))

TESTS_OBJ_DIR = tests/objects

# Place test object files under $(TESTS_OBJ_DIR), preserving subpaths
TEST_OBJS = $(patsubst tests/%.cpp,$(TESTS_OBJ_DIR)/%.o,$(filter tests/%.cpp,$(TEST_SRCS)))


# **************************************************************************** #
#                                  COLORS                                      #
# **************************************************************************** #

RED				= \033[31;1m
CYAN			= \033[36;1m
RESET			= \033[0m


# **************************************************************************** #
#                             MAKEFILE'S RULES                                 #
# **************************************************************************** #

all: header $(NAME)

# Compilation des fichiers objets
$(OBJ_ROOTDIR)%.o: $(SRC_ROOTDIR)%.cpp
	$(INFO) "Compiling $< -> $@"
	@mkdir -p $(dir $@)
	$(call RUN,$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@)
	@printf "\n"


# Création de la librairie statique
$(NAME): $(OBJS)
	$(INFO) "Archiving $(NAME)"
	# ensure all object files are present before archiving
	@missing=0; \
	for f in $(OBJS); do \
		if [ ! -f $$f ]; then echo "Missing object: $$f"; missing=1; fi; \
	done; \
	if [ $$missing -eq 1 ]; then echo "One or more object files are missing, aborting archive."; exit 1; fi;
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
	@echo "Note: logs, documentation and other generated artifacts are preserved. Use 'make distclean' to remove them."


# Recompilation complète
re : fclean all


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


tests-debug:
	@echo "Running tests in debug mode (KEEP_TEST_TMP=1)..."
	@KEEP_TEST_TMP=1 $(MAKE) tests

docs:
	@echo "Generating documentation..."
	@# Try repo script first
	@if [ -x scripts/generate_docs.sh ]; then ./scripts/generate_docs.sh || { echo "Documentation generation failed"; exit 1; }; fi
	@# If the script produced output, open it; otherwise fallback to system doxygen
	@if [ -f docs/doxygen/html/index.html ]; then open docs/doxygen/html/index.html; elif command -v doxygen >/dev/null 2>&1; then doxygen Doxyfile || { echo "Doxygen failed to generate documentation"; exit 1; }; if [ -f documentation/html/index.html ]; then open documentation/html/index.html; else echo "Documentation generated but documentation/html/index.html not found"; fi; else echo "Doxygen not found. Install it (brew install doxygen) or add scripts/generate_docs.sh"; exit 1; fi


tests: $(TEST_BIN_DIR) tests/tests_launcher.cpp $(TEST_OBJS) tests/framework/libunit.a $(NAME)
		$(INFO) "Building tests binary"
		$(STEP) "$(CXX) $(CXXFLAGS) $(INCLUDES) -I tests/framework/includes -o $(TEST_BIN_DIR)/run_tests $(TEST_OBJS) tests/framework/libunit.a $(NAME)"
		$(CXX) $(CXXFLAGS) $(INCLUDES) -I tests/framework/includes -o $(TEST_BIN_DIR)/run_tests $(TEST_OBJS) tests/framework/libunit.a $(NAME)
		@printf "\n"
		$(INFO) "Tests binary created: $(TEST_BIN_DIR)/run_tests"
		$(INFO) "Running tests..."
		$(TEST_BIN_DIR)/run_tests


test-only:
		@if [ -x $(TEST_BIN_DIR)/run_tests ]; then $(TEST_BIN_DIR)/run_tests; else echo "No test binary found. Run 'make tests' first."; fi


tests-clean:
	@rm -f $(TEST_BIN_DIR)/run_tests $(TEST_OBJS)
	@rm -rf $(TESTS_OBJ_DIR)


$(TEST_BIN_DIR):
		@mkdir -p $(TEST_BIN_DIR)

# Fallback generic rule for other .o (library objects)
%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -I tests/framework/includes -c $< -o $@


# Rule to compile test .cpp files into tests/objs/... preserving directory structure
$(TESTS_OBJ_DIR)/%.o: tests/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -I tests/framework/includes -c $< -o $@


# Ensure the test framework archive is built when requested by top-level tests target
tests/framework/libunit.a:
		@if [ -d tests/framework ]; then $(MAKE) -C tests/framework all || true; else echo "No tests/framework directory"; fi


# Auto-generate tests/tests_launcher.cpp from Python script
tests/tests_launcher.cpp: tests/generate_launcher.py $(TEST_SRCS_CPP)
	$(INFO) "Generating tests/tests_launcher.cpp..."
	$(STEP) "python3 tests/generate_launcher.py"
	@python3 tests/generate_launcher.py


.PHONY : all clean fclean re help docs tests test-only tests-clean run-tests

distclean: fclean
	@echo "Removing generated documentation (documentation/html) if present..."
	@rm -rf documentation/html || true
	@echo "Removing .log files under tests/..."
	@find tests -type f -name "*.log" -print -delete || true
	@echo "Removing tests logs directory if present..."
	@rm -rf tests/logs || true
	@echo "Removing bin/ directory if present..."
	@rm -rf bin || true