/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/15 13:05:31 by lumenthi          #+#    #+#             */
/*   Updated: 2022/08/17 13:51:07 by lumenthi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PING_H
# define PING_H

#include "libft.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>

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

struct sockaddr{
	sa_family_t     sa_family;       address family
	char            sa_data[];       socket address (variable-length data)
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

typedef struct	s_data {
	char ipv4[INET_ADDRSTRLEN];
	char ipv6[INET6_ADDRSTRLEN];
	struct addrinfo *host_info;
	struct sockaddr *host_addr;
	struct timeval start_time;
	char *address;
	unsigned int seq;
	unsigned int sent;
	unsigned int rec;
	int sockfd;
	int ttl;
}	t_data;

/* global */
extern t_data g_data;

/* signal.c */
void inthandler(int sig);
void alarmhandler(int sig);

/* ping.c */
void process_packet();
void print_end();

/* Packets */
#include <netinet/ip_icmp.h>

/*
struct icmphdr
{
	u_int8_t type;				message type
	u_int8_t code;				type sub-code
	u_int16_t checksum;
	union
	{
		struct
		{
			u_int16_t id;
			u_int16_t sequence;
		} echo;					echo datagram
		u_int32_t gateway;		gateway address
		struct
		{
			u_int16_t __unused;
			u_int16_t mtu;
		} frag;					path mtu discovery
	} un;
};
*/

typedef struct s_packet
{
	struct icmphdr hdr;
	char msg[64-sizeof(struct icmphdr)]; /* Define dynamically */
}	t_packet;

/* NOTES:

$ sudo tcpdump ip proto -> to capture ping packets
$ sudo tcpdump -i lo -> to capture local ping packets
$ vagrant rsync-auto -> to synchronize folder

The default is to wait for one second between each packet normally

TODO: google ttl=63 ??
TODO: goole DNS resolve -> par21s12-in-f4.1e100.net # NOT MANDATORY
TODO: Multiple args # NOT MANDATORY
TODO: time formatting

TODO:

sudo ./ft_ping 69.162.81.155
64 bytes from 69.162.81.155 (69.162.81.155): icmp_seq=5 ttl=64 time=110 ms
64 bytes from 69.162.81.155 (69.162.81.155): icmp_seq=6 ttl=64 time=1.4295077 ms ???

start sec: 1660730435, start usec: 965108
end sec:   1660730436, end usec:   75830
64 bytes from 69.162.81.155 (69.162.81.155): icmp_seq=5 ttl=64 time=1.4295078 ms

64 bytes from 69.162.81.155 (69.162.81.155): icmp_seq=1 ttl=64 time=ms: 4295077, sec: 1, usec: -889549
1.4295077 ms

64 bytes from www.google.com (172.217.22.132): icmp_seq=17 ttl=64 time=2.109 ms ????

vagrant@stretch:/vagrant$ sudo ./ft_ping 95.142.107.181
PING 95.142.107.181 (95.142.107.181) 56(84) bytes of data.
64 bytes from 95.142.107.181 (95.142.107.181): icmp_seq=1 ttl=64 time=ms: 9, ms2: 4294975, sec: 1, usec: -991322
9.42939759 ms

ADDRESSES:
69.162.81.155 -> 110ms
95.142.107.181 -> 8.90ms

*/


#endif
