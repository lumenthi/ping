/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/15 13:04:04 by lumenthi          #+#    #+#             */
/*   Updated: 2022/08/15 19:04:52 by lumenthi         ###   ########.fr       */
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
	struct addrinfo *tmp = ret;
	while (tmp) {
		if (tmp->ai_family == AF_INET) {
			if (!(inet_ntop(AF_INET,
					&((const struct sockaddr_in *)tmp->ai_addr)->sin_addr,
					g_data.ipv4, sizeof(g_data.ipv4)))) {
				return 1;
			}
			else {
				g_data.host_addr = tmp->ai_addr;
				return 0;
			}
		}
		tmp = tmp->ai_next;
	}
	return 0;
}

void ping_loop()
{
	struct icmphdr	packet_hdr;
	t_packet		packet;

	signal(SIGINT, inthandler);

	printf("PING %s (%s) 56(84) bytes of data.\n", g_data.address, g_data.ipv4);
	while (1) {
		/* Formatting packet */
		ft_memset(&packet_hdr, 0, sizeof(struct icmphdr));
		packet_hdr.type = ICMP_ECHO;
		packet_hdr.un.echo.id = getpid();
		if (sendto(g_data.sockfd, &packet, sizeof(packet), 0,
			g_data.host_addr, sizeof(g_data.host_addr)) <= 0) {
			/* Packet failed */
		}
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
	if ((g_data.sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP)) < 0) {
		fprintf(stderr, "%s: %s: Failed to create the socket\n", path, address);
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
