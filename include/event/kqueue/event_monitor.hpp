#ifndef EVENT_KQUEUE_EVENT_CENTER_HPP
#define EVENT_KQUEUE_EVENT_CENTER_HPP

#include <mutex>  // NOLINT
#include <optional>

#include "../define.hpp"
#include "../event_monitor.hpp"

namespace sam {
namespace event {
namespace kqueue {
class KqueueEventMonitor : public IEventMonitor {
 public:
  KqueueEventMonitor();
  ~KqueueEventMonitor();

 public:
  std::optional<error_code_t> register_event(int fd) const override;

 private:
  mutable std::mutex m_mutex;
};
}  // namespace kqueue
}  // namespace event

}  // namespace sam

#endif  // !EVENT_KQUEUE_EVENT_CENTER_HPP
