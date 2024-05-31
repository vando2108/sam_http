#ifndef DATA_STRUCTURE_SCSP_LOCKFREE_QUEUE_HPP
#define DATA_STRUCTURE_SCSP_LOCKFREE_QUEUE_HPP

#include <glog/logging.h>

#include <atomic>
#include <cassert>
#include <cstddef>
#include <memory>

namespace sam {
namespace data_structure {
template <typename T, typename Alloc = std::allocator<T>>
class ScspLockFreeQueue : private Alloc {
  std::size_t capacity_;
  T* ring_;
  std::atomic<std::size_t> push_cursor_{};
  std::atomic<std::size_t> pop_cursor_{};

 public:
  explicit ScspLockFreeQueue(size_t capacity, Alloc const& alloc = Alloc{})
      : Alloc{alloc},
        capacity_{capacity},
        ring_{std::allocator_traits<Alloc>::allocate(*this, capacity)} {}

  ~ScspLockFreeQueue() {
    while (!empty()) {
      ring_[pop_cursor_.load(std::memory_order_relaxed) % capacity_].~T();
      pop_cursor_.fetch_add(1, std::memory_order_relaxed);
    }
    std::allocator_traits<Alloc>::deallocate(*this, ring_, capacity_);
  }

  size_t capacity() const { return capacity_; }
  size_t size() const {
    return push_cursor_.load(std::memory_order_relaxed) -
           pop_cursor_.load(std::memory_order_relaxed);
  }
  bool empty() const { return size() == 0; }
  bool full() const { return size() == capacity(); }

  bool push(T const& value);
  bool pop(T* value);
};

template <typename T, typename Alloc>
bool ScspLockFreeQueue<T, Alloc>::push(T const& value) {
  auto push_cursor = push_cursor_.load(std::memory_order_relaxed);
  auto pop_cursor = pop_cursor_.load(std::memory_order_acquire);
  assert(pop_cursor < push_cursor);

  if (push_cursor - pop_cursor == capacity_) {
    return false;
  }

  auto index = push_cursor % capacity_;
  new (&ring_[index]) T(value);
  push_cursor_.fetch_add(1, std::memory_order_release);

  return true;
}

template <typename T, typename Alloc>
bool ScspLockFreeQueue<T, Alloc>::pop(T* value) {
  auto pop_cursor = pop_cursor_.load(std::memory_order_relaxed);
  auto push_cursor = push_cursor_.load(std::memory_order_acquire);
  assert(pop_cursor <= push_cursor);

  if (push_cursor == pop_cursor) {
    return false;
  }

  *value = ring_[pop_cursor_ % capacity_];
  LOG_IF(INFO, (*value) < 0) << push_cursor << ' ' << pop_cursor;
  ring_[pop_cursor_ % capacity_].~T();
  pop_cursor_.fetch_add(1, std::memory_order_release);

  return true;
}
}  // namespace data_structure
}  // namespace sam

#endif  // !DATA_STRUCTURE_SCSP_LOCKFREE_QUEUE_HPP
