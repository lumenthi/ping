# GNU Linux ping implementation
Send ICMP ECHO_REQUEST to network hosts
## Description
### General
- Ping works by sending a ICMP echo request packet to a target host and waiting for the ICMP echo reply
- The Internet Control Message Protocol (ICMP) is a network layer protocol used by network devices to diagnose network communication issues
### Properties
- Internet Control Message Protocol (ICMP) is a stand-alone protocol
- ICMP is not associated with any transport layer protocol, such as Transmission Control Protocol (TCP) or User Datagram Protocol (UDP).
- It is a connectionless protocol, meaning a device does not need to open a connection with the target device before sending a message.
- ICMP packets are described within this structure
```C
struct s_packet
{
  struct icmphdr hdr {
      u_int8_t type;            /* Message type (echo, echo reply, etc...) */
      u_int8_t code;            /* Error codes (Host unreachable, TTL exceeded...) */
      u_int16_t checksum;       /* 16-bit one's complement of the one's complement sum of the ICMP message */
      u_int16_t echo.id;        /* PID of the program */
      u_int16_t echo.sequence;  /* Sequence number, is incremented by 1 for each packet */
      u_int32_t gateway;        /* Used for ICMP redirect type */
      [...]                     /* Unused */
  }
  char *msg;
}
```
### Transport
- ICMP packets are encapsulated in IP packets, it relies on IP to transmit its messages.
- Internet Protocol (IP) is a protocol for connectionless services.
## Implementation
### Socket
- A user protocol may receive ICMP packets for all local sockets by opening a raw socket with the protocol IPPROTO_ICMP
```C
socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)
```
### Resolve
- Convert domain names, hostnames, and IP addresses to structured binary formats for the operating system's networking API
```C
getaddrinfo(host, NULL, &hints, &res)
```
- Convert IPv4 and IPv6 addresses from binary to text form
```C
inet_ntop(AF_INET, &((const struct sockaddr_in *)ret->ai_addr)->sin_addr, g_data.ipv4, sizeof(g_data.ipv4))
```
### IP options
- Since ICMP packets are encapsulated in IP packets, we use IPROTO_IP in the socket option, by doing this the kernel will manage the IP header by himself.
- TTL option
```C
setsockopt(g_data.sockfd, IPPROTO_IP, IP_TTL, &g_data.ttl, sizeof(g_data.ttl))
```
- Timeout option
```C
setsockopt(g_data.sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout))
```
### Send and receive loop
- Wait time is handled with SIG_ALRM, a signal is raised every X sec. (subject's constraint)
```C
signal(SIGALRM, alarmhandler);
```
- Format packet
```C
ft_memset(&packet, 0, sizeof(packet));
packet.hdr.type = ICMP_ECHO;
packet.hdr.un.echo.id = getpid();
packet.hdr.un.echo.sequence = g_data.seq++;
packet.hdr.checksum = checksum(&packet, sizeof(packet));
```
- Send packet
```C
sendto(g_data.sockfd, &packet, sizeof(packet), 0, g_data.host_addr, sizeof(*(g_data.host_addr))
```
- Wait for reply
```C
recvfrom(g_data.sockfd, &packet, sizeof(packet), 0, &receiver,&receiver_len)
```
- Print the packet
```C
print_packet(packet, g_data.seq, start_time);
```
### End of the loop (CTRL-C pressed or -c option specified)
- Print summary
```C
print_end();
```
- Free host infos
```C
freeaddrinfo(g_data.host_info);
```
## Options
Implemented options:  
```
-c <count>      stop after <count> replies.
-f              flood ping.
-h              print help and exit.
-i <interval>   seconds between sending each packet.
-q              quiet output.
-v              verbose output.
RTT             round trip time, min, max, average.
```
## Make
Given Makefile contains all standard rules
- all
- clean
- fclean
- re

## Example output
```shell
$ make
 ~ | Compiled libft
 ~ | Compiled ft_ping
$ sudo ./ft_ping localhost     
PING localhost (127.0.0.1) 56(84) bytes of data.
64 bytes from localhost (127.0.0.1): icmp_seq=1 ttl=64 time=0.013 ms
64 bytes from localhost (127.0.0.1): icmp_seq=2 ttl=64 time=0.034 ms
64 bytes from localhost (127.0.0.1): icmp_seq=3 ttl=64 time=0.021 ms
64 bytes from localhost (127.0.0.1): icmp_seq=4 ttl=64 time=0.021 ms
64 bytes from localhost (127.0.0.1): icmp_seq=5 ttl=64 time=0.021 ms
^C
--- localhost ping statistics ---
5 packets transmitted, 5 received, 0% packet loss, time 4411ms
rtt min/avg/max = 0.013/0.022/0.034 ms
```
