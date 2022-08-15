/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/15 13:04:04 by lumenthi          #+#    #+#             */
/*   Updated: 2022/08/15 14:43:58 by lumenthi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

int ft_ping(char *address)
{
	signal(SIGINT, inthandler);
	printf("Starting ping on %s...\n", address);
	while (1);
	return 0;
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("%s: usage error: Destination address required\n", argv[0]);
		return 1;
	}
	ft_ping(argv[1]);
	return 0;
}
