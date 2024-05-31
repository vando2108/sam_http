#include <glog/logging.h>
#include <netinet/in.h>
#include <sys/_endian.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cassert>
#include <chrono>  // NOLINT
#include <cstring>
#include <thread>  // NOLINT

#include "../include/data_structure/scsp_lockfree_queue.hpp"
#include "../include/http/http_server.hpp"
#include "../include/socket/stream.hpp"
#include "../include/utils/time.hpp"

const int PORT = 3000;
const int BACKLOG = 10;
const int BUFFER_SIZE = 1024;

void testScspLockFreeQueue() {
  // Test multithreading
  const int num_threads = 1;
  const int num_elements = 10;

  sam::data_structure::ScspLockFreeQueue<int> mt_queue(num_elements * num_threads);

  LOG(INFO) << "timestamp: " << sam::utils::now();
  auto producer = [&mt_queue](int start) {
    for (int i = start; i < start + num_elements; ++i) {
      while (!mt_queue.push(i)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(sam::utils::now() % 1000));
      }
      LOG(INFO) << "push: " << i << ' ';
      std::this_thread::sleep_for(std::chrono::milliseconds(sam::utils::now() % 1000));
    }
  };

  auto consumer = [&mt_queue](std::vector<int>& results) {
    int value = -1;
    for (int i = 0; i < num_elements; ++i) {
      while (!mt_queue.pop(&value)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(sam::utils::now() % 1000));
      }
      LOG(INFO) << "poped: " << value;
      results.push_back(value);
      std::this_thread::sleep_for(std::chrono::milliseconds(sam::utils::now() % 1000));
    }
  };

  std::vector<std::thread> producers;
  std::vector<std::thread> consumers;
  std::vector<std::vector<int>> results(num_threads);

  // Start producers
  for (int i = 0; i < num_threads; ++i) {
    producers.emplace_back(producer, i * num_elements);
  }

  // Start consumers
  for (int i = 0; i < num_threads; ++i) {
    consumers.emplace_back(consumer, std::ref(results[i]));
  }

  // Join producers
  for (auto& producer : producers) {
    producer.join();
  }

  // Join consumers
  for (auto& consumer : consumers) {
    consumer.join();
  }

  // Verify results
  std::vector<int> all_results;
  for (const auto& res : results) {
    all_results.insert(all_results.end(), res.begin(), res.end());
  }
  std::sort(all_results.begin(), all_results.end());

  for (int i = 0; i < num_threads * num_elements; ++i) {
    assert(all_results[i] == i);
  }
}

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);

  // Set log info to console
  FLAGS_alsologtostderr = 1;

  sam::socket::ServerStream server = sam::socket::ServerStream("127.0.0.1", 3000);
  server.start();

  testScspLockFreeQueue();

  // // Accept and handle incoming connections
  // while (true) {
  //   sockaddr_in client_address;
  //   socklen_t client_addr_len = sizeof(client_address);
  //   int client_fd =
  //       accept(server_fd, reinterpret_cast<sockaddr*>(&client_address), &client_addr_len);
  //
  //   if (client_fd < 0) {
  //     std::cerr << "Failed to accept connection" << std::endl;
  //     continue;
  //   }
  //
  //   // Read the request
  //   char buffer[BUFFER_SIZE] = {0};
  //   ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE);
  //   if (bytes_read < 0) {
  //     std::cerr << "Failed to read from socket" << std::endl;
  //     close(client_fd);
  //     continue;
  //   }
  //
  //   std::cout << "Received request:\n" << buffer << std::endl;
  // }

  // Close the server socket
}
