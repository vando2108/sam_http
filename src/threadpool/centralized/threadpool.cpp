#include <glog/logging.h>

#include <mutex>
#include <thread>
#include <utility>

#include "threadpool/centralized_threadpool.hpp"

namespace sam {
namespace threadpool {
namespace centralized {
Threadpool::Threadpool(Config&& config) : ICentralizedThreadpool(std::forward<Config>(config)) {}

Threadpool::~Threadpool() {
  LOG(INFO) << "destructor";
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

void Threadpool::initialize_() {
  auto this_ptr = shared_from_this();
  if (this_ptr == nullptr) {
    LOG(INFO) << "nullptr";
  }

  for (size_t i = 0; i < config_.minimum_thread; ++i) {
    // LOG(INFO) << "Test";
    workers_.emplace_back(std::thread{Worker{i, this_ptr}});
  }
}

std::shared_ptr<Threadpool> Threadpool::create(Config&& config) {
  auto pool = std::shared_ptr<Threadpool>(new Threadpool(std::move(config)));
  pool->initialize_();

  return pool;
}
}  // namespace centralized
}  // namespace threadpool
}  // namespace sam
