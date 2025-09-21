/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   results.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmariot <cmariot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/08 21:42:14 by cmariot           #+#    #+#             */
/*   Updated: 2022/08/26 15:28:14 by cmariot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libunit.hpp"

int	results(int succeeded, int total, std::ostream &fd, bool recursive)
{
	if (recursive == true)
		results(succeeded, total, static_cast<std::ostream &>(std::cout), false);
	// Print without colors to the provided stream
	fd << std::endl << succeeded << " / " << total << " = ";
	if (succeeded == total)
	{
		if (&fd == &std::cout)
			fd << GREEN << "[OK]" << RESET << std::endl;
		else
			fd << "[OK]" << std::endl;
	}
	else
	{
		if (&fd == &std::cout)
			fd << RED_COLOR << "[KO]" << RESET << std::endl;
		else
			fd << "[KO]" << std::endl;
	}
	return (0);
}
