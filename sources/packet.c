/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   packet.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/20 11:05:20 by lumenthi          #+#    #+#             */
/*   Updated: 2022/09/01 17:18:31 by lumenthi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

static void print_packet_time(long long ms, unsigned int sec, unsigned int usec)
{
	int nbr = 100;
	long long ms2 = sec*1000 + usec/1000;
	int zeroes = 1;

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
	unsigned int usec_r = usec;
	int tmp = ms;
	while (tmp) {
		if (!(usec / nbr))
			zeroes++;
		nbr /= 10;
		tmp /= 10;
		zeroes--;
	}
	float mult = (float)nbr / (float)100;
	if (usec < 10 && zeroes > 0 && nbr < 10)
		zeroes--;
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
		(ms == g_data.min.ms && usec_r < g_data.min.timeval.tv_usec))
	{
		g_data.min.timeval.tv_sec = sec;
		g_data.min.timeval.tv_usec = usec_r;
		g_data.min.ms = ms;
	}
	if ((g_data.max.timeval.tv_usec == 0 && g_data.max.ms == 0) ||
		ms > g_data.max.ms ||
		(ms == g_data.max.ms && usec_r > g_data.max.timeval.tv_usec))
	{
		g_data.max.timeval.tv_sec = sec;
		g_data.max.timeval.tv_usec = usec_r;
		g_data.max.ms = ms;
	}
	/* RTT average */
	g_data.total.timeval.tv_usec += usec_r;
	g_data.total.ms += ms;
	if (g_data.total.timeval.tv_usec / 1000) {
		g_data.total.ms++;
		g_data.total.timeval.tv_usec %= 1000;
	}
}

static void print_packet(t_packet packet, unsigned int packet_nbr, struct timeval start_time)
{
	struct timeval end_time;

	if ((gettimeofday(&end_time, NULL)) < 0) {
		end_time.tv_sec = 1;
		end_time.tv_usec = 1;
	}

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
		if (packet.hdr.type == 11) {
			ft_putstr(" Time to live exceeded\n");
			return ;
		}
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
	struct timeval	start_time;
	int				received;
	struct msghdr	msg;
	struct iovec	iov[1];
	char			buf[sizeof(t_packet)+sizeof(struct iphdr)];
	t_packet		ret;

	/* Prepare ret */
	ft_memset(&msg, 0, sizeof(msg));
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	iov[0].iov_base = buf;
	iov[0].iov_len = sizeof(buf);

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

	/* Prepare timer */
	if ((gettimeofday(&start_time, NULL)) < 0) {
		start_time.tv_sec = 1;
		start_time.tv_usec = 1;
	}

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

	while (!received) {
		if (recvmsg(g_data.sockfd, &msg, 0) < 0 && g_data.seq > 0)
		{
		}
		else {
			void *tmp = &buf;
			ret = *(t_packet*)(tmp+sizeof(struct iphdr));
			g_data.ttl = ((struct iphdr*)tmp)->ttl;
			if (ret.hdr.type == ICMP_TIME_EXCEEDED) {
				if (ARGS_F)
					ft_putstr("\bE");
				g_data.error++;
			}
			else if (ret.hdr.type == ICMP_ECHOREPLY) {
				if (ARGS_F)
					ft_putstr("\b \b");
				g_data.rec++;
			}
			if (ret.hdr.type != ICMP_ECHO)
				received = 1;
		}
	}
	print_packet(ret, g_data.seq, start_time);
	if (!(ARGS_F) && g_data.interval > 0)
		alarm(g_data.interval);
	return;
}
