/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/15 13:04:04 by lumenthi          #+#    #+#             */
/*   Updated: 2022/08/16 14:38:30 by lumenthi         ###   ########.fr       */
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
	unsigned short *buf = b;
	unsigned int sum=0;
	unsigned short result;

	for ( sum = 0; len > 1; len -= 2 )
		sum += *buf++;
	if ( len == 1 )
		sum += *(unsigned char*)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
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

void ping_loop()
{
	t_packet		packet;
	int				packet_nbr;

	signal(SIGINT, inthandler);

	printf("PING %s (%s) 56(84) bytes of data.\n", g_data.address, g_data.ipv4);

	int i = 1;
	packet_nbr = 0;
	while (i) {
		/* Formatting packet header */
		ft_memset(&packet, 0, sizeof(packet));
		packet.hdr.type = ICMP_ECHO;
		packet.hdr.un.echo.id = getpid();
		packet.hdr.un.echo.sequence = packet_nbr++;
		packet.hdr.checksum = checksum(&packet, sizeof(packet));

		debug_packet(packet);
		if (sendto(g_data.sockfd,
					&packet,
					sizeof(packet),
					0,
					g_data.host_addr,
					sizeof(*(g_data.host_addr))) <= 0)
		{
			fprintf(stderr, "Failed to send packet\n");
		}
		i--;
	}
}

int ft_ping(char *path, char *address)
{
	/* default TTL */
	int ttl = 64;
	/* default timeout (seconds) */
	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	g_data.address = address;
	/* Socket creation RAW/DGRAM ? */
	if ((g_data.sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
		fprintf(stderr, "%s: %s: Failed to create socket\n", path, address);
		return 1;
	}
	/* Resolving host */
	if (!(g_data.host_info = resolve(address))) {
		fprintf(stderr, "%s: %s: Name or service not known\n", path, address);
		return 1;
	}
	/* g_data.host_info is allocated ! Must free it now */
	/* Getting informations about host */
	if (host_informations(g_data.host_info)) {
		fprintf(stderr, "%s: %s: Failed to get informations about the host\n", path, address);
		freeaddrinfo(g_data.host_info);
		return 1;
	}
	/* Setting TTL option */
	if (setsockopt(g_data.sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) != 0) {
		fprintf(stderr, "%s: %s: Failed to set TTL option\n", path, address);
		freeaddrinfo(g_data.host_info);
		return 1;
	}
	/* Setting timeout option */
	if (setsockopt(g_data.sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) != 0) {
		fprintf(stderr, "%s: %s: Failed to set timeout option\n", path, address);
		freeaddrinfo(g_data.host_info);
		return 1;
	}

	ping_loop();

	return 0;
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "%s: usage error: Destination address required\n", argv[0]);
		return 1;
	}
	ft_ping(argv[0], argv[1]);
	return 0;
}
