#include <benchmark/benchmark.h>

#include "../include/data_structure/scsp_lockfree_queue.hpp"
#include "../include/data_structure/scsp_mutex_queue.hpp"
#include "./data_strucutre/scsp_lockfree_queue_benchmark.hpp"
#include "./data_strucutre/scsp_mutex_queue_benchmark.hpp"

using sam::benchmark::data_structure::ScspLockFreeQueueFixture;
using sam::benchmark::data_structure::ScspLockFreeQueueFixture_ScspLockFreeQueueBenchmark_Benchmark;
using sam::data_structure::ScspLockFreeQueue;

using sam::benchmark::data_structure::ScspMutexQueueFixture;
using sam::benchmark::data_structure::ScspMutexQueueFixture_ScspMutexQueueBenchmark_Benchmark;
using sam::data_structure::ScspMutexQueue;

ScspLockFreeQueue<int> ScspLockFreeQueueFixture::queue(1000);
ScspMutexQueue<int> ScspMutexQueueFixture::queue(1000);

std::mutex ScspLockFreeQueueFixture::mu;

BENCHMARK_REGISTER_F(ScspMutexQueueFixture, ScspMutexQueueBenchmark)->Range(1000, 1000000)->Threads(2)->UseRealTime();
BENCHMARK_REGISTER_F(ScspLockFreeQueueFixture, ScspLockFreeQueueBenchmark)->Range(1000, 1000000)->Threads(2)->UseRealTime();

BENCHMARK_MAIN();
