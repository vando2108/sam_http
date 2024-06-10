#ifndef DATA_STRUCTURE_UNIQUE_LIST_HPP
#define DATA_STRUCTURE_UNIQUE_LIST_HPP

#include <glog/logging.h>

#include <cassert>
#include <functional>
#include <list>
#include <optional>
#include <unordered_map>

namespace sam {
namespace data_structure {

template <typename T, typename Hash = std::hash<T>>
class UniqueList {
 public:
  enum REPLACE_RESULT {
    SUCCESS = 0,
    DUPLICATE_HASH,
    KEY_NOT_FOUND,
  };

 private:
  std::list<T> list_;
  std::unordered_map<T, typename std::list<T>::iterator, Hash> map_;

 public:
  UniqueList() {}
  explicit UniqueList(const std::vector<T>&);
  ~UniqueList() {}

 public:
  bool push_back(const T&);
  bool pop_back(T&);
  bool pop(const T&);
  bool contain(const T&) const;
  REPLACE_RESULT replace(const T&, const T&);

  bool empty() const { return list_.empty(); }
  size_t size() const {
    assert(list_.size() == map_.size());
    return list_.size();
  }

  typename std::list<T>::iterator begin() { return list_.begin(); }
  typename std::list<T>::iterator end() { return list_.end(); }
  std::optional<typename std::list<T>::const_iterator> try_get_node(const T&) const;
};

template <typename T, typename Hash>
UniqueList<T, Hash>::UniqueList(const std::vector<T>& other) {
  for (const auto& it : other) {
    push_back(it);
  }
}

template <typename T, typename Hash>
bool UniqueList<T, Hash>::push_back(const T& value) {
  if (map_.find(value) != map_.end()) {
    return false;
  }

  list_.push_back(value);
  map_[value] = --list_.end();
  return true;
}

template <typename T, typename Hash>
bool UniqueList<T, Hash>::pop_back(T& out) {
  if (list_.empty()) {
    return false;
  }

  out = list_.back();
  list_.pop_back();
  map_.erase(out);

  return true;
}

template <typename T, typename Hash>
bool UniqueList<T, Hash>::pop(const T& key) {
  if (contain(key)) {
    list_.erase(map_[key]);
    map_.erase(key);
    return true;
  }

  return false;
}

template <typename T, typename Hash>
bool UniqueList<T, Hash>::contain(const T& key) const {
  return map_.find(key) != map_.end();
}

template <typename T, typename Hash>
UniqueList<T, Hash>::REPLACE_RESULT UniqueList<T, Hash>::replace(const T& value, const T& replace_value) {
  if (!contain(value)) {
    return KEY_NOT_FOUND;
  }

  Hash hash_function;
  auto value_hashed = hash_function(value), replace_value_hashed = hash_function(replace_value);
  if (value_hashed != replace_value_hashed && !contain(replace_value)) {
    auto node = map_.at(value);
    map_.erase(value);
    map_[replace_value] = node;
    *node = replace_value;
  } else {
    return DUPLICATE_HASH;
  }

  return SUCCESS;
}

template <typename T, typename Hash>
std::optional<typename std::list<T>::const_iterator> UniqueList<T, Hash>::try_get_node(const T& key) const {
  if (contain(key)) {
    return map_.at(key);
  }

  return std::nullopt;
}

}  // namespace data_structure
}  // namespace sam

#endif  // !DATA_STRUCTURE_UNIQUE_LIST_HPP
