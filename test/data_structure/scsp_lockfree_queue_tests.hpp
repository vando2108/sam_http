#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <thread>

#include "../../include/data_structure/scsp_lockfree_queue.hpp"
#include "../../include/utils/time.hpp"

TEST(ScspLockFreeQueueTest, BasicOperations) {
  sam::data_structure::ScspLockFreeQueue<int> queue(5);

  int value;
  EXPECT_FALSE(queue.pop(value));

  EXPECT_TRUE(queue.push(1));
  EXPECT_TRUE(queue.push(2));
  EXPECT_TRUE(queue.push(3));
  EXPECT_TRUE(queue.push(4));
  EXPECT_TRUE(queue.push(5));
  EXPECT_FALSE(queue.push(6));  // Queue should be full now

  EXPECT_TRUE(queue.pop(value));
  EXPECT_EQ(value, 1);
  EXPECT_TRUE(queue.pop(value));
  EXPECT_EQ(value, 2);
  EXPECT_TRUE(queue.pop(value));
  EXPECT_EQ(value, 3);
  EXPECT_TRUE(queue.pop(value));
  EXPECT_EQ(value, 4);
  EXPECT_TRUE(queue.pop(value));
  EXPECT_EQ(value, 5);
  EXPECT_FALSE(queue.pop(value));  // Queue should be empty now
}

TEST(ScspLockFreeQueueTest, MultiThread) {
  const int num_elements = 10;
  const int sleep_time = 100;
  sam::data_structure::ScspLockFreeQueue<int> queue(num_elements);

  auto producer = [&]() {
    for (int i = 0; i < num_elements; ++i) {
      while (!queue.push(i)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(sam::utils::now() % sleep_time));
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(sam::utils::now() % sleep_time));
    }
  };

  std::vector<int> consumed;
  auto consumer = [&]() {
    int value = -1;
    for (int i = 0; i < num_elements; ++i) {
      while (!queue.pop(value)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(sam::utils::now() % sleep_time));
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(sam::utils::now() % sleep_time));
      consumed.emplace_back(value);
    }
  };

  std::vector<std::thread> threads;
  threads.emplace_back(producer);
  threads.emplace_back(consumer);

  for (auto& thread : threads) {
    thread.join();
  }

  EXPECT_EQ(consumed.size(), num_elements);
  std::sort(consumed.begin(), consumed.end());
  for (int i = 0; i < num_elements; ++i) {
    EXPECT_EQ(i, consumed[i]);
  }
}
