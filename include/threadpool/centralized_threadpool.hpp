#include <future>
#include <memory>
#include <thread>
#include <utility>

#include "data_structure/scsp_lockfree_queue.hpp"
#include "threadpool/base.hpp"

namespace sam {
namespace threadpool {
namespace centralized {
class ICentralizedThreadpool : public IThreadpool, public std::enable_shared_from_this<ICentralizedThreadpool> {
 protected:
  std::mutex task_queue_mutex_;
  std::condition_variable task_queue_conditional_variable_;
  data_structure::ScspLockFreeQueue<Task> task_queue_;

  friend class Worker;

 public:
  ICentralizedThreadpool(Config&& config) : task_queue_(config.task_queue_cap), IThreadpool(std::forward<Config>(config)) {}
};

class CentralizedThreadpool : public ICentralizedThreadpool {
  friend class Worker;

 private:
  std::vector<std::thread> workers_;

  void initialize_() override;

 public:
  CentralizedThreadpool() = delete;
  explicit CentralizedThreadpool(Config&&);
  ~CentralizedThreadpool();

  static std::shared_ptr<CentralizedThreadpool> create(Config&&);

  template <typename F, typename... Args>
  auto submit_task(F&& f, Args&&... args) -> std::future<decltype(f(args...))>;
};

class Worker : public IWorker<ICentralizedThreadpool> {
 public:
  Worker() = delete;
  Worker(size_t, std::shared_ptr<ICentralizedThreadpool>);

  void operator()() override;
};

template <typename F, typename... Args>
auto CentralizedThreadpool::submit_task(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
  using return_type = decltype(f(args...));

  std::packaged_task<return_type()> task(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
  auto task_ptr = std::make_shared<std::packaged_task<return_type()>>(std::move(task));
  auto result = task_ptr->get_future();

  auto wrapper = [task_ptr]() { (*task_ptr)(); };

  {
    std::scoped_lock<std::mutex> lock{task_queue_mutex_};
    task_queue_.push(std::move(wrapper));
  }
  task_queue_conditional_variable_.notify_one();

  return result;
}
}  // namespace centralized
}  // namespace threadpool
}  // namespace sam
