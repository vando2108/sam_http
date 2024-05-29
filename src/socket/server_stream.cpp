#include <glog/logging.h>
#include <sys/_endian.h>
#include <sys/socket.h>

#include <mutex>

#include "../../include/socket/helper.hpp"
#include "../../include/socket/stream.hpp"

namespace sam {
namespace socket {

#define BACKLOG 256

ServerStream::ServerStream(std::string_view address, size_t port)
    : m_addr(helper::convert_inaddr(address, port)) {
  int fd = ::socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    LOG(FATAL) << "Failed to create new socket";
  }
  set_fd(fd);

  int opt = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    LOG(FATAL) << "Failed to set socket options";
  }

  if (bind(fd, reinterpret_cast<sockaddr*>(&m_addr), sizeof(m_addr)) < 0) {
    LOG(FATAL) << "Failed to bind socket";
  }
}

ServerStream::~ServerStream() {}

void ServerStream::start() {
  if (listen(fd(), BACKLOG) < 0) {
    LOG(FATAL) << "Failed to start listen on socket";
  }

  LOG(INFO) << "Server is listening on port: " << ntohs(m_addr.sin_port);
}

void ServerStream::accept() { const std::lock_guard<std::mutex> lock(m_mu); }
}  // namespace socket
}  // namespace sam
