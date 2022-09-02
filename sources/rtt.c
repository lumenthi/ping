/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rtt.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/20 11:11:47 by lumenthi          #+#    #+#             */
/*   Updated: 2022/09/02 15:23:15 by lumenthi         ###   ########.fr       */
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

static long llsqrt(long long a)
{
	long long prev = LLONG_MAX;
	long long x = a;

	if (x > 0) {
		while (x < prev) {
			prev = x;
			x = (x + (a / x)) / 2;
		}
	}

	return (long)x;
}

static void print_rtt_mdev(long long tsum, long long tsum2, unsigned int count)
{
	double tmdev;
	count = count == 0 ? 1: count;
	long long tmvar;

	tmvar = (tsum2 - ((tsum * tsum) / count)) / count;

	tmdev = llsqrt(tmvar);

	print_rtt_time((long)tmdev/1000, (long)tmdev%1000);
}

void print_rtt()
{
	if (g_data.error > 0)
		return;
	ft_putstr("rtt min/avg/max/mdev = ");

	/* min */
	print_rtt_time(g_data.min.ms, g_data.min.timeval.tv_usec);
	ft_putchar('/');

	/* avg */
	print_rtt_avg();
	ft_putchar('/');

	/* max */
	print_rtt_time(g_data.max.ms, g_data.max.timeval.tv_usec);
	ft_putchar('/');

	/* mdev */
	print_rtt_mdev(g_data.tsum, g_data.tsum2, g_data.rec);

	ft_putstr(" ms\n");
}
