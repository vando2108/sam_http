#include "threadpool/stealing_threadpool.hpp"

namespace sam {
namespace threadpool {
namespace stealing {
Worker::Worker(size_t id, std::shared_ptr<IStealingThreadpool> threadpool)
    : IWorker(id, threadpool), task_queue_(threadpool->config().task_queue_cap) {}

void Worker::operator()() { LOG(INFO) << "test"; }
}  // namespace stealing
}  // namespace threadpool
}  // namespace sam
