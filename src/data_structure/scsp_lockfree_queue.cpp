#include "../../include/data_structure/scsp_lockfree_queue.hpp"

namespace sam {
namespace data_structure {
template <typename T, typename Alloc>
auto ScspLockFreeQueue<T, Alloc>::push(T const& value) {
  if (full()) {
    return false;
  }

  auto index = push_cursor_ % capacity_;
  new (&ring_[index]) T(value);
  ++push_cursor_;
  return true;
}

template <typename T, typename Alloc>
auto ScspLockFreeQueue<T, Alloc>::pop(T* value) {
  if (empty()) {
    return false;
  }

  value = ring_[pop_cursor_ % capacity_];
  ring_[pop_cursor_ % capacity_].~T();
  ++pop_cursor_;

  return true;
}
}  // namespace data_structure
}  // namespace sam
