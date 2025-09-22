/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_lst_utils.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmariot <cmariot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/07 23:35:51 by cmariot           #+#    #+#             */
/*   Updated: 2022/06/07 22:05:23 by cmariot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libunit.hpp"

void	clear_test_list(t_test **test)
{
	t_test	*tmp;

	if (test)
	{
		while (*test)
		{
			tmp = (*test)->next;
			// remove any lingering temp files unless debugging flag set
			if (!g_keep_test_tmp)
			{
				if (!(*test)->filename.empty())
					unlink((*test)->filename.c_str());
				if (!(*test)->err_filename.empty())
					unlink((*test)->err_filename.c_str());
			}
			delete *test;
			*test = tmp;
		}
	}
}
