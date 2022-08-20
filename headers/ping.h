/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/15 13:05:31 by lumenthi          #+#    #+#             */
/*   Updated: 2022/08/17 17:42:44 by lumenthi         ###   ########.fr       */
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

typedef struct s_time {
	struct timeval timeval;
	unsigned int ms;
}	t_time;

typedef struct	s_data {
	uint8_t args;
	char *path;
	char ipv4[INET_ADDRSTRLEN];
	char ipv6[INET6_ADDRSTRLEN];
	struct addrinfo *host_info;
	struct sockaddr *host_addr;
	struct timeval start_time;
	struct timeval end_time;
	char *address;
	unsigned int seq;
	unsigned int sent;
	unsigned int rec;
	int sockfd;
	int ttl;
	int count;
	/* RTT related */
	t_time min;
	t_time max;
	t_time total;
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

# define ARGS_V g_data.args & 0x01
# define ARGS_H g_data.args & 0x02
# define ARGS_F g_data.args & 0x04
# define ARGS_C g_data.args & 0x08
# define ARGS_Q g_data.args & 0x10

/* NOTES:

$ sudo tcpdump ip proto -> to capture ping packets
$ sudo tcpdump -i lo -> to capture local ping packets
$ vagrant rsync-auto -> to synchronize folder

The default is to wait for one second between each packet normally

TODO: google ttl=63 ?? IPv6 !
TODO: goole DNS resolve -> par21s12-in-f4.1e100.net # NOT MANDATORY
TODO: Multiple args # NOT MANDATORY

ADDRESSES:
69.162.81.155 -> 110ms
95.142.107.181 -> 8.90ms
152.99.1.10 -> Timeout


TODO: RTT line -> bonus
64 bytes from localhost (127.0.0.1): icmp_seq=1 ttl=64 time=0.022 ms
64 bytes from localhost (127.0.0.1): icmp_seq=2 ttl=64 time=0.046 ms
^C
--- localhost ping statistics ---
2 packets transmitted, 2 received, 0% packet loss, time 1030ms
rtt min/avg/max/mdev = 0.022/0.034/0.046/0.012 ms <--- This line

Round-trip time (RTT) is the duration, measured in milliseconds, from when a browser sends a request to when it receives a response from a server

(FQDN) Fully qualified domain name

mdev:       tsum += triptime;
            tsum2 += (long long)triptime * (long long)triptime
            tsum /= nreceived + nrepeats;
            tsum2 /= nreceived + nrepeats;
            tmdev = llsqrt(tsum2 - tsum * tsum);

-i interval

-q quiet output | DONE
-f flood ping | DONE
-c count | DONE
-RTT | WIP

➜  ping git:(master) ✗ sudo ./ft_ping localhost -f
PING localhost (127.0.0.1) 56(84) bytes of data.
^C
--- localhost ping statistics ---
602897 packets transmitted, 602897 received, 0% packet loss, time 1347ms
rtt min/avg/max = 0.001/0.002/6.045 ms ???

vagrant plugin install vagrant-disksize
vagrant rsync-auto

vagrant@stretch:/vagrant$ sudo ping www.google.com -t 0 -4
ping: can't set unicast time-to-live: Invalid argument

From 192.168.106.2 (192.168.106.2) icmp_seq=1 Time to live exceeded

*/

#endif
