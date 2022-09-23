#include <iostream>
#include <string>
#include <cstring>
#include <array>

#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>

#include "package_headers.h"

struct pseudo_header
{
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t udp_length;
};

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


int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "No passing input args: <server ip address> <port number>"
                << std::endl;
        return EXIT_FAILURE;
    }

    const auto serverIpAddress(argv[1]);
    const auto port(std::atoi(argv[2]));
    std::ostream &os = std::cout;

    const auto sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock < 0) {
        std::cerr << "Can`t create socket" << std::endl;
        return EXIT_FAILURE;
    }

    struct pseudo_header psh;
    struct sockaddr_in serverSockAddr;
    memset(&serverSockAddr, 0, sizeof(serverSockAddr));
    serverSockAddr.sin_family = AF_INET;
    serverSockAddr.sin_port = htons(port);
    serverSockAddr.sin_addr.s_addr = inet_addr(serverIpAddress);
    if (bind(sock, (struct sockaddr*)&serverSockAddr, sizeof(serverSockAddr)) < 0) {
        std::cerr << "Can`t bind client socket with server socket address" << std::endl;
        return EXIT_FAILURE;
    }

    typedef std::array<__u8, ETH_FRAME_LEN> Buffer;
    const auto sourceIpAddrStr = "127.0.0.1";
    Buffer buffer = {0};
    auto const datagram = buffer.data();
    //IP header
    struct iphdr *iph = (struct iphdr*)datagram;

    //UDP header
    struct udphdr *udph = (struct udphdr*)(datagram + sizeof (struct ip));

    //Data part
    auto const data = datagram + sizeof(struct iphdr) + sizeof(struct udphdr);
    const auto payload = std::string("Hello world from raw udp client!");
    memcpy(data , payload.data(), payload.size());

    //Fill in the IP Header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr) + payload.size();
    iph->id = htonl(54321);	//Id of this packet
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_UDP;
    iph->check = 0;		//Set to 0 before calculating checksum
    iph->saddr = inet_addr(sourceIpAddrStr);	//Spoof the source ip address
    iph->daddr = serverSockAddr.sin_addr.s_addr;

    //Ip checksum
    iph->check = csum ((unsigned short *)datagram, iph->tot_len);

    //UDP header
    udph->source = htons(port);
    udph->dest = htons(port);
    udph->len = htons(8 + payload.size());	//tcp header size
    udph->check = 0;	//leave checksum 0 now, filled later by pseudo header

    //Now the UDP checksum using the pseudo header
    psh.source_address = inet_addr(sourceIpAddrStr);
    psh.dest_address = serverSockAddr.sin_addr.s_addr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_UDP;
    psh.udp_length = htons(sizeof(struct udphdr) + payload.size());

    int psize = sizeof(struct pseudo_header) + sizeof(struct udphdr) + payload.size();
    auto pseudogram = (char*)malloc(psize);

    memcpy(pseudogram, (char*)&psh, sizeof (struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header), udph , sizeof(struct udphdr) +
            payload.size());

    udph->check = csum((unsigned short*)pseudogram, psize);

    if (sendto(sock, buffer.data(), buffer.size(), 0,
               (struct sockaddr*)&serverSockAddr, sizeof(serverSockAddr)) < 0) {
        std::cerr << "syscall send to is failed" << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}