#ifndef EVENT_HPP
#define EVENT_HPP

#include <glog/logging.h>
#include <unistd.h>

namespace sam {
namespace event {
class IEvent {
 public:
  IEvent() : m_fd(0) {}
  ~IEvent() {
    close(m_fd);
    LOG(INFO) << "Closed event fd: " << m_fd;
  }

 public:
  int fd() const noexcept { return m_fd; }
  void set_fd(int fd) noexcept { m_fd = fd; }

 private:
  int m_fd;
};
}  // namespace event
}  // namespace sam

#endif  // !EVENT_HPP
