/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   launch_tests.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmariot <cmariot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/07 22:01:37 by cmariot           #+#    #+#             */
/*   Updated: 2023/12/12 19:41:34 by cmariot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libunit.hpp"
#include <sstream>
#include <iomanip>

int g_test_left_width = 0;

static int	clean_exit(t_test **test, std::ofstream &log_file)
{
	log_file.close();
	clear_test_list(test);
	return (1);
}

/*
 * Launch the tester :
 *  - Create a logfile to store the output
 *  - Exectute each test store in the list t_test
 *  - Print test results
 */

int	launch_tests(t_test **test)
{
	std::ofstream	log_file;
	t_test			*element;
	int				succeeded = 0;
	int				total = 0;

	// Compute max width for left column (function_##: test_name)
	{
		t_test *it = *test;
		while (it)
		{
			std::ostringstream oss;
			oss << it->function << "_" << std::setw(2) << std::setfill('0') << total << ": " << it->test_name;
			int len = static_cast<int>(oss.str().size());
			if (len > g_test_left_width)
				g_test_left_width = len;
			it = it->next;
			++total;
		}
		// reset total for actual run
		total = 0;
	}

	std::cout << std::endl;
	std::cout << BOLDWHITE << "libftpp"
		<< " TESTS :" << RESET << std::endl << std::endl;
	log_file = create_log_file(*test);
	if (log_file.is_open() == false)
		return (1);
	while (*test)
	{
		if (execute_test(test, log_file))
			return (clean_exit(test, log_file));
		// print to log file without colors
		print_test_output(*test, total, log_file, false);
		// also print to console with colors
		print_test_output(*test, total, std::cout, true);
		if ((*test)->status == OK)
			succeeded++;
		total++;
		element = *test;
		*test = (*test)->next;
		delete element;
	}
	return (results(succeeded, total, log_file, true));
}
