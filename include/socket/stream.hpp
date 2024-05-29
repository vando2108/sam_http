#ifndef SOCKET_STREAM_HPP
#define SOCKET_STREAM_HPP

#include <netinet/in.h>

#include <mutex>  // NOLINT
#include <string_view>

#include "../event/event.hpp"

namespace sam {
namespace socket {
class IStream : public event::IEvent {
 public:
  virtual ~IStream() {}
};

class ServerStream : IStream {
 public:
  ServerStream(std::string_view, size_t);
  ~ServerStream();

 public:
  void start();
  void accept();

 private:
  sockaddr_in m_addr;
  mutable std::mutex m_mu;
};

class ClientStream : IStream {};
}  // namespace socket
}  // namespace sam

#endif  // !SOCKET_STREAM_HPP
