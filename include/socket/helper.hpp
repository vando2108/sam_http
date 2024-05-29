#ifndef SOCKET_HELPER_HPP
#define SOCKET_HELPER_HPP

#include <netinet/in.h>

#include <string_view>

namespace sam {
namespace socket {
namespace helper {
struct sockaddr_in convert_inaddr(std::string_view, size_t);
}
}  // namespace socket
}  // namespace sam

#endif
