#ifndef RAW_SOCKET_PROJECTS_PARSER_NET_PACKAGE_H
#define RAW_SOCKET_PROJECTS_PARSER_NET_PACKAGE_H

#include <linux/types.h>

namespace parser {

bool ParserNetPackage(__u8 *buff, size_t buffSize);

} // namespace parser

#endif //RAW_SOCKET_PROJECTS_PARSER_NET_PACKAGE_H
