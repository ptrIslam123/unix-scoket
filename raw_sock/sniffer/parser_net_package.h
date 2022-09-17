#ifndef RAW_SOCKET_PROJECTS_PARSER_NET_PACKAGE_H
#define RAW_SOCKET_PROJECTS_PARSER_NET_PACKAGE_H

#include <linux/types.h>

namespace parser {

bool ParserNetPackage(const __u8 *buff, unsigned int buffSize);

} // namespace parser

#endif //RAW_SOCKET_PROJECTS_PARSER_NET_PACKAGE_H
