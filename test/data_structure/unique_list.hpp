#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <unordered_set>

#include "data_structure/unique_list.hpp"
#include "gtest/gtest.h"
#include "utils/rand.hpp"
#include "utils/time.hpp"

using sam::data_structure::UniqueList;
using sam::utils::now;

TEST(UniqueListTest, PushPopOperations) {
  const auto number_elements = now() % 1000;
  UniqueList<int> unique_list;

  ASSERT_TRUE(unique_list.empty()) << "Expected unique_list to be empty initially.";

  auto mock_data = sam::utils::rand::rand_list_int(1000, 1000);
  for (auto it : mock_data) {
    unique_list.push_back(it);
  }

  std::unordered_set<int> s(mock_data.begin(), mock_data.end());

  ASSERT_EQ(unique_list.size(), s.size()) << "Expected unique_list size to match the size of the set created from mock_data.";

  for (auto it : s) {
    ASSERT_TRUE(unique_list.contain(it)) << "Expected unique_list to contain " << it;
  }

  while (unique_list.size()) {
    int value = 0;
    ASSERT_TRUE(unique_list.pop_back(value)) << "Expected to pop back an element from unique_list.";
    ASSERT_TRUE(s.find(value) != s.end()) << "Expected the popped value to be found in the set.";
    s.erase(value);
  }
}

TEST(UniqueListTest, PushDuplicateOperation) {
  auto unique_list = UniqueList<int>();
  unique_list.push_back(0);
  unique_list.push_back(0);

  ASSERT_EQ(unique_list.size(), 1) << "Expected unique_list size to be 1 after attempting to push duplicate elements.";
  ASSERT_TRUE(unique_list.contain(0)) << "Expected unique_list to contain the element 0.";
}

TEST(UniqueListTest, PopRandom) {
  auto mock_data = sam::utils::rand::rand_list_int(10000, 10000);
  UniqueList<int> unique_list(mock_data);

  int rand_pos = sam::utils::rand::Xorshift::next() % mock_data.size();

  ASSERT_TRUE(unique_list.contain(mock_data[rand_pos])) << "Expected unique_list to contain the element at random position " << rand_pos;
  ASSERT_TRUE(unique_list.pop(mock_data[rand_pos])) << "Expected to pop the element at random position " << rand_pos << " from unique_list.";
  ASSERT_FALSE(unique_list.contain(mock_data[rand_pos])) << "Expected unique_list to no longer contain the element at random position " << rand_pos;
}

TEST(UniqueListTest, TryGetNode) {
  auto mock_data = sam::utils::rand::rand_list_int(10000, 10000);
  UniqueList<int> unique_list(mock_data);

  int rand_pos = sam::utils::rand::Xorshift::next() % mock_data.size();
  auto it = unique_list.try_get_node(mock_data[rand_pos]);
  ASSERT_EQ(*it.value(), mock_data[rand_pos]) << "Expected the node retrieved to match the element at random position " << rand_pos;
}

TEST(UniqueListTest, ReplaceOperation) {
  auto mock_data = sam::utils::rand::rand_list_int(10000, 10000);
  UniqueList<int> unique_list(mock_data);

  // Successful replacement
  int value = mock_data[0];
  int replace_value = 10001;  // Assume this value is not in mock_data
  auto result = unique_list.replace(value, replace_value);
  ASSERT_EQ(result, UniqueList<int>::SUCCESS) << "Expected SUCCESS when replacing an existing element with a new unique element.";
  ASSERT_FALSE(unique_list.contain(value)) << "Expected unique_list to no longer contain the old value after replacement.";
  ASSERT_TRUE(unique_list.contain(replace_value)) << "Expected unique_list to contain the new value after replacement.";

  // Replacement with duplicate hash
  value = mock_data[1];
  replace_value = mock_data[2];  // Assume this value is already in mock_data
  result = unique_list.replace(value, replace_value);
  ASSERT_EQ(result, UniqueList<int>::DUPLICATE_HASH)
      << "Expected DUPLICATE_HASH when replacing with an element that has the same hash as an existing element.";
  ASSERT_TRUE(unique_list.contain(value)) << "Expected unique_list to still contain the original value when replacement fails due to duplicate hash.";
  ASSERT_TRUE(unique_list.contain(replace_value))
      << "Expected unique_list to still contain the conflicting value when replacement fails due to duplicate hash.";

  // Replacement when key not found
  value = 20000;          // Assume this value is not in mock_data
  replace_value = 30000;  // Assume this value is also not in mock_data
  result = unique_list.replace(value, replace_value);
  ASSERT_EQ(result, UniqueList<int>::KEY_NOT_FOUND) << "Expected KEY_NOT_FOUND when trying to replace a non-existent element.";
  ASSERT_FALSE(unique_list.contain(value)) << "Expected unique_list to not contain the non-existent original value.";
  ASSERT_FALSE(unique_list.contain(replace_value)) << "Expected unique_list to not contain the non-existent replacement value.";
}
