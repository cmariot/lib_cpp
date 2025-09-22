/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libunit.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmariot <cmariot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/07 23:29:59 by cmariot           #+#    #+#             */
/*   Updated: 2023/12/06 12:56:24 by cmariot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBUNIT_H
# define LIBUNIT_H

# include <unistd.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <sys/time.h>
# include <pthread.h>

# include <iostream>
# include <iomanip>
# include <fstream>

# define OK				0
# define KO				255
# define TIMEOUT		32
# define TIMEOUT_DELAY	10

# define GREEN		"\033[1;32m"
# define RED_COLOR	"\033[1;31m"
# define BOLDWHITE	"\033[1m\033[37m"
# define RESET		"\033[0m"

typedef struct s_test {
	std::string		function;				// Function name
	std::string		test_name;				// Test name
	int				(*test_add)(void);		// Function pointer
	int				status;					// Exit status
	std::string		filename;				// Log file for this test
	std::string		err_filename;			// Separate stderr file for this test
	bool            display_stdout;         // Display stdout or not
    std::string		expected_output;		// If expected != output -> Error
	size_t			start_time;			    // start time in ms since epoch for the test run
	size_t			duration_ms;			// duration of the test in milliseconds
	struct s_test	*next;					// Pointer on the next test
}	t_test;

// Load all the tests with this function before execute them with the launch test function.
void			load_test(t_test **test, std::string function, std::string test_name,
					void *function_add, const char *expected_output);

int				launch_tests(t_test **test);
std::ofstream	create_log_file(t_test *test);
int				execute_test(t_test **test, std::ofstream &log_file);
size_t			get_time(void);
int				check_timeout(size_t init_time);
void			clear_test_list(t_test **test);
void			print_test_output(t_test *test, int test_nb, std::ostream &fd, bool to_console);
int				results(int succeeded, int total, std::ostream &fd, bool recursive);

// Global width of the left column for printing test results aligned
extern int      g_test_left_width;

// When true, temporary per-test files are kept (not unlinked) for debugging.
extern bool     g_keep_test_tmp;

#endif
