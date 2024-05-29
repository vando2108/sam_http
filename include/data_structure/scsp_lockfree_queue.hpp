#ifndef DATA_STRUCTURE_SCSP_LOCKFREE_QUEUE_HPP
#define DATA_STRUCTURE_SCSP_LOCKFREE_QUEUE_HPP

#include <cstddef>
#include <memory>

namespace sam {
namespace data_structure {
template <typename T, typename Alloc = std::allocator<T>>
class ScspLockFreeQueue : private Alloc {
  std::size_t capacity_;
  T* ring_;
  std::size_t push_cursor_{};
  std::size_t pop_cursor_{};

 public:
  explicit ScspLockFreeQueue(size_t capacity, Alloc const& alloc = Alloc{})
      : Alloc{alloc},
        capacity_{capacity},
        ring_{std::allocator_traits<T>::allocate(*this, capacity)} {}

  ~ScspLockFreeQueue() {
    while (!empty()) {
      ring_[pop_cursor_ % capacity_].~T();
      ++pop_cursor_;
    }
    std::allocator_traits<T>::deallocate(*this, ring_, capacity_);
  }

  auto capacity() const { return capacity_; }
  auto size() const { return push_cursor_ - pop_cursor_; }
  auto empty() const { return size() == 0; }
  auto full() const { return size() == capacity(); }

  auto push(T const& value);
  auto pop(T* value);
};
}  // namespace data_structure
}  // namespace sam

#endif  // !DATA_STRUCTURE_SCSP_LOCKFREE_QUEUE_HPP
