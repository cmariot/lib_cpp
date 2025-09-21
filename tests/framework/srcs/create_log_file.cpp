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
#include <filesystem>
std::ofstream	create_log_file(t_test *test)
{
	(void)test; // parameter intentionally unused: header/title is global 'libftpp'
	const std::string logs_dir = "tests/logs";
	// Ensure logs directory exists using std::filesystem
	try {
		std::filesystem::create_directories(logs_dir);
	} catch (const std::exception &e) {
		std::cerr << "Warning: could not create logs directory '" << logs_dir << "': " << e.what() << std::endl;
	}

	// Use a single global log file for the whole test run
	const std::string filename = logs_dir + "/libftpp.log";
	// Open in append mode so multiple runs don't overwrite unless intended
	std::ofstream	log_file(filename, std::ios::app);

	if (!log_file.is_open())
		std::cerr << "Error, can't open log file '" << filename << "'" << std::endl;
	else
	{
        // Write std::endl if the file is not empty to separate runs
        log_file.seekp(0, std::ios::end);
        if (log_file.tellp() > 0)
            log_file << std::endl << std::string(40, '-') << std::endl << std::endl;

		// Write without ANSI color sequences to keep log files clean
		log_file << "Tests du projet 'libftpp':" << std::endl << std::endl;
		log_file.flush();
	}
	return (log_file);
}
