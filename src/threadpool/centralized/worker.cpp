#include <memory>

#include "threadpool/base.hpp"
#include "threadpool/centralized_threadpool.hpp"

namespace sam {
namespace threadpool {
namespace centralized {
Worker::Worker(size_t id, std::shared_ptr<ICentralizedThreadpool> thread_pool) : IWorker(id, thread_pool) {}

void Worker::operator()() {
  if (thread_pool_) {
    std::unique_lock<std::mutex> lock{thread_pool_->task_queue_mutex_};  // lock the task_queue
  } else {
    LOG(INFO) << "Error";
  }

  // while (thread_pool_->is_running_) {
  //   thread_pool_->task_queue_conditional_variable_.wait(lock, [this] { return this->thread_pool_->is_running_; });
  // }
  // while (thread_pool_->is_running_ || (!thread_pool_->is_running_ && !thread_pool_->task_queue_.empty())) {
  //   thread_pool_->task_queue_conditional_variable_.wait(
  //       lock, [this] { return this->thread_pool_->is_running_ || this->thread_pool_->task_queue_.empty(); });
  //   //
  //   //   if (!this->thread_pool_->task_queue_.empty()) {
  //   //     Task task;
  //   //     thread_pool_->task_queue_.pop(task);
  //   //
  //   //     // unlock the task_queue after got a job
  //   //     lock.unlock();
  //   //
  //   //     // execute the task got from the task_queue
  //   //     task();
  //   //
  //   //     // lock the task_queue again to continue wait for new task
  //   //     lock.lock();
  //   //   }
  // }
}
}  // namespace centralized
}  // namespace threadpool
}  // namespace sam
