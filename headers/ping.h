/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/15 13:05:31 by lumenthi          #+#    #+#             */
/*   Updated: 2022/08/15 16:06:46 by lumenthi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PING_H
# define PING_H

#include "libft.h"

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

/*
	$ strace ping www.google.com
	[...]
	socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP)
	[...]
*/

/*
struct addrinfo {
	int              ai_flags;
	int              ai_family;
	int              ai_socktype;
	int              ai_protocol;
	size_t           ai_addrlen;
	struct sockaddr *ai_addr;
	char            *ai_canonname;
	struct addrinfo *ai_next;
};

struct sockaddr_in {
	short            sin_family;
	unsigned short   sin_port;
	struct in_addr   sin_addr;
	char             sin_zero[8];
};

struct in_addr {
	unsigned long s_addr;  // load with inet_aton()
};
*/

/* signal.c */
void inthandler(int sig);

#endif
