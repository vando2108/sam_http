#include <glog/logging.h>

#include <mutex>
#include <thread>
#include <utility>

#include "threadpool/centralized_threadpool.hpp"

namespace sam {
namespace threadpool {
namespace centralized {
CentralizedThreadpool::CentralizedThreadpool(Config&& config) : ICentralizedThreadpool(std::forward<Config>(config)) {}

CentralizedThreadpool::~CentralizedThreadpool() {
  {
    std::unique_lock<std::mutex> lock{task_queue_mutex_};
    is_running_ = false;
  }

  task_queue_conditional_variable_.notify_all();
  for (auto& worker : workers_) {
    if (worker.joinable()) {
      worker.join();
    } else {
      LOG(ERROR) << "Can't join thread";
    }
  }
}

void CentralizedThreadpool::initialize_() {
  for (size_t i = 0; i < config_.minimum_thread; ++i) {
    workers_.emplace_back(std::thread{Worker{i, shared_from_this()}});
  }
}

std::shared_ptr<CentralizedThreadpool> CentralizedThreadpool::create(Config&& config) {
  auto pool = std::shared_ptr<CentralizedThreadpool>(new CentralizedThreadpool(std::move(config)));
  pool->initialize_();

  return pool;
}
}  // namespace centralized
}  // namespace threadpool
}  // namespace sam
