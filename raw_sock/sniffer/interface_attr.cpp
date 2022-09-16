#include "interface_attr.h"

#include <stdexcept>
#include <array>

#include <sys/ioctl.h>
#include <linux/if.h>
#include <unistd.h>
#include <cstring> // for strerror
#include <cassert>
#include <cerrno> // for errno

namespace interface_attr {

InterfaceAttr GetInterfaceAttr(const std::string &name) {
    typedef unsigned int AttrValue;
    typedef std::string ErrorMsg;
    static std::array<std::pair<AttrValue, ErrorMsg>, 4> attrs = {
            std::make_pair(SIOCGIFINDEX, std::string("Can`t get index of interface: " + name)),
            std::make_pair(SIOCGIFHWADDR, std::string("Can`t get MAC address of interface: " + name)),
            std::make_pair(SIOCGIFADDR, std::string("Can`t get Ip address of interface: " + name)),
            std::make_pair(SIOCGIFMTU, std::string("Can`t get MTU of interface: " + name))
    };

    struct ifreq ifReg;
    memset(&ifReg, 0, sizeof(struct ifreq));
    assert(name.size() < IFNAMSIZ);
    strncpy(ifReg.ifr_ifrn.ifrn_name, name.data(), IFNAMSIZ - 1);


    const int sock = socket(AF_INET,SOCK_DGRAM,0);
    if (sock < 0) {
        throw std::runtime_error("Can`t get socket");
    }

    std::for_each_n(attrs.cbegin(), attrs.size(), [&sock, &ifReg](const auto &attrItem){
        const auto attrValue = attrItem.first;
        const auto errorMsg = attrItem.second;
        if ((ioctl(sock, attrValue, ifReg)) < 0) {
            close(sock);
            throw std::runtime_error(strerror(errno) + std::string(":\t") + errorMsg);
        }
    });

    return InterfaceAttr(sock, ifReg);
}

std::ostream &operator<<(std::ostream &os, const InterfaceAttr &attr) {
    return attr.operator<<(os);
}

InterfaceAttr::InterfaceAttr():
sock_(-1),
ifReq_() {
}

InterfaceAttr::InterfaceAttr(const int sock, const struct ifreq &ifReg):
sock_(sock),
ifReq_(ifReg) {
}

InterfaceAttr::~InterfaceAttr() {
    if (sock_ > 0) {
        close(sock_);
    }
}

const InterfaceAttr::Address &InterfaceAttr::getMacAddress() const {
    return ifReq_.ifr_ifru.ifru_hwaddr;
}

const InterfaceAttr::Address &InterfaceAttr::getIpAddress() const {
    return ifReq_.ifr_ifru.ifru_addr;
}

InterfaceAttr::Index InterfaceAttr::getIndex() const {
    return ifReq_.ifr_ifru.ifru_ivalue;
}

bool InterfaceAttr::enablePromiscuousMode() {
    if(ioctl(sock_, SIOCGIFFLAGS, &ifReq_) < 0) {
        return false;
    }
    ifReq_.ifr_flags |= IFF_PROMISC;
    if (ioctl(sock_, SIOCSIFFLAGS, &ifReq_) < 0) {
        return false;
    }
    return true;
}

bool InterfaceAttr::disablePromiscuousMode() {
    if(ioctl(sock_, SIOCGIFFLAGS, &ifReq_) < 0) {
        return false;
    }
    ifReq_.ifr_flags &= ~(IFF_PROMISC);
    if (ioctl(sock_, SIOCSIFFLAGS, &ifReq_) < 0) {
        return false;
    }
    return true;
}

std::ostream &InterfaceAttr::operator<<(std::ostream &os) const {
    return os << "index: " << getIndex()
        << "MAC address: " << getMacAddress().sa_data
        << "Ip address: " << getIpAddress().sa_data;
}

} // namespace interface_attr