#include <glog/logging.h>
#include <sys/_endian.h>
#include <sys/_types/_socklen_t.h>
#include <sys/socket.h>

#include <cerrno>
#include <memory>

#include "../../include/socket/helper.hpp"
#include "../../include/socket/stream.hpp"

namespace sam {
namespace socket {

const int PORT = 3000;
const int BACKLOG = 10;
const int BUFFER_SIZE = 1024;

ServerStream::ServerStream(std::string_view address, size_t port) : m_addr(helper::convert_inaddr(address, port)) {
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

void ServerStream::start() const {
  if (listen(fd(), BACKLOG) < 0) {
    LOG(FATAL) << "Failed to start listen on socket";
  }
  LOG(INFO) << "Server is listen at port: " << PORT;

  while (true) {
    auto client_stream = accept();
    LOG(INFO) << "test";

    if (client_stream != nullptr) {
      std::vector<char> buffer(BUFFER_SIZE);
      auto bytes_read = client_stream->recv(buffer, MSG_DONTWAIT);
      if (bytes_read < 0) {
        LOG(ERROR) << "Failed to read from socket: " << errno;
        continue;
      }

      LOG(INFO) << "Received request: \n" << buffer.data();
    }
  }
}

std::unique_ptr<ClientStream> ServerStream::accept() const {
  sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  int client_fd = ::accept(fd(), reinterpret_cast<sockaddr*>(&client_addr), &client_addr_len);

  if (client_fd < 0) {
    LOG(WARNING) << "Failed to accept new connection";
    return nullptr;
  }

  return std::unique_ptr<ClientStream>(new ClientStream(client_fd));
}
}  // namespace socket
}  // namespace sam
