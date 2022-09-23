#ifndef UNIX_NET_PACKAGE_HEADER_TYPES_H
#define UNIX_NET_PACKAGE_HEADER_TYPES_H

#include <linux/if_ether.h>
#include <linux/if.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>

typedef struct ethhdr EthernetHeader;
typedef struct iphdr IpHeader;
typedef struct udphdr UdpHeader;
typedef struct tcphdr TcpHeader;

#endif //UNIX_NET_PACKAGE_HEADER_TYPES_H
