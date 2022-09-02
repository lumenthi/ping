/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/15 13:05:31 by lumenthi          #+#    #+#             */
/*   Updated: 2022/09/02 12:37:02 by lumenthi         ###   ########.fr       */
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
#include <limits.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>

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
	unsigned long s_addr; load with inet_aton()
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
	unsigned int error;
	long long tsum;
	long long tsum2;
	int resolved;
	int sockfd;
	int ttl;
	int count;
	int interval;
	/* RTT related */
	t_time min;
	t_time max;
	t_time total;
}	t_data;

/* global */
extern t_data g_data;

/* signal.c */
void	inthandler(int sig);
void	alarmhandler(int sig);

/* ping.c */
void	print_end();
int		ft_ping(char *address);

/* packet.c */
void	process_packet();

/* rtt.c */
void	print_rtt();

/* display.c */
int		print_help();
void	print_end();
void	print_begin();

/* Message */
/*
struct iovec {                    Scatter/gather array items
    void  *iov_base;              Starting address
    size_t iov_len;               Number of bytes to transfer
};

struct msghdr {
    void         *msg_name;       optional address
    socklen_t     msg_namelen;    size of address
    struct iovec *msg_iov;        scatter/gather array
    size_t        msg_iovlen;     # elements in msg_iov
    void         *msg_control;    ancillary data, see below
    size_t        msg_controllen; ancillary data buffer len
    int           msg_flags;      flags on received message
}; */

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
# define ARGS_I g_data.args & 0x20
# define ARGS_T g_data.args & 0x40

/* NOTES:

$ sudo tcpdump ip proto -> to capture ping packets
$ sudo tcpdump -i lo -> to capture local ping packets

The default is to wait for one second between each packet normally

-i interval | DONE
-q quiet output | DONE
-f flood ping | DONE
-c count | DONE
-t ttl | DONE
-RTT | DONE

vagrant plugin install vagrant-disksize
vagrant rsync-auto

tsum += triptime;
tsum2 += (long long)triptime * (long long)triptime

tsum /= nreceived + nrepeats;
tsum2 /= nreceived + nrepeats;
tmdev = llsqrt(tsum2 - tsum * tsum);

(long)tmdev / 1000, (long)tmdev % 1000

*/

#endif
