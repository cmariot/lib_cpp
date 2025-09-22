/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_test.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmariot <cmariot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/07 18:37:09 by cmariot           #+#    #+#             */
/*   Updated: 2022/06/09 22:54:38 by cmariot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libunit.hpp"
#include <signal.h>

static void	exit_child_and_restore(int fd_out, int fd_err, int stdout_backup, int stderr_backup, int status)
{
	// restore fds and exit
	if (fd_out >= 0) close(fd_out);
	if (fd_err >= 0) close(fd_err);
	dup2(stdout_backup, STDOUT_FILENO);
	close(stdout_backup);
	dup2(stderr_backup, STDERR_FILENO);
	close(stderr_backup);
	exit(status);
}

/*
 * Create a tmp file and redirect the STDOUT output into this file.
 */



/*
 * Fork the process :
 *
 *	- in the child : STDOUT redirection in a tmp file & create a thread
 *	  (one for the execution, one for the timer)
 *
 *	- in the parent : wait the exxit status of the child
 */

int	execute_test(t_test **test, std::ofstream &log_file)
{
	(void)log_file;
	pid_t pid;
	int status = 0;
	int fd_out = -1;
	int fd_err = -1;
	int stdout_backup = -1;
	int stderr_backup = -1;

	// record start time
	(*test)->start_time = get_time();
	pid = fork();
	if (pid == -1)
	{
		std::cerr << "Error, fork failed." << std::endl;
		return (1);
	}
	else if (pid == 0)
	{
		// Child: redirect stdout and stderr to separate files and run test function directly
		fd_out = open((*test)->filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0640);
		if (fd_out == -1)
		{
			std::cerr << "Error, opening stdout file failed." << std::endl;
			exit(1);
		}
		fd_err = open((*test)->err_filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0640);
		if (fd_err == -1)
		{
			std::cerr << "Error, opening stderr file failed." << std::endl;
			close(fd_out);
			exit(1);
		}
		stdout_backup = dup(STDOUT_FILENO);
		stderr_backup = dup(STDERR_FILENO);
		dup2(fd_out, STDOUT_FILENO);
		dup2(fd_err, STDERR_FILENO);

		// execute the test function directly
	int ret = (*test)->test_add();
		// flush streams
		fsync(fd_out);
		fsync(fd_err);
		exit_child_and_restore(fd_out, fd_err, stdout_backup, stderr_backup, ret);
	}
	else
	{
		// Parent: waitpid with timeout
		size_t init = get_time();
		while (1)
		{
			pid_t w = waitpid(pid, &status, WNOHANG);
			if (w == -1)
			{
				// error
				(*test)->status = 1;
				break;
			}
			else if (w == 0)
			{
				// child still running
				if (check_timeout(init))
				{
					// kill child
					kill(pid, SIGKILL);
					waitpid(pid, &status, 0);
					(*test)->status = TIMEOUT;
					break;
				}
				usleep(500);
				continue;
			}
			else
			{
				// child exited
				if (WIFEXITED(status))
					(*test)->status = WEXITSTATUS(status);
				else if (WIFSIGNALED(status))
					(*test)->status = WTERMSIG(status);
				// compute duration
				(*test)->duration_ms = get_time() - (*test)->start_time;
				break;
			}
		}
	}
	// parent doesn't unlink files here; print_test_output will consume and unlink
	return (0);
}
