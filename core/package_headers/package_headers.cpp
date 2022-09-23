#include "package_headers.h"

#include <cstring>
#include <cassert>

#include "parser/parser_net_package.h"

namespace {

ushort Checksum(ushort *buff, int _16BitWord)
{
    ulong sum;
    for(sum = 0; _16BitWord > 0; --_16BitWord)
        sum += htons(*(buff)++);
    sum = ((sum >> 16) + (sum & 0xFFFF));
    sum += (sum >> 16);
    return (ushort)(~sum);
}

} // namespace

namespace package {

size_t ConstructDgram(__u8 *const  buff, const size_t buffSize, const char *const data,
                    const size_t dataSize, const ushort idSequence) {
    auto totalSize = 0U;
    /// Construct ethernet header
    EthernetHeader *ethernetHeader = nullptr;
    {
        const auto destinationMacAddr = "";
        const auto sourceMacAddr = "";
        ethernetHeader = ConstructEthernetHeader(buff, destinationMacAddr, sourceMacAddr);
        totalSize += sizeof(EthernetHeader);
    }
    /// Construct ipv4 header
    IpHeader *ipHeader = nullptr;
    {
        const auto ttl = 225;
        const auto protocol = IPPROTO_UDP;
        const auto destinationIpAddr = 0U;
        const auto sourceIpAddr = 0U;
        totalSize += sizeof(IpHeader);
        assert(totalSize <= buffSize);
        ipHeader = ConstructIpv4Header(buff, idSequence, ttl, protocol,
                                       destinationIpAddr, sourceIpAddr);
    }
    /// Construct udp header
    UdpHeader *udpHeader = nullptr;
    {
        const auto destinationPort = 0U;
        const auto sourcePort = 0U;
        totalSize += sizeof(UdpHeader);
        assert(totalSize <= buffSize);
        udpHeader = ConstructUdpHeader(buff, destinationPort, sourcePort, dataSize);
    }
    /// Copy payload to the buffer
    assert(totalSize + dataSize <= buffSize);
    memcpy(buff + totalSize, data, dataSize);
    totalSize += dataSize;
    /// Filling the remaining fields of the IP and UDP headers:
    /// Set UDP length
    udpHeader->len = htons(totalSize - sizeof(IpHeader) - sizeof(EthernetHeader));
    /// Set IP length
    ipHeader->tot_len = htons(totalSize - sizeof(EthernetHeader));
    /// Set checksum
    ipHeader->check = Checksum((ushort*)(buff + sizeof(EthernetHeader)), sizeof(IpHeader) / 2);
    return totalSize;
}

UdpHeader *ConstructUdpHeader(__u8 *const buff, const ushort destinationPort,
                              const ushort sourcePort, const ushort payloadSize) {
    const auto ipHeaderLength = sizeof(IpHeader);
    UdpHeader *const udpHeader = (UdpHeader*)(buff + ETH_HLEN + ipHeaderLength);
    /// Copy destination port number
    udpHeader->dest = htons(destinationPort);
    /// Copy source port number
    udpHeader->source = htons(sourcePort);
    /// Copy UDP header length (8 bytes) = [dest/source ports, checksum, udp length]
    constexpr auto udpHeaderLen = 8;
    udpHeader->len = udpHeaderLen + payloadSize;
    return udpHeader;
}


IpHeader *ConstructIpv4Header(__u8 *const buff, const ushort id, const u_char timeToLevel,
                           const u_char protocol, const uint destinationIpAddr,
                           const uint sourceIpAddr) {
    IpHeader *const ipHeader = (IpHeader*)(buff + ETH_HLEN);
    {
        /// real length in bytes is word(4) * (wordCount)5 = 20 bytes
        const auto wordCount = 5;
        ipHeader->ihl = wordCount;
    }
    {
        const auto ipHeaderVersion = 4;
        ipHeader->version = ipHeaderVersion;
    }
    {
        const auto typeOfService = 16;
        ipHeader->tos = typeOfService;
    }
    ipHeader->tot_len = 0;
    ipHeader->id = htons(id);
    ipHeader->frag_off = 0;
    ipHeader->ttl = timeToLevel;
    ipHeader->protocol = protocol;
    ipHeader->check = 0;
    ipHeader->daddr = destinationIpAddr;
    ipHeader->saddr = sourceIpAddr;
    return ipHeader;
}

EthernetHeader *ConstructEthernetHeader(__u8 *const buff, const std::string_view destinationMacAddr,
                             const std::string_view sourceMacAddr) {
    const auto macAddressLen = ETH_ALEN;
    EthernetHeader *const ethernetHeader = (EthernetHeader*)(buff);
    /// Copy destination MAC address
    memcpy(ethernetHeader->h_dest, destinationMacAddr.data(), macAddressLen);
    /// Copy source MAC address
    memcpy(ethernetHeader->h_source, sourceMacAddr.data(), macAddressLen);
    /// Set protocol for next layer (it will be IP protocol)
    ethernetHeader->h_proto = htons(ETH_P_IP);
    return ethernetHeader;
}

} // namespace package