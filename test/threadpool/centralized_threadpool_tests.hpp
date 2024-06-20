#include <gtest/gtest.h>

#include <future>
#include <memory>

#include "threadpool/base.hpp"
#include "threadpool/centralized_threadpool.hpp"

using sam::threadpool::centralized::Threadpool;

class ThreadpoolTest : public ::testing::Test {
 protected:
  std::unique_ptr<Threadpool> threadpool;

  void SetUp() override {
    sam::threadpool::Config config;
    config.task_queue_cap = 100;  // Set an appropriate queue capacity
    config.minimum_thread = 4;    // Set the number of worker threads
    threadpool = std::make_unique<Threadpool>(std::move(config));
  }

  void TearDown() override {}
};

TEST_F(ThreadpoolTest, SingleTaskExecution) {
  auto task = []() { return 42; };
  auto future = threadpool->submit_task(task);

  ASSERT_EQ(future.get(), 42);
}

TEST_F(ThreadpoolTest, MultipleTaskExecution) {
  auto task1 = []() { return 1; };
  auto task2 = []() { return 2; };
  auto task3 = []() { return 3; };

  auto future1 = threadpool->submit_task(task1);
  auto future2 = threadpool->submit_task(task2);
  auto future3 = threadpool->submit_task(task3);

  ASSERT_EQ(future1.get(), 1);
  ASSERT_EQ(future2.get(), 2);
  ASSERT_EQ(future3.get(), 3);
}

TEST_F(ThreadpoolTest, ConcurrentTaskExecution) {
  auto task = [](int value) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return value;
  };

  std::vector<std::future<int>> futures;
  for (int i = 0; i < 10; ++i) {
    futures.push_back(threadpool->submit_task(task, i));
  }

  for (int i = 0; i < 10; ++i) {
    ASSERT_EQ(futures[i].get(), i);
  }
}

TEST_F(ThreadpoolTest, ThreadpoolDestruction) {
  {
    sam::threadpool::Config config;
    config.task_queue_cap = 100;
    config.minimum_thread = 4;
    Threadpool local_threadpool(std::move(config));

    auto task = []() { return 42; };
    auto future = local_threadpool.submit_task(task);

    ASSERT_EQ(future.get(), 42);
  }

  // At this point, the local_threadpool should be destroyed without issues
  SUCCEED();
}
