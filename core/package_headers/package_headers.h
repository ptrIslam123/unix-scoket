#ifndef UNIX_NET_PACKAGE_HEADERS_H
#define UNIX_NET_PACKAGE_HEADERS_H

#include <linux/types.h>
#include <string_view>

#include "types/package_header_types.h"

namespace package {

UdpHeader *ConstructUdpHeader(__u8 *buff, ushort destinationPort, ushort sourcePort,
                              ushort payloadSize);

IpHeader *ConstructIpv4Header(__u8 *buff, ushort id, u_char timeToLevel, u_char protocol,
                           uint destinationIpAddr, uint sourceIpAddr);

EthernetHeader *ConstructEthernetHeader(__u8 *buff, std::string_view destinationMacAddr,
                             std::string_view sourceMacAddr);

size_t ConstructDgram(__u8 *buff, size_t buffSize, const char *data, size_t dataSize,
                      ushort idSequence);

} // namespace package

#endif //UNIX_NET_PACKAGE_HEADERS_H
