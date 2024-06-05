#include <glog/logging.h>
#include <netinet/in.h>
#include <sys/_endian.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cassert>
#include <chrono>
#include <cstring>
#include <functional>
#include <thread>

#include "../include/data_structure/scsp_lockfree_queue.hpp"
#include "../include/http/http_server.hpp"
#include "../include/socket/stream.hpp"
#include "../include/utils/time.hpp"
#include "data_structure/scsp_mutex_queue.hpp"

const int PORT = 3000;
const int BACKLOG = 10;
const int BUFFER_SIZE = 1024;
const int num_elements = 1000;

void testScspMutexQueue() {
  sam::data_structure::ScspMutexQueue<int> queue(num_elements);

  auto producer = [&]() {
    for (int i = 0; i < num_elements; ++i) {
      queue.push(i);
    }
  };

  auto consumer = [&]() {
    int value = -1;
    for (int i = 0; i < num_elements; ++i) {
      queue.pop(&value);
    }
  };

  std::thread producer_thread(producer);
  std::thread consumer_thread(consumer);
  producer_thread.join();
  consumer_thread.join();
}

void testScspLockFreeQueue() {
  sam::data_structure::ScspLockFreeQueue<int> queue(num_elements);

  auto producer = [&]() {
    for (int i = 0; i < num_elements; ++i) {
      queue.push(i);
    }
  };

  auto consumer = [&]() {
    int value = -1;
    for (int i = 0; i < num_elements; ++i) {
      queue.pop(value);
    }
  };

  std::thread producer_thread(producer);
  std::thread consumer_thread(consumer);
  producer_thread.join();
  consumer_thread.join();
}

void logTime(std::string_view label, std::function<void()> func) {
  const int start_time = sam::utils::now();
  func();
  const int end_time = sam::utils::now();
  LOG(INFO) << "execution time of " << label << " function: " << end_time - start_time;
}

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);

  // Set log info to console
  FLAGS_alsologtostderr = 1;

  // sam::socket::ServerStream server = sam::socket::ServerStream("127.0.0.1", 3000);
  // server.start();

  logTime("mutex", testScspMutexQueue);
  logTime("lockfree", testScspLockFreeQueue);

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
