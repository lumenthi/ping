/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/15 13:04:04 by lumenthi          #+#    #+#             */
/*   Updated: 2022/08/24 11:06:46 by lumenthi         ###   ########.fr       */
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
	if (getaddrinfo(host, NULL, &hints, &res) != 0)
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

void ping_loop()
{
	g_data.seq = 0;
	g_data.rec = 0;
	g_data.error = 0;
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
	if ((gettimeofday(&(g_data.start_time), NULL)) < 0) {
		g_data.start_time.tv_sec = 1;
		g_data.start_time.tv_usec = 1;
	}
	if (ARGS_V)
		printf("%s: Starting the ping loop...\n", g_data.path);
	print_begin();
	process_packet();
	while (1) {
		if (ARGS_F || g_data.interval == 0)
			process_packet();
	}
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
		if (ARGS_V)
			printf("%s: Freeing addrinfo structure...\n", g_data.path);
		freeaddrinfo(g_data.host_info);
		return 1;
	}
	/* Setting TTL option */
	if (ARGS_V)
		printf("%s: Setting TTL option to %d...\n", g_data.path, g_data.ttl);
	if (setsockopt(g_data.sockfd, IPPROTO_IP, IP_TTL, &g_data.ttl, sizeof(g_data.ttl)) != 0) {
		fprintf(stderr, "%s: %s: Failed to set TTL option\n", g_data.path, address);
		if (ARGS_V)
			printf("%s: Freeing addrinfo structure...\n", g_data.path);
		freeaddrinfo(g_data.host_info);
		return 1;
	}
	/* Setting timeout option */
	if (ARGS_V)
		printf("%s: Setting timeout option to %ld...\n", g_data.path, timeout.tv_sec);
	if (setsockopt(g_data.sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) != 0) {
		fprintf(stderr, "%s: %s: Failed to set timeout option\n", g_data.path, address);
		if (ARGS_V)
			printf("%s: Freeing addrinfo structure...\n", g_data.path);
		freeaddrinfo(g_data.host_info);
		return 1;
	}

	ping_loop();

	return 0;
}
