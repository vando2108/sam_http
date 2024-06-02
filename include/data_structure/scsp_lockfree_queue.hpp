#ifndef DATA_STRUCTURE_SCSP_LOCKFREE_QUEUE_HPP
#define DATA_STRUCTURE_SCSP_LOCKFREE_QUEUE_HPP

#include <glog/logging.h>

#include <atomic>
#include <cassert>
#include <cstddef>
#include <memory>

#ifdef __cpp_lib_hardware_interference_size
using std::hardware_constructive_interference_size;
using std::hardware_destructive_interference_size;
#else
// 64 bytes on x86-64 │ L1_CACHE_BYTES │ L1_CACHE_SHIFT │ __cacheline_aligned │ ...
constexpr std::size_t hardware_constructive_interference_size = 64;
constexpr std::size_t hardware_destructive_interference_size = 64;
#endif

namespace sam {
namespace data_structure {
template <typename T, typename Alloc = std::allocator<T>>
class ScspLockFreeQueue : private Alloc {
  std::size_t capacity_;
  T* ring_;
  static_assert(std::atomic<std::size_t>::is_always_lock_free);
  alignas(hardware_constructive_interference_size) std::atomic<std::size_t> push_cursor_{0};
  alignas(hardware_constructive_interference_size) std::atomic<std::size_t> pop_cursor_{0};
  char padding_[hardware_constructive_interference_size - sizeof(std::size_t)];

 public:
  explicit ScspLockFreeQueue(size_t capacity, Alloc const& alloc = Alloc{})
      : Alloc{alloc},
        capacity_{capacity},
        ring_{std::allocator_traits<Alloc>::allocate(*this, capacity)} {}

  ~ScspLockFreeQueue() {
    while (!empty(push_cursor_, pop_cursor_)) {
      ring_[pop_cursor_.load(std::memory_order_relaxed) % capacity_].~T();
      pop_cursor_.fetch_add(1, std::memory_order_relaxed);
    }
    std::allocator_traits<Alloc>::deallocate(*this, ring_, capacity_);
  }

  size_t capacity() const { return capacity_; }

  bool full(std::size_t push_cursor, std::size_t pop_cursor) const {
    return (push_cursor - pop_cursor) == capacity();
  }

  bool empty(std::size_t push_cursor, std::size_t pop_cursor) const {
    return push_cursor == pop_cursor;
  }

  auto element(std::size_t cursor) { return &ring_[cursor % capacity_]; }

  bool push(T const& value);
  bool pop(T& value);  // NOLINT
};

template <typename T, typename Alloc>
bool ScspLockFreeQueue<T, Alloc>::push(T const& value) {
  auto push_cursor = push_cursor_.load(std::memory_order_relaxed);
  auto pop_cursor = pop_cursor_.load(std::memory_order_acquire);
  if (full(push_cursor, pop_cursor)) {
    return false;
  }

  new (element(push_cursor)) T(value);
  push_cursor_.store(push_cursor + 1, std::memory_order_release);

  return true;
}

template <typename T, typename Alloc>
bool ScspLockFreeQueue<T, Alloc>::pop(T& value) {  // NOLINT
  auto push_cursor = push_cursor_.load(std::memory_order_acquire);
  auto pop_cursor = pop_cursor_.load(std::memory_order_relaxed);
  if (empty(push_cursor, pop_cursor)) {
    return false;
  }

  value = *element(pop_cursor);
  element(pop_cursor)->~T();
  pop_cursor_.store(pop_cursor + 1, std::memory_order_release);

  return true;
}
}  // namespace data_structure
}  // namespace sam

#endif  // !DATA_STRUCTURE_SCSP_LOCKFREE_QUEUE_HPP
