/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/15 13:04:04 by lumenthi          #+#    #+#             */
/*   Updated: 2022/08/15 16:18:50 by lumenthi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

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

	/* TODO: freeaddrinfo(res) when exiting */

	return res;
}

int ft_ping(char *path, char *address)
{
	struct addrinfo *ret;
	struct addrinfo *tmp;
	char ip[INET6_ADDRSTRLEN] = {0};

	signal(SIGINT, inthandler);

	if (!(ret = resolve(address))) {
		printf("%s: %s: Name or service not known\n", path, address);
		return 1;
	}

	tmp = ret;
	while (tmp) {
		if (tmp->ai_family == AF_INET) {
			if (!ip[0] && !(inet_ntop(AF_INET,
				&((const struct sockaddr_in *)ret->ai_addr)->sin_addr,
				ip, sizeof(ip)))) {
				printf("%s: %s: inet_ntop error\n", path, address);
				freeaddrinfo(ret);
				return 1;
			}
		}
		tmp = tmp->ai_next;
	}
	printf("PING %s (%s) 56(84) bytes of data.\n", address, ip);
	while (1);
	return 0;
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("%s: usage error: Destination address required\n", argv[0]);
		return 1;
	}
	ft_ping(argv[0], argv[1]);
	return 0;
}
