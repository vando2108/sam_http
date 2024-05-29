#ifndef EVENT_EVENT_MONITOR_HPP
#define EVENT_EVENT_MONITOR_HPP

#include <optional>

#include "../define.hpp"
#include "./event.hpp"

namespace sam {
namespace event {
class IEventMonitor : public IEvent {
 public:
  ~IEventMonitor() {}

 public:
  virtual std::optional<error_code_t> register_event(int) const = 0;
};
}  // namespace event
}  // namespace sam

#endif  // !EVENT_EVENT_MONITOR_HPP
