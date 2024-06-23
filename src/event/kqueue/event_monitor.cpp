#include "../../../include/event/kqueue/event_monitor.hpp"

#include <cerrno>
#include <optional>

#include "sys/event.h"

namespace sam {
namespace event {
namespace kqueue {
KqueueEventMonitor::KqueueEventMonitor() {
  int kqueue_fd = ::kqueue();
  if (kqueue_fd < 0) {
    LOG(FATAL) << "Failed to create new kqueue";
  }

  set_fd(kqueue_fd);
}

bool KqueueEventMonitor::register_event(int fd, int filter) const {
  if (fd < 0) {
    return EINVAL;
  }

  // return std::nullopt;
  return false;
}
}  // namespace kqueue
}  // namespace event
}  // namespace sam
