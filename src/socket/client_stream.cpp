#include <sys/socket.h>

#include "../../include/socket/stream.hpp"

namespace sam {
namespace socket {
ClientStream::ClientStream(int fd) { set_fd(fd); }

long ClientStream::recv(std::vector<char>& buffer, int flags) const {
  if (fd() < 0) {
    LOG(FATAL) << "Client stream has invalid fd";
  }

  return ::recv(fd(), buffer.data(), buffer.size(), flags);
}
}  // namespace socket
}  // namespace sam
