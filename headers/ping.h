/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/15 13:05:31 by lumenthi          #+#    #+#             */
/*   Updated: 2022/08/15 14:43:25 by lumenthi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PING_H
# define PING_H

#include "libft.h"

#include <stdio.h> // PRINTF
#include <signal.h> // SIGNAL HANDLING

/*
	$ strace ping www.google.com
	[...]
	socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP)
	[...]
*/

/* signal.c */
void inthandler(int sig);

#endif
