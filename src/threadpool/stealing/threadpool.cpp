#include "threadpool/stealing_threadpool.hpp"

namespace sam {
namespace threadpool {
namespace stealing {
StealingThreadpool::StealingThreadpool(Config&& config) : IStealingThreadpool(std::forward<Config>(config)) {}

StealingThreadpool::~StealingThreadpool() {}

void StealingThreadpool::initialize_() {
  for (size_t i = 0; i < config_.minimum_thread; ++i) {
    workers_.emplace_back(std::thread{Worker{i, shared_from_this()}});
  }
}

std::shared_ptr<StealingThreadpool> StealingThreadpool::create(Config&& config) {
  auto pool = std::shared_ptr<StealingThreadpool>(new StealingThreadpool(std::forward<Config>(config)));
  pool->initialize_();

  return pool;
}
}  // namespace stealing
}  // namespace threadpool
}  // namespace sam
