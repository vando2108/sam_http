#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <functional>
#include <future>
#include <memory>

namespace sam {
namespace threadpool {
typedef std::function<void()> Task;

struct Config {
  size_t minimum_thread;
  size_t task_queue_cap;
  std::string worker_prefix;

  Config(size_t minimum_thread = 100, size_t task_queue_cap = 1000, std::string&& worker_prefix = "worker")
      : minimum_thread(minimum_thread), task_queue_cap(task_queue_cap), worker_prefix(std::move(worker_prefix)) {}
};

class IThreadpool {
 protected:
  Config config_;
  bool is_running_;
  size_t busy_thread_;

  // friend class IWorker;

 public:
  IThreadpool() = delete;
  IThreadpool(Config&& config) : config_(std::move(config)), is_running_(true), busy_thread_(0) {}
  virtual ~IThreadpool() = default;

  static std::shared_ptr<IThreadpool> create(Config&&);

  template <typename Func, typename... Args>
  auto submit_task(Func&& func, Args&&... args) -> std::future<decltype(func(args...))>;

 private:
  virtual void initialize_() = 0;
};

template <typename ThreadpoolType>
class IWorker {
 protected:
  size_t id_;
  std::shared_ptr<ThreadpoolType> thread_pool_;

 public:
  IWorker() = delete;
  IWorker(size_t id, std::shared_ptr<ThreadpoolType> thread_pool) : id_(id), thread_pool_(thread_pool) {}

  virtual ~IWorker() = default;

  virtual void operator()() = 0;
};
}  // namespace threadpool
}  // namespace sam

#endif  // !THREADPOOL_HPP
