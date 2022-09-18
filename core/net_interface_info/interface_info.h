#ifndef RAW_SOCKET_PROJECTS_GET_INTERFACE_ATTR_H
#define RAW_SOCKET_PROJECTS_GET_INTERFACE_ATTR_H

#include <iostream>
#include <string>
#include <string_view>

#include <sys/socket.h>
#include <linux/if.h>

namespace interface_attr {

class InterfaceInfo {
public:
    typedef struct sockaddr Address;
    typedef int Index;

    struct Data {
        Data() = default;
        Data(Data &&other) noexcept;
        Data &operator=(Data &&other) noexcept;
        Data(const Data &other) = delete;
        Data &operator=(const Data &other) = delete;

        std::string name_;
        Address macAddress_;
        Address ipAddress_;
        Index index_;
        int mtu_;
        int sock_;
    };

    InterfaceInfo();
    explicit InterfaceInfo(Data &&data);
    ~InterfaceInfo();
    InterfaceInfo(InterfaceInfo &&other) = default;
    InterfaceInfo &operator=(InterfaceInfo &&other) = default;
    InterfaceInfo(const InterfaceInfo &other) = delete;
    InterfaceInfo &operator=(const InterfaceInfo &other) = delete;

    const std::string_view getName() const;
    const Address &getMacAddress() const;
    const Address &getIpAddress() const;
    Index getIndex() const;
    int getMtu() const;

    bool enablePromiscuousMode();
    bool disablePromiscuousMode();

    std::ostream &operator<<(std::ostream &os) const;
private:
    Data data_;
};

InterfaceInfo GetInterfaceAttr(const std::string &name);
std::ostream &operator<<(std::ostream &os, const InterfaceInfo &attr);

} // namespace interface_attr

#endif //RAW_SOCKET_PROJECTS_GET_INTERFACE_ATTR_H
