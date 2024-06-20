#ifndef DATA_STRUCTURE_SCSP_MUTEX_QUEUE_HPP
#define DATA_STRUCTURE_SCSP_MUTEX_QUEUE_HPP

#include <glog/logging.h>

#include <cassert>
#include <cstddef>
#include <memory>
#include <mutex>

namespace sam {
namespace data_structure {
template <typename T, typename Alloc = std::allocator<T>>
class ScspMutexQueue : private Alloc {
  std::size_t capacity_;
  T* ring_;
  size_t push_cursor_{0}, pop_cursor_{0};
  mutable std::mutex mutex_;

 public:
 public:
  explicit ScspMutexQueue(size_t capacity, Alloc const& alloc = Alloc{})
      : Alloc{alloc}, capacity_{capacity}, ring_{std::allocator_traits<Alloc>::allocate(*this, capacity)} {}

  ~ScspMutexQueue() { free_up_(); }

  size_t capacity() const { return capacity_; }
  size_t size() const { return push_cursor_ - pop_cursor_; }
  bool empty() const { return size() == 0; }
  bool full() const { return size() == capacity(); }

  bool push(T const& value);
  bool pop_front(T* value);

 public:
  void operator=(ScspMutexQueue<T, Alloc>&& other);

 private:
  void free_up_() {
    while (!empty()) {
      ring_[pop_cursor_ % capacity_].~T();
      ++pop_cursor_;
    }
    std::allocator_traits<Alloc>::deallocate(*this, ring_, capacity_);
  }
};

template <typename T, typename Alloc>
bool ScspMutexQueue<T, Alloc>::push(T const& value) {
  std::lock_guard<std::mutex> guard(mutex_);
  if (full()) {
    return false;
  }

  assert(push_cursor_ >= pop_cursor_);
  auto index = push_cursor_ % capacity_;
  new (&ring_[index]) T(value);
  ++push_cursor_;

  return true;
}

template <typename T, typename Alloc>
bool ScspMutexQueue<T, Alloc>::pop_front(T* value) {
  std::lock_guard<std::mutex> guard(mutex_);
  if (empty()) {
    return false;
  }

  assert(push_cursor_ >= pop_cursor_);
  *value = ring_[pop_cursor_ % capacity_];
  assert(*value < 0);
  ring_[pop_cursor_ % capacity_].~T();
  ++pop_cursor_;

  return true;
}

template <typename T, typename Alloc>
void ScspMutexQueue<T, Alloc>::operator=(ScspMutexQueue<T, Alloc>&& other) {
  if (this != &other) {
    free_up_();

    // Transfer ownership of resources from other to *this
    capacity_ = other.capacity_;
    ring_ = other.ring_;
    push_cursor_ = other.push_cursor_;
    pop_cursor_ = other.pop_cursor_;

    // Reset other to a valid state
    other.capacity_ = 0;
    other.ring_ = nullptr;
    other.push_cursor_ = 0;
    other.pop_cursor_ = 0;
  }
}
}  // namespace data_structure
}  // namespace sam

#endif  // !DATA_STRUCTURE_SCSP_MUTEX_QUEUE_HPP
