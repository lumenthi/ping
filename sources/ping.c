/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/15 13:04:04 by lumenthi          #+#    #+#             */
/*   Updated: 2022/08/15 17:01:07 by lumenthi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

t_data g_data;

struct addrinfo *resolve(char *host)
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

int host_informations(struct addrinfo *ret)
{
	struct addrinfo *tmp = ret;
	while (tmp) {
		if (tmp->ai_family == AF_INET) {
			if (!(inet_ntop(AF_INET,
				&((const struct sockaddr_in *)tmp->ai_addr)->sin_addr,
				g_data.ipv4, sizeof(g_data.ipv4)))) {
				return 1;
			}
		}
		tmp = tmp->ai_next;
	}
	return 0;
}

int ft_ping(char *path, char *address)
{
	signal(SIGINT, inthandler);

	g_data.address = address;
	if (!(g_data.host_info = resolve(address))) {
		fprintf(stderr, "%s: %s: Name or service not known\n", path, address);
		return 1;
	}

	if (host_informations(g_data.host_info)) {
		fprintf(stderr, "%s: %s: Failed to get informations about the host\n", path, address);
		freeaddrinfo(g_data.host_info);
	}

	printf("PING %s (%s) 56(84) bytes of data.\n", address, g_data.ipv4);
	while (1);
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
