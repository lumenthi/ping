/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rtt.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/20 11:11:47 by lumenthi          #+#    #+#             */
/*   Updated: 2022/09/01 12:37:15 by lumenthi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

static void print_rtt_time(unsigned int ms, unsigned int usec)
{
	int nbr = 100;
	int zeroes = 3;

	ft_putnbr(ms);
	ft_putchar('.');
	while (nbr) {
		if (!(usec / nbr))
			zeroes++;
		zeroes--;
		nbr /= 10;
	}
	while (zeroes > 0) {
		ft_putchar('0');
		zeroes--;
	}
	if (usec)
		ft_putnbr(usec);
}

static void print_rtt_avg()
{
	unsigned int total_usec = g_data.total.ms*1000+g_data.total.timeval.tv_usec;
	if (g_data.rec > 0)
		total_usec /= g_data.rec;

	unsigned int usec = total_usec;
	while (usec >= 1000)
		usec -= 1000;
	print_rtt_time(total_usec/1000, total_usec%1000);
}

void print_rtt()
{
	if (g_data.error > 0)
		return;
	ft_putstr("rtt min/avg/max = ");

	/* min */
	print_rtt_time(g_data.min.ms, g_data.min.timeval.tv_usec);
	ft_putchar('/');

	/* avg */
	print_rtt_avg();
	ft_putchar('/');

	/* max */
	print_rtt_time(g_data.max.ms, g_data.max.timeval.tv_usec);

	ft_putstr(" ms\n");
}
