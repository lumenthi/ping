/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/15 13:04:04 by lumenthi          #+#    #+#             */
/*   Updated: 2022/08/17 17:48:55 by lumenthi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

t_data g_data;

static struct addrinfo *resolve(char *host)
{
	struct addrinfo hints;
	struct addrinfo *res;

	if (!ft_memset(&hints, 0, sizeof(struct addrinfo)))
		return NULL;
	hints.ai_family = AF_INET;

	/* subject to any restrictions imposed by hints */
	if (getaddrinfo(host, NULL, &hints, &res) == -1)
		return NULL;

	return res;
}

static int host_informations(struct addrinfo *ret)
{
	if (!(inet_ntop(AF_INET,
		&((const struct sockaddr_in *)ret->ai_addr)->sin_addr,
		g_data.ipv4,
		sizeof(g_data.ipv4))))
	{
		return 1;
	}
	else {
		g_data.host_addr = ret->ai_addr;
		return 0;
	}
	return 0;
}

unsigned short checksum(void *b, int len)
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

void debug_packet(t_packet packet)
{
	printf("===========HEADER============\n");
	printf("type: %d\ncode: %d\nchecksum: %d\n", packet.hdr.type, packet.hdr.code, packet.hdr.checksum);

	printf("echo.id: %d\necho.sequence: %d\n", packet.hdr.un.echo.id, packet.hdr.un.echo.sequence);

	printf("gateway: %d\n", packet.hdr.un.gateway);

	printf("frag.mtu: %d\n", packet.hdr.un.frag.mtu);

	printf("==========MESSAGE==========\n");
	printf("%s\n", packet.msg);
}

void print_packet_time(long long ms, unsigned int sec, unsigned int usec)
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
		usec*= mult;
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

void print_packet(t_packet packet, unsigned int packet_nbr, struct timeval start_time)
{
	struct timeval end_time;

	gettimeofday(&end_time, NULL);
	/* ft_putstr("start sec: ");
	ft_putnbr(start_time.tv_sec);
	ft_putstr(", start usec: ");
	ft_putnbr(start_time.tv_usec);

	ft_putstr("end sec: ");
	ft_putnbr(end_time.tv_sec);
	ft_putstr(", end usec: ");
	ft_putnbr(end_time.tv_usec);
	ft_putchar('\n'); */

	int sec = end_time.tv_sec - start_time.tv_sec;
	int usec = end_time.tv_usec - start_time.tv_usec;

	long long start_ms = start_time.tv_sec*1000 + start_time.tv_usec/1000;
	long long end_ms = end_time.tv_sec*1000 + end_time.tv_usec/1000;

	long long ms = end_ms - start_ms;

	//printf("start_ms: %lld, end_ms: %lld, ms: %lld\n", start_ms, end_ms, ms);

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

void print_begin()
{
	ft_putstr("PING ");
	ft_putstr(g_data.address);
	ft_putstr(" (");
	ft_putstr(g_data.ipv4);
	ft_putstr(") 56(84) bytes of data.\n");
}

void print_rtt_time(unsigned int ms, unsigned int usec)
{
	int nbr = 100;
	int zeroes = 3;

	while (usec >= 1000)
		usec -= 1000;
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

void print_rtt_avg()
{
	unsigned int total_usec = g_data.total.ms*1000+g_data.total.timeval.tv_usec;
	double average_usec = (float)total_usec/(float)g_data.rec;
	double usec = (unsigned int)average_usec % 1000;

	if (!usec) {
		usec = average_usec;
		while ((usec / 1) < 1)
			usec*=10;
	}
	print_rtt_time(average_usec/1000, usec);
}

void print_rtt()
{
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

void ping_loop()
{
	g_data.seq = 0;
	g_data.rec = 0;
	g_data.sent = 0;

	/* max */
	g_data.min.timeval.tv_sec = 0;
	g_data.min.timeval.tv_usec = 0;
	g_data.min.ms = 0;
	/* min */
	g_data.max.timeval.tv_sec = 0;
	g_data.max.timeval.tv_usec = 0;
	g_data.max.ms = 0;
	/* average */
	g_data.total.timeval.tv_sec = 0;
	g_data.total.timeval.tv_usec = 0;
	g_data.total.ms = 0;

	signal(SIGALRM, alarmhandler);
	signal(SIGINT, inthandler);

	if (ARGS_V)
		printf("%s: Capturing start time...\n", g_data.path);
	gettimeofday(&(g_data.start_time), NULL);
	if (ARGS_V)
		printf("%s: Starting the ping loop...\n", g_data.path);
	print_begin();
	process_packet();
	while (1) {
		if (ARGS_F || g_data.interval == 0)
			process_packet();
	}
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

int ft_ping(char *address)
{
	/* default TTL */
	g_data.ttl = 64;
	/* default timeout (seconds) */
	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	if (ARGS_C && g_data.count <= 0) {
		fprintf(stderr, "%s: %s: bad number of packets to transmit\n", g_data.path, address);
		return 1;
	}

	if (ARGS_I && g_data.interval < 0) {
		fprintf(stderr, "%s: %s: bad timing interval\n", g_data.path, address);
		return 1;
	}

	g_data.address = address;
	/* Socket creation RAW/DGRAM ? */
	if (ARGS_V)
		printf("%s: Creating raw socket...\n", g_data.path);
	if ((g_data.sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
		fprintf(stderr, "%s: %s: Failed to create socket\n", g_data.path, address);
		return 1;
	}
	/* Resolving host */
	if (ARGS_V)
		printf("%s: Resolving host...\n", g_data.path);
	if (!(g_data.host_info = resolve(address))) {
		fprintf(stderr, "%s: %s: Name or service not known\n", g_data.path, address);
		return 1;
	}
	/* g_data.host_info is allocated ! Must free it now */
	/* Getting informations about host */
	if (ARGS_V)
		printf("%s: Gathering informations about host...\n", g_data.path);
	if (host_informations(g_data.host_info)) {
		fprintf(stderr, "%s: %s: Failed to get informations about the host\n", g_data.path, address);
		freeaddrinfo(g_data.host_info);
		return 1;
	}
	/* Setting TTL option */
	if (ARGS_V)
		printf("%s: Setting TTL option to %d...\n", g_data.path, g_data.ttl);
	if (setsockopt(g_data.sockfd, IPPROTO_IP, IP_TTL, &g_data.ttl, sizeof(g_data.ttl)) != 0) {
		fprintf(stderr, "%s: %s: Failed to set TTL option\n", g_data.path, address);
		freeaddrinfo(g_data.host_info);
		return 1;
	}
	/* Setting timeout option */
	if (ARGS_V)
		printf("%s: Setting timeout option to %ld...\n", g_data.path, timeout.tv_sec);
	if (setsockopt(g_data.sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) != 0) {
		fprintf(stderr, "%s: %s: Failed to set timeout option\n", g_data.path, address);
		freeaddrinfo(g_data.host_info);
		return 1;
	}

	ping_loop();

	return 0;
}

int		get_args(int argc, char **argv, uint8_t *args)
{
	int i = 1;
	int j = 0;
	int ret = 0;
	int next = 0;
	*args = 0x00;
	while (i < argc) {
		next = 0;
		if (argv[i] && argv[i][0] == '-') {
			j = 0;
			while (argv[i][j]) {
				if (argv[i][j] == 'v')
					(*args) |= 0x01; // 0000 0001
				else if (argv[i][j] == 'h')
					(*args) |= 0x02; // 0000 0010
				else if (argv[i][j] == 'f')
					(*args) |= 0x04; // 0000 0100
				else if (argv[i][j] == 'c') {
					(*args) |= 0x08; // 0000 1000
					if (i+1 < argc) {
						next = 1;
						g_data.count = ft_atoi(argv[i+1]);
					}
				}
				else if (argv[i][j] == 'q')
					(*args) |= 0x10; // 0001 0000
				else if (argv[i][j] == 'i') {
					(*args) |= 0x20; // 0010 0000
					if (i+1 < argc) {
						next = 1;
						g_data.interval = ft_atoi(argv[i+1]);
					}
				}
				j++;
			}
		}
		else
			ret = i;
		i += next ? 2 : 1;
	}
	return ret;
}

int main(int argc, char **argv)
{
	int destination = 0;

	/* Default ping interval */
	g_data.interval = 1;

	destination = get_args(argc, argv, &g_data.args);

	if (argc < 2 || !destination) {
		fprintf(stderr, "%s: usage error: Destination address required\n", argv[0]);
		return 1;
	}

	if (ARGS_H)
		return (print_help());

	g_data.path = argv[0];

	ft_ping(argv[destination]);
	return 0;
}
