#include "utils/rand.hpp"

#include <cstdint>
#include <vector>

namespace sam {
namespace utils {
namespace rand {
std::uint32_t Xorshift::x_ = 21082001;
std::uint32_t Xorshift::y_ = 6061997;
std::uint32_t Xorshift::z_ = 29122022;
std::uint32_t Xorshift::w_ = 12312312;

std::vector<int> rand_list_int(size_t size, size_t element_size) {
  std::vector<int> ret(size);
  std::for_each(ret.begin(), ret.end(), [&](int &x) { x = Xorshift::next() % element_size; });

  return ret;
}
}  // namespace rand
}  // namespace utils
}  // namespace sam
