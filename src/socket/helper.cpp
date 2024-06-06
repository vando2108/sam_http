#include "../../include/socket/helper.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/_endian.h>
#include <sys/socket.h>

#include <cstring>

namespace sam {
namespace socket {
namespace helper {
struct sockaddr_in convert_inaddr(std::string_view address, size_t port) {
  struct sockaddr_in addr;
  std::memset(&addr, 0, sizeof(addr));

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;

  // if (inet_pton(AF_INET, address.data(), &addr.sin_addr) <= 0) {
  //   return std::nullopt;
  // }

  return addr;
}
}  // namespace helper
}  // namespace socket
}  // namespace sam
