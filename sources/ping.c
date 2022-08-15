/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/15 13:04:04 by lumenthi          #+#    #+#             */
/*   Updated: 2022/08/15 13:08:49 by lumenthi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

int main(int argc, char **argv)
{

	if (argc < 2)
		printf("%s: usage error: Destination address required\n", argv[0]);

	return 0;
}
