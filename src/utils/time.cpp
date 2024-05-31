#include "../../include/utils/time.hpp"

#include <chrono>  // NOLINT
#include <cstdint>

namespace sam {
namespace utils {
uint64_t now() {
  const auto p1 = std::chrono::system_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(p1.time_since_epoch()).count();
}
}  // namespace utils
}  // namespace sam
