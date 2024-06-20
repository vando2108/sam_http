#include <glog/logging.h>
#include <netinet/in.h>
#include <sys/_endian.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cassert>
#include <condition_variable>
#include <cstring>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

#include "http/http_server.hpp"
#include "socket/stream.hpp"
#include "threadpool/base.hpp"
#include "threadpool/centralized_threadpool.hpp"

std::mutex mutex;
std::condition_variable condition_variable;
bool lock_key;

void thread(int id) {
  std::unique_lock<std::mutex> lock{mutex};
  condition_variable.wait(lock, [&] { return lock_key; });
  LOG(INFO) << "thread " << id << " finish running";
}

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);

  // Set log info to console
  FLAGS_alsologtostderr = 1;

  // sam::socket::ServerStream server = sam::socket::ServerStream("127.0.0.1", 3000);
  // server.start();

  sam::threadpool::Config config;
  config.minimum_thread = 10;
  auto temp = sam::threadpool::centralized::Threadpool::create(std::move(config));

  for (;;) {
    char buff[1024];
    char* cmd = fgets(buff, sizeof(buff), stdin);

    if (cmd == nullptr) {
      break;
    }
  }

  LOG(INFO) << "terminate...";
  //
  // auto task = []() { return 42; };
  //
  // auto future = threadpool.submit_task(task);
  // LOG(INFO) << future.get();

  return 0;
}
