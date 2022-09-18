#ifndef RAW_SOCKET_PROJECTS_PARSER_NET_PACKAGE_H
#define RAW_SOCKET_PROJECTS_PARSER_NET_PACKAGE_H

#include <iostream>
#include <linux/types.h>
#include <linux/if_ether.h>
#include <linux/if_ether.h>
#include <linux/if.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

namespace parser {

typedef struct ethhdr EthernetHeader;
typedef struct iphdr IpHeader;
typedef struct udphdr UdpHeader;

EthernetHeader ExtractEthernetHeader(const __u8 *buff);

IpHeader ExtractIpHeader(const __u8 *buff);

UdpHeader ExtractUdpHeader(const __u8 *buff, int size);

void WriteEthernetHeaderTo(std::ostream &ostream, const EthernetHeader &ethernetHeader);

void WriteIpHeaderTo(std::ostream &ostream, const IpHeader &ipHeader);

#endif //RAW_SOCKET_PROJECTS_PARSER_NET_PACKAGE_H

} // namespace parser