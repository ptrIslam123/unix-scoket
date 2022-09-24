#include "package_headers.h"

#include <vector>
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

unsigned short csum(unsigned short *ptr,int nbytes)
{
    register long sum;
    unsigned short oddbyte;
    register short answer;

    sum=0;
    while(nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }

    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;

    return(answer);
}

} // namespace

namespace package {

UdpHeader *ConstructUdpHeader(__u8 *const buff, const ushort destinationPort, const ushort sourcePort,
                              const ushort payloadSize, const PseudoHeader &pseudoHeader) {
    UdpHeader *const udpHeader = (UdpHeader*)(buff);

    /// Copy destination port number
    udpHeader->dest = htons(destinationPort);
    /// Copy source port number
    udpHeader->source = htons(sourcePort);
    /// Copy UDP header length (8 bytes) = [dest/source ports, checksum, udp length]
    constexpr auto udpHeaderLen = 8;
    udpHeader->len = htons(udpHeaderLen + payloadSize);

    int psize = sizeof(PseudoHeader) + sizeof(UdpHeader) + payloadSize;
    std::vector<char> pseudogram(psize);

    memcpy(pseudogram.data(), (char*)&pseudoHeader, sizeof(PseudoHeader));
    memcpy(pseudogram.data() + sizeof(PseudoHeader), udpHeader, sizeof(struct udphdr) +
                                                                        payloadSize);
    udpHeader->check = csum((unsigned short*)pseudogram.data(), psize);
    return udpHeader;
}


IpHeader *ConstructIpv4Header(__u8 *const buff, const ushort id, const u_char timeToLevel,
                           const u_char protocol, const uint destinationIpAddr,
                           const uint sourceIpAddr, const size_t payloadSize) {
    IpHeader *const ipHeader = (IpHeader*)(buff);
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
    ipHeader->tot_len = sizeof(IpHeader) + sizeof(UdpHeader) + payloadSize;
    ipHeader->id = htons(id);
    ipHeader->frag_off = 0;
    ipHeader->ttl = timeToLevel;
    ipHeader->protocol = protocol;
    ipHeader->check = csum((unsigned short *)buff, ipHeader->tot_len);
    ipHeader->daddr = destinationIpAddr;
    ipHeader->saddr = sourceIpAddr;
    return ipHeader;
}

EthernetHeader *ConstructEthernetHeader(__u8 *const buff,
                                        const std::string_view destinationMacAddr,
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