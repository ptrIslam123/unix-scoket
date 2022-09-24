#ifndef UNIX_NET_PACKAGE_HEADERS_H
#define UNIX_NET_PACKAGE_HEADERS_H

#include <linux/types.h>
#include <string_view>

#include "types/package_header_types.h"

namespace package {

typedef struct PseudoHeader {
    u_int32_t sourceAddress; // IP address
    u_int32_t destinationAddress; // IP address
    u_int8_t placeholder; // just placeholder
    u_int8_t protocol; // IPPROTOCOL_UDP
    u_int16_t udpLength; // UDP header length + payload size
} PseudoHeaderForCalcUdpChecksum;

UdpHeader *ConstructUdpHeader(__u8 *buff, ushort destinationPort, ushort sourcePort,
                              ushort payloadSize, const PseudoHeader &pseudoHeader);

IpHeader *ConstructIpv4Header(__u8 *buff, ushort id, u_char timeToLevel, u_char protocol,
                           uint destinationIpAddr, uint sourceIpAddr, size_t payloadSize);

EthernetHeader *ConstructEthernetHeader(__u8 *buff, std::string_view destinationMacAddr,
                             std::string_view sourceMacAddr);

} // namespace package

#endif //UNIX_NET_PACKAGE_HEADERS_H
