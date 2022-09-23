#ifndef RAW_SOCKET_PROJECTS_PARSER_NET_PACKAGE_H
#define RAW_SOCKET_PROJECTS_PARSER_NET_PACKAGE_H

#include <iostream>
#include <linux/types.h>

#include "types/package_header_types.h"

namespace parser {

void ParsePackageAndPayload(std::ostream &ostream, const __u8 *buff, const uint buffSize);

void ParsePackage(std::ostream &ostream, const __u8 *buff);

EthernetHeader ExtractEthernetHeader(const __u8 *buff);

IpHeader ExtractIpHeader(const __u8 *buff);

UdpHeader ExtractUdpHeader(const __u8 *buff);

TcpHeader ExtractTcpHeader(const __u8 *buff);

std::pair<__u8*, uint> ExtractData(const __u8 *buff, uint size);

void WriteEthernetHeaderTo(std::ostream &ostream, const EthernetHeader &ethernetHeader);

void WriteIpHeaderTo(std::ostream &ostream, const IpHeader &ipHeader);

void WriteUdpHeaderTo(std::ostream &ostream, const UdpHeader &udpHeader);

void WriteTcpHeaderTo(std::ostream &ostream, const TcpHeader &tcpHeader);

#endif //RAW_SOCKET_PROJECTS_PARSER_NET_PACKAGE_H

} // namespace parser