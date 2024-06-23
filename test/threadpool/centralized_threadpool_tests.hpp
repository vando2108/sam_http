#include <gtest/gtest.h>

#include <future>
#include <memory>

#include "threadpool/base.hpp"
#include "threadpool/centralized_threadpool.hpp"
#include "utils/rand.hpp"

using sam::threadpool::Config;
using sam::threadpool::centralized::CentralizedThreadpool;

TEST(CentralizedThreadpoolTest, Initialize) {
  auto pool = CentralizedThreadpool::create(std::move(Config(6, 6, "sam")));
  ASSERT_TRUE(pool->is_running());
  ASSERT_EQ(pool->config().worker_prefix, "sam");
  ASSERT_EQ(pool->config().minimum_thread, 6);
  ASSERT_EQ(pool->config().task_queue_cap, 6);
}

TEST(CentralizedThreadpoolTest, SingleTaskExecution) {
  auto pool = CentralizedThreadpool::create(std::move(Config(6, 6, "sam")));
  auto task = []() { return 42; };
  auto future = pool->submit_task(task);

  ASSERT_EQ(future.get(), 42);
}

TEST(CentralizedThreadpoolTest, MultipleTaskExecution) {
  auto pool = CentralizedThreadpool::create(std::move(Config(6, 6, "sam")));

  auto task1 = []() { return 1; };
  auto task2 = []() { return 2; };
  auto task3 = []() { return 3; };

  auto future1 = pool->submit_task(task1);
  auto future2 = pool->submit_task(task2);
  auto future3 = pool->submit_task(task3);

  ASSERT_EQ(future1.get(), 1);
  ASSERT_EQ(future2.get(), 2);
  ASSERT_EQ(future3.get(), 3);
}

TEST(CentralizedThreadpoolTest, ConcurrentTaskExecution) {
  auto pool = CentralizedThreadpool::create(std::move(Config()));
  auto data = sam::utils::rand::rand_list_int(100, 1000);
  std::vector<std::future<int>> futures;

  for (int i = 0; i < data.size(); ++i) {
    auto task = [](int value) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      return value;
    };

    futures.push_back(pool->submit_task(task, data[i]));
  }

  for (int i = 0; i < data.size(); ++i) {
    ASSERT_EQ(futures[i].get(), data[i]);
  }
}

TEST(CentralizedThreadpoolTest, Destruction) {
  {
    sam::threadpool::Config config;
    config.task_queue_cap = 100;
    config.minimum_thread = 4;
    auto pool = CentralizedThreadpool::create(std::move(Config()));

    auto task = []() { return 42; };
    auto future = pool->submit_task(task);

    ASSERT_EQ(future.get(), 42);
  }

  SUCCEED();
}
