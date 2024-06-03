#include <benchmark/benchmark.h>

#include <thread>  // NOLINT

#include "../../include/data_structure/scsp_lockfree_queue.hpp"
#include "../../include/data_structure/scsp_mutex_queue.hpp"

static void BM_ScspLockFreeQueue(benchmark::State& state) {  // NOLINT
  const int num_elements = state.range(0);

  for (auto _ : state) {
    sam::data_structure::ScspLockFreeQueue<int> queue(num_elements);

    size_t push_count = 0, pop_count = 0;
    auto producer = [&]() {
      for (int i = 0; i < num_elements; ++i) {
        if (queue.push(push_count)) {
          ++push_count;
        }
      }
    };

    auto consumer = [&]() {
      int value = -1;
      for (int i = 0; i < num_elements; ++i) {
        if (queue.pop(value)) {
          ++pop_count;
        }
      }
    };

    std::thread producer_thread(producer);
    std::thread consumer_thread(consumer);
    producer_thread.join();
    consumer_thread.join();

    state.SetItemsProcessed(push_count + pop_count);
  }
}

// Register the function as a benchmark with different input sizes
BENCHMARK(BM_ScspLockFreeQueue)->Arg(1000)->Arg(10000)->Arg(100000);

static void BM_ScspMutexQueue(benchmark::State& state) {  // NOLINT
  const int num_elements = state.range(0);

  for (auto _ : state) {
    sam::data_structure::ScspMutexQueue<int> queue(num_elements);

    size_t push_count = 0, pop_count = 0;
    auto producer = [&]() {
      for (int i = 0; i < num_elements; ++i) {
        if (queue.push(push_count)) {
          ++push_count;
        }
      }
    };

    auto consumer = [&]() {
      int value = -1;
      for (int i = 0; i < num_elements; ++i) {
        if (queue.pop(&value)) {
          ++pop_count;
        }
      }
    };

    std::thread producer_thread(producer);
    std::thread consumer_thread(consumer);
    producer_thread.join();
    consumer_thread.join();

    state.SetItemsProcessed(push_count + pop_count);
  }
}

BENCHMARK(BM_ScspMutexQueue)->Arg(1000)->Arg(10000)->Arg(100000);

// Main function to run the benchmarks
BENCHMARK_MAIN();
