/**
 * @file centralized_threadpool.hpp
 * @brief Centralized Threadpool Implementation for Task Execution
 *
 * This file contains the implementation of a centralized threadpool for managing and executing tasks.
 * It includes the base class for the threadpool, the main threadpool class, and the worker class.
 */

#include <future>
#include <memory>
#include <thread>
#include <utility>

#include "data_structure/scsp_lockfree_queue.hpp"
#include "threadpool/base.hpp"

namespace sam {
namespace threadpool {
namespace centralized {

/**
 * @class ICentralizedThreadpool
 * @brief Base class for a centralized threadpool.
 *
 * This class provides the basic infrastructure for a centralized threadpool, including task queue management
 * and synchronization mechanisms.
 */
class ICentralizedThreadpool : public IThreadpool, public std::enable_shared_from_this<ICentralizedThreadpool> {
 protected:
  std::mutex task_queue_mutex_;
  std::condition_variable task_queue_conditional_variable_;
  data_structure::ScspLockFreeQueue<Task> task_queue_;

  friend class Worker;

 public:
  /**
   * @brief Constructor for ICentralizedThreadpool.
   *
   * @param config Configuration settings for the threadpool.
   */
  ICentralizedThreadpool(Config&& config) : task_queue_(config.task_queue_cap), IThreadpool(std::forward<Config>(config)) {}
};

/**
 * @class CentralizedThreadpool
 * @brief A centralized threadpool implementation.
 *
 * This class manages a pool of worker threads and provides an interface for submitting tasks to be executed by the pool.
 */
class CentralizedThreadpool : public ICentralizedThreadpool {
  std::vector<std::thread> workers_;

  /**
   * @brief Initializes the worker threads.
   */
  void initialize_() override;

 public:
  CentralizedThreadpool() = delete;

  /**
   * @brief Constructs a CentralizedThreadpool with the given configuration.
   *
   * @param config Configuration settings for the threadpool.
   */
  explicit CentralizedThreadpool(Config&& config);

  /**
   * @brief Destructor for CentralizedThreadpool.
   */
  ~CentralizedThreadpool();

  /**
   * @brief Factory method to create a shared pointer to a CentralizedThreadpool instance.
   *
   * @param config Configuration settings for the threadpool.
   * @return std::shared_ptr<CentralizedThreadpool> Shared pointer to the created CentralizedThreadpool instance.
   */
  static std::shared_ptr<CentralizedThreadpool> create(Config&& config);

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

/**
 * @class Worker
 * @brief A worker thread that executes tasks from the threadpool.
 *
 * This class represents a worker thread that continuously fetches and executes tasks from the threadpool's task queue.
 */
class Worker : public IWorker<ICentralizedThreadpool> {
 public:
  Worker() = delete;

  /**
   * @brief Constructs a Worker with the given ID and threadpool.
   *
   * @param id The ID of the worker.
   * @param thread_pool A shared pointer to the centralized threadpool.
   */
  Worker(size_t id, std::shared_ptr<ICentralizedThreadpool> thread_pool);

  /**
   * @brief The main function executed by the worker thread.
   */
  void operator()() override;
};

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
