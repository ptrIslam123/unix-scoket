#include "interface_info.h"

#include <stdexcept>
#include <map>
#include <string_view>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <unistd.h>
#include <cstring> // for strerror
#include <cassert>
#include <cerrno> // for errno

namespace {

typedef unsigned int AttrFlag;

struct Arg {
    int sock;
    AttrFlag flag;
    const std::string_view interfaceName;
};

typedef void (*GetterInterfaceAttr)(const Arg &arg);

struct ifreq GetIfReq(const std::string_view name) {
    struct ifreq ifReg;
    memset(&ifReg, 0, sizeof(struct ifreq));
    assert(name.size() < IFNAMSIZ);
    strncpy(ifReg.ifr_ifrn.ifrn_name, name.data(), IFNAMSIZ);
    return ifReg;
}

struct ifreq GetIfReg(const Arg &arg, const std::string_view errorMsg) {
    struct ifreq ifReg = GetIfReq(arg.interfaceName);
    if (ioctl(arg.sock, arg.flag, &ifReg) < 0) {
        throw std::runtime_error(std::string(errorMsg) + std::string(arg.interfaceName));
    }
    return ifReg;
}

} // namespace

namespace interface_attr {

InterfaceInfo GetInterfaceAttr(const std::string &name) {
    const int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        throw std::runtime_error("Can`t create socket");
    }

    static InterfaceInfo::Data data;
    data.name_ = name;
    data.index_ = 0;
    data.sock_ = sock;
    memset(&data.macAddress_, 0, sizeof(InterfaceInfo::Address));
    memset(&data.ipAddress_, 0, sizeof(InterfaceInfo::Address));

    const auto getterIndex = [](const Arg &arg) {
        data.index_ = GetIfReg(arg, "Can`t get index for interface with name: ")
                .ifr_ifru.ifru_ivalue;
    };
    const auto getterMacAddress = [](const Arg &arg){
        data.macAddress_ = GetIfReg(arg, "Can`t get MAC address for interface with name: ")
                .ifr_ifru.ifru_hwaddr;
    };
    const auto getterIpAddress = [](const Arg &arg){
        data.ipAddress_ = GetIfReg(arg, "Can`t get IP address for interface with name: ")
                .ifr_ifru.ifru_addr;
    };
    const auto getterMTU = [](const Arg &arg) {
        data.mtu_ = GetIfReg(arg, "Can`t get MTU value for interface with name: ")
                .ifr_ifru.ifru_mtu;
    };

    static const std::map<AttrFlag, GetterInterfaceAttr> attrs = {
        std::make_pair(SIOCGIFINDEX, getterIndex),
        std::make_pair(SIOCGIFHWADDR, getterMacAddress),
        std::make_pair(SIOCGIFADDR, getterIpAddress),
        std::make_pair(SIOCGIFMTU, getterMTU)
    };

    for (auto it = attrs.cbegin(); it != attrs.cend(); ++it) {
        const auto flagValue = it->first;
        const auto getter = it->second;
        const Arg arg = {.sock = sock, .flag = flagValue, .interfaceName = name};
        getter(arg);
    }
    return InterfaceInfo(std::move(data));
}

std::ostream &operator<<(std::ostream &os, const InterfaceInfo &attr) {
    return attr.operator<<(os);
}

InterfaceInfo::InterfaceInfo(InterfaceInfo::Data &&data):
data_(std::move(data)) {
}

InterfaceInfo::InterfaceInfo():
data_() {
}

InterfaceInfo::~InterfaceInfo() {
    if (data_.sock_ > 0) {
        close(data_.sock_);
    }
}

const std::string_view InterfaceInfo::getName() const {
    return data_.name_;
}

const InterfaceInfo::Address &InterfaceInfo::getMacAddress() const {
    return data_.macAddress_;
}

const InterfaceInfo::Address &InterfaceInfo::getIpAddress() const {
    return data_.ipAddress_;
}

InterfaceInfo::Index InterfaceInfo::getIndex() const {
    return data_.index_;
}

int InterfaceInfo::getMtu() const {
    return data_.mtu_;
}

bool InterfaceInfo::enablePromiscuousMode() {
    struct ifreq ifReq = GetIfReq(data_.name_);
    if(ioctl(data_.sock_, SIOCGIFFLAGS, &ifReq) < 0) {
        return false;
    }
    ifReq.ifr_flags |= IFF_PROMISC;
    if (ioctl(data_.sock_, SIOCSIFFLAGS, &ifReq) < 0) {
        return false;
    }
    return true;
}

bool InterfaceInfo::disablePromiscuousMode() {
    struct ifreq ifReq = GetIfReq(data_.name_);
    if(ioctl(data_.sock_, SIOCGIFFLAGS, &ifReq) < 0) {
        return false;
    }
    ifReq.ifr_flags &= ~(IFF_PROMISC);
    if (ioctl(data_.sock_, SIOCSIFFLAGS, &ifReq) < 0) {
        return false;
    }
    return true;
}

std::ostream &InterfaceInfo::operator<<(std::ostream &os) const {
    os << "name: " << getName() << "\t";
    os << "index: [" << getIndex() << "]\t";
    os << "MAC address: [";
    {
        const struct sockaddr_in *const inAddress = (struct sockaddr_in*)(&getMacAddress());
        const auto address = inet_ntoa(inAddress->sin_addr);
        os << address;
    }
    os << "]\t";
    os << "IP address: [";
    {
        const struct sockaddr_in *const inAddress = (struct sockaddr_in*)(&getIpAddress());
        const auto address = inet_ntoa(inAddress->sin_addr);
        os << address;
    }
    os << "]\t";
    os << "MTU: " << getMtu();
    return os;
}

InterfaceInfo::Data::Data(InterfaceInfo::Data &&other) noexcept:
name_(),
macAddress_(),
ipAddress_(),
index_(),
mtu_(),
sock_(){
    this->operator=(std::move(other));
}

InterfaceInfo::Data &InterfaceInfo::Data::operator=(InterfaceInfo::Data &&other) noexcept {
    name_ = std::move(other.name_);
    macAddress_ = other.macAddress_;
    ipAddress_ = other.ipAddress_;
    index_ = other.index_;
    mtu_ = other.mtu_;
    sock_ = other.index_;

    memset(&other.macAddress_, 0, sizeof(Address));
    memset(&other.ipAddress_, 0, sizeof(Address));
    other.index_ = 0;
    other.mtu_ = 0;
    other.sock_ = -1;
    return *this;
}

} // namespace interface_attr