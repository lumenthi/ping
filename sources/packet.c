/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   packet.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/20 11:05:20 by lumenthi          #+#    #+#             */
/*   Updated: 2022/08/20 13:26:40 by lumenthi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

static void print_packet_time(long long ms, unsigned int sec, unsigned int usec)
{
	int nbr = 100;
	long long ms2 = sec*1000 + usec/1000;
	int zeroes = 2;

	if (ms2 < ms)
		ms = ms2;

	if (sec && usec > 0 && ms2 < ms && !(ARGS_F) && !(ARGS_Q)) {
		ft_putnbr(sec);
		ft_putstr(".");
	}
	if (!(ARGS_F) && !(ARGS_Q))
		ft_putnbr(ms);
	if (!ms) {
		if (!(ARGS_F) && !(ARGS_Q))
			ft_putchar('.');
		while (nbr > 1) {
			if (!(usec / nbr)) {
				if (!(ARGS_F) && !(ARGS_Q))
					ft_putchar('0');
			}
			nbr /= 10;
		}
		if (!(ARGS_F) && !(ARGS_Q))
			ft_putnbr(usec);
	}
	while (usec >= 1000)
		usec-=1000;
	int tmp = ms;
	while (tmp) {
		nbr /= 10;
		tmp /= 10;
		zeroes--;
	}
	float mult = (float)nbr / (float)100;
	if (ms && mult) {
		usec *= mult;
		if (!(ARGS_F) && !(ARGS_Q))
			ft_putchar('.');
		while (zeroes > 0) {
			if (!(ARGS_F) && !(ARGS_Q))
				ft_putchar('0');
			zeroes--;
		}
		if (!(ARGS_F) && !(ARGS_Q))
			ft_putnbr(usec);
	}
	/* RTT min / max */
	if ((g_data.min.timeval.tv_usec == 0 && g_data.min.ms == 0) ||
		ms < g_data.min.ms ||
		(ms == g_data.min.ms && usec < g_data.min.timeval.tv_usec))
	{
		g_data.min.timeval.tv_sec = sec;
		g_data.min.timeval.tv_usec = usec;
		g_data.min.ms = ms;
	}
	if ((g_data.max.timeval.tv_usec == 0 && g_data.max.ms == 0) ||
		ms > g_data.max.ms ||
		(ms == g_data.max.ms && usec > g_data.max.timeval.tv_usec))
	{
		g_data.max.timeval.tv_sec = sec;
		g_data.max.timeval.tv_usec = usec;
		g_data.max.ms = ms;
	}
	/* RTT average */
	g_data.total.timeval.tv_usec += usec;
	g_data.total.ms += ms;
	if (g_data.total.timeval.tv_usec / 1000)
		g_data.total.timeval.tv_usec /= 1000;
}

static void print_packet(t_packet packet, unsigned int packet_nbr, struct timeval start_time)
{
	struct timeval end_time;

	gettimeofday(&end_time, NULL);

	int sec = end_time.tv_sec - start_time.tv_sec;
	int usec = end_time.tv_usec - start_time.tv_usec;

	long long start_ms = start_time.tv_sec*1000 + start_time.tv_usec/1000;
	long long end_ms = end_time.tv_sec*1000 + end_time.tv_usec/1000;

	long long ms = end_ms - start_ms;

	if (!(ARGS_F) && !(ARGS_Q)) {
		ft_putnbr(sizeof(packet));
		ft_putstr(" bytes from ");
		ft_putstr(g_data.address);
		ft_putstr(" (");
		ft_putstr(g_data.ipv4);
		ft_putstr("): icmp_seq=");
		ft_putnbr(packet_nbr);
		ft_putstr(" ttl=");
		ft_putnbr(g_data.ttl);
		ft_putstr(" time=");
	}
	print_packet_time(ms, sec, usec);
	if (!(ARGS_F) && !(ARGS_Q)) {
		ft_putstr(" ms");
		ft_putchar('\n');
	}
}

static unsigned short checksum(void *b, int len)
{
	unsigned short	*buf = b;
	unsigned int	sum = 0;
	unsigned short	result;

	/* Sum up 2-byte values until none or only one byte left. */
	while (len > 1) {
		sum += *buf++;
		len -= 2;
	}

	/* Add left-over byte, if any. */
	if (len == 1)
		sum += *(unsigned char*)buf;

	/* Fold 32-bit sum into 16 bits; we lose information by doing this,
		increasing the chances of a collision.
		sum = (lower 16 bits) + (upper 16 bits shifted right 16 bits)*/
	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	/* Checksum is one's compliment of sum. */
	result = ~sum;
	return result;
}

void process_packet()
{
	t_packet		packet;
	struct sockaddr	receiver;
	socklen_t		receiver_len;
	struct timeval	start_time;
	int				received;

	/* Count */
	if (ARGS_C && g_data.seq >= (unsigned int)g_data.count)
		inthandler(2);

	/* Set our receive flag to 0 */
	received = 0;

	/* Formatting packet header */
	ft_memset(&packet, 0, sizeof(packet));
	packet.hdr.type = ICMP_ECHO;
	packet.hdr.un.echo.id = getpid();
	packet.hdr.un.echo.sequence = g_data.seq++;
	packet.hdr.checksum = checksum(&packet, sizeof(packet));

	/* Preparing receiver */
	receiver_len = sizeof(receiver);

	/* Prepare timer */
	gettimeofday(&start_time, NULL); /* TODO: CHECK RET FOR TIMEOFDAY */

	/* debug_packet(packet); */
	if (sendto(g_data.sockfd,
				&packet,
				sizeof(packet),
				0,
				g_data.host_addr,
				sizeof(*(g_data.host_addr))) <= 0)
	{
		fprintf(stderr, "Failed to send packet\n");
	}
	else {
		if (ARGS_F)
			ft_putchar('.');
		g_data.sent++;
	}

	if (recvfrom(g_data.sockfd,
				&packet,
				sizeof(packet),
				0,
				&receiver,
				&receiver_len) <= 0 && g_data.seq > 0)
	{
	}
	else {
		if (ARGS_F)
			ft_putstr("\b \b");
		received = 1;
		g_data.rec++;
	}
	if (received)
		print_packet(packet, g_data.seq, start_time);
	if (!(ARGS_F) && g_data.interval > 0)
		alarm(g_data.interval);
	return;
}