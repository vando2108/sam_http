#ifndef UTILS_STRING_HPP
#define UTILS_STRING_HPP

#include <string>

namespace sam {
namespace utils {
template <typename T, typename... Arg>
std::string format(std::string& result, T replace_str, Arg... args) {
  format(args...);
}
}  // namespace utils
}  // namespace sam
#endif  // !UTILS_STRING_HPP
