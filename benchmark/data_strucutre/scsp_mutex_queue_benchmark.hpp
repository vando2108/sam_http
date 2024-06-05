
#ifndef BENCHMARK_DATA_STRUCTURE_SCSP_MUTEX_QUEUE
#define BENCHMARK_DATA_STRUCTURE_SCSP_MUTEX_QUEUE

#include <benchmark/benchmark.h>

#include "../../include/data_structure/scsp_mutex_queue.hpp"

namespace sam {
namespace benchmark {
namespace data_structure {
class ScspMutexQueueFixture : public ::benchmark::Fixture {
 public:
  static sam::data_structure::ScspMutexQueue<int> queue;

  void SetUp(::benchmark::State& state) override {
    const int64_t number_of_elements = state.range(0);
    if (number_of_elements != queue.capacity()) {
      ScspMutexQueueFixture::queue = std::move(sam::data_structure::ScspMutexQueue<int>(number_of_elements));
    }
  }

  void TearDown(::benchmark::State& state) override {}
};

BENCHMARK_DEFINE_F(ScspMutexQueueFixture, ScspMutexQueueBenchmark)(::benchmark::State& state) {
  const bool is_push_thread = state.thread_index() == 0;
  const int64_t number_of_elements = state.range(0);

  if (is_push_thread) {
    for (auto _ : state) {
      for (int64_t i = 0; i < number_of_elements; ++i) {
        queue.push(i);
      }
    }
  } else {
    int value = -1;
    for (auto _ : state) {
      for (int64_t i = 0; i < number_of_elements; ++i) {
        queue.pop(&value);
      }
    }
  }
}

}  // namespace data_structure
}  // namespace benchmark
}  // namespace sam

#endif
