/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   display.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/20 11:17:41 by lumenthi          #+#    #+#             */
/*   Updated: 2022/08/20 11:18:25 by lumenthi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

void print_begin()
{
	ft_putstr("PING ");
	ft_putstr(g_data.address);
	ft_putstr(" (");
	ft_putstr(g_data.ipv4);
	ft_putstr(") 56(84) bytes of data.\n");
}

void print_end()
{
	gettimeofday(&g_data.end_time, NULL);

	long long start_ms = g_data.start_time.tv_sec*1000 + g_data.start_time.tv_usec/1000;
	long long end_ms = g_data.end_time.tv_sec*1000 + g_data.end_time.tv_usec/1000;

	long long diff = end_ms - start_ms;

	int average;
	if (g_data.rec == 0) {
		if (g_data.sent == 0)
			average = 0;
		else
			average = 100;
	}
	else
		average = ((float)(g_data.sent - g_data.rec) / (float)g_data.sent) * 100.0;

	ft_putstr("\n--- ");
	ft_putstr(g_data.address);
	ft_putstr(" ping statistics ---\n");

	ft_putnbr(g_data.sent);
	ft_putstr(" packets transmitted, ");
	ft_putnbr(g_data.rec);
	ft_putstr(" received, ");
	ft_putnbr(average);
	ft_putstr("% packet loss, time ");
	ft_putnbr(diff);
	ft_putstr("ms\n");
	print_rtt();
}

int print_help()
{
	printf("Usage\n"
		"  ping [options] <destination>\n\n"
		"Options:\n"
		"  <destination>      dns name or ip address\n"
		"  -h                 print help and exit\n"
		"  -v                 verbose output\n"
		"  IPv4 options:\n"
		"  -4                 use IPv4\n"
		"  IPv6 options:\n"
		"  -6                 use IPv6\n");
	return 0;
}
