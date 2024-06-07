#ifndef EVENT_HPP
#define EVENT_HPP

#include <glog/logging.h>
#include <unistd.h>

namespace sam {
namespace event {
class IEvent {
  int fd_;

 public:
  IEvent() : fd_(0) {}
  ~IEvent() {}

 public:
  int fd() const noexcept { return fd_; }

  bool set_fd(const int& fd) noexcept {
    if (fd < 0) {
      return false;
    }

    fd_ = fd;
    return true;
  }
};

class IObserver {
  int fd_;
  int filter_;

 public:
  IObserver() : fd_(0), filter_(0) {}
  ~IObserver() {}

 public:
  int fd() const noexcept { return fd_; }

  bool set_fd(const int& fd) noexcept {
    if (fd < 0) {
      return false;
    }

    fd_ = fd;
    return true;
  }
};
}  // namespace event
}  // namespace sam

#endif  // !EVENT_HPP
