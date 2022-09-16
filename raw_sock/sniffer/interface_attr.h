#ifndef RAW_SOCKET_PROJECTS_GET_INTERFACE_ATTR_H
#define RAW_SOCKET_PROJECTS_GET_INTERFACE_ATTR_H

#include <iostream>
#include <string>

#include <sys/socket.h>
#include <linux/if.h>

namespace interface_attr {

class InterfaceAttr {
public:
    typedef struct sockaddr Address;
    typedef int Index;

    InterfaceAttr();
    InterfaceAttr(int sock, const struct ifreq &ifReg);
    ~InterfaceAttr();
    InterfaceAttr(InterfaceAttr &&other) = default;
    InterfaceAttr &operator=(InterfaceAttr &&other) = default;
    InterfaceAttr(const InterfaceAttr &other) = delete;
    InterfaceAttr &operator=(const InterfaceAttr &other) = delete;

    const Address &getMacAddress() const;
    const Address &getIpAddress() const;
    Index getIndex() const;

    bool enablePromiscuousMode();
    bool disablePromiscuousMode();

    std::ostream &operator<<(std::ostream &os) const;
private:
    int sock_;
    struct ifreq ifReq_;
};

InterfaceAttr GetInterfaceAttr(const std::string &name);
std::ostream &operator<<(std::ostream &os, const InterfaceAttr &attr);

} // namespace interface_attr

#endif //RAW_SOCKET_PROJECTS_GET_INTERFACE_ATTR_H
