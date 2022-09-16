#include <iostream>
#include <string>
#include <array>
#include <cstring>
#include <cassert>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <unistd.h>

#include "interface_attr.h"
#include "parser_net_package.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "No passing input args: <interface name>" << std::endl;
        return EXIT_FAILURE;
    }

    /**
    * Определим новый обработчик сигнала SIGINT - функцию mode_off
    */
    // TODO

    /**
     * Создаем пакетный тип сокетов для работы с пакетами на уровне l2(ethernet).
     */
    int sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock < 0) {
        std::cerr << "Can`t create raw socket" << std::endl;
        return EXIT_FAILURE;
    }

    const auto interfaceName = std::string(argv[1]);
    /**
     * после нужно определить параметры сетевого интерфейса:
     * IP адрес, маска подсети, размер MTU, индекс и Mac адрес.
     */

    auto interfaceAttr = interface_attr::InterfaceAttr();
     try {
         interfaceAttr = interface_attr::GetInterfaceAttr(interfaceName);
     } catch (const std::runtime_error &e) {
         close(sock);
         throw;
     }
     std::cout << interfaceAttr << std::endl;

    /**
     * При работе с пакетными сокетами для хранения адресной информации
     * сетевого интерфейса вместо структуры sockaddr_in используется структура
     * sockaddr_ll (<linux/if_packet.h>)
     */
    struct sockaddr_ll sockAddress;
    memset(&sockAddress, 0, sizeof(struct sockaddr_ll));
    sockAddress.sll_family = PF_PACKET;
    sockAddress.sll_protocol = htons(ETH_P_ALL);
    sockAddress.sll_ifindex = interfaceAttr.getIndex();

    /**
     * Привязываем сокет к сетевому интерфейсу. В принципе, делать это не
     * обязательно, если на хосте активен только один сетевой интерфейс.
     * При наличии двух и более сетевых плат пакеты будут приниматься сразу со всех
     * активных интерфейсов, и если нас интересуют пакеты только из одного сегмента
     * сети, целесообразно выполнить привязку сокета к нужному интерфейсу
     */
    if(bind(sock, (struct sockaddr *)&sockAddress, sizeof(struct sockaddr_ll)) < 0) {
        std::cerr << "Can`t bind raw socket address" << std::endl;
        return EXIT_FAILURE;
    }

    typedef std::array<__u8, ETH_FRAME_LEN> Buffer;
    Buffer buffer;
    while (true) {
        memset(buffer.data(), 0, buffer.size());

        const auto receiveBuffSize = recvfrom(sock, buffer.data(), buffer.size(), 0, NULL, NULL);
        if (receiveBuffSize < 0) {
            std::cerr << "sys call recvfrom is failed" << std::endl;
        }

        parser::ParserNetPackage(buffer.data(), receiveBuffSize);
    }

    return 0;
}
