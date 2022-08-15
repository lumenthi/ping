/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/15 14:37:38 by lumenthi          #+#    #+#             */
/*   Updated: 2022/08/15 17:28:30 by lumenthi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

#include  <stdio.h>
#include  <signal.h>
#include  <stdlib.h>

/* Control-C */
void  inthandler(int sig)
{
	(void)sig;
	printf("\n--- %s ping statistics ---", g_data.address);
	if (g_data.host_info)
		freeaddrinfo(g_data.host_info);
	exit(0);
}
