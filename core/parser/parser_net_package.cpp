#include "parser_net_package.h"

#include <cstring>
#include <sstream>
#include <iomanip>
#include <arpa/inet.h>

namespace parser {

__u8 *ExtractData(const __u8 *buff, uint size) {
    return NULL;
}


EthernetHeader ExtractEthernetHeader(const __u8 *const buff) {
    const EthernetHeader *const ethernetHeader = (EthernetHeader*)(buff);
    struct ethhdr result;
    /// Copy destination address (for Ethernet (L2 lever) it is the MAC address)
    memcpy(result.h_dest, ethernetHeader->h_dest, ETH_ALEN);
    /// Copy source address (for Ethernet (L2 lever) it is the MAC address)
    memcpy(result.h_source, ethernetHeader->h_source, ETH_ALEN);
    /// Copy protocol type (Ethernet package type)
    result.h_proto = ethernetHeader->h_proto;
    return result;
}

IpHeader ExtractIpHeader(const __u8 *const buff) {
    /// Лучше инспоьзовать константу ETH_HLEN для определения размера заголовка Ethernet
    /// Так как sizeof может дать не верный результат из-за особеностей выравнивания структур
    constexpr int ethernetHeaderLen = ETH_HLEN;
    const IpHeader *const ipHeader = (IpHeader*)(buff + ethernetHeaderLen);
    IpHeader result;
    /// Copy IP protocol version (Ipv4/Ipv6)
    result.version = ipHeader->version;
    /// Copy Internet(IP) header length
    result.ihl = ipHeader->ihl;
    /// Copy type of service
    result.tos = ipHeader->tos;
    /// Copy total length
    result.tot_len = ipHeader->tot_len;
    /// Copy identification number of package
    result.id = ipHeader->id;
    /// Copy Shift fragment and flags
    result.frag_off = ipHeader->frag_off;
    /// Copy TTL(Time to live)
    result.ttl = ipHeader->ttl;
    /// Copy type of protocol
    result.protocol = ipHeader->protocol;
    /// Checksum of package
    result.check = ipHeader->check;
    /// Copy source address (for Ip level it is IP address)
    result.saddr = ipHeader->saddr;
    /// Copy destination address (for Ip level it is IP address)
    result.daddr = ipHeader->daddr;
    return result;
}

UdpHeader ExtractUdpHeader(const __u8 *const buff) {
    /// Лучше инспоьзовать константу ETH_HLEN для определения размера заголовка Ethernet
    /// Так как sizeof может дать не верный результат из-за особеностей выравнивания структур
    constexpr auto ethernetHeaderLen = ETH_HLEN;
    constexpr auto wordSize = 4;
    const auto ipHeaderLen = (unsigned int)(ExtractIpHeader(buff).ihl) * wordSize;
    const UdpHeader *const udpHeader = (UdpHeader*)(buff + ethernetHeaderLen + ipHeaderLen);
    UdpHeader result;
    /// Copy source address (for UDP level it is port number)
    result.source = udpHeader->source;
    /// Copy destination address (for UDP lever it is port number)
    result.dest = udpHeader->dest;
    /// Copy UDP package length
    result.len = udpHeader->len;
    /// Copy UDP checksum
    result.check = udpHeader->check;
    return result;
}

TcpHeader ExtractTcpHeader(const __u8 *buff) {
    /// Лучше инспоьзовать константу ETH_HLEN для определения размера заголовка Ethernet
    /// Так как sizeof может дать не верный результат из-за особеностей выравнивания структур
    constexpr auto ethernetHeaderLen = ETH_HLEN;
    constexpr auto wordSize = 4;
    const auto ipHeaderLen = (unsigned int)(ExtractIpHeader(buff).ihl) * wordSize;
    const TcpHeader *const tcpHeader = (TcpHeader*)(buff + ethernetHeaderLen + ipHeaderLen);
    TcpHeader result;
    memcpy(&result, tcpHeader, sizeof(TcpHeader));
    return result;
}


void WriteEthernetHeaderTo(std::ostream &ostream, const EthernetHeader &ethernetHeader) {
    ostream << "************************ Ethernet HEADER ************************" << "\n";

    ostream << "protocol: " << ethernetHeader.h_proto << "\n";

    ostream << "Destination MAC address: [";
    for (auto i = 0; i < ETH_ALEN; ++i) {
        ostream << std::hex << std::setfill('0') << std::setw(2)
                << static_cast<int>(ethernetHeader.h_dest[i]);
        if (i + 1 < ETH_ALEN) {
            ostream << "-";
        }
    }
    ostream << "]" << "\n";

    ostream << "Source MAC address: [";
    for (auto i = 0; i < ETH_ALEN; ++i) {
        ostream << std::hex << std::setfill('0') << std::setw(2)
                << static_cast<int>(ethernetHeader.h_source[i]);
        if (i + 1 < ETH_ALEN) {
            ostream << "-";
        }
    }
    ostream << "]" << std::dec << std::endl;
}

void WriteIpHeaderTo(std::ostream &ostream, const IpHeader &ipHeader) {
    ostream << "************************ IP HEADER ************************\n";
    ostream << "Ip version: " << (unsigned int)ipHeader.version << "\n";
    ostream << "Internet(IP) header bytes length: " << (unsigned int)(ipHeader.ihl) * 4 << "\n";
    ostream << "Type of service: " << (unsigned int)ipHeader.tos << "\n";
    ostream << "Total length: " << ntohs(ipHeader.tot_len) << "\n";
    ostream << "Identification number: " << ntohs(ipHeader.id) << "\n";
    ostream << "TTL(Time to live): " << (unsigned int)ipHeader.ttl << "\n";
    ostream << "Type of protocol: " << (unsigned int)ipHeader.protocol << "\n";
    ostream << "Checksum: " << ntohs(ipHeader.check) << "\n";
    {
        struct sockaddr_in *inAddress = (struct sockaddr_in*)(&ipHeader.saddr);
        ostream << "Source IP address: " << inet_ntoa(inAddress->sin_addr) << "\n";
    }
    {
        struct sockaddr_in *inAddress = (struct sockaddr_in*)(&ipHeader.daddr);
        ostream << "Destination IP address: " << inet_ntoa(inAddress->sin_addr) << "\n";
    }
}

void WriteUdpHeaderTo(std::ostream &ostream, const UdpHeader &udpHeader) {
    ostream << "************************ UDP HEADER ************************" << "\n";
    ostream << "Destination port: " << udpHeader.dest << "\n";
    ostream << "Source port: " << udpHeader.source << "\n";
    ostream << "Length: " << ntohs(udpHeader.len) << "\n";
    ostream << "Checksum: " << ntohs(udpHeader.check) << "\n";
}

void WriteTcpHeaderTo(std::ostream &ostream, const TcpHeader &tcpHeader) {
    ostream << "************************ TCP HEADER ************************" << "\n";
    ostream << "Destination port: " << ntohs(tcpHeader.dest) << "\n";
    ostream << "Source port: " << ntohs(tcpHeader.source) << "\n";
    ostream << "Window size: " << ntohs(tcpHeader.window) << "\n";
    ostream << "Sequence number(id): " << ntohs(tcpHeader.seq) << "\n";
    ostream << "Data offset(data order): " << ntohs(tcpHeader.th_off) << "\n";
}


} // namespace parser