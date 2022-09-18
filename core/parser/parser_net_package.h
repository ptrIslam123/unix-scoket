#ifndef RAW_SOCKET_PROJECTS_PARSER_NET_PACKAGE_H
#define RAW_SOCKET_PROJECTS_PARSER_NET_PACKAGE_H

#include <iostream>
#include <linux/types.h>
#include <linux/if_ether.h>
#include <linux/if_ether.h>
#include <linux/if.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>

namespace parser {

constexpr auto TCP_PROTOCOL = 6;
constexpr auto UDP_PROTOCOL = 17;

typedef struct ethhdr EthernetHeader;
typedef struct iphdr IpHeader;
typedef struct udphdr UdpHeader;
typedef struct tcphdr TcpHeader;

EthernetHeader ExtractEthernetHeader(const __u8 *buff);

IpHeader ExtractIpHeader(const __u8 *buff);

UdpHeader ExtractUdpHeader(const __u8 *buff);

TcpHeader ExtractTcpHeader(const __u8 *buff);

__u8 *ExtractData(const __u8 *buff, uint size);

void WriteEthernetHeaderTo(std::ostream &ostream, const EthernetHeader &ethernetHeader);

void WriteIpHeaderTo(std::ostream &ostream, const IpHeader &ipHeader);

void WriteUdpHeaderTo(std::ostream &ostream, const UdpHeader &udpHeader);

void WriteTcpHeaderTo(std::ostream &ostream, const TcpHeader &tcpHeader);

#endif //RAW_SOCKET_PROJECTS_PARSER_NET_PACKAGE_H

} // namespace parser