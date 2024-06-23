#ifndef SOCKET_STREAM_HPP
#define SOCKET_STREAM_HPP

#include <netinet/in.h>

#include <mutex>
#include <string_view>

#include "../event/event.hpp"

namespace sam {
namespace socket {
class IStream : public event::IEvent {
 public:
  virtual ~IStream() {}
};

class ClientStream : public IStream {
 public:
  ClientStream(int);
  ~ClientStream() {}

 public:
  long recv(std::vector<char>&, int) const;
};

class ServerStream : public IStream {
 public:
  ServerStream(std::string_view, size_t);
  ~ServerStream();

 public:
  void start() const;
  std::unique_ptr<ClientStream> accept() const;

 private:
  sockaddr_in m_addr;
  mutable std::mutex m_mu;
};
}  // namespace socket
}  // namespace sam

#endif  // !SOCKET_STREAM_HPP
