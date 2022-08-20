/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/20 11:15:03 by lumenthi          #+#    #+#             */
/*   Updated: 2022/08/20 11:23:48 by lumenthi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

static int		get_args(int argc, char **argv, uint8_t *args)
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
						if (!ft_strisnum(argv[i+1])) {
							fprintf(stderr, "%s: bad timing interval\n", argv[0]);
							return -1;
						}
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
	if (destination == -1)
		return 1;
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
