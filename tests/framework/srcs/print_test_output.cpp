/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_test_output.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmariot <cmariot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/07 18:55:57 by cmariot           #+#    #+#             */
/*   Updated: 2023/12/06 13:00:01 by cmariot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libunit.hpp"
#include <sstream>
#include <iomanip>

/*
 * Get all the file content and store it in a std::string
 */

static std::string	get_file_content(std::string file)
{
	std::ifstream	ifs(file);

	if (ifs.is_open() == false)
		return ("");
	std::string		content(
				(std::istreambuf_iterator<char>(ifs)),
				(std::istreambuf_iterator<char>())
			);
	ifs.close();
	unlink(file.c_str());
	return (content);
}

/*
 * Get the content of the file that stores the function output,
 * Compare the output with the expected output
 * or display the output depending the function call parameters.
 */

static void	check_stdout_output(t_test *test, std::ostream &fd, bool with_color)
{
	std::string	output;

	output = get_file_content(test->filename);
	if (test->expected_output.empty() == false)
	{
		if (output == test->expected_output)
		{
			if (with_color) fd << GREEN << "[OUTPUT : OK]" << RESET << std::endl;
			else fd << "[OUTPUT : OK]" << std::endl;
		}
		else
		{
			if (with_color) fd << RED_COLOR << "[OUTPUT : KO]" << RESET << std::endl;
			else fd << "[OUTPUT : KO]" << std::endl;
			fd << "\t[OUTPUT]:\t[" << output << "]" << std::endl;
			fd << "\t[EXPECTED]:\t[" << test->expected_output <<  "]" << std::endl;
			test->status = KO;
		}
	}
	else
	{
		fd << std::endl;
		if (test->display_stdout && output.empty() == false)
			fd << "[OUTPUT] :" << std::endl << output << std::endl;
	}
}

/*
 * Print the test result on the ofstream fd (i.e. on the logfile)
 * (and call recursively the same function to print on std::cout)
 */

void	print_test_output(t_test *test, int test_number, std::ostream &fd, bool to_console)
{
	// Caller is responsible for invoking this twice when both log and console outputs
	// Build left column and pad to global width for alignment
	std::ostringstream left;
	left << test->function << "_";
	left << std::setw(2) << std::setfill('0') << test_number;
	left << ": " << test->test_name;
	std::string lefts = left.str();
	fd << std::left << std::setw(g_test_left_width + 2) << std::setfill(' ') << lefts;

	auto write_status = [&](std::ostream &out, bool with_color){
		if (test->status == OK)
		{
			if (with_color) out << GREEN << "[OK]" << RESET;
			else out << "[OK]";
		}
		else if (test->status == KO)
		{
			if (with_color) out << RED_COLOR << "[KO]" << RESET;
			else out << "[KO]";
		}
		else if (test->status == TIMEOUT)
		{
			if (with_color) out << RED_COLOR << "[TIMEOUT]" << RESET;
			else out << "[TIMEOUT]";
		}
		else if (test->status == SIGSEGV)
		{
			if (with_color) out << RED_COLOR << "[SIGSEGV]" << RESET;
			else out << "[SIGSEGV]";
		}
		else if (test->status == SIGBUS)
		{
			if (with_color) out << RED_COLOR << "[SIGBUS]" << RESET;
			else out << "[SIGBUS]";
		}
		else if (test->status == SIGABRT)
		{
			if (with_color) out << RED_COLOR << "[SIGABRT]" << RESET;
			else out << "[SIGABRT]";
		}
		else if (test->status == SIGFPE)
		{
			if (with_color) out << RED_COLOR << "[SIGFPE]" << RESET;
			else out << "[SIGFPE]";
		}
		else if (test->status == SIGPIPE)
		{
			if (with_color) out << RED_COLOR << "[SIGPIPE]" << RESET;
			else out << "[SIGPIPE]";
		}
		else if (test->status == SIGILL)
		{
			if (with_color) out << RED_COLOR << "[SIGILL]" << RESET;
			else out << "[SIGILL]";
		}
		else if (test->status == 66)
		{
			if (with_color) out << RED_COLOR << "[LEAKS]" << RESET;
			else out << "[LEAKS]";
		}
		else
		{
			if (with_color) out << RED_COLOR << "[EXIT : " << test->status << "]" << RESET;
			else out << "[EXIT : " << test->status << "]";
		}
	};

	// For console output we want colors; for file output we don't.
	write_status(fd, to_console);
	check_stdout_output(test, fd, to_console);
}
