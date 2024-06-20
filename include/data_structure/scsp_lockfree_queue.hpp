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

constexpr std::size_t kPaddingSize = hardware_constructive_interference_size - sizeof(std::size_t);

namespace sam {
namespace data_structure {
template <typename T, typename Alloc = std::allocator<T>>
class ScspLockFreeQueue : private Alloc {
  std::size_t capacity_;
  T* ring_;
  static_assert(std::atomic<std::size_t>::is_always_lock_free);

  // push_cursor
  alignas(hardware_constructive_interference_size) std::atomic<std::size_t> push_cursor_{0};
  alignas(hardware_constructive_interference_size) std::size_t cached_push_cursor_{0};

  // pop cursor
  alignas(hardware_constructive_interference_size) std::atomic<std::size_t> pop_cursor_{0};
  alignas(hardware_constructive_interference_size) std::size_t cached_pop_cursor_{0};

  char padding_[kPaddingSize];

 public:
  explicit ScspLockFreeQueue(size_t capacity, Alloc const& alloc = Alloc{})
      : Alloc{alloc}, capacity_{capacity}, ring_{std::allocator_traits<Alloc>::allocate(*this, capacity)} {}

  ~ScspLockFreeQueue() { free_up_(); }

  size_t capacity() const { return capacity_; }

  bool full(std::size_t push_cursor, std::size_t pop_cursor) const { return (push_cursor - pop_cursor) == capacity(); }
  bool empty() {
    if (empty_(cached_push_cursor_, cached_pop_cursor_)) {
      return true;
    }

    cached_push_cursor_ = push_cursor_.load(std::memory_order_acquire);
    cached_pop_cursor_ = pop_cursor_.load(std::memory_order_acquire);
    return empty_(cached_push_cursor_, cached_pop_cursor_);
  }

  auto element(std::size_t cursor) { return &ring_[cursor % capacity_]; }

  bool push(T const&);
  bool pop(T&);
  bool push(T&&);

 public:
  void operator=(ScspLockFreeQueue<T, Alloc>&& other);

 private:
  bool empty_(std::size_t push_cursor, std::size_t pop_cursor) const { return push_cursor == pop_cursor; }
  void free_up_() {
    while (!empty_(push_cursor_, pop_cursor_)) {
      ring_[pop_cursor_.load(std::memory_order_relaxed) % capacity_].~T();
      pop_cursor_.fetch_add(1, std::memory_order_relaxed);
    }
    std::allocator_traits<Alloc>::deallocate(*this, ring_, capacity_);
  }
};

template <typename T, typename Alloc>
bool ScspLockFreeQueue<T, Alloc>::push(T const& value) {
  auto push_cursor = push_cursor_.load(std::memory_order_relaxed);
  if (full(push_cursor, cached_pop_cursor_)) {
    cached_pop_cursor_ = pop_cursor_.load(std::memory_order_acquire);
    if (full(push_cursor, cached_pop_cursor_)) {
      return false;
    }
  }

  new (element(push_cursor)) T(value);
  push_cursor_.store(push_cursor + 1, std::memory_order_release);

  return true;
}

template <typename T, typename Alloc>
bool ScspLockFreeQueue<T, Alloc>::push(T&& value) {
  auto push_cursor = push_cursor_.load(std::memory_order_relaxed);
  if (full(push_cursor, cached_pop_cursor_)) {
    cached_pop_cursor_ = pop_cursor_.load(std::memory_order_acquire);
    if (full(push_cursor, cached_pop_cursor_)) {
      return false;
    }
  }

  new (element(push_cursor)) T(std::forward<T>(value));
  push_cursor_.store(push_cursor + 1, std::memory_order_release);

  return true;
}

template <typename T, typename Alloc>
bool ScspLockFreeQueue<T, Alloc>::pop(T& value) {
  auto pop_cursor = pop_cursor_.load(std::memory_order_relaxed);
  if (empty_(cached_push_cursor_, pop_cursor)) {
    cached_push_cursor_ = push_cursor_.load(std::memory_order_acquire);
    if (empty_(cached_push_cursor_, pop_cursor)) {
      return false;
    }
  }

  value = std::move(*element(pop_cursor));
  element(pop_cursor)->~T();
  pop_cursor_.store(pop_cursor + 1, std::memory_order_release);

  return true;
}

template <typename T, typename Alloc>
void ScspLockFreeQueue<T, Alloc>::operator=(ScspLockFreeQueue<T, Alloc>&& other) {
  if (this != &other) {
    free_up_();

    // Transfer ownership of resources from other to *this
    capacity_ = other.capacity_;
    ring_ = other.ring_;
    push_cursor_.store(other.push_cursor_.load(std::memory_order_relaxed), std::memory_order_relaxed);
    cached_push_cursor_ = other.cached_push_cursor_;
    pop_cursor_.store(other.pop_cursor_.load(std::memory_order_relaxed), std::memory_order_relaxed);
    cached_pop_cursor_ = other.cached_pop_cursor_;

    // Reset other to a valid state
    other.capacity_ = 0;
    other.ring_ = nullptr;
    other.push_cursor_.store(0, std::memory_order_relaxed);
    other.cached_push_cursor_ = 0;
    other.pop_cursor_.store(0, std::memory_order_relaxed);
    other.cached_pop_cursor_ = 0;
  }
}
}  // namespace data_structure
}  // namespace sam

#endif  // !DATA_STRUCTURE_SCSP_LOCKFREE_QUEUE_HPP
