#include <memory>

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
}  // namespace stealing
}  // namespace threadpool
}  // namespace sam
