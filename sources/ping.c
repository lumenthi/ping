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

void print_time(long long ms, unsigned int sec, unsigned int usec)
{
	int nbr = 100;
	long long ms2 = sec*1000 + usec/1000;
	int zeroes = 2;

	/*ft_putstr("ms: ");
	ft_putnbr(ms);

	ft_putstr(", ms2: ");
	ft_putnbr(ms2);

	ft_putstr(", sec: ");
	ft_putnbr(sec);

	ft_putstr(", usec: ");
	ft_putnbr(usec);

	ft_putchar('\n');*/

	if (ms2 < ms)
		ms = ms2;

	if (sec && usec > 0 && ms2 < ms) {
		ft_putnbr(sec);
		ft_putstr(".");
	}
	ft_putnbr(ms);
	if (!ms) {
		ft_putchar('.');
		while (nbr > 1) {
			if (!(usec / nbr))
				ft_putchar('0');
			nbr /= 100;
		}
		ft_putnbr(usec);
	}
	else if (ms < 100) {
		while (usec > 1000)
			usec-=1000;
		ft_putchar('.');
		nbr = 10;
		usec = ms > 9 ? usec*0.01 : usec*0.1;
		while (nbr) {
			if (!(usec / nbr))
				zeroes++;
			zeroes--;
			nbr /= 10;
		}
		if (usec == 0)
			zeroes -= 1;
		while (zeroes > 0) {
			ft_putchar('0');
			zeroes--;
		}
		ft_putnbr(usec);
	}
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
	int msec = end_time.tv_usec - start_time.tv_usec;

	long long start_ms = start_time.tv_sec*1000 + start_time.tv_usec/1000;
	long long end_ms = end_time.tv_sec*1000 + end_time.tv_usec/1000;

	long long ms = end_ms - start_ms;

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
	print_time(ms, sec, msec);
	ft_putstr(" ms");
	ft_putchar('\n');
}

void print_begin()
{
	ft_putstr("PING ");
	ft_putstr(g_data.address);
	ft_putstr(" (");
	ft_putstr(g_data.ipv4);
	ft_putstr(") 56(84) bytes of data.\n");
}

void print_rtt()
{
	float min = 0.5;
	float max = 0.5;
	float avg = 0.5;
	float mdev = 0.5;

	printf("rtt min/avg/max/mdev = %.6f/%.6f/%.6f/%.6f ms\n",
	min, avg, max, mdev);
}

void print_end()
{
	struct timeval end_time;
	gettimeofday(&end_time, NULL);

	long long start_ms = g_data.start_time.tv_sec*1000 + g_data.start_time.tv_usec/1000;
	long long end_ms = end_time.tv_sec*1000 + end_time.tv_usec/1000;

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

	/* For debug */
	if (g_data.seq > 500)
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
	else
		g_data.sent++;

	if (recvfrom(g_data.sockfd,
				&packet,
				sizeof(packet),
				0,
				&receiver,
				&receiver_len) <= 0 && g_data.seq > 0)
	{
	}
	else {
		received = 1;
		g_data.rec++;
	}
	if (received)
		print_packet(packet, g_data.seq, start_time);
	alarm(1);
}

void ping_loop()
{
	g_data.seq = 0;
	g_data.rec = 0;
	g_data.sent = 0;
	signal(SIGALRM, alarmhandler);
	signal(SIGINT, inthandler);

	if (ARGS_V)
		printf("%s: Capturing start time...\n", g_data.path);
	gettimeofday(&(g_data.start_time), NULL);
	if (ARGS_V)
		printf("%s: Starting the ping loop...\n", g_data.path);
	print_begin();
	process_packet();
	while (1);
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

void	get_args(int argc, char **argv, uint8_t *args)
{
	int i = 1;
	int j = 0;
	*args = 0x00;
	while (i < argc) {
		if (argv[i] && argv[i][0] == '-') {
			j = 0;
			while (argv[i][j]) {
				if (argv[i][j] == 'v')
					(*args) |= 0x01; // 0000 0001
				else if (argv[i][j] == 'h')
					(*args) |= 0x02; // 0000 0010
				j++;
			}
		}
		i++;
	}
}

int main(int argc, char **argv)
{
	int i = 1;
	if (argc < 2) {
		fprintf(stderr, "%s: usage error: Destination address required\n", argv[0]);
		return 1;
	}

	get_args(argc, argv, &g_data.args);

	if (ARGS_H)
		return (print_help());

	g_data.path = argv[0];

	while (i < argc) {
		if (argv[i] && argv[i][0] != '-')
			ft_ping(argv[i]);
		i++;
	}
	return 0;
}
