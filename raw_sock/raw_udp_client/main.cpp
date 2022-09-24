#include <iostream>
#include <string>
#include <cstring>
#include <array>
#include <vector>

#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>

#include "package_headers.h"
#include "parser_net_package.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "No passing input args: <server ip address> <port number>"
                << std::endl;
        return EXIT_FAILURE;
    }

    const auto serverIpAddress(argv[1]);
    const auto port(std::atoi(argv[2]));
    const auto sourceIpAddrStr = "127.0.0.1";
    const auto sourcePort = 2556U;
    std::ostream &os = std::cout;

    /**
     * @brief Создаем raw сокет для отправки datagrams.
     */
    const auto sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock < 0) {
        std::cerr << "Can`t create socket" << std::endl;
        return EXIT_FAILURE;
    }

    struct sockaddr_in serverSockAddr;
    memset(&serverSockAddr, 0, sizeof(serverSockAddr));
    serverSockAddr.sin_family = AF_INET;
    serverSockAddr.sin_port = htons(3455U);
    serverSockAddr.sin_addr.s_addr = inet_addr(serverIpAddress);
    if (bind(sock, (struct sockaddr*)&serverSockAddr, sizeof(serverSockAddr)) < 0) {
        std::cerr << "Can`t bind client socket with server socket address" << std::endl;
        return EXIT_FAILURE;
    }

    /**
     * @brief По каким-то неведомым мне причинам через сырой udp сокет отправляется на
     * сервер создаваемая datagram`а, но принимать ответы от того же сервера через этот raw сокет
     * не получается. Зато получилось принимать ответы сервера на второй сокет, работающий на
     * транспортном уровне с SOC_DGRAM. Исправлю этот момент, если узнаю в чем проблема.
     */
    const auto receiverSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (receiverSock < 0) {
        std::cerr << "Can`t create receiver socket" << std::endl;
        return EXIT_FAILURE;
    }

    struct sockaddr_in receiverSockAddr;
    memset(&receiverSockAddr, 0, sizeof(receiverSockAddr));
    receiverSockAddr.sin_family = AF_INET;
    receiverSockAddr.sin_port = htons(sourcePort);
    receiverSockAddr.sin_addr.s_addr = inet_addr(sourceIpAddrStr);
    if (bind(receiverSock, (struct sockaddr*)&receiverSockAddr, sizeof(receiverSockAddr)) < 0) {
        std::cerr << "Can`t bind receiver socket address" << std::endl;
        return EXIT_FAILURE;
    }

    typedef std::array<__u8, ETH_FRAME_LEN> Buffer;
    Buffer buffer = {0};
    auto const datagram = buffer.data();
    auto const data = datagram + sizeof(struct iphdr) + sizeof(struct udphdr);
    const auto payload = std::string("Hello world from raw udp client!");
    auto id = 0U;

    /**
     * @brief В цикле будет отправлять datagram на сервер через raw соккет, а
     * через второй сокет будем получать ответ от сервера.
     * @details Снова по каким-то причинам не получается отправить на сервер больше одного пакета.
     * Возможно неправльное вычисление checksum`ы или еще что-то.
     * Как то все сложно с raw сокетами!!!
     */
    for (auto i = 0; i < 3; ++i) {
        memcpy(data , payload.data(), payload.size());

        {
            /// Construct Ipv4 header
            const auto destIpAddr = serverSockAddr.sin_addr.s_addr;
            const auto sourceIpAddr = inet_addr(sourceIpAddrStr);
            const auto ttl = 255;
            const auto protocol = IPPROTO_UDP;
            (void)package::ConstructIpv4Header(buffer.data(), id, ttl, protocol,
                                               destIpAddr, sourceIpAddr, payload.size());
        }

        {
            /// Construct UDP header
            package::PseudoHeader pseudoHeader;
            pseudoHeader.sourceAddress = inet_addr(sourceIpAddrStr);
            pseudoHeader.destinationAddress = serverSockAddr.sin_addr.s_addr;
            pseudoHeader.placeholder = 0;
            pseudoHeader.protocol = IPPROTO_UDP;
            pseudoHeader.udpLength = htons(sizeof(struct udphdr) + payload.size());

            (void)package::ConstructUdpHeader(buffer.data() + sizeof(struct ip), port,
                                              sourcePort, payload.size(), pseudoHeader);
        }
        /// Increase id sequence for next package
        id += sizeof(IpHeader) + sizeof(UdpHeader) + payload.size();

        if (sendto(sock, buffer.data(), buffer.size(), 0,
                   (struct sockaddr*)&serverSockAddr, sizeof(serverSockAddr)) < 0) {
            std::cerr << "syscall sendto is failed" << std::endl;
            return EXIT_FAILURE;
        }
        memset(buffer.data(), 0, buffer.size());
        os << "Send udp package via raw udp socket" << std::endl;


        auto receiveBuffSize = 0;
        struct sockaddr_in responseAddress;
        memset(&responseAddress, 0, sizeof(responseAddress));
        socklen_t responseAddressLen = sizeof(responseAddress);
        if ((receiveBuffSize = recvfrom(receiverSock, buffer.data(), buffer.size(), 0,
                                        (struct sockaddr*)&responseAddress, &responseAddressLen)) < 0) {
            std::cerr << "syscall recv from is failed" << std::endl;
            return EXIT_FAILURE;
        }
        os << "Receive buffer with size: " << receiveBuffSize << "\tfrom host address("
           << inet_ntoa(responseAddress.sin_addr) << ", "
           << htons(responseAddress.sin_port) << ")" << std::endl;
        os << "receive buffer data: " << buffer.data() << std::endl;
    }

    return 0;
}