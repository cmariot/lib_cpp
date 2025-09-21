/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_log_file.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmariot <cmariot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/07 18:32:16 by cmariot           #+#    #+#             */
/*   Updated: 2022/06/07 20:47:59 by cmariot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libunit.hpp"
#include <string>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

// Create directories recursively (POSIX). Returns 0 on success, -1 on error.
static int make_dirs(const std::string &path, mode_t mode = 0755)
{
	if (path.empty())
		return 0;
	std::string cur;
	for (size_t i = 0; i < path.size(); ++i)
	{
		cur.push_back(path[i]);
		if (path[i] == '/' || i + 1 == path.size())
		{
			if (cur.empty())
				continue;
			// skip if already exists
			struct stat st;
			if (stat(cur.c_str(), &st) != 0)
			{
				if (mkdir(cur.c_str(), mode) != 0 && errno != EEXIST)
					return -1;
			}
			else if (!S_ISDIR(st.st_mode))
			{
				errno = ENOTDIR;
				return -1;
			}
		}
	}
	return 0;
}

std::ofstream	create_log_file(t_test *test)
{
	const std::string logs_dir = "tests/logs";
	// Ensure logs directory exists using portable POSIX helper
	(void)make_dirs(logs_dir);

	// Use a single global log file for the whole test run
	const std::string filename = logs_dir + "/libftpp.log";
	// Open in append mode so multiple runs don't overwrite unless intended
	std::ofstream	log_file(filename, std::ios::app);

	if (!log_file.is_open())
		std::cerr << "Error, can't open log file '" << filename << "'" << std::endl;
	else
	{
		log_file << BOLDWHITE << test->function << " TESTS:" << std::endl << std::endl;
		log_file.flush();
	}
	return (log_file);
}
