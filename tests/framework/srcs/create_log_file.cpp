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
#include <ctime>
#include <cstdlib>
#include <array>
#include <memory>
#include <cstdio>

bool g_keep_test_tmp = false;
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

	// Optionally keep tmp files when KEEP_TEST_TMP is set in the environment
	const char *keep_env = std::getenv("KEEP_TEST_TMP");
	if (keep_env && std::string(keep_env) == "1")
		g_keep_test_tmp = true;

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
		// Add timestamp and git sha (if available)
		std::time_t t = std::time(nullptr);
		char buf[64];
		std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
		std::string git_sha = "(unknown)";
		// try to read git sha (short) via popen; ignore failures
		{
			std::array<char, 128> out;
			std::string cmd = "git rev-parse --short HEAD 2>/dev/null";
			std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
			if (pipe) {
				if (fgets(out.data(), (int)out.size(), pipe.get()) != nullptr) {
					git_sha = std::string(out.data());
					// trim newline
					if (!git_sha.empty() && git_sha.back() == '\n') git_sha.pop_back();
				}
			}
		}
		log_file << "Tests du projet 'libftpp' - " << buf << " - git:" << git_sha << std::endl << std::endl;
		log_file.flush();
	}
	return (log_file);
}
