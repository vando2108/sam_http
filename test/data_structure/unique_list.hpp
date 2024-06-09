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

  ASSERT_TRUE(unique_list.empty());

  auto mock_data = sam::utils::rand::rand_list_int(1000, 1000);
  for (auto it : mock_data) {
    unique_list.push_back(it);
  }

  std::unordered_set<int> s(mock_data.begin(), mock_data.end());

  ASSERT_EQ(unique_list.size(), s.size());

  for (auto it : s) {
    ASSERT_TRUE(unique_list.contain(it));
  }

  while (unique_list.size()) {
    int value = 0;
    ASSERT_TRUE(unique_list.pop_back(value));
    ASSERT_TRUE(s.find(value) != s.end());
    s.erase(value);
  }
}

TEST(UniqueListTest, PushDuplicateOperation) {
  auto unique_list = UniqueList<int>();
  unique_list.push_back(0);
  unique_list.push_back(0);

  ASSERT_EQ(unique_list.size(), 1);
  ASSERT_TRUE(unique_list.contain(0));
}

TEST(UniqueListTest, PopRandom) {
  auto mock_data = sam::utils::rand::rand_list_int(10000, 10000);
  UniqueList<int> unique_list(mock_data);

  int rand_pos = sam::utils::rand::Xorshift::next() % mock_data.size();

  ASSERT_TRUE(unique_list.contain(mock_data[rand_pos]));
  ASSERT_TRUE(unique_list.pop(mock_data[rand_pos]));
  ASSERT_FALSE(unique_list.contain(mock_data[rand_pos]));
}

TEST(UniqueListTest, TryGetNode) {
  auto mock_data = sam::utils::rand::rand_list_int(10000, 10000);
  UniqueList<int> unique_list(mock_data);

  int rand_pos = sam::utils::rand::Xorshift::next() % mock_data.size();
  auto it = unique_list.try_get_node(mock_data[rand_pos]);
  ASSERT_EQ(*it.value(), mock_data[rand_pos]);
}

TEST(UniqueListTest, ReplaceOperation) {
  auto mock_data = sam::utils::rand::rand_list_int(10000, 10000);
  UniqueList<int> unique_list(mock_data);

  int rand_pos = sam::utils::rand::Xorshift::next() % mock_data.size();
  ASSERT_TRUE(unique_list.replace(rand_pos, const int &))
}
