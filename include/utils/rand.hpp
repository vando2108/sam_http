#ifndef UTILS_RAND_HPP
#define UTILS_RAND_HPP

#include <vector>

namespace sam {
namespace utils {
namespace rand {
class Xorshift {
 public:
  Xorshift() {}

  static std::uint32_t next() {
    auto temp = x_ ^ (x_ << 11);

    x_ = y_, y_ = z_, z_ = w_;

    return w_ = w_ ^ (w_ >> 19) ^ temp ^ (temp >> 8);
  }

 private:
  static std::uint32_t x_, y_, z_, w_;
};

std::vector<int> rand_list_int(size_t size, size_t element_size);
}  // namespace rand
}  // namespace utils
}  // namespace sam

#endif  // !UTILS_RAND_HPP
