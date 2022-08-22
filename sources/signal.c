/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/15 14:37:38 by lumenthi          #+#    #+#             */
/*   Updated: 2022/08/22 16:47:21 by lumenthi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

/* Control-C */
void  inthandler(int sig)
{
	(void)sig;
	print_end();
	if (g_data.host_info) {
		if (ARGS_V)
			printf("%s: Freeing addinfo structure...\n", g_data.path);
		freeaddrinfo(g_data.host_info);
	}
	exit(0);
}

/* Alarm */
void  alarmhandler(int sig)
{
	(void)sig;
	process_packet();
}
