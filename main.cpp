#include <glog/logging.h>
#include <netinet/in.h>
#include <sys/_endian.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cassert>
#include <cstring>

#include "../include/http/http_server.hpp"
#include "../include/socket/stream.hpp"

const int PORT = 3000;
const int BACKLOG = 10;
const int BUFFER_SIZE = 1024;

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);

  // Set log info to console
  FLAGS_alsologtostderr = 1;

  sam::socket::ServerStream server = sam::socket::ServerStream("127.0.0.1", 3000);
  server.start();

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
