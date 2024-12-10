#include <future>
#include <memory>
#include <mutex>

#include "data_structure/scsp_lockfree_queue.hpp"
#include "threadpool/base.hpp"

namespace sam {
namespace threadpool {
namespace stealing {
class IStealingThreadpool : public IThreadpool, public std::enable_shared_from_this<IStealingThreadpool> {
 public:
  IStealingThreadpool(Config&& config) : IThreadpool(std::forward<Config>(config)) {}
  virtual ~IStealingThreadpool() {}
};

class StealingThreadpool : public IStealingThreadpool {
  std::vector<std::thread> workers_;

  /**
   * @brief Since we are using the scsp_lockfree_queue, we need to have two separate mutex locks for each side of the queue.
   */
  data_structure::ScspLockFreeQueue<Task> global_task_queue_;

  /**
   * @brief Mutex lock for the push/enqueue side of the global task queue.
   */
  std::mutex global_task_queue_mutex_enqueue_;

  /**
   * @brief Mutex lock for the pop/dequeue side of the global task queue.
   */
  std::mutex global_task_queue_mutex_dequeue_;

  /**
   * @brief Initializes the worker threads.
   */
  void initialize_() override;

 public:
  StealingThreadpool() = delete;
  ~StealingThreadpool();

  /**
   * @brief Constructs a StealingThreadpool with the given configuration.
   *
   * @param config Configuration settings for the threadpool.
   */
  explicit StealingThreadpool(Config&& config);

  /**
   * @brief Factory method to create a shared pointer to a CentralizedThreadpool instance.
   *
   * @param config Configuration settings for the threadpool.
   * @return std::shared_ptr<CentralizedThreadpool> Shared pointer to the created CentralizedThreadpool instance.
   */
  static std::shared_ptr<StealingThreadpool> create(Config&& config);

  /**
   * @brief Submits a task to the threadpool for execution.
   *
   * @tparam F The type of the function to execute.
   * @tparam Args The types of the arguments to pass to the function.
   * @param f The function to execute.
   * @param args The arguments to pass to the function.
   * @return std::future<decltype(f(args...))> Future representing the result of the task.
   */
  template <typename F, typename... Args>
  auto submit_task(F&& f, Args&&... args) -> std::future<decltype(f(args...))>;
};

class Worker : public IWorker<IStealingThreadpool> {
  sam::data_structure::ScspLockFreeQueue<Task> task_queue_;

 public:
  Worker() = delete;
  Worker(size_t, std::shared_ptr<IStealingThreadpool>);
  // Worker(Worker&& other) : IWorker(other.id_, other.thread_pool_) { LOG(INFO) << "Move constructor"; }

  void operator()() override;
};

template <typename F, typename... Args>
auto StealingThreadpool::submit_task(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
  using return_type = decltype(f(args...));

  std::packaged_task<return_type()> task(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
  auto task_ptr = std::make_shared<std::packaged_task<return_type()>>(std::move(task));
  auto result = task_ptr->get_future();
  auto wrapper = [task_ptr]() { (*task_ptr)(); };

  { std::scoped_lock<std::mutex> lock{global_task_queue_mutex_enqueue_}; }

  return result;
}
}  // namespace stealing
}  // namespace threadpool
}  // namespace sam
